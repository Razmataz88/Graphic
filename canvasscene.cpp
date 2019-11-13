/*
 * File:    canvasscene.cpp
 * Author:  Rachel Bood 100088769
 * Date:    2014/11/07
 * Version: 1.0
 *
 * Purpose: Initializes a QGraphicsScene to implement a drag and drop feature.
 *          still very much a WIP
 *
 * Modification history:
 * Oct 13, 2019 (JD V1.1)
 *  - no functional code changes: some formatting, some addition of comments,
 *    made some debug statements more verbose, deleted some
 *    long-commented-out-by-Rachel code, and deleted the setting of an
 *    unused variable.
 *    (Note that removing braces from some cases caused errors on
 *    stmts of the form "type var = value;", but, oddly, these go away
 *    if these are replaced with "type var; var = value;".  I took
 *    away the braces before I noticed this because it made the
 *    indentation a bit weird, but perhaps I should have left well
 *    enough alone in this case.)
 * Nov 13, 2019 (JD V1.2)
 *  - rename Label to HTML_Label, as per changes to the naming scheme.
 */

#include "canvasscene.h"
#include "canvasview.h"
#include "edge.h"
#include "graph.h"
#include "graphmimedata.h"
#include "node.h"

#include <QtDebug>
#include <QGraphicsSceneMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QCursor>
#include <qmath.h>
#include <QApplication>
#include <QPainter>
#include <QtCore>
#include <QtGui>

static const bool verbose = false;

CanvasScene::CanvasScene()
    :  mCellSize(25, 25)
{
    setItemIndexMethod(QGraphicsScene::NoIndex);
    setSortCacheEnabled(true);
    numOfNodes = 0;

    connectNode1a = nullptr;
    connectNode2a = nullptr;
    connectNode1b = nullptr;
    connectNode2b = nullptr;

    modeType = CanvasView::none;
    mDragged = nullptr;
    snapToGrid = true;
    undoPositions = QList<undo_Node_Pos*>();
}



void CanvasScene::dragMoveEvent(QGraphicsSceneDragDropEvent * event)
{
    Q_UNUSED(event);
}



void CanvasScene::dropEvent(QGraphicsSceneDragDropEvent * event)
{
    const GraphMimeData * mimeData
	= qobject_cast<const GraphMimeData *> (event->mimeData());
    if (mimeData)
    {
        Graph * graphItem = mimeData->graphItem();

        graphItem->setPos(event->scenePos().rx()
			  - graphItem->boundingRect().x(),
                          event->scenePos().ry()
                          - graphItem->boundingRect().y());
        addItem(graphItem);
        graphItem->isMoved();
        clearSelection();
    }
}



void CanvasScene::drawBackground(QPainter * painter, const QRectF &rect)
{
    if (snapToGrid)
    {
        qreal left = int(rect.left()) - (int(rect.left()) % mCellSize.width());
        qreal top = int(rect.top()) - (int(rect.top()) % mCellSize.height());

        for (qreal x = left; x < rect.right(); x += mCellSize.width())
            for (qreal y = top; y < rect.bottom(); y += mCellSize.height())
                painter->drawPoint(QPointF(x, y));
    }
    else
        QGraphicsScene::drawBackground(painter, rect);
}



void CanvasScene::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    if (itemAt(event->scenePos(), QTransform()) != nullptr)
    {
        QList<QGraphicsItem *> itemList
	    = items(event->scenePos(), Qt::IntersectsItemShape,
		    Qt::DescendingOrder, QTransform());

        switch (getMode())
        {
	  case CanvasView::join:
	    foreach (QGraphicsItem * item, itemList)
	    {
		QGraphicsItem * parent1 = nullptr;
		QGraphicsItem * parent2 = nullptr;
		if (item->type() == Node::Type)
		{
		    if (connectNode1a == nullptr)
		    {
			connectNode1a = qgraphicsitem_cast<Node*>(item);
			connectNode1a->chosen(1);
			break;
		    }
		    else if (connectNode1b == nullptr)
		    {
			connectNode1b = qgraphicsitem_cast<Node*>(item);
			parent1 = connectNode1a->findRootParent();
			parent2 = connectNode1b->findRootParent();

			if (parent2 == parent1)
			{
			    connectNode1b->chosen(2);
			    break;
			}
			else
			    connectNode1b = nullptr;
		    }
		    if (connectNode2a == nullptr)
		    {
			connectNode2a = qgraphicsitem_cast<Node*>(item);
			connectNode2a->chosen(1);
			break;
		    }
		    else if (connectNode2b == nullptr)
		    {
			connectNode2b = qgraphicsitem_cast<Node*>(item);

			parent1 = connectNode2a->findRootParent();
			parent2 = connectNode2b->findRootParent();

			if (parent2 == parent1)
			{
			    connectNode2b->chosen(2);
			    break;
			}
			else
			    connectNode2b = nullptr;
		    }
		}
	    }
	    break;

	  case CanvasView::del:
	    foreach (QGraphicsItem * item, itemList)
	    {
		if (item != nullptr || item != 0)
		{
		    if (item->type() == HTML_Label::Type)
		    {
			if (verbose)
			    qDebug() << "mousepress/delete LABEL";
		    }
		    else if (item->type() == Node::Type)
		    {
			if (verbose)
			    qDebug() << "mousepress/Delete Node";

			Node * node = qgraphicsitem_cast<Node *>(item);

			// Removes node from undolist if deleted.
			// Safety precaution to avoid null pointers.
			for (int i = 0; i < undoPositions.length(); i++)
			{
			    if (undoPositions.at(i)->node == node)
				undoPositions.removeAt(i);
			}

			// Delete all edges incident to node to be deleted
			foreach (Edge * edge, node->edgeList)
			{
			    if (edge != nullptr || edge != 0)
			    {
				edge->destNode()->removeEdge(edge);
				edge->sourceNode()->removeEdge(edge);

				edge->setParentItem(nullptr);
				removeItem(edge);
				delete edge;
				edge = nullptr;
			    }
			}

			Graph * parent =
			    qgraphicsitem_cast<Graph*>(node->parentItem());

			if (parent != nullptr || parent != 0)
			{
			    if (parent->childItems().length() == 0)
			    {
				parent->setParentItem(nullptr);
				removeItem(parent);
				delete parent;
				parent = nullptr;
			    }
			}

			// Delete the node.
			node->setParentItem(nullptr);
			removeItem(node);
			delete node;
			node = nullptr;
			break;
		    }
		    else if (item->type() == Edge::Type)
		    {
			if (verbose)
			    qDebug() << "mousepress/Delete Edge";

			Edge * edge = qgraphicsitem_cast<Edge *>(item);
			edge->destNode()->removeEdge(edge);
			edge->sourceNode()->removeEdge(edge);

			edge->setParentItem(nullptr);
			removeItem(edge);
			delete edge;
			edge = nullptr;
			break;
		    }
		}
	    }
	    break;

	  case CanvasView::edit:
	    undo_Node_Pos * undoPos;
	    undoPos = new undo_Node_Pos();

	    foreach (QGraphicsItem * item, itemList)
	    {
		if (item->type() == Node::Type)
		{
		    if (event->button() == Qt::LeftButton)
		    {
			mDragged = qgraphicsitem_cast<Node*>(item);
			undoPos->node = qgraphicsitem_cast<Node *>(mDragged);
			undoPos->pos = mDragged->pos();
			undoPositions.append(undoPos);
			if (snapToGrid)
			{
			    if (verbose)
				qDebug() << "mousepress/edit/drag/snap2grid"
					 << mDragged->type();
			    mDragOffset = event->scenePos() - mDragged->pos();
			}
		    }
		}

	    }
	    if (mDragged != nullptr)
		if (mDragged->type() == Node::Type)
		    QGraphicsScene::mousePressEvent(event);
	    break;

	  case CanvasView::none:
	    foreach(QGraphicsItem * item, itemList)
	    {
		if (item != nullptr || item != 0)
		    if (item->type() == Graph::Type)
		    {
			mDragged = item;
			while (mDragged->parentItem() != nullptr)
			    mDragged = mDragged->parentItem();

			mDragOffset = event->scenePos() - mDragged->pos();

			QGraphicsScene::mousePressEvent(event);
			break;
		    }
	    }
	    break;

	  default:
            break;
        }
    }
    else
    {
        mDragged = nullptr;
        QGraphicsScene::mousePressEvent(event);
    }
}



void CanvasScene::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    if (verbose)
        qDebug() << "mosueMove/Mode: " + QString::number(getMode());
    if (mDragged
	&& (getMode() == CanvasView::none || getMode() == CanvasView::edit))
    {
        if (mDragged->type() == Graph::Type)
        {
            // Ensure that the item's offset from the mouse cursor
	    // stays the same.
            mDragged->setPos(event->scenePos() - mDragOffset);
        }
        else if (mDragged->type() == Node::Type)
        {
            mDragged->setPos(mDragged->mapToParent(
				 mDragged->mapFromScene(event->scenePos())));
        }
    }
}



void CanvasScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    if (mDragged && snapToGrid
	&& (getMode() == CanvasView::none || getMode() == CanvasView::edit))
    {
        int x = 0;
        int y = 0;
        if (mDragged->type() == Graph::Type)
        {
            x = floor(mDragged->scenePos().x()
                      / mCellSize.width()) * mCellSize.width();
            y = floor(mDragged->scenePos().y()
                      / mCellSize.height()) * mCellSize.height();
            mDragged->setPos(x, y);
        }

        else if (mDragged->type() == Node::Type)
        {

            x = round(mDragged->pos().x() / mCellSize.width())
		* mCellSize.width();
            y = round(mDragged->pos().y() / mCellSize.height())
		* mCellSize.height();
            mDragged->setPos(x , y);
            clearSelection();
        }
    }

    mDragged = nullptr;
    clearSelection();
    QGraphicsScene::mouseReleaseEvent(event);
}



void CanvasScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
    switch (getMode())
    {
      case CanvasView::del:
      {
	  if (itemAt(event->scenePos(), QTransform()) != nullptr)
	  {
	      QGraphicsItem * item = itemAt(event->scenePos(), QTransform());

	      Graph * graph = qgraphicsitem_cast<Graph*>(item);

	      if (!graph)
		  graph =  qgraphicsitem_cast<Graph*>(item->parentItem());

	      if (graph)
	      {
		  while (graph->parentItem() != nullptr
			 && graph->parentItem()->type() == Graph::Type)
		      graph = qgraphicsitem_cast<Graph*>(graph->parentItem());

		  removeItem(graph);
		  delete graph;
		  graph = nullptr;
	      }
	  }
	  break;

      }
      default:
      {
	  // do nothing
      }
    }
    QGraphicsScene::mouseDoubleClickEvent(event);
}



void CanvasScene::keyReleaseEvent(QKeyEvent * event)
{
    switch (event->key())
    {
      case Qt::Key_J:
	Graph * item;
	Graph * root1;
	Graph * root2;

	item = new Graph();
	item->isMoved();
	root1 = nullptr;
	root2 = nullptr;

	if (connectNode1a != nullptr && connectNode2a != nullptr
	    && connectNode1b != nullptr && connectNode2b != nullptr)
	{
	    if (connectNode1a->parentItem() != nullptr
		&& connectNode2a != nullptr
		&& connectNode1b != nullptr && connectNode2b != nullptr)
	    {
		QPointF cn1a(connectNode1a->scenePos());
		QPointF cn1b(connectNode1b->scenePos());
		QPointF cn2a(connectNode2a->scenePos());
		QPointF cn2b(connectNode2b->scenePos());

		qreal angle1 = qAtan2(cn1b.ry() - cn1a.ry(),
				      cn1b.rx() - cn1a.rx());
		qreal angle2 = qAtan2(cn2b.ry() - cn2a.ry(),
				      cn2b.rx() - cn2a.rx());
		qreal angle = angle2 - angle1;

		if (verbose)
		{
		    qDebug() << "cn1a" << cn1a;
		    qDebug() << "cn1b" << cn1b;
		    qDebug() << "cn2a" << cn2a;
		    qDebug() << "cn2b" << cn2b;

		    qDebug() << QString::number(cn1b.ry()) << " - " <<
			QString::number(cn1a.ry()) << ", " <<
			QString::number(cn1b.rx()) << " - " <<
			QString::number(cn1a.rx()) << endl;

		    qDebug() << QString::number(cn2b.ry()) << " - " <<
			QString::number(cn2a.ry()) << ", " <<
			QString::number(cn2b.rx()) << " - " <<
			QString::number(cn2a.rx()) << endl;

		    qDebug() << angle1 << endl;
		    qDebug() << angle2 << endl;
		    qDebug() << angle << endl;
		    qDebug() << qRadiansToDegrees(-angle) << endl;
		}

		if (connectNode2a->parentItem() != nullptr)
		{
		    root2 = qgraphicsitem_cast<Graph*>(
			connectNode2a->parentItem());
		    while (root2->parentItem() != nullptr)
			root2 = qgraphicsitem_cast<Graph*>(
			    root2->parentItem());
		    root2->setRotation(qRadiansToDegrees(-angle));
		}

		if (connectNode1a->parentItem() != nullptr)
		{
		    root1 = qgraphicsitem_cast<Graph*>(
			connectNode1a->parentItem());
		    while (root1->parentItem() != nullptr)
			root1 = qgraphicsitem_cast<Graph*>(
			    root1->parentItem());
		}

		qreal deltaX = connectNode1a->scenePos().rx()
		    - connectNode2a->scenePos().rx();
		qreal deltaY = connectNode1a->scenePos().ry()
		    - connectNode2a->scenePos().ry();

		if (root2)
		    root2->moveBy(deltaX, deltaY);

		// Set connectNode2a edges to connectNode1a edges
		foreach (Edge * edge, connectNode2a->edges())
		{
		    if (edge->sourceNode() == connectNode2a)
			edge->setSourceNode(connectNode1a);
		    else
			edge->setDestNode(connectNode1a);
		    connectNode1a->addEdge(edge);
		}

		// Set connectNode2b edges to connectNode1b edges
		foreach (Edge * edge, connectNode2b->edges())
		{
		    if (edge->sourceNode() == connectNode2b)
			edge->setSourceNode(connectNode1b);
		    else
			edge->setDestNode(connectNode1b);
		    connectNode1b->addEdge(edge);
		}

		bool check;
		connectNode1a->getLabel().toInt(&check);
		if (check)
		{
		    int count = 0;

		    foreach(QGraphicsItem * i, root1->childItems())
		    {
			if (i->type() == Node::Type)
			{
			    Node * node = qgraphicsitem_cast<Node*>(i);
			    node->setNodeLabel(count);
			    count++;
			}
		    }

		    foreach (QGraphicsItem * i, root2->childItems())
		    {
			if (i->type() == Node::Type
			    && i != connectNode2a && i != connectNode2b)
			{
			    Node * node = qgraphicsitem_cast<Node*>(i);
			    node->setNodeLabel(count);
			    count++;
			}
		    }
		}

		root2->setParentItem(item);
		root1->setParentItem(item);
		addItem(item);

		// Dispose of unneeded nodes
		connectNode2a->setParentItem(nullptr);
		removeItem(connectNode2a);
		delete connectNode2a;

		connectNode2b->setParentItem(nullptr);
		removeItem(connectNode2b);
		delete connectNode2b;

		connectNode1a->chosen(0);
		connectNode1b->chosen(0);

		connectNode2a = nullptr;
		connectNode2b = nullptr;
	    }
	}
	else if (connectNode1a != nullptr && connectNode2a != nullptr)
	{
	    QPointF p1(connectNode1a->scenePos());
	    QPointF p2(connectNode2a->scenePos());

	    qreal deltaX = p1.rx() - p2.rx();
	    qreal deltaY = p1.ry() - p2.ry();

	    if (connectNode2a->parentItem() != nullptr)
	    {
		root2 = qgraphicsitem_cast<Graph*>(
		    connectNode2a->parentItem());
		while (root2->parentItem() != nullptr)
		    root2 = qgraphicsitem_cast<Graph*>(root2->parentItem());
		root2->moveBy(deltaX, deltaY);
	    }

	    if (connectNode1a->parentItem() != nullptr)
	    {
		root1 = qgraphicsitem_cast<Graph*>(
		    connectNode1a->parentItem());
		while (root1->parentItem() != nullptr)
		    root1 = qgraphicsitem_cast<Graph*>(root1->parentItem());
	    }

	    foreach (Edge * edge, connectNode2a->edges())
	    {
		if (edge->sourceNode() == connectNode2a)
		    edge->setSourceNode(connectNode1a);
		else
		    edge->setDestNode(connectNode1a);
		connectNode1a->addEdge(edge);
		edge->setZValue(0);
		connectNode1a->setZValue(3);
	    }

	    bool check;
	    connectNode1a->getLabel().toInt(&check);
	    if (check)
	    {
		int count = 0;
		foreach(QGraphicsItem * i, root1->childItems())
		{
		    if (i->type() == Node::Type)
		    {
			Node * node = qgraphicsitem_cast<Node*>(i);

			node->setNodeLabel(count);
			count++;
		    }
		}
		foreach (QGraphicsItem * i, root2->childItems())
		{
		    if (i->type() == Node::Type && i != connectNode2a )
		    {
			Node * node = qgraphicsitem_cast<Node*>(i);
			node->setNodeLabel(count);
			count++;
		    }
		}
	    }
	    root2->setParentItem(item);
	    root1->setParentItem(item);
	    addItem(item);

	    // Properly dispose of unneeded node
	    removeItem(connectNode2a);
	    delete connectNode2a;
	    connectNode2a = nullptr;
	    connectNode1a->chosen(0);
	}

	if (connectNode1a)
	{
	    connectNode1a->chosen(0);
	    connectNode1a = nullptr;
	}

	if (connectNode2a)
	{
	    connectNode2a->chosen(0);
	    connectNode2a = nullptr;
	}

	if (connectNode1b)
	{
	    connectNode1b->chosen(0);
	    connectNode1b = nullptr;
	}

	if (connectNode2b)
	{
	    connectNode2b->chosen(0);
	    connectNode2b = nullptr;
	}

	clearSelection();
	break;

      case Qt::Key_Escape:
	if (undoPositions.length() > 0)
	{
	    undoPositions.last()->node->setPos(undoPositions.last()->pos);
	    undoPositions.removeLast();
	}

      default:
        break;
    }
    QGraphicsScene::keyReleaseEvent(event);
}



void CanvasScene::setCanvasMode(int mode)
{
    modeType = mode;

    if (connectNode1a)
    {
        connectNode1a->chosen(0);
        connectNode1a = nullptr;
    }
    if (connectNode2a)
    {
        connectNode2a->chosen(0);
        connectNode2a = nullptr;
    }
    if (connectNode1b)
    {
        connectNode1b->chosen(0);
        connectNode1b = nullptr;
    }
    if (connectNode2b)
    {
        connectNode2b->chosen(0);
        connectNode2b = nullptr;
    }
    undoPositions.clear();

    foreach (QGraphicsItem * item, items())
    {
        if (item->type() == Node::Type)
        {
            Node * node = qgraphicsitem_cast<Node *>(item);
            if (modeType == CanvasView::edit)
            {
                node->editLabel(true);
            }
            else
                node->editLabel(false);
        }
        else if (item->type() == Edge::Type)
        {
            Edge * edge = qgraphicsitem_cast<Edge *>(item);
            if (modeType == CanvasView::edit)
            {
                edge->editWeight(true);
            }
            else
                edge->editWeight(false);
        }
    }
}



void CanvasScene::isSnappedToGrid(bool snap)
{
    snapToGrid = snap;
}



int CanvasScene::getMode() const
{
    return modeType;
}

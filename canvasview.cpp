/*
 * File:    canvas.cpp
 * Author:  Rachel Bood
 * Date:    2014/11/07
 * Version: 1.1
 *
 * Purpose: Initializes a QGraphicsView that is used to house the
 *	    QGraphicsScene.
 *
 * Modification history:
 * Feb 8, 2016 (JD):
 * (a) Change the help text for join (JOIN_DESCRIPTION) to clarify usage.
 * (b) Drive-by cleanup.
 */

#include "canvasview.h"
#include "edge.h"
#include "node.h"
#include "graph.h"

#include <math.h>
#include <QKeyEvent>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>

static const bool verbose = true;

static const QString JOIN_DESCRIPTION =
    "Join mode: Select one or two nodes from each graph and press 'J'.  "
    "The first selected node from graph 1 is joined to the first selected "
    "node from graph 2.  "
    "If two other nodes were selected, they are joined to each other.";

static const QString DELETE_DESCRIPTION =
    "Delete mode: Click on any node or edge to be deleted.  "
    "Double Click on a graph to delete it entirely.";

static const QString EDIT_DESCRIPTION =
    "Edit mode: Move individual nodes around within a graph.";

static const QString FREESTYLE_DESCRIPTION =
    "You can add nodes by double clicking and edges by left "
    "clicking on two nodes consecutively.";


/*
 * Name:        Canvas
 * Purpose:     Contructor for Canvas class.
 * Arguments:   QWidget *
 * Output:      none
 * Modifies:    canvas
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

CanvasView::CanvasView(QWidget * parent)
: QGraphicsView(parent), timerId(0)
{
    aScene = new CanvasScene();
    aScene->setSceneRect(rect());
    setViewportUpdateMode(BoundingRectViewportUpdate); // Updates the canvas
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setAcceptDrops(true);
    setScene(aScene);

    nodeParams = new Node_Params;
    edgeParams = new Edge_Params;
    freestyleGraph = nullptr;
    setMode(mode::none);
    node1 = nullptr;
    node2 = nullptr;
}


void CanvasView::setUpNodeParams(qreal nodeDiameter, bool numberedLabels,
                                 QString label, qreal nodeLabelSize,
                                 QColor nodeFillColor, QColor nodeOutLineColor)
{
    nodeParams->diameter = nodeDiameter;
    nodeParams->isNumbered = numberedLabels;
    nodeParams->label = label;
    nodeParams->labelSize = nodeLabelSize;
    nodeParams->fillColour = nodeFillColor;
    nodeParams->outlineColour = nodeOutLineColor;
}


Node * CanvasView::createNode(QPointF pos)
{
    Node * node = new Node();
    node->setDiameter(nodeParams->diameter);
    node->setEdgeWeight(edgeParams->size);
    node->setRotation(0);
    node->setFillColour(nodeParams->fillColour);
    node->setLineColour(nodeParams->outlineColour);
    node->setPos(pos.rx(), pos.ry());
    node->setParentItem(freestyleGraph);
    return node;
}


/*
 * Name:        keyPressEvent
 * Purpose:
 * Arguments:   QKeyEvent
 * Output:      none
 * Modifies:    none
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void CanvasView::keyPressEvent(QKeyEvent * event)
{
    QGraphicsView::keyPressEvent(event);
}


void CanvasView::setMode(int m)
{
    modeType = m;
    freestyleGraph = nullptr;

    switch (modeType)
    {
      case mode::join:
	emit setKeyStatusLabelText(JOIN_DESCRIPTION);
	break;

      case mode::del:
	emit setKeyStatusLabelText(DELETE_DESCRIPTION);
	break;

      case mode::edit:
	emit setKeyStatusLabelText(EDIT_DESCRIPTION);
	break;

      case mode::none:
	emit setKeyStatusLabelText("");
	break;

      case mode::freestyle:
	emit setKeyStatusLabelText(FREESTYLE_DESCRIPTION);
	freestyleGraph = new Graph();
	aScene->addItem(freestyleGraph);
	freestyleGraph->isMoved();
	node1 = nullptr;
	node2 = nullptr;
	break;

      default:
        break;
    }
    aScene->setCanvasMode(m);
}


/*
 * Name:        MouseDoubleClickEvent
 * Purpose:     Generates a new node in the CanvasScene if in freestyle mode
 *              when use doubleclicks the mouse.
 * Arguments:   QMouseEvent
 * Output:      none
 * Modifies:    CanvasScene
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void CanvasView::mouseDoubleClickEvent(QMouseEvent * event)
{
    QPointF pt;

    switch(getMode())
    {
      case mode::freestyle:
	pt = mapToScene(event->pos());
	createNode(pt);
	break;

      default:
        QGraphicsView::mouseDoubleClickEvent(event);
    }
}


void CanvasView::mousePressEvent(QMouseEvent * event)
{
    QList<QGraphicsItem *> itemList = this->scene()->items(
	this->mapToScene(event->pos()),
	Qt::IntersectsItemShape,
	Qt::DescendingOrder,
	QTransform());

    switch(getMode())
    {
      case(mode::freestyle):
	if (event->button() == Qt::LeftButton)
	{
	    foreach(QGraphicsItem * item, itemList)
	    {
		// TODO: should this not be && item != 0 ?
		if (item != nullptr || item != 0)  // Pointer is not null
		{
		    if (item->type() == Node::Type)
		    {
			if (node1 == nullptr)
			    node1 = qgraphicsitem_cast<Node*>(item);
			else if (node2 == nullptr)
			    node2 = qgraphicsitem_cast<Node*>(item);
		    }
		}
		// If the user selected two nodes make an edge.
		if (node1 != nullptr && node2 != nullptr && node1 != node2)
		{
		    addEdgeToScene(node1, node2);
		    // Set up the node variables so the user can add edges.
		    node1 = node2;
		    node2 = nullptr;
		    break;
		}
	    }
	}
	break;

      default:
	node1 = nullptr;
	node2 = nullptr;
	QGraphicsView::mousePressEvent(event);
    }
}


void CanvasView::snapToGrid(bool snap)
{
    aScene->isSnappedToGrid(snap);
    aScene->update();
}


void CanvasView::dragEnterEvent(QDragEnterEvent * event)
{
    emit resetNoMode();
    QGraphicsView::dragEnterEvent(event);
}

Edge * CanvasView::addEdgeToScene(Node * source, Node * destination)
{
    Edge * edge = createEdge(source, destination);
    if (node1->parentItem() == node2->parentItem())
    {
	// Both nodes are from the same parent item
        Graph * parent = qgraphicsitem_cast<Graph*>(node1->parentItem());
        edge->setParentItem(parent);
    }
    else
    {
	// Each node has a different parent
        /*
         * Create a graph that will be the parent of the two
         * graphs that each contain the nodes that the new
         * edge will be incident to
         */
        Graph * root = new Graph();
        Graph * parent1 = nullptr;
        Graph * parent2 = nullptr;

        parent1 = qgraphicsitem_cast<Graph*>(node1->parentItem());
        parent2 = qgraphicsitem_cast<Graph*>(node2->parentItem());

	// TODO: should this not be &&
        while (parent1->parentItem() != nullptr
               || parent1->parentItem() != 0)
            parent1 = qgraphicsitem_cast<Graph*>(parent1->parentItem());

	// TODO: should this not be &&
        while (parent2->parentItem() != nullptr
               || parent2->parentItem() != 0)
            parent2 = qgraphicsitem_cast<Graph*>(parent2->parentItem());

	// TODO: should these not be &&
        if ((parent2 != nullptr || parent2 != 0)
	    && (parent1 != nullptr || parent1 != 0))
        {
            edge->setZValue(0);
            edge->setParentItem(root);
            parent1->setParentItem(root);
            parent2->setParentItem(root);
            root->setHandlesChildEvents(false);
            aScene->addItem(root);
            aScene->addItem(edge);
            edge->adjust();
        }
    }
    return edge;
}


Edge * CanvasView::createEdge(Node * source, Node * destination)
{
    Edge * edge = new Edge(source, destination);
    edge->setPenWidth(edgeParams->size);
    edge->setColour(edgeParams->color);
    edge->setWeightLabelSize((edgeParams->LabelSize > 0)
			     ? edgeParams->LabelSize : 1);
    edge->setWeight(edgeParams->label);
    edge->setDestRadius(node2->getDiameter() / 2.);
    edge->setSourceRadius(node1->getDiameter() / 2.);
    return edge;
}


void CanvasView::setUpEdgeParams(qreal edgeSize, QString edgeLabel,
				 qreal edgeLabelSize, QColor edgeLineColor)
{
    if (verbose)
    {
        qDebug() << "edgeSize = " << edgeSize << endl;
        qDebug() << "edgeLabel = " << edgeLabel << endl;
        qDebug() << "edgeLabelSize =" << edgeLabelSize << endl;
    }
    edgeParams->size = edgeSize;
    edgeParams->label = edgeLabel;
    edgeParams->color = edgeLineColor;
    edgeParams->LabelSize = edgeLabelSize;
}


int CanvasView::getMode() const
{
    return modeType;
}

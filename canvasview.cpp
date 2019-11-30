/*
 * File:    canvasview.cpp
 * Author:  Rachel Bood
 * Date:    2014/11/07
 * Version: 1.7
 *
 * Purpose: Initializes a QGraphicsView that is used to house the
 *	    QGraphicsScene.
 *
 * Modification history:
 * Feb 8, 2016 (JD V1.2):
 *  (a) Change the help text for join (JOIN_DESCRIPTION) to clarify usage.
 *  (b) Drive-by cleanup.
 * Oct 11, 2019 (JD V1.3):
 *  (a) Further clarify JOIN_DESCRIPTION text.
 *  (b) Add the second sentence to EDIT_DESCRIPTION.
 *  (c) Improved a few debug messages.
 * Nov 13, 2019 (JD V1.4):
 *  (a) Rename setWeightLabelSize() to setLabelSize().
 * Nov 13, 2019 (JD V1.5)
 *  (a) Rename "Weight" to "Label" for edge function names.
 * Nov 29, 2019 (JD V1.6)
 *  (a) Introduce qDeb() macro to get better debug output control.
 *  (b) Rename "none" mode to "drag" mode, for less confusion.
 *  (c) Add a description string for drag mode;
 *	update freestyle mode description string.
 *  (d) Add some comments, formatting tweaks, ...
 *  (e) When in free style mode, a click on empty space or a double
 *	click takes the user out of "path drawing mode".
 *  (f) Commented out aScene->addItem(edge) for in addEdgeToScene()
 *      which was causing whinage from Qt.
 *  (g) Added getModeName() to help with debugging output.
 * Nov 30, 2019 (JD V1.7)
 *  (a) Added the #ifdef DEBUG stuff to set the debug variable.
 *  (b) Added a TODO note in addEdgeToScene().
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


// Debugging aids (without editing the source file):
#ifdef DEBUG
static const bool debug = true;
#else
static const bool debug = false;
#endif

// Like qDebug(), but a little more literal, and turn-offable:
#define qDeb if (debug) \
        QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE,  \
                       QT_MESSAGELOG_FUNC).debug().noquote().nospace

static const QString JOIN_DESCRIPTION =
    "Join mode: Select 1 or 2 nodes from each of 2 graph components "
    "and press 'J'.  "
    "The second component is moved and the second selected node is "
    "identified with the first.  "
    "If two other nodes were selected, they are also identified.";

static const QString DELETE_DESCRIPTION =
    "Delete mode: Click on any node or edge to be deleted.  "
    "Double Click on a graph to delete it entirely.";

static const QString EDIT_DESCRIPTION =
    "Edit mode: Drag individual nodes around within a graph drawing.  "
    "Click on a node to give it a label.";

static const QString FREESTYLE_DESCRIPTION =
    "Freestyle mode: Add nodes by double clicking at the desired location.  "
    "Add a path between existing nodes by clicking on two or more nodes "
    "consecutively.  "
    "To finish a path, click on empty space.";
    
static const QString DRAG_DESCRIPTION =
    "Drag mode: Hold the mouse down over any part of a graph and move "
    "the mouse to drag that graph around the canvas.";



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
    setMode(mode::drag);
    node1 = nullptr;
    node2 = nullptr;
}



void
CanvasView::setUpNodeParams(qreal nodeDiameter, bool numberedLabels,
                                 QString label, qreal nodeLabelSize,
                                 QColor nodeFillColour, QColor nodeOutLineColour)
{
    nodeParams->diameter = nodeDiameter;
    nodeParams->isNumbered = numberedLabels;
    nodeParams->label = label;
    nodeParams->labelSize = nodeLabelSize;
    nodeParams->fillColour = nodeFillColour;
    nodeParams->outlineColour = nodeOutLineColour;
}


Node *
CanvasView::createNode(QPointF pos)
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

void
CanvasView::keyPressEvent(QKeyEvent * event)
{
    QGraphicsView::keyPressEvent(event);
}



/*
 * Name:	setMode()
 * Purpose:	Set up for one of the different canvas "modes".
 * Arguments:	The new mode's index (see "enum mode" in the .h file).
 * Outputs:	Nothing.
 * Modifies:	The help text and the mode index.
 * Returns:	Nothing.
 * Assumptions:	?
 * Bugs:	?
 * Notes:	Also calls canvasscene's setCanvasMode() function.
 *		Ideally, the mode would be stored in one place, but
 *		at the moment, here it is in two places.  Bah!
 */

void
CanvasView::setMode(int m)
{
    qDeb() << "CV::setMode(" << m << ") called; "
	   << "previous mode was " << modeType
	   << " == " << getModeName(getMode());

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

      case mode::drag:
	emit setKeyStatusLabelText(DRAG_DESCRIPTION);
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
 *		Otherwise pass the event to QGraphicsView.
 * Arguments:   QMouseEvent
 * Output:      none
 * Modifies:    CanvasScene
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void
CanvasView::mouseDoubleClickEvent(QMouseEvent * event)
{
    qDeb() << "CV::mouseDoubleClickEvent("
	   << event->screenPos() << ")";

    QPointF pt;

    switch (getMode())
    {
      case mode::freestyle:
	pt = mapToScene(event->pos());
	createNode(pt);
	node1 = nullptr;
	node2 = nullptr;
	break;

      default:
        QGraphicsView::mouseDoubleClickEvent(event);
    }
}



void
CanvasView::mousePressEvent(QMouseEvent * event)
{
    qDeb() << "CV::mousePressEvent(" << event->screenPos() << ")"
	   << " mode is " << getModeName(getMode());

    QList<QGraphicsItem *> itemList = this->scene()->items(
	this->mapToScene(event->pos()),
	Qt::IntersectsItemShape,
	Qt::DescendingOrder,
	QTransform());

    bool clickedInEmptySpace = true;
    switch (getMode())
    {
      case (mode::freestyle):
	if (event->button() == Qt::LeftButton)
	{
	    qDeb() << "\tLeftButton pressed in freestyle mode";

	    foreach (QGraphicsItem * item, itemList)
	    {
		qDeb() << "\t\tlooking at item of type " << item->type();
		clickedInEmptySpace = false;
		if (item->type() == Node::Type)
		{
		    if (node1 == nullptr)
		    {
			qDeb() << "\t\tsetting node 1 !";
			node1 = qgraphicsitem_cast<Node*>(item);
			node2 = nullptr;	// Redundant?  Be safe!
		    }
		    else if (node2 == nullptr)
		    {
			qDeb() << "\t\tsetting node 2 !";
			node2 = qgraphicsitem_cast<Node*>(item);
		    }
		}

		// If the user selected two nodes make an edge.
		if (node1 != nullptr && node2 != nullptr && node1 != node2)
		{
		    qDeb() << "\t\tcalling addEdgeToScene(n1, n2) !";
		    addEdgeToScene(node1, node2);
		    // qDeb() << "node1->pos() is " << node1->pos();
		    // qDeb() << "node1->scenePos() is " << node1->scenePos();
		    // TODO: without this horrible hack, edges
		    // connecting two nodes of a library graph
		    // (dragged over from the preview pane) are drawn
		    // in the wrong place on the canvas.  Moving one
		    // node away and back causes the edge to be drawn
		    // in the correct place.  Q1: why?  Q2: what is
		    // the Right Thing to do here?
		    QPointF n1pos = node1->pos();
		    node1->setPos(n1pos + QPoint(10, 10));
		    node1->setPos(n1pos);
		    // qDeb() << "node1->pos() is " << node1->pos();
		    // qDeb() << "node1->scenePos() is " << node1->scenePos();

		    // Update vars so that another click on a node
		    // continues a path.
		    node1 = node2;
		    node2 = nullptr;
		    break;
		}
	    }
	}
	if (clickedInEmptySpace)
	{
	    qDeb() << "\t\tclicked in empty space, clearing node1 & 2";
	    node1 = node2 = nullptr;	    
	}
	break;

      default:
	node1 = nullptr;
	node2 = nullptr;
	QGraphicsView::mousePressEvent(event);
    }
}



void
CanvasView::snapToGrid(bool snap)
{
    aScene->isSnappedToGrid(snap);
    aScene->update();
}



void
CanvasView::dragEnterEvent(QDragEnterEvent * event)
{
    emit resetDragMode();
    QGraphicsView::dragEnterEvent(event);
}



Edge *
CanvasView::addEdgeToScene(Node * source, Node * destination)
{
    qDeb() << "CV::addEdgeToScene() called; "
	   << "source label is /" << source->getLabel()
	   << "/ dest label is /" << destination->getLabel() << "/";

    Edge * edge = createEdge(source, destination);
    if (node1->parentItem() == node2->parentItem())
    {
	// Both nodes are from the same parent item
	qDeb() << "\taETS: both nodes have the same parentItem";
        Graph * parent = qgraphicsitem_cast<Graph*>(node1->parentItem());
        edge->setParentItem(parent);
    }
    else
    {
	qDeb() << "\taETS: nodes have different parentItems";
        /*
	 * Each node has a different parent.
         * Create a graph that will be the parent of the two
         * graphs that each contain the nodes that the new
         * edge will be incident to.
	 * TODO: should we amalgamate the graphs rather than having a
	 * recursive structure?  Joining doesn't currently (Nov 2019)
	 * work properly when joining two "recursive" graphs.
         */
        Graph * root = new Graph();
        Graph * parent1 = nullptr;
        Graph * parent2 = nullptr;

        parent1 = qgraphicsitem_cast<Graph*>(node1->parentItem());
        parent2 = qgraphicsitem_cast<Graph*>(node2->parentItem());

        while (parent1->parentItem() != nullptr)
            parent1 = qgraphicsitem_cast<Graph*>(parent1->parentItem());

        while (parent2->parentItem() != nullptr)
            parent2 = qgraphicsitem_cast<Graph*>(parent2->parentItem());

        if (parent1 != nullptr && parent2 != nullptr)
        {
            edge->setZValue(0);
            edge->setParentItem(root);
            parent1->setParentItem(root);
            parent2->setParentItem(root);
            root->setHandlesChildEvents(false);
            aScene->addItem(root);
	    // The following line causes Qt to whine
	    // "QGraphicsScene::addItem: item has already been added
	    //		to this scene"
	    // so in V1.6 it was cautiously commented out.
            // aScene->addItem(edge);
            edge->adjust();
        }
    }
    qDeb() << "\taETS: done!";
    return edge;
}



Edge *
CanvasView::createEdge(Node * source, Node * destination)
{
    qDeb() << "CV::createEdge() called; calling 'new Edge()'";

    Edge * edge = new Edge(source, destination);
    edge->setPenWidth(edgeParams->size);
    edge->setColour(edgeParams->color);
    edge->setLabelSize((edgeParams->LabelSize > 0)
			     ? edgeParams->LabelSize : 1);
    edge->setLabel(edgeParams->label);
    edge->setDestRadius(node2->getDiameter() / 2.);
    edge->setSourceRadius(node1->getDiameter() / 2.);
    return edge;
}



void
CanvasView::setUpEdgeParams(qreal edgeSize, QString edgeLabel,
			    qreal edgeLabelSize, QColor edgeLineColour)
{
    qDeb() << "CV::setUpEdgeParams(): edgeSize = " << edgeSize;
    qDeb() << "CV::setUpEdgeParams(): edgeLabel = " << edgeLabel;
    qDeb() << "CV::setUpEdgeParams(): edgeLabelSize = " << edgeLabelSize;
    qDeb() << "CV::setUpEdgeParams(): edgeLineColour = " << edgeLineColour;

    edgeParams->size = edgeSize;
    edgeParams->label = edgeLabel;
    edgeParams->LabelSize = edgeLabelSize;
    edgeParams->color = edgeLineColour;
}



int
CanvasView::getMode() const
{
    return modeType;
}



QString
CanvasView::getModeName(int mode)
{
    // These MUST match the mode enum in canvasview.h.
    // TODO: This array should really be defined in the .h file, but
    // when I try to do that *and* declare this function static (so we
    // don't need an object reference, which we don't have in
    // canvasscene.cpp), g++ gets unhappy with me, regardless of the
    // 12 things I have tried.  Perhaps the 13th would have worked,
    // but I gave it up as a bad loss and put the string array in here.
    static QString modes[5] = {  
	    "drag", "join", "del", "edit", "freestyle"
    };

    if ((unsigned)mode > sizeof(modes) / sizeof(modes[0]))
	return "mode " + QString::number(mode) + " is UNKNOWN";
    return modes[mode];
}

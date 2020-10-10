/*
 * File:    canvasview.cpp
 * Author:  Rachel Bood
 * Date:    2014/11/07
 * Version: 1.23
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
 * Nov 30, 2019 (JD V1.8)
 *  (a) In freestyle mode, when creating a path, show the "current"
 *	node by drawing it with a dashed line (pen style 1).
 * Dec 4, 2019 (JD V1.9)
 *  (a) Add some comments to some functions.  Tweak formatting.
 * Dec 6, 2019 (JD V1.10)
 *  (a) Add some debug outputs.
 *  (b) Bug fix: createNode() now also sets the node label size.
 * Dec 8, 2019 (JD V1.11)
 *  (a) Remove node->edgeWeight, which is used nowhere.
 * Dec 13, 2019 (JD V1.12)
 *  (a) Added defuns.h, removed debug #defines.
 * Dec 17, 2019 (JD V1.13)
 *  (a) Add usage of Esc to edit mode help.
 * Jun 12, 2020 (IC V1.14)
 *  (a) Added measures to addEdgeToScene() to prevent edges being made between
 *      nodes that already have an edge.
 * Jun 17, 2020 (IC V1.15)
 *  (a) Updated setMode() to delete freestyle graph after a mode change if
 *      the freestyle graph is empty.
 * Jun 19, 2020 (IC V1.16)
 *  (a) Added nodeCreated() and edgeCreated() signals to tell mainWindow to
 *      update the edit tab.
 * Jun 25, 2020 (IC V1.17)
  * (a) Updated addEdgeToScene() to prevent additional root parents from being
  *     created if a root parent was previously created.
  * (b) Pass the created node or edge to the respective signal.
 * Jul 23, 2020 (IC V1.18)
 *  (a) Initialize modeType in the constructor(!).
 *  (b) Delete empty free-style graphs.
 *  (c) Don't allow freestyle mode to make edges from a node to itself.
 *  (d) Removed the graph recursion from addEdgeToScene.  Children of the two
 *      graphs are now reassigned to a singular graph and the old graph objects
 *      are deleted.
 * Jul 24, 2020 (IC V1.19)
 *  (a) Added clearCanvas() function that removes all items from the canvas.
 *  (b) Initialize modeType to 0 to prevent accidental deletion of nonexistant
 *      freestyleGraph on startup.
 *  (c) Don't allow freestyle to make two edges between a given node pair.
 * Aug 5, 2020 (IC V1.20)
 *  (a) Added nodeThickness to nodeParams, setUpNodeParams, and createNode.
 * Aug 11, 2020 (IC V1.21)
 *  (a) Added scaleView, wheelEvent, zoomIn, and zoomOut as well as updated
 *      keyPressEvent to allow for zooming on the canvas, similar to the zoom
 *      from preview.cpp, using either a key press or mouse wheel scroll.
 * Aug 12, 2020 (IC V1.22)
 *  (a) Created macros to be used for zoom level min and max for clarity.
 *  (b) Delete the help text from here; similar text is now in mainwindow.ui.
 *  (c) Remove some old commented code.
 * Aug 21, 2020 (IC V1.23)
 *  (a) Update the names setLabelSize() -> setEdgeLabelSize() and
 *       setLabel() -> setEdgeLabel().
 *  (b) Add code to allow edges to be sequentially numbered, a la nodes.
 */

#include "canvasview.h"
#include "defuns.h"
#include "edge.h"
#include "graph.h"
#include "node.h"

#include <math.h>
#include <QKeyEvent>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>

// This is the factor by which the canvas is zoomed for each
// zoom in or zoom out operation.
#define SCALE_FACTOR    1.1
static qreal zoomValue = 100;
static QString zoomDisplayText = "Zoom: " + QString::number(zoomValue) + "%";

// Empirically chosen values, modify as you see fit:
#define MIN_ZOOM_LEVEL  0.07
#define MAX_ZOOM_LEVEL  10.0



/*
 * Name:        Canvas
 * Purpose:     Contructor for Canvas class.
 * Arguments:   QWidget *
 * Output:      Nothing.
 * Modifies:    canvas
 * Returns:     Nothing.
 * Assumptions: ?
 * Bugs:        ?
 * Notes:       None.
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
    node1 = nullptr;
    node2 = nullptr;
    modeType = 0; // Can randomly be 4 at startup and cause crash, so fix it!
    setMode(mode::drag);     // This must be after 'node1 = nullptr;' !
}



/*
 * Name:	setUpNodeParams()
 * Purpose:	Store the node drawing parameters (as defined by the
 *		"Create Graph" tab) in a struct.
 * Arguments:	None.
 * Outputs:	Nothing.
 * Modifies:	The nodeParams struct.
 * Returns:	Nothing.
 * Assumptions:	?
 * Bugs:	?
 * Notes:	This is called from MW::generate_Freestyle_Nodes(), which
 *		itself is connected to UI elements which change node
 *		drawing parameters.
 */

void
CanvasView::setUpNodeParams(qreal nodeDiameter, bool numberedLabels,
			    QString label, qreal nodeLabelSize,
			    QColor nodeFillColour, QColor nodeOutLineColour,
			    qreal nodeThickness)
{
    qDeb() << "CV::setUpNodeParams(): nodeDiameter = " << nodeDiameter;
    qDeb() << "CV::setUpNodeParams(): nodeLabelsNumbered = " << numberedLabels;
    qDeb() << "CV::setUpNodeParams(): nodeLabel = /" << label << "/";
    qDeb() << "CV::setUpNodeParams(): nodeLabelSize = " << nodeLabelSize;
    qDeb() << "CV::setUpNodeParams(): nodeOutLineColour = " << nodeOutLineColour;
    qDeb() << "CV::setUpNodeParams(): nodeFillColour = " << nodeFillColour;
    qDeb() << "CV::setUpNodeParams(): nodeThickness = " << nodeThickness;

    nodeParams->diameter = nodeDiameter;
    nodeParams->isNumbered = numberedLabels;
    nodeParams->label = label;
    nodeParams->labelSize = nodeLabelSize;
    nodeParams->fillColour = nodeFillColour;
    nodeParams->outlineColour = nodeOutLineColour;
    nodeParams->nodeThickness = nodeThickness;
}



Node *
CanvasView::createNode(QPointF pos)
{
    Node * node = new Node();
    node->setDiameter(nodeParams->diameter);
    node->setPenWidth(nodeParams->nodeThickness);
    node->setNodeLabelSize(nodeParams->labelSize);
    node->setRotation(0);
    node->setFillColour(nodeParams->fillColour);
    node->setLineColour(nodeParams->outlineColour);
    node->setPos(pos.rx(), pos.ry());
    node->setParentItem(freestyleGraph);
    return node;
}



/*
 * Name:        keyPressEvent
 * Purpose:     Perform the appropriate action for known key presses.
 * Arguments:   QKeyEvent
 * Output:      Nothing.
 * Modifies:    The scale of the canvas window for the zoom operations.
 * Returns:     Nothing.
 * Assumptions: ?
 * Bugs:        ?
 * Notes:       Unhandled key events are passed on to QGraphicsView.
 */

void
CanvasView::keyPressEvent(QKeyEvent * event)
{
    qDeb() << "CV:keyPressEvent(" << event->key() << ") called.";

    if (event->modifiers().testFlag(Qt::ControlModifier))
    {
        switch (event->key())
        {
          case Qt::Key_Equal:
            zoomIn();
            break;
          case Qt::Key_Minus:
            zoomOut();
            break;
          default:
            QGraphicsView::keyPressEvent(event);
        }
    }
    else
        QGraphicsView::keyPressEvent(event);
}



/*
 * Name:        wheelEvent
 * Purpose:     Perform the appropriate action for wheel scroll.
 * Arguments:   QWheelEvent
 * Output:      Nothing.
 * Modifies:    The scale of the canvas window for the zoom operations.
 * Returns:     Nothing.
 * Assumptions: ?
 * Bugs:        ?
 * Notes:       Unhandled key events are passed on to QGraphicsView
 */

void
CanvasView::wheelEvent(QWheelEvent * event)
{
    qDeb() << "PV:wheelEvent(" << event->angleDelta() << ") called.";

    if (event->modifiers().testFlag(Qt::ControlModifier))
    {
        if (event->angleDelta().y() > 0)
            zoomIn();
        else if (event->angleDelta().y() < 0)
            zoomOut();
        else
            QGraphicsView::wheelEvent(event);
    }
    else
        QGraphicsView::wheelEvent(event);
}



/*
 * Name:        scaleView()
 * Purpose:     Scales the view of the QGraphicsScene
 * Arguments:   A qreal
 * Output:      Nothing.
 * Modifies:    The scale view of the QGraphicsScene
 * Returns:     Nothing.
 * Assumptions: None.
 * Bugs:        ?
 * Notes:       None.
 */

void
CanvasView::scaleView(qreal scaleFactor)
{
    qDeb() << "CV::scaleView(" << scaleFactor << ") called";

    qreal factor = transform().scale(scaleFactor, scaleFactor)
                .mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < MIN_ZOOM_LEVEL || factor > MAX_ZOOM_LEVEL)
        return;
    scale(scaleFactor, scaleFactor);

    // Determine how displayed zoom value needs to update
    qreal afterFactor = transform().scale(scaleFactor, scaleFactor)
            .mapRect(QRectF(0, 0, 1, 1)).width();
    if (afterFactor > factor)
        zoomValue = zoomValue * SCALE_FACTOR;
    else
        zoomValue = zoomValue / SCALE_FACTOR;

    // Update and show the current zoom.
    zoomDisplayText = "Zoom: " + QString::number(zoomValue, 'f', 0) + "%";
    emit zoomChanged(zoomDisplayText);
}



void
CanvasView::zoomIn()
{
    scaleView(qreal(SCALE_FACTOR));
}



void
CanvasView::zoomOut()
{
    scaleView(1. / qreal(SCALE_FACTOR));
}



/*
 * Name:	setMode()
 * Purpose:	Set up for one of the different canvas "modes".
 * Arguments:	The new mode's index (see "enum mode" in the .h file).
 * Outputs:	Nothing.
 * Modifies:	The help text and the mode index.
 * Returns:	Nothing.
 * Assumptions:	node1 and modeType have been initialized to meaningful values.
 * Bugs:	?
 * Notes:	Also calls canvasscene's setCanvasMode() function.
 *		Ideally, the mode would be stored in one place, but
 *		at the moment, here it is in two places.  Bah!
 */

void
CanvasView::setMode(int m)
{
    int lastModeType = modeType;

    qDeb() << "CV::setMode(" << m << ") called; "
	   << "previous mode was " << modeType
	   << " == " << getModeName(getMode());

    if (modeType == m)
    {
	qDeb() << "\tSame mode as before, returning.";
	return;
    }

    if (lastModeType == mode::freestyle) // Deletes empty freestyle graph
    {
	if (freestyleGraph  != nullptr)
	{
	    if (freestyleGraph->childItems().isEmpty())
	    {
		aScene->removeItem(freestyleGraph);
		delete freestyleGraph;
	    }
	}
    }

    if (node1 != nullptr)
	node1->chosen(0);

    modeType = m;
    freestyleGraph = nullptr;

    if (modeType == mode::freestyle)
    {
	freestyleGraph = new Graph();
	aScene->addItem(freestyleGraph);
	freestyleGraph->isMoved();
	node1 = nullptr;
	node2 = nullptr;
    }
    aScene->setCanvasMode(m);
}



/*
 * Name:        MouseDoubleClickEvent
 * Purpose:     Generates a new node in the CanvasScene if in freestyle mode
 *              when use doubleclicks the mouse.
 *		Otherwise pass the event to QGraphicsView.
 * Arguments:   QMouseEvent
 * Output:      Nothing.
 * Modifies:    CanvasScene
 * Returns:     Nothing.
 * Assumptions: ?
 * Bugs:        ?
 * Notes:       None.
 */

void
CanvasView::mouseDoubleClickEvent(QMouseEvent * event)
{
    qDeb() << "CV::mouseDoubleClickEvent("
	   << event->screenPos() << ") in mode " << getModeName(getMode());

    QPointF pt;

    switch (getMode())
    {
      case mode::freestyle:
	pt = mapToScene(event->pos());
	qDeb() << "\tfreestyle mode: create a new node at " << pt;
	createNode(pt);
	emit nodeCreated();
	if (node1 != nullptr)
	    node1->chosen(0);
	node1 = nullptr;
	node2 = nullptr;
	break;

      default:
	qDeb() << "\tdefault: call QGV:mouseDoubleClickEvent()";
	QGraphicsView::mouseDoubleClickEvent(event);
    }
}



void
CanvasView::mousePressEvent(QMouseEvent * event) // Should this be in CS?
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
		if (item->type() == Node::Type)
		{
		    clickedInEmptySpace = false;
		    if (node1 == nullptr)
		    {
			qDeb() << "\t\tsetting node 1 !";
			node1 = qgraphicsitem_cast<Node*>(item);
			node1->chosen(1);
			node2 = nullptr;	// Redundant?  Be safe!
		    }
		    else if (node2 == nullptr)
		    {
			qDeb() << "\t\tsetting node 2 !";
			if (item != node1)
			    node2 = qgraphicsitem_cast<Node*>(item);
		    }
		}

		// If the user selected two nodes make an edge.
		if (node1 != nullptr && node2 != nullptr && node1 != node2)
		{
		    // Prevent edges being made if one already exists
		    // between source and dest.
		    int exists = 0;
		    for (int i = 0; i < node1->edgeList.count(); i++)
			if ((node1->edgeList.at(i)->sourceNode() == node1
			     && node1->edgeList.at(i)->destNode() == node2)
			     || (node1->edgeList.at(i)->sourceNode() == node2
			     && node1->edgeList.at(i)->destNode() == node1))
			    exists = 1;

		    if (exists == 0)
		    {
			qDeb() << "\t\tcalling addEdgeToScene(n1, n2) !";
			addEdgeToScene(node1, node2);
			emit edgeCreated();
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
		    }
		    // Update vars so that another click on a node
		    // continues a path.
		    node1->chosen(0);
		    node2->chosen(1);
		    node1 = node2;
		    node2 = nullptr;
		    break;
		}
	    }
	}
	if (clickedInEmptySpace)
	{
	    qDeb() << "\t\tclicked in empty space, clearing node1 & 2";
	    if (node1 != nullptr)
		node1->chosen(0);
	    node1 = nullptr;
	    node2 = nullptr;
	}
	break;

      default:
	if (node1 != nullptr)
	    node1->chosen(0);
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
	 * Create a graph that will contain the children of the old
	 * graphs as well as the new edge being made.
	 */

	Graph * root = new Graph();
	Graph * parent1 = nullptr;
	Graph * parent2 = nullptr;
	QPointF itemPos;

	parent1 = qgraphicsitem_cast<Graph*>(node1->parentItem());
	parent2 = qgraphicsitem_cast<Graph*>(node2->parentItem());

	foreach (QGraphicsItem * item, parent1->childItems())
	{
	    itemPos = item->scenePos();
	    item->setParentItem(root);
	    item->setPos(itemPos);
	    item->setRotation(0);
	}
	foreach (QGraphicsItem * item, parent2->childItems())
	{
	    itemPos = item->scenePos();
	    item->setParentItem(root);
	    item->setPos(itemPos);
	    item->setRotation(0);
	}
	edge->setZValue(-1);
	edge->setParentItem(root);
	root->setHandlesChildEvents(false);
	aScene->addItem(root);

	edge->adjust();

	if (parent1 == freestyleGraph || parent2 == freestyleGraph)
	{
	    freestyleGraph = new Graph;
	    aScene->addItem(freestyleGraph);
	}

	aScene->removeItem(parent1);
	delete parent1;
	aScene->removeItem(parent2);
	delete parent2;

	edge->causedConnect = 1;
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
    edge->setEdgeLabelSize((edgeParams->LabelSize > 0)
			     ? edgeParams->LabelSize : 1);
    edge->setEdgeLabel(edgeParams->label);
    edge->setDestRadius(node2->getDiameter() / 2.);
    edge->setSourceRadius(node1->getDiameter() / 2.);
    return edge;
}



/*
 * Name:	setUpEdgeParams()
 * Purpose:	Store the edge drawing parameters (as defined by the
 *		"Create Graph" tab) in a struct.
 * Arguments:	None.
 * Outputs:	Nothing.
 * Modifies:	The edgeParams struct.
 * Returns:	Nothing.
 * Assumptions:	?
 * Bugs:	?
 * Notes:	This is called from MW::generate_Freestyle_Edges(), which
 *		itself is connected to UI elements which change edge
 *		drawing parameters.
 */

void
CanvasView::setUpEdgeParams(qreal edgeSize, QString edgeLabel,
			    qreal edgeLabelSize, QColor edgeLineColour,
			    bool numberedLabels)
{
    qDeb() << "CV::setUpEdgeParams(): edgeSize = " << edgeSize;
    qDeb() << "CV::setUpEdgeParams(): edgeLabel = /" << edgeLabel << "/";
    qDeb() << "CV::setUpEdgeParams(): edgeLabelSize = " << edgeLabelSize;
    qDeb() << "CV::setUpEdgeParams(): edgeLineColour = " << edgeLineColour;
    qDeb() << "CV::setUpEdgeParams(): edgeLabelsNumbered = " << numberedLabels;

    edgeParams->size = edgeSize;
    edgeParams->label = edgeLabel;
    edgeParams->LabelSize = edgeLabelSize;
    edgeParams->color = edgeLineColour;
    edgeParams->isNumbered = numberedLabels;
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



/*
 * Name:	clearCanvas()
 * Purpose:	Removes all items from the canvas.
 * Arguments:	None.
 * Outputs:	Nothing.
 * Modifies:	The canvas.
 * Returns:	Nothing.
 * Assumptions:	Something is on the canvas?
 * Bugs:	?
 * Notes:	None.
 */

void
CanvasView::clearCanvas()
{
    QList<Graph *> graphList;
    foreach (QGraphicsItem * item, aScene->items())
    {
	if (item != nullptr || item != 0)
	{
	    if (item->type() == Graph::Type)
	    {
		Graph * graph = qgraphicsitem_cast<Graph *>(item);
		graphList.append(graph);
	    }
	}
    }
    for (Graph * graph: graphList)
    {
	aScene->removeItem(graph);
	delete graph;
	graph = nullptr;
    }

    if (getMode() == CanvasView::freestyle)
    {
	freestyleGraph = new Graph;
	aScene->addItem(freestyleGraph);
    }
}

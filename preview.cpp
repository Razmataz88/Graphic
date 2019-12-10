/*
 * File:    preview.cpp
 * Author:  Rachel Bood 100088769
 * Date:    2014/11/07
 * Version: 1.5
 *
 * Purpose: Initializes a QGraphicsView that is used to house the QGraphicsScene
 *
 * Modification history:
 * Nov 13, 2019 (JD, V1.1):
 *  (a) Rename setWeightLabelSize() to setLabelSize().
 * Nov 13, 2019 (JD, V1.2)
 *  (a) Rename "Weight" to "Label" for edge function names.
 * Nov 13, 2019 (JD, V1.3)
 *  (a) Some formatting and comment tweaks.
 *  (b) Added Ctrl-= as a way to zoom in the preview pane.
 * Dec 1, 2019 (JD, V1.4)
 *  (a) Added qDeb() / DEBUG stuff.
 *  (b) Set edges' source radius correctly (fwiw) in Style_Graph().
 *  (c) Added some debug outputs and some comment additions/mods.
 *  (d) In Style_Graph() call setNodeLabelSize() regardless of
 *	whether or not the node is getting a label.  Simplify the
 *	related code in that part of the function.
 *  (e) Also in Style_Graph() call edge->setSourceRadius() with a value
 *	which seems to be more sensible to me.
 * Dec 7, 2019 (JD V1.5)
 *  (a) Use the currentGraphTypeIndex field in Create_Graph() to
 *	decide whether we have to create a new graph or just modify
 *	the one we already have.
 *  (b) Remove node->edgeWeight, which is used nowhere.
 *  (c) Redesign code so that when a graph is created it is designed
 *	for a 1"x1" region, and only when it is styled are the node
 *	positions set as per the size widgets.  This is a step toward
 *	allowing library graphs to be scaled (and styled in other ways).
 *  (c) With that, redesign code so that a graph is not re-created
 *	every time a style parameter is changed.  (That is, create a
 *	new graph only when the graph type, number of nodes, or
 *	whether to draw edges changes.)
 *  (d) Yet more comment additions and formatting changes.
 *      In particular rename "complete" to "drawEdges", since
 *	this checkBox refers to a different notion than that of a
 *	complete graph.
 *  (e) Obsessively alphabetize the cases in Create_Graph().
 */

#include "preview.h"
#include "basicgraphs.h"
#include "edge.h"
#include "node.h"
#include "graph.h"
#include "graphmimedata.h"

#include <math.h>
#include <QKeyEvent>
#include <QDrag>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QMessageBox>
#include <QFileDialog>
#include <QShortcut>
#include <QtGui>
#include <qmath.h>


// Debugging aids (without editing the source file):
#ifdef DEBUG
static const bool debug = true;
#define qDeb() qDebug().nospace().noquote() << fixed \
    << qSetRealNumberPrecision(4)
#else
static const bool debug = false;
#define qDeb() if (false) qDebug()
#endif


// This is the factor by which the preview pane is zoomed for each
// zoom in or zoom out operation.
#define SCALE_FACTOR    1.2


/*
 * Name:        PreView
 * Purpose:     Contructor for PreView class
 * Arguments:   QWidget *
 * Output:      none
 * Modifies:    none
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

PreView::PreView(QWidget * parent)
    : QGraphicsView(parent)
{
    aScene = new QGraphicsScene();
    aScene->setSceneRect(0, 0, this->width(), this->height());

    currentGraphTypeIndex = -1;	    // I.e., there is no preview right now.

    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate); // Updates the canvas
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setScene(aScene);
}



/*
 * Name:        keyPressEvent
 * Purpose:     Perform the appropriate action for known key presses.
 * Arguments:   QKeyEvent
 * Output:      Nothing.
 * Modifies:    The scale of the preview window for the zoom operations.
 * Returns:     Nothing.
 * Assumptions: ?
 * Bugs:        ?
 * Notes:       Unhandled key events are passed on to QGraphicsView.
 */

void
PreView::keyPressEvent(QKeyEvent * event)
{
    qDeb() << "PV:keyPressEvent(" << event->key() << ") called.";

    switch (event->key())
    {
      case Qt::Key_Plus:
      case Qt::Key_Equal:
        zoomIn();
        break;
      case Qt::Key_Minus:
        zoomOut();
        break;
      case Qt::Key_Delete:
	// TODO: why is this here??
        break;
      default:
        QGraphicsView::keyPressEvent(event);
    }
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
PreView::scaleView(qreal scaleFactor)
{
    qDeb() << "PV::scaleView(" << scaleFactor << ") called";

    qreal factor = transform().scale(scaleFactor, scaleFactor)
                .mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;
    scale(scaleFactor, scaleFactor);
}



/*
 * Name:	mousePressEvent()
 * Purpose:	Handle the dragging of the preview graph to the main canvas.
 * Arguments:	A mouse event.
 * Outputs:	Nothing.
 * Modifies:	The canvas and the preview window.
 * Returns:	Nothing.
 * Assumptions:	?
 * Bugs:	?
 * Notes:	Passes the mouse press on to QGraphicsView() after it is done.
 */

void
PreView::mousePressEvent(QMouseEvent * event)
{
    qDeb() << "PV::mousePressEvent(" << event << ") called";

    if (event->button() == Qt::LeftButton)
    {
        QList<QGraphicsItem *> itemList = this->scene()->items(
	    this->mapToScene(event->pos()),
	    Qt::IntersectsItemShape,
	    Qt::DescendingOrder, QTransform());

        foreach (QGraphicsItem * item, itemList)
        {
	    qDeb() << "\tlooking at a graphics item of type " << item->type();
            if (item->type() == Graph::Type)
            {
                Graph * graph = qgraphicsitem_cast<Graph *>(item);
		GraphMimeData * data = new GraphMimeData(graph);
		QDrag * drag = new QDrag(this->scene());
		drag->setMimeData(data);

		QPixmap * image = new QPixmap(
		    graph->childrenBoundingRect().size().toSize());
		image->fill(Qt::white);
		QPainter painter(image);
		painter.setRenderHints(QPainter::Antialiasing |
				       QPainter::HighQualityAntialiasing |
				       QPainter::NonCosmeticDefaultPen,
				       true);
		scene()->render(&painter,
				QRectF(0, 0,
				       scene()->itemsBoundingRect().width(),
				       scene()->itemsBoundingRect().height()),
				scene()->itemsBoundingRect(),
				Qt::IgnoreAspectRatio);
		painter.end();

		drag->setPixmap(*image);
		drag->exec();
		break;
            }
        }
    }
    QGraphicsView::mousePressEvent(event);
}



/*
 * Name:        zoomIn()
 * Purpose:     Zoom in the preview pane.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    The scale of the preview QGraphicsScene.
 * Returns:     Nothing.
 * Assumptions: None.
 * Bugs:        None.
 * Notes:       None.
 */

void
PreView::zoomIn()
{
    scaleView(qreal(SCALE_FACTOR));
}



/*
 * Name:        zoomOut()
 * Purpose:     Zoom out the preview pane.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    The scale of the preview QGraphicsScene.
 * Returns:     Nothing.
 * Assumptions: None.
 * Bugs:        None.
 * Notes:       None.
 */

void
PreView::zoomOut()
{
    scaleView(1. / qreal(SCALE_FACTOR));
}



/*
 * Name:	Create_Graph
 * Purpose:	Create a "basic graph" and add it to the preview scene.
 * Arguments:	The graph type index, the node count(s), and a flag
 *		indicating whether edges should be added to the graph
 *		(otherwise just the nodes are drawn).
 * Outputs:	Nothing.
 * Modifies:	The preview scene, erasing any previous graph there.
 * Returns:	Nothing.
 * Assumptions:	?
 * Bugs:	?
 * Notes:	Creates a graph where the centers of the nodes occupy
 *		a 1" by 1" square (or a 1" line in the case of
 *		1-dimensional graphs such as paths).  However, the
 *		width and height stored in the graph struct have been
 *		converted into PIXELS.  When the graph is subsequently
 *		styled the current node diameter must be taken into
 *		account when positioning the nodes.
 */

void
PreView::Create_Graph(int graphType, int numOfNodes1, int numOfNodes2,
		      bool drawEdges)
{
    qDeb() << "PV:Create_Graph(): the preview scene has "
	   << this->scene()->items().size() << " items";

    if (this->scene()->items().size() != 0
	&& currentGraphTypeIndex == graphType
	&& currentTopNodes == numOfNodes1
	&& currentBottomNodes == numOfNodes2
	&& currentDrawEdges == drawEdges)
    {
	// No change in the graph type or node/edge structure from
	// last time.  Thus we can return and let the styling change
	// the appearance of the graph.
	qDeb() << "PV::Create_Graph(): same graph params as before; returning.";
	return;
    }

    qDeb() << "PV::Create_Graph(): diff graph params; creating a new graph.";
    currentGraphTypeIndex = graphType;
    currentTopNodes = numOfNodes1;
    currentBottomNodes = numOfNodes2;
    currentDrawEdges = drawEdges;

    // We are making a new graph, away with the old one.
    this->scene()->clear();

    Graph * graphItem = new Graph();
    BasicGraphs * simpleG = new BasicGraphs();

    switch (graphType)
    {
      case BasicGraphs::Antiprism:
        simpleG->generate_antiprism(graphItem, numOfNodes1, drawEdges);
        break;

      case BasicGraphs::BBTree:
        simpleG->generate_balanced_binary_tree(graphItem, numOfNodes1,
					       drawEdges);
        break;

      case BasicGraphs::Bipartite:
        simpleG->generate_bipartite(graphItem, numOfNodes1, numOfNodes2,
				    drawEdges);
        break;

      case BasicGraphs::Complete:
        simpleG->generate_complete(graphItem, numOfNodes1, drawEdges);
        break;

      case BasicGraphs::Crown:
        simpleG->generate_crown(graphItem, numOfNodes1, drawEdges);
        break;

      case BasicGraphs::Cycle:
        simpleG->generate_cycle(graphItem, numOfNodes1, drawEdges);
        break;

      case BasicGraphs::Dutch_Windmill:
        simpleG->generate_dutch_windmill(graphItem, numOfNodes1, numOfNodes2,
					 drawEdges);
        break;

      case BasicGraphs::Gear:
	simpleG->generate_gear(graphItem, numOfNodes1, drawEdges);
        break;

      case BasicGraphs::Grid:
        simpleG->generate_grid(graphItem, numOfNodes1, numOfNodes2, drawEdges);
        break;

      case BasicGraphs::Helm:
        simpleG->generate_helm(graphItem, numOfNodes1, drawEdges);
        break;

      case BasicGraphs::Path:
        simpleG->generate_path(graphItem, numOfNodes1, drawEdges);
        break;

      case BasicGraphs::Petersen:
        simpleG->generate_petersen(graphItem, numOfNodes1, numOfNodes2,
				   drawEdges);
        break;

      case BasicGraphs::Prism:
        simpleG->generate_prism(graphItem, numOfNodes1, drawEdges);
        break;

      case BasicGraphs::Star:
        simpleG->generate_star(graphItem, numOfNodes1, drawEdges);
        break;

      case BasicGraphs::Wheel:
        simpleG->generate_wheel(graphItem, numOfNodes1, drawEdges);
        break;

      default:
        break;
    }

    this->scene()->addItem(graphItem);
}



/*
 * Name:	Style_Graph()
 * Purpose:	Given a previously-created graph, apply the given
 *		drawing parameters to get the desired drawing.
 * Arguments:	The graph object and all the drawing info.
 *		Note that the width and height arguments specify the
 *		bounding box of the graph drawing (in inches), so the
 *		node diameters must be taken into account when
 *		positioning node centers.
 * Outputs:	Nothing.
 * Modifies:	The drawing of the graph.
 * Returns:	Nothing.
 * Assumptions:	?
 * Bugs:	?
 * Notes:	Each node of the graph has previewX and previewY
 *		values which locate the node in a 1"*1" region,
 *		centered on (0, 0).  When styling the graph, we use
 *		this information, the width and height, and the DPI
 *		information to scale the node location accordingly.
 */

void
PreView::Style_Graph(Graph * graph, int graphType, qreal nodeDiameter,
		     QString topNodeLabels, QString bottomNodeLabels,
		     bool labelsAreNumbered, qreal nodeLabelSize,
		     QColor nodeFillColor, QColor nodeOutlineColor,
		     qreal edgeSize, QString edgeLabel, qreal edgeLabelSize,
		     QColor edgeLineColor,
		     qreal totalWidth, qreal totalHeight, qreal rotation)
{
    qDeb() << "PV::Style_Graph() called.";

    int i = 0, j = 0;

    QScreen * screen = QGuiApplication::primaryScreen();
    qreal xDPI = screen->logicalDotsPerInchX();
    qreal yDPI = screen->logicalDotsPerInchY();

    // The w & h args are *total* w & h for the graph, but we need to
    // locate the center of the nodes.  So first calculate the
    // nodecenter-to-nodecenter dimensions, then calculate the scale
    // factors, and finally factor in the inch->screen mapping; this
    // will be used to set the position of the nodes.
    qreal centerWidth = totalWidth - nodeDiameter;
    if (centerWidth < 0.1)
	centerWidth = 0.1;
    qreal widthScaleFactor = centerWidth * xDPI;
    qreal centerHeight = totalHeight - nodeDiameter;
    if (centerHeight < 0.1)
	centerHeight = 0.1;
    qreal heightScaleFactor = centerHeight * yDPI;

    qDeb() << "Desired total width: " << totalWidth
	   << "; desired center width " << centerWidth
	   << "\n\twidthScaleFactor: " << widthScaleFactor;
    qDeb() << "Desired total height: " << totalHeight
	   << "; desired center height " << centerHeight
	   << "\n\theightScaleFactor: " << heightScaleFactor;

    foreach (QGraphicsItem * item, graph->childItems())
    {
        if (item->type() == Node::Type)
        {
	    Node * node = qgraphicsitem_cast<Node *>(item);
	    node->setParentItem(nullptr);	    // ?? Eh?
	    node->setDiameter(nodeDiameter);
	    node->setFillColour(nodeFillColor);
	    node->setLineColour(nodeOutlineColor);
	    node->setNodeLabelSize(nodeLabelSize);
	    node->setPos(node->getPreviewX() * widthScaleFactor,
			 node->getPreviewY() * heightScaleFactor);

	    // Clear the node label, in case it was set in a previous styling.
	    node->setNodeLabel("");
	    if (labelsAreNumbered)
		node->setNodeLabel(i++);
	    else if (graphType == BasicGraphs::Bipartite)
	    {
		// Special case for labeling bipartite graphs.
		if (bottomNodeLabels.length() != 0
		    && graph->nodes.bipartite_bottom.contains(node))
		    node->setNodeLabel(bottomNodeLabels, j++);
		else if (topNodeLabels.length() != 0
			 && graph->nodes.bipartite_top.contains(node))
                    node->setNodeLabel(topNodeLabels, i++);
		else if (topNodeLabels.length() != 0
			 && graph->nodes.bipartite_bottom.contains(node))
		    node->setNodeLabel(topNodeLabels, i++);
	    }
	    else if (topNodeLabels.length() != 0)
		node->setNodeLabel(topNodeLabels, i++);

	    qDeb() << "PV::Style_Graph(): node with label /" << node->getLabel()
		   << "/ has preview coords (" << node->getPreviewX()
		   << ", " << node->getPreviewY() << ")";
	    node->setParentItem(graph);
        }
        else if (item->type() == Edge::Type)
        {
	    Edge * edge = qgraphicsitem_cast<Edge *>(item);
	    edge->setParentItem(nullptr);	// ?? Eh?
	    edge->setPenWidth(edgeSize);
	    edge->setColour(edgeLineColor);
	    edge->setLabelSize((edgeLabelSize > 0) ? edgeLabelSize : 1);
	    if (edgeLabel.length() != 0)
                edge->setLabel(edgeLabel);
	    else
		edge->setLabel("");	// Clear any old label
	    edge->setDestRadius(nodeDiameter / 2.);
	    // Q: why did RB do this?  It gives a bizarre value.
	    // edge->setSourceRadius(edge->sourceNode()->getDiameter() / 2.);
	    edge->setSourceRadius(nodeDiameter / 2.);
	    edge->setParentItem(graph);
        }
    }
    graph->setPos(mapToScene(viewport()->rect().center()));
    graph->setRotation(-1 * rotation);
}

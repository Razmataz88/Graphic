/*
 * File:    preview.cpp
 * Author:  Rachel Bood 100088769
 * Date:    2014/11/07
 * Version: 1.4
 *
 * Purpose: Initializes a QGraphicsView that is used to house the QGraphicsScene
 *
 * Modification history:
 * Nov 13, 2019 (JD, V1.1):
 *  - Rename setWeightLabelSize() to setLabelSize().
 * Nov 13, 2019 (JD, V1.2)
 *   - Rename "Weight" to "Label" for edge function names.
 * Nov 13, 2019 (JD, V1.3)
 *   (a) Some formatting and comment tweaks.
 *   (b) Added Ctrl-= as a way to zoom in the preview pane.
 * Dec 1, 2019 (JD, V1.4)
 *  (a) Added qDeb() / DEBUG stuff.
 *  (b) Set edges' source radius correctly (fwiw) in Style_Graph().
 *  (c) Added some debug outputs and some comment additions/mods.
 *  (d) In Style_Graph() call setNodeLabelSize() regardless of
 *	whether or not the node is getting a label.  Simplify the
 *	related code in that part of the function.
 *  (e) Also in Style_Graph() call edge->setSourceRadius() with a value
 *	which seems to be more sensible to me.
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
#include <qmath.h>

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



Graph *
PreView::Create_Graph(int graph, int topNodes, int bottomNodes,
		      qreal height, qreal width, bool complete)
{
    Graph * graphItem = new Graph();
    BasicGraphs * simpleG = new BasicGraphs();

    switch (graph)
    {
      case BasicGraphs::Bipartite:
        simpleG->generate_bipartite(graphItem, topNodes, bottomNodes,
                                    height, width, complete);
        break;

      case BasicGraphs::Complete:
        simpleG->generate_complete(graphItem, width / 2, height / 2, topNodes,
                                   complete);
        break;

      case BasicGraphs::Cycle:
        simpleG->generate_cycle(graphItem, width / 2, height / 2, topNodes,
                                complete);
        break;
      case BasicGraphs::Star:
        simpleG->generate_star(graphItem, width / 2, height / 2, topNodes,
                               complete);
        break;

      case BasicGraphs::Wheel:
        simpleG->generate_wheel(graphItem, width / 2, height / 2, topNodes,
                                complete);
        break;

      case BasicGraphs::Petersen:
        simpleG->generate_petersen(graphItem, width / 2, height / 2,
				   topNodes, bottomNodes, complete);
        break;

      case BasicGraphs::BBTree:
        simpleG->generate_balanced_binary_tree(graphItem, height, width,
                                               topNodes, complete);
        break;

      case BasicGraphs::Crown:
        simpleG->generate_crown(graphItem, width / 2, height / 2, topNodes,
                                complete);
        break;

      case BasicGraphs::Windmill:
        simpleG->generate_dutch_windmill(graphItem, height, topNodes,
                                         bottomNodes, complete);
        break;

      case BasicGraphs::Gear:
	simpleG->generate_gear(graphItem, width / 2, height / 2, topNodes,
			       complete);
        break;

      case BasicGraphs::Grid:
        simpleG->generate_grid(graphItem, height, width, topNodes,bottomNodes,
                               complete);
        break;

      case BasicGraphs::Helm:
        simpleG->generate_helm(graphItem, width / 2, height / 2, topNodes,
                               complete);
        break;

      case BasicGraphs::Path:
        simpleG->generate_path(graphItem, width, topNodes, complete);
        break;

      case BasicGraphs::Prism:
        simpleG->generate_prism(graphItem, width / 2, height / 2, topNodes,
                                complete);
        break;

      case BasicGraphs::Antiprism:
        simpleG->generate_antiprism(graphItem, width / 2, height / 2, topNodes,
                                    complete);
        break;

      default:
        break;
    }

    this->scene()->addItem(graphItem);
    return graphItem;
}



void
PreView::Style_Graph(Graph * graph, int graphType,
		     qreal nodeDiameter,
		     QString topNodeLabels, QString bottomNodeLabels,
		     bool numberedLabels, qreal nodeLabelSize,
		     qreal edgeSize, QString edgeLabel, qreal edgeLabelSize,
		     qreal rotation,
		     QColor nodeFillColor, QColor nodeOutlineColor,
		     QColor edgeLineColor)
{
    qDeb() << "PV::Style_Graph() called.";

    int i = 0, j = 0;

    foreach (QGraphicsItem * item, graph->childItems())
    {
        if (item->type() == Node::Type)
        {
	    Node * node = qgraphicsitem_cast<Node *>(item);
	    node->setParentItem(nullptr);
	    node->setDiameter(nodeDiameter);
	    node->setEdgeWeight(edgeSize);
	    node->setFillColour(nodeFillColor);
	    node->setLineColour(nodeOutlineColor);
	    node->setNodeLabelSize(nodeLabelSize);
	    if (numberedLabels)
	    {
		node->setNodeLabel(i);
		i++;
	    }
	    // Special case for labeling Bipartite Graphs
	    else if (graphType == BasicGraphs::Bipartite)
	    {
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
	    node->setParentItem(graph);
        }
        else if (item->type() == Edge::Type)
        {
	    Edge * edge = qgraphicsitem_cast<Edge *>(item);
	    edge->setParentItem(nullptr);
	    edge->setPenWidth(edgeSize);
	    edge->setColour(edgeLineColor);
	    edge->setLabelSize((edgeLabelSize > 0) ? edgeLabelSize : 1);
	    if (edgeLabel.length() != 0)
                edge->setLabel(edgeLabel);
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

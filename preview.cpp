/*
 * File:    preview.cpp
 * Author:  Rachel Bood 100088769
 * Date:    2014/11/07
 * Version: 1.0
 *
 * Purpose: Initializes a QGraphicsView that is used to house the QGraphicsScene
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
PreView::PreView(QWidget *parent)
    : QGraphicsView(parent)
{
    aScene = new QGraphicsScene();
    aScene->setSceneRect(0, 0, this->width(), this->height());

    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate); //updates the canvas
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setScene(aScene);
}

/*
 * Name:        keyPressEvent
 * Purpose:     user can zoom in or out based on user's use of the keyboard
 * Arguments:   QKeyEvent
 * Output:      none
 * Modifies:    none
 * Returns:     none
 * Assumptions: none
 * Bugs:        none...so far
 * Notes:       none
 */
void PreView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Delete:
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}


/*
 * Name:        scaleView
 * Purpose:     scales the view of the QGraphicsScene
 * Arguments:   a qreal
 * Output:      none
 * Modifies:    the scale view of the QGraphicsScene
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */
void PreView::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor)
                .mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;
    scale(scaleFactor, scaleFactor);
}

void PreView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QList<QGraphicsItem *> itemList = this->scene()->items(
                    this->mapToScene(event->pos()),
                    Qt::IntersectsItemShape,
                    Qt::DescendingOrder,QTransform());

        foreach(QGraphicsItem * item, itemList)
        {
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
                    scene()->render(&painter,QRectF(0,
                                                    0,
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
 * Name:        zoomIn
 * Purpose:     Zoom in featuture of the QGraphicsScene
 * Arguments:   none
 * Output:      none
 * Modifies:    the scale of the QGraphicsScene
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */
void PreView::zoomIn()
{
    scaleView(qreal(SCALE_FACTOR));
}

/*
 * Name:        zoomOut
 * Purpose:     zoom out feature in the QGraphicsScene
 * Arguments:   none
 * Output:      none
 * Modifies:    the scale of the QGraphicsScene
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */
void PreView::zoomOut()
{
    scaleView(1 / qreal(SCALE_FACTOR));
}

Graph * PreView::Create_Graph(int graph, int topNodes, int bottomNodes,
                              qreal height, qreal width, bool complete)
{
    Graph * graphItem = new Graph();
    BasicGraphs * simpleG = new BasicGraphs();
    switch (graph) {
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

void PreView::Style_Graph(Graph * graph, int graphType, qreal nodeDiameter,
                          QString topNodeLabels, QString bottomNodeLabels,
                          bool numberedLabels, qreal nodeLabelSize, qreal edgeSize,
                          QString edgeLabel, qreal edgeLabelSize, qreal rotation,
                          QColor nodeFillColor, QColor nodeOutlineColor,
                          QColor edgeLineColor)
{
    int i = 0, j = 0;
    //Styling Nodes
    foreach(QGraphicsItem * item, graph->childItems())
    {
        if (item->type() == Node::Type)
        {
             Node * node = qgraphicsitem_cast<Node *>(item);
             node->setParentItem(nullptr);
             node->setDiameter(nodeDiameter);
             node->setEdgeWeight(edgeSize);
             node->setFillColour(nodeFillColor);
             node->setLineColour(nodeOutlineColor);
             if (numberedLabels)
             {

                 node->setNodeLabel(i);
                 node->setNodeLabelSize(nodeLabelSize);
                 i++;
             }
             //Special case for labeling Bipartite Graphs
             else if (graphType == BasicGraphs::Bipartite)
             {
                 if ( bottomNodeLabels.length() != 0
                      && graph->nodes.bipartite_bottom.contains(node))
                 {

                     node->setNodeLabel(bottomNodeLabels, j);
                     node->setNodeLabelSize(nodeLabelSize);
                     j++;

                 }
                 else if ( topNodeLabels.length() != 0
                           && graph->nodes.bipartite_top.contains(node))
                 {
                    node->setNodeLabel(topNodeLabels, i);
                    node->setNodeLabelSize(nodeLabelSize);
                     i++;
                 }
                 else if (topNodeLabels.length() != 0
                          && graph->nodes.bipartite_bottom.contains(node))
                 {
                     node->setNodeLabel(topNodeLabels, i);
                     node->setNodeLabelSize(nodeLabelSize);
                     i++;
                 }
             }
             else if (topNodeLabels.length() != 0)
             {
                 node->setNodeLabel(topNodeLabels, i);
                 node->setNodeLabelSize(nodeLabelSize);
                 i++;
             }
             node->setParentItem(graph);
        }
        //Styling Edges
        else if (item->type() == Edge::Type)
        {
             Edge * edge = qgraphicsitem_cast<Edge *>(item);
             edge->setParentItem(nullptr);
             edge->setDestRadius(nodeDiameter / 2.);
             edge->setSourceRadius(edge->sourceNode()->getDiameter() / 2.);
             edge->setPenWidth(edgeSize);
             edge->setColour(edgeLineColor);
             edge->setWeightLabelSize((edgeLabelSize > 0) ? edgeLabelSize : 1);
             if (edgeLabel.length() != 0)
                edge->setWeight(edgeLabel);
             edge->adjust();
             edge->setParentItem(graph);
        }
    }
    graph->setPos(mapToScene(viewport()->rect().center()));
    graph->setRotation(-1 * rotation);
}

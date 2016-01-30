#include "graph.h"
#include "canvasview.h"
#include "node.h"
#include "edge.h"
#include "graphmimedata.h"

#include <QMimeData>
#include <QDrag>
#include <QDebug>
#include <QByteArray>
#include <QGraphicsSceneMouseEvent>
#include <QtAlgorithms>
#include <QApplication>
#include <QtCore>
#include <QtGui>
#include <QtMath>

#define MOVED   0

static const bool verbose = false; // used for debugging

/*
 * Name:        Graph()
 * Purpose:     Constructor for the graph object
 * Arguments:   none
 * Output:      none
 * Modifies:    private variables of the graph object
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */
Graph::Graph()
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemIsFocusable);
    setCacheMode(DeviceCoordinateCache);
    moved = 0;
    setAcceptHoverEvents(true);
    setZValue(0);
}

/*
 * Name:        isMoved()
 * Purpose:     a flag used to determined if the graph was dropped onto the canvasscene
 * Arguments:   none
 * Output:      none
 * Modifies:    int moved
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */
void Graph::isMoved()
{
    moved = 1;
    setHandlesChildEvents(false);
}

/*
 * Name:        mouseReleaseEvent()
 * Purpose:     the event that is triggered after the user releases the mouse click
 * Arguments:   QGraphicsSceneMouseEvent*
 * Output:      none
 * Modifies:    the Cursor icon
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */
void Graph::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::CrossCursor);
    QGraphicsItem::mouseReleaseEvent(event);
}

/*
 * Name:        paint()
 * Purpose:     no purpose but required to implement for custom QGraphicsItems
 * Arguments:   QPainter*, QStyleOptionGraphicsItem *, QWidget *
 * Output:      none
 * Modifies:    none
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       A Graph object is more of a container to house the nodes and edges
 *              therefore nothing is required to be drawn in a graph object
 */
void Graph::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                  QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    Q_UNUSED(painter);
}

/*
 * Name:        boundingRect()
 * Purpose:     returns the bouding rectangle of the graph
 * Arguments:   none
 * Output:      none
 * Modifies:    none
 * Returns:     boudingRect()
 * Assumptions: none
 * Bugs:        none
 * Notes:       returns the bounding rectangle that surrounds the nodes and edges
 *              of the graph
 */
QRectF Graph::boundingRect() const
{
    return childrenBoundingRect();
}

/*
 * Name:        setRotation()
 * Purpose:     sets the Rotation of the graph
 * Arguments:   qreal
 * Output:      none
 * Modifies:    the graph, node and edge of the graph
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       the nodes and edge labels need to be rotated in the opposite direction
 *              otherwise the labels of the edges and nodes won't be easily read
 */
void Graph::setRotation(qreal aRotation)
{
    for (QGraphicsItem * child: this->childItems())
    {
        if (child != nullptr || child != 0)
        {
            if (child->type() == Node::Type)
            {
                child->setRotation(-aRotation);
            }
            else if(child->type() == Edge::Type)
            {
                child->setRotation(-aRotation);
            }
        }
    }
    QGraphicsItem::setRotation(aRotation);
}

/*
 * Name:        getRootParent()
 * Purpose:     returns the root parent of the graph
 * Arguments:   none
 * Output:      none
 * Modifies:    none
 * Returns:     QGraphicsItem *
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */
QGraphicsItem *Graph::getRootParent()
{
    QGraphicsItem * parent = this->parentItem();
    while (parent != nullptr || parent != 0)
        parent = parent->parentItem();
    return parent;
}


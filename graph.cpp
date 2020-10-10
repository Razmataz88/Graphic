/*
 * File:    graph.cpp
 * Author:  Rachel Bood
 * Date:    2014/11/07 (?)
 * Version: 1.5
 *
 * Purpose:
 *
 * Modification history:
 * July 20, 2020 (IC V1.1)
 *  (a) Fixed setRotation to properly rotate graph items while taking into
 *      account their previous rotation value.
 * July 30, 2020 (IC & JD V1.2)
 *  (a) Fix getRootParent().
 *  (b) Clean up formatting and improve comments.
 * August 12, 2020 (IC V1.3)
 *  (a) Reversed the previous change to setRotation since it was only needed
 *      when graphs could be children of other graphs which can no longer
 *      happen.
 * Aug 14, 2020 (IC V1.4)
 *  (a) Initialize rotation in constructor.
 *  (b) Once again changed setRotation() back to the July 20 change.
 *	The issue was that the GraphicsItem rotation call at the end
 *	of the function wasn't using the additive rotation value but
 *	instead the passed value.
 * Aug 20, 2020 (IC V1.5)
 *  (a) Once again changed setRotation back to the July 20 change.  The issue
 *      was that the GraphicsItem rotation call at the end of the function
 *      wasn't using the additive rotation value but instead the passed value.
 */

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
 * Purpose:     Constructor for the graph object.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    Private variables of the graph object.
 * Returns:     Nothing.
 * Assumptions: None.
 * Bugs:        None known.
 * Notes:       None.
 */

Graph::Graph()
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemIsFocusable);
    setCacheMode(DeviceCoordinateCache);
    moved = 0;
    rotation = 0;
    setAcceptHoverEvents(true);
    setZValue(0);
}



/*
 * Name:        isMoved()
 * Purpose:     Set a flag used to determined if the graph was dropped
 *		onto the canvas scene.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    int moved
 * Returns:     Nothing.
 * Assumptions: None.
 * Bugs:        None known.
 * Notes:       None.
 */

void
Graph::isMoved()
{
    moved = 1;
    setHandlesChildEvents(false);
}



/*
 * Name:        mouseReleaseEvent()
 * Purpose:     Handle the event that is triggered after the user releases
 *		the mouse button.
 * Arguments:   QGraphicsSceneMouseEvent *
 * Output:      Nothing.
 * Modifies:    the Cursor icon
 * Returns:     Nothing.
 * Assumptions: None.
 * Bugs:        None known.
 * Notes:       None.
 */

void
Graph::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    setCursor(Qt::CrossCursor);
    QGraphicsItem::mouseReleaseEvent(event);
}



/*
 * Name:        paint()
 * Purpose:     No purpose but required to implement for custom QGraphicsItems
 * Arguments:   QPainter*, QStyleOptionGraphicsItem *, QWidget *
 * Output:      Nothing.
 * Modifies:    Nothing.
 * Returns:     Nothing.
 * Assumptions: None.
 * Bugs:        None known.
 * Notes:       A Graph object is just a container to house the nodes
 *		and edges, therefore nothing is required to be drawn
 *		in a graph object.
 */

void
Graph::paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
	     QWidget * widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    Q_UNUSED(painter);
}



/*
 * Name:        boundingRect()
 * Purpose:     Returns the bouding rectangle of the graph.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    Nothing.
 * Returns:     The graph's boundingRect().
 * Assumptions: None.
 * Bugs:        None known.
 * Notes:       Returns the bounding rectangle that surrounds the
 *		nodes and edges of the graph.
 */

QRectF
Graph::boundingRect() const
{
    return childrenBoundingRect();
}



/*
 * Name:        setRotation()
 * Purpose:     Sets the Rotation of the graph.
 * Arguments:   qreal
 * Output:      Nothing.
 * Modifies:    The graph, nodes and edges of the graph.
 * Returns:     Nothing.
 * Assumptions: None.
 * Bugs:        None known.
 * Notes:       The node and edge labels need to be rotated in the
 *		opposite direction, otherwise the labels of the edges
 *		and nodes won't be easily read.
 */

void
Graph::setRotation(qreal aRotation, bool keepRotation)
{
    QList<QGraphicsItem *> list;

    foreach (QGraphicsItem * gItem, this->childItems())
        list.append(gItem);

    while (!list.isEmpty())
    {
        foreach (QGraphicsItem * child, list)
        {
            if (child != nullptr || child != 0)
            {
                if (child->type() == Graph::Type)
                {
                    list.append(child->childItems());
                }
                else if (child->type() == Node::Type)
                {
                    Node * node = qgraphicsitem_cast<Node*>(child);
                    if (keepRotation)
                        node->setRotation(node->getRotation() + -aRotation);
                    else
                        node->setRotation(-aRotation);
                }
                else if(child->type() == Edge::Type)
                {
                    Edge * edge = qgraphicsitem_cast<Edge*>(child);
                    if (keepRotation)
                        edge->setRotation(edge->getRotation() + -aRotation);
                    else
                        edge->setRotation(-aRotation);
                }
                list.removeOne(child);
            }
        }
    }

    if (keepRotation)
        rotation = getRotation() + aRotation;
    else
        rotation = aRotation;

    QGraphicsItem::setRotation(rotation);
}



/*
 * Name:	getRotation()
 * Purpose:	Getter for the graph's rotation.
 * Arguments:	None.
 * Output:	Nothing.
 * Modifies:	Nothing.
 * Returns:	The graph rotation.
 * Assumptions:	None.
 * Bugs:	Hard to imagine.
 * Notes:	None.
 */

qreal
Graph::getRotation()
{
    return rotation;
}



/*
 * Name:        getRootParent()
 * Purpose:     Returns the root parent of the graph.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    Nothing.
 * Returns:     QGraphicsItem *
 * Assumptions: None.
 * Bugs:        None known.
 * Notes:       None.
 */

QGraphicsItem *
Graph::getRootParent()
{
    QGraphicsItem * parent = this;

    while (parent->parentItem() != nullptr || parent->parentItem() != 0)
        parent = parent->parentItem();

    return parent;
}


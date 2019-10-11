/*
 * File:	graph.h
 * Author:	Rachel Bood
 * Date:	2014 or 2015?
 * Version:	1.0
 *
 * Purpose:	Define the graph class.
 *
 * Modification history:
 * Oct 10, 2019 (JD)
 *  (a) Add this header comment.
 *  (b) Remove unused field "point_star" from the graph struct.
 *  (c) Minor reformatting.
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <QGraphicsItemGroup>
#include "cornergrabber.h"

class CanvasView;
class Node;
class Edge;

class Graph : public QGraphicsItem
{
public:
    typedef struct graph
    {
        QList<Node *> cycle;
        QList<QList <Node *>> double_cycle;
        QList<QList <Node *>> list_of_cycles;
        QList<Node *> bipartite_top;
        QList<Node *> bipartite_bottom;
        QList<Node *> grid;
        QList<Node *> path;
        QVector<Node *> binaryHeap;
        Node * center;
    } Nodes;

    Graph();
    void isMoved();
    enum {Type = UserType + 3};
    int type() const {return Type;}
    Nodes nodes;

    QRectF boundingRect() const;
    void setRotation(qreal aRotation);
    QGraphicsItem * getRootParent();

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    void paint(QPainter * painter,
	       const QStyleOptionGraphicsItem * option,
	       QWidget * widget);

private:
     int moved;		// 1 means the graph was dropped onto the canvas.
};

#endif // GRAPH_H

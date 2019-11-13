/*
 * File:    edge.cpp
 * Author:  Rachel Bood
 * Date:    2014/11/07 (?)
 * Version: 1.2
 *
 * Purpose: creates an edge for the users graph
 * Modification history:
 * Nov 13, 2019 (JD, V1.1):
 *  (a) Minor formatting tweaks.
 *  (b) Apply change of name label.h -> html-label.h
 * Nov 13, 2019 (JD, V1.2):
 *  (a) Rename (in this order!)
 *	       label -> htmlLabel,
 *	       setWeightLabelSize() -> setLabelSize(),
 *             getWeightLabelSize() -> getLabelSize(),
 *	       setWeight() -> setLabel(), 
 *	       getWeight() -> getLabel(),
 *	       editWeight() -> editLabel(),
 *	       weight -> label,
 *	       esize -> labelSize,
 *     in order to rationalize the naming scheme.
 */

#ifndef EDGE_H
#define EDGE_H

#include "html-label.h"
#include <QGraphicsItem>
#include <QGraphicsObject>
#include <QList>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsSceneMouseEvent>

class Node;
class CanvasView;
class PreView;

class Edge: public QGraphicsObject
{
public:
    Edge(Node * sourceNode, Node * destNode);

    void setDestRadius(qreal aRadius);
    qreal getDestRadius();

    void setSourceRadius(qreal aRadius);
    qreal getSourceRadius();

    void setPenWidth(qreal aPenWidth);
    qreal getPenWidth();

    void setRotation(qreal aRotation);
    qreal getRotation();

    void setColour(QColor color);
    QColor getColour();

    void setLabelSize(qreal edgeLabelSize);
    qreal getLabelSize();

    QRectF boundingRect() const;
    QPainterPath shape() const;

    void setLabel(QString label);
    QString getLabel();

    void adjust();
    void setDestNode(Node * node);
    void setSourceNode(Node * node);

    enum { Type = UserType + 2 };
    int type() const { return Type; }

    Node * sourceNode() const;
    Node * destNode() const;

    void editLabel(bool edit);
    QGraphicsItem * getRootParent();

    bool isDigits(const std::string &str);

     //~Edge(); deconstructor a WIP
protected:
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
               QWidget * widget);

signals:
    void edgeDeleted();

private:
    void createSelectionPolygon();
    Node	* source, * dest;   // Original names based on directed graphs
    QPointF	offset1, offset2;
    QPointF	sourcePoint, destPoint;
    QPolygonF	selectionPolygon;
    qreal	destRadius, sourceRadius, penWidth, rotation;
    QLineF	edgeLine;
    QString	label;
    qreal	labelSize, penSize;
    QColor	edgeColour;
    HTML_Label	* htmlLabel;
};

#endif // EDGE_H

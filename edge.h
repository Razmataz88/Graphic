#ifndef EDGE_H
#define EDGE_H

#include "label.h"
#include <QGraphicsItem>
#include <QGraphicsObject>
#include <QList>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsSceneMouseEvent>

class Node;
class CanvasView;
class PreView;

class Edge :public QGraphicsObject
{

public:
    Edge(Node *sourceNode, Node *destNode);

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

    void setWeightLabelSize(qreal edgeWieghtLabelSize);
    qreal getWeightLabelSize();

    QRectF boundingRect() const;
    QPainterPath shape() const;

    void setWeight(QString aWeight);
    QString getWeight();

    void adjust();
    void setDestNode(Node * node);
    void setSourceNode(Node * node);

    enum { Type = UserType + 2 };
    int type() const { return Type; }

    Node *sourceNode() const;
    Node *destNode() const;

    void editWeight(bool edit);
    QGraphicsItem * getRootParent();

    bool isDigits(const std::string &str);

     //~Edge(); deconstructor a WIP
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

signals:
    void edgeDeleted();

private:
    void createSelectionPolygon();
    Node *source, *dest; // original naming convention based on directed graphs
    QPointF offset1, offset2;
    QPointF sourcePoint, destPoint;
    QPolygonF selectionPolygon;
    qreal destRadius, sourceRadius, penWidth, rotation;
    QLineF edgeLine;
    QString weight;
    qreal eSize, penSize;
    QColor edgeColour;
    Label *label;
};

#endif // EDGE_H

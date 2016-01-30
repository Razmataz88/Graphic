#ifndef NODE_H
#define NODE_H

#include "label.h"
#include <QGraphicsItem>
#include <QList>
#include <QGraphicsSceneMouseEvent>

class Edge;
class CanvasView;
class PreView;

class Node : public QGraphicsObject
{
public:
    Node();

    void addEdge(Edge *edge);

    bool removeEdge(Edge * edge);

    void setDiameter(qreal diameter);
    qreal getDiameter();

    void setEdgeWeight(qreal aEdgeWeight);

    void setRotation(qreal aRotation);
    qreal getRotation();

    void setFillColour(QColor fColor);
    QColor getFillColour();

    void setLineColour(QColor lColor);
    QColor getLineColour();
    QGraphicsItem *findRootParent();
    void setID(int id);
    int getID();

    void setNodeLabel(qreal number);
    void setNodeLabel(QString aLabel);
    void setNodeLabel(QString aLabel, qreal number);
    void setNodeLabel(QString htmltext, qreal labelSize, QString label);
    void setNodeLabelSize(qreal labelSize);
    
    QString getLabel() const;
    qreal getLabelSize() const;

    QRectF boundingRect() const;

    enum { Type = UserType + 1 };
    int type() const { return Type; }

    QList<Edge *> edgeList;

    QList<Edge *> edges() const;
    void chosen(int group1);

    void editLabel(bool edit);
   // ~Node();
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);  
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
    void nodeDeleted();

private:
    QPointF newPos;
    qreal nodeDiameter, edgeWeight, rotation;
    QString  label;
    Label * text;
    qreal lSize;
    QColor nodeLine, nodeFill;
    int nodeID, choose;
bool select;
    qreal logicalDotsPerInchX;
    qreal logicalDotsPerInchY;
};

#endif // NODE_H

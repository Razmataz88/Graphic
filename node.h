/*
 * File:    node.cpp
 * Author:  Rachel Bood
 * Date:    2014/11/07
 * Version: 1.3
 *
 * Purpose: Declare the node class.
 * 
 * Modification history:
 * Oct 13, 2019 (JD V1.1)
 *  (a) Remove unused lSize from here and node.cpp.
 *  (b) Minor formatting changes.
 *  (c) renamed "choose" to "penStyle".
 * Nov 13, 2019 (JD V1.2)
 *  - rename Label -> HTML_Label, label-h -> html-label.h,
 *    remove strToHtml() decl.
 * Nov 13, 2019 (JD V1.3)
 *  - rename HTML_Label text to HTML_Label htmlLabel.
 *  - delete cruft.
 */


#ifndef NODE_H
#define NODE_H

#include "html-label.h"
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

    void addEdge(Edge * edge);

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
    QGraphicsItem * findRootParent();
    void setID(int id);
    int getID();

    void setNodeLabel(qreal number);
    void setNodeLabel(QString aLabel);
    void setNodeLabel(QString aLabel, qreal number);
    void setNodeLabel(QString aLabel, QString subscript);
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
    void mousePressEvent(QGraphicsSceneMouseEvent * event);  
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
	       QWidget * widget);

signals:
    void nodeDeleted();

private:
    QPointF	newPos;
    qreal	nodeDiameter, edgeWeight, rotation;
    QString	label;
    HTML_Label	* htmlLabel;
    QColor	nodeLine, nodeFill;
    int		nodeID;		    // The (internal) number of the node.
    int		penStyle;
    bool	select;
    qreal	logicalDotsPerInchX;
    qreal	logicalDotsPerInchY;
    void labelToHtml();
};

#endif // NODE_H

#ifndef LABEL_H
#define LABEL_H
#include <QGraphicsTextItem>

class Label : public QGraphicsTextItem
{
public:
    Label(QGraphicsItem * parent = 0);
    void setTextInteraction(bool on, bool selectAll = false);

    enum { Type = UserType + 4 };
    int type() const { return Type; }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // LABEL_H

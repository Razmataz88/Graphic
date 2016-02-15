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

    void setLabel(QString string);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QString labelText;
};

#endif // LABEL_H

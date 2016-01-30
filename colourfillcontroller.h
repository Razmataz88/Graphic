#ifndef COLORFILLCONTROLLER_H
#define COLORFILLCONTROLLER_H
#include "node.h"

#include <QPushButton>
#include <QObject>

class ColorFillController : public QObject
{
    Q_OBJECT
public:
    ColorFillController(Node * aNode, QPushButton * aButton);

private slots:
    void setNodeFillColor();
    void deleteButton();

private:
    Node * node;
    QPushButton * button;
};

#endif // COLORFILLCONTROLLER_H

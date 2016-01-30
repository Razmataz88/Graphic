#ifndef EDGECOLORCONTROLLER_H
#define EDGECOLORCONTROLLER_H
#include "edge.h"
#include "node.h"

#include <QPushButton>
#include <QObject>

class ColorLineController: public QObject
{
    Q_OBJECT

public:
    ColorLineController(Edge * anEdge, QPushButton * aButton);
    ColorLineController(Node * anNode, QPushButton * aButton);

private slots:
    void setEdgeLineColor();
    void setNodeOutlineColor();
    void deleteButton();

private:
    Edge * edge;
    Node * node;
    QPushButton * button;
};

#endif // EDGECOLORCONTROLLER_H

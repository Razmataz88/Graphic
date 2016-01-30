#ifndef EDGESIZECONTROLLER_H
#define EDGESIZECONTROLLER_H

#include "edge.h"
#include "node.h"

#include <QObject>
#include <QDoubleSpinBox>

class SizeController : QObject

{
    Q_OBJECT
public:
    SizeController(Edge * anEdge, QDoubleSpinBox * aBox);
    SizeController(Node * aNode, QDoubleSpinBox * aBox);


private slots:
    void setEdgeSize(double value);
    void setNodeSize(double value);
    void deletedBox();

private:
    Edge * edge;
    Node * node;
    QDoubleSpinBox * box;
};

#endif // EDGESIZECONTROLLER_H

#ifndef EDGEWEIGTHCONTROLLLER_H
#define EDGEWEIGTHCONTROLLLER_H
#include "edge.h"
#include "node.h"

#include <QDoubleSpinBox>
#include <QObject>

class LabelSizeController : public QObject
{
    Q_OBJECT
public:
    LabelSizeController(Edge * anEdge, QDoubleSpinBox * aBox);
    LabelSizeController(Node * aNode, QDoubleSpinBox * aBox);

private slots:
    void setEdgeWeightSize(double value);
    void setNodeLabelSize(double value);
    void deletedSpinBox();

private:
    Edge * edge;
    Node * node;
    QDoubleSpinBox * box;

};

#endif // EDGEWEIGTHCONTROLLLER_H

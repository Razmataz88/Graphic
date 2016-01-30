#include "sizecontroller.h"


SizeController::SizeController(Edge *anEdge, QDoubleSpinBox *aBox)
{
    edge = anEdge;
    box = aBox;
    if (box != nullptr || box != 0)
    {
        box->setValue(edge->getPenWidth());
        connect(box, SIGNAL( valueChanged(double)),
                this, SLOT(setEdgeSize(double)));
        connect(anEdge, SIGNAL(destroyed(QObject*)),
                this, SLOT(deletedBox()));
        connect(anEdge, SIGNAL(destroyed(QObject*)),
                this, SLOT(deleteLater()));
    }
}

SizeController::SizeController(Node * aNode, QDoubleSpinBox *aBox)
{
    node = aNode;
    box = aBox;
    if (box != nullptr || box != 0)
    {
        box->setValue(node->getDiameter());
        box->setSingleStep(0.05
                           );
        connect(box, SIGNAL( valueChanged(double)),
                this, SLOT(setNodeSize(double)));

        connect(aNode, SIGNAL(destroyed(QObject*)),
                this, SLOT(deletedBox()));
        connect(aNode, SIGNAL(destroyed(QObject*)),
                this, SLOT(deleteLater()));
    }
}

void SizeController::setEdgeSize(double value)
{
    if (edge != nullptr || edge != 0)
    edge->setPenWidth(value);
}

void SizeController::setNodeSize(double value)
{
    if (node != nullptr || node != 0)
        node->setDiameter(value);
}

void SizeController::deletedBox()
{
    delete box;
}


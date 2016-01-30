#include "labelsizecontroller.h"


LabelSizeController::LabelSizeController(Edge *anEdge, QDoubleSpinBox *aBox)
{
    edge = anEdge;
    box = aBox;
    if (box != nullptr || box != 0)
    {
        if (box->value() == 0)
            box->setValue(12);
        else
            box->setValue(edge->getWeightLabelSize());

        connect(box, SIGNAL(valueChanged(double)),
                this, SLOT(setEdgeWeightSize(double)));
        connect(anEdge, SIGNAL(destroyed(QObject*)),
                this, SLOT(deletedSpinBox()));
        connect(anEdge, SIGNAL(destroyed(QObject*)),
                this, SLOT(deleteLater()));

    }
}

LabelSizeController::LabelSizeController(Node * aNode, QDoubleSpinBox *aBox)
{
    node = aNode;
    box = aBox;
    if (box != nullptr || box != 0)
    {
        if (box->value() == 0)
            box->setValue(12);
        else
            box->setValue(node->getLabelSize());

        connect(box, SIGNAL(valueChanged(double)),
                this, SLOT(setNodeLabelSize(double)));
        connect(aNode, SIGNAL(destroyed(QObject*)),
                this, SLOT(deletedSpinBox()));
        connect(aNode, SIGNAL(destroyed(QObject*)),
                this, SLOT(deleteLater()));
    }
}

void LabelSizeController::setNodeLabelSize(double value)
{
    if (node != nullptr || node != 0)
        node->setNodeLabelSize(value);
}

void LabelSizeController::deletedSpinBox()
{
    delete box;
}

void LabelSizeController::setEdgeWeightSize(double value)
{
    if (edge != nullptr || edge != 0)
        edge->setWeightLabelSize(value);
}



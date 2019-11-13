/*
 * File:    labelsizecontroller.cpp
 * Author:  Rachel Bood
 * Date:    2014/11/07 (?)
 * Version: 1.1
 *
 * Purpose: Initializes a QGraphicsView that is used to house the QGraphicsScene
 *
 * Modification history:
 *  Nov 13, 2019 (JD, V1.1):
 *   (a) Renamed setWeightLabelSize() -> setEdgeLabelSize().
 *   (b) Removed apparently redundant "|| edge != 0" in setEdgeLabelSize().
 */

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
            box->setValue(edge->getLabelSize());

        connect(box, SIGNAL(valueChanged(double)),
                this, SLOT(setEdgeLabelSize(double)));
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


/*
 * Name:	setEdgeLabelSize()
 * Purpose:	Set the size of the edge label.
 * Arguments:	The size, in points.
 * Outputs:	Nothing.
 * Modifies:	The edge label size.
 * Returns:	Nothing.
 * Assumptions:	None.
 * Bugs:	?
 * Notes:	None.
 */

void
LabelSizeController::setEdgeLabelSize(double ptSize)
{
    if (edge != nullptr)
        edge->setLabelSize(ptSize);
}



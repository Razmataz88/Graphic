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
 *  Nov 24, 2019 (JD, V1.2):
 *   (a) Fixed bugs in the (node and edge) constructors which
 *	 incorrectly set the text size box value.
 *   (b) Minor formatting tweaks, comment additions.
 *   (c) Removed apparently redundant "|| box != 0" tests.
 */

#include "labelsizecontroller.h"

#define DEFAULT_EDGE_LABEL_SIZE  12
#define DEFAULT_NODE_LABEL_SIZE  12


LabelSizeController::LabelSizeController(Edge * anEdge, QDoubleSpinBox * aBox)
{
    edge = anEdge;
    box = aBox;
    if (box != nullptr)
    {
	if (edge->getLabel().length() > 0)
	    box->setValue(edge->getLabelSize());
	else
            box->setValue(DEFAULT_EDGE_LABEL_SIZE);

        connect(box, SIGNAL(valueChanged(double)),
                this, SLOT(setEdgeLabelSize(double)));
        connect(anEdge, SIGNAL(destroyed(QObject*)),
                this, SLOT(deletedSpinBox()));
        connect(anEdge, SIGNAL(destroyed(QObject*)),
                this, SLOT(deleteLater()));

    }
}



LabelSizeController::LabelSizeController(Node * aNode, QDoubleSpinBox * aBox)
{
    node = aNode;
    box = aBox;
    if (box != nullptr)
    {
	if (node->getLabel().length() > 0)
            box->setValue(node->getLabelSize());
	else
            box->setValue(DEFAULT_NODE_LABEL_SIZE);

        connect(box, SIGNAL(valueChanged(double)),
                this, SLOT(setNodeLabelSize(double)));
        connect(aNode, SIGNAL(destroyed(QObject*)),
                this, SLOT(deletedSpinBox()));
        connect(aNode, SIGNAL(destroyed(QObject*)),
                this, SLOT(deleteLater()));
    }
}



/*
 * Name:	setNodeLabelSize()
 * Purpose:	Set the size of the node label.
 * Arguments:	The size, in points.
 * Outputs:	Nothing.
 * Modifies:	The node label size.
 * Returns:	Nothing.
 * Assumptions:	None.
 * Bugs:	?
 * Notes:	None.
 */

void LabelSizeController::setNodeLabelSize(double ptSize)
{
    if (node != nullptr)
        node->setNodeLabelSize(ptSize);
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

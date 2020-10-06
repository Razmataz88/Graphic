/*
 * File:    sizecontroller.h
 * Author:  Rachel Bood
 * Date:    2014 (?)
 * Version: 1.1
 *
 * Modification history:
 * Jul 15, 2020 (IC V1.1)
 *  (a) Add a second value to node sizes; formerly nodes only had
 *	variable diameters, now the pen thickness can be changed as well.
 *	This requires a second spinbox, and so some renaming was done
 *	and the deletebox() function was split into two, one for edges
 *	(which have but one box) and one for nodes (which now have two).
 */

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
    SizeController(Node * aNode, QDoubleSpinBox * diamBox,
		   QDoubleSpinBox * thicknessBox);


private slots:
    void setEdgeSize(double value);
    void setNodeSize(double value);
    void setNodeSize2(double value);
    void deletedEdgeBox();
    void deletedNodeBoxes();

private:
    Edge * edge;
    Node * node;
    QDoubleSpinBox * box1;
    QDoubleSpinBox * box2;
};

#endif // EDGESIZECONTROLLER_H

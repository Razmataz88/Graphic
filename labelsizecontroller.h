/*
 * File:    labelsizecontroller.h
 * Author:  Rachel Bood
 * Date:    2014/11/07 (?)
 * Version: 1.1
 *
 * Purpose: Declare the node class.
 * 
 * Modification history:
 * Nov 13, 2019 (JD V1.1)
 *  (a) Rename setEdgeWeightSize() -> setEdgeLabelSize().
 *  (b) Fix incorrect #ifndef token name.
 */


#ifndef LABELSIZECONTROLLER_H
#define LABELSIZECONTROLLER_H
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
    void setEdgeLabelSize(double value);
    void setNodeLabelSize(double value);
    void deletedSpinBox();

private:
    Edge * edge;
    Node * node;
    QDoubleSpinBox * box;

};

#endif // LABELSIZECONTROLLER_H

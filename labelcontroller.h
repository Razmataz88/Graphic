/*
 * File:    labelcontroller.h
 * Author:  Rachel Bood
 * Date:    2014/11/07 (?)
 * Version: 1.1
 *
 * Purpose: 
 *
 * Modification history:
 * Nov 13, 2019 (JD, V1.1)
 *  - rename "Weight" to "Label" for edge function names.
 */


#ifndef EDGEWEIGHTCONTROLLER_H
#define EDGEWEIGHTCONTROLLER_H
#include "edge.h"
#include "node.h"

#include <QLineEdit>
#include <QObject>

class LabelController : public QObject
{
    Q_OBJECT
public:
    LabelController(Edge * anEdge, QLineEdit * anEdit);
    LabelController(Node * aNode, QLineEdit * anEdit);

private slots:
    void setEdgeLabel(QString string);
    void setNodeLabel(QString string);
    void deletedLineEdit();

private:
    Edge * edge;
    Node * node;
    QLineEdit * edit;
};

#endif // EDGEWEIGHTCONTROLLER_H

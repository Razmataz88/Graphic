/*
 * File:    labelcontroller.cpp
 * Author:  Rachel Bood
 * Date:    2014/11/07 (?)
 * Version: 1.1
 *
 * Purpose: ?
 *
 * Modification history:
 * Nov 13, 2019 (JD, V1.1)
 *  - rename "Weight" to "Label" for edge function names.
 */


#include "labelcontroller.h"


LabelController::LabelController(Edge * anEdge, QLineEdit * anEdit)
{
    edit = anEdit;
    edge = anEdge;

    if (edit != nullptr || edit != 0)
    {
        edit->setText(edge->getLabel());
        connect(edit, SIGNAL(textChanged(QString)),
                this, SLOT(setEdgeLabel(QString)));
        connect(anEdge, SIGNAL(destroyed(QObject*)),
                this, SLOT(deletedLineEdit()));
        connect(anEdge, SIGNAL(destroyed(QObject*)),
                this, SLOT(deleteLater()));
    }
}



LabelController::LabelController(Node * aNode, QLineEdit * anEdit)
{
    node = aNode;
    edit = anEdit;
    if (edit != nullptr || edit != 0)
    {
        edit->setText(node->getLabel());
        connect(edit, SIGNAL(textChanged(QString)),
                this, SLOT(setNodeLabel(QString)));
        connect(aNode, SIGNAL(destroyed(QObject*)),
                this, SLOT(deletedLineEdit()));
        connect(aNode, SIGNAL(destroyed(QObject*)),
                this, SLOT(deleteLater()));
    }
}



void
LabelController::setEdgeLabel(QString string)
{
    if (edge != nullptr || edge != 0)
        edge->setLabel(string);
}



void
LabelController::setNodeLabel(QString string)
{
    if (edge != nullptr || edge != 0)
        node->setNodeLabel(string);
}



void
LabelController::deletedLineEdit()
{
    delete edit;
}

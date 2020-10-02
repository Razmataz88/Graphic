/*
 * File:    labelcontroller.cpp
 * Author:  Rachel Bood
 * Date:    2014/11/07 (?)
 * Version: 1.4
 *
 * Purpose: ?
 *
 * Modification history:
 * Nov 13, 2019 (JD, V1.1)
 *  - Rename "Weight" to "Label" for edge function names.
 * June 17, 2020 (IC V1.2)
 *  - Changed setNodeLabel() to properly check if node is null or 0 instead
 *    of edge.
 * June 18, 2020 (IC, V1.3)
 *  - Added setEdgeLabel2() and setNodeLabel2() for updating the edit tab
 *    labels when labels are changed via edit mode on the canvas.
 *  - Added extra connect statements to handle those edit events.
 * June 19, 2020 (IC, V1.4)
 *  - Updated setters to check for appropriate focusEvents.
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
        connect(edge->htmlLabel->document(), SIGNAL(contentsChanged()),
                this, SLOT(setEdgeLabel2()));
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
        connect(node->htmlLabel->document(), SIGNAL(contentsChanged()),
                this, SLOT(setNodeLabel2()));
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
        if (edit->hasFocus())
            edge->setLabel(string);
}


void
LabelController::setEdgeLabel2()
{
    if (edge->htmlLabel->hasFocus())
        edit->setText(edge->htmlLabel->toPlainText());
}


void
LabelController::setNodeLabel(QString string)
{
    if (node != nullptr || node != 0)
        if (edit->hasFocus())
            node->setNodeLabel(string);
}


void
LabelController::setNodeLabel2()
{
    if (node->htmlLabel->hasFocus())
        edit->setText(node->htmlLabel->toPlainText());
}


void
LabelController::deletedLineEdit()
{
    delete edit;
}

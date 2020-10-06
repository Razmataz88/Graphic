/*
 * File:    labelcontroller.cpp
 * Author:  Rachel Bood
 * Date:    2014/11/07 (?)
 * Version: 1.6
 *
 * Purpose: ?
 *
 * Modification history:
 * Nov 13, 2019 (JD V1.1)
 *  (a) Rename "Weight" to "Label" for edge function names.
 * Jun 17, 2020 (IC V1.2)
 *  (a) Changed setNodeLabel() to properly check if node is null or 0 instead
 *      of edge.
 * Jun 18, 2020 (IC V1.3)
 *  (a) Added setEdgeLabel2() and setNodeLabel2() for updating the edit tab
 *      labels when labels are changed via edit mode on the canvas.
 *  (b) Added extra connect statements to handle those edit events.
 * Jun 19, 2020 (IC V1.4)
 *  (a) Updated setters to check for appropriate focusEvents.
 * Jun 24, 2020 (IC, V1.5)
 *  (a) Rename some functions.
 *  (b) Update/fix connections in label controllers accordingly.
 * Jul 14, 2020 (IC V1.6)
 *  (a) Update ...EditLabel slot names to more meaningful values.
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
                this, SLOT(setEdgeEditLabel()));
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
                this, SLOT(setNodeEditLabel()));
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
LabelController::setNodeLabel(QString string)
{
    if (node != nullptr || node != 0)
        if (edit->hasFocus())
            node->setNodeLabel(string);
}


void
LabelController::setEdgeEditLabel()
{ // Sets the line edit text to u1 instead of u_{1} for subscripts.
    if (edge->htmlLabel->hasFocus())
        edit->setText(edge->htmlLabel->toPlainText());
}


void
LabelController::setNodeEditLabel()
{ // Sets the line edit text to u1 instead of u_{1} for subscripts.
    if (node->htmlLabel->hasFocus())
        edit->setText(node->htmlLabel->toPlainText());
}


void
LabelController::deletedLineEdit()
{
    delete edit;
}

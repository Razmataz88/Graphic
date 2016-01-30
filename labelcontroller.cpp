#include "labelcontroller.h"


LabelController::LabelController(Edge *anEdge, QLineEdit *anEdit)
{
    edit = anEdit;
    edge = anEdge;

    if (edit != nullptr || edit != 0)
    {
        edit->setText(edge->getWeight());
        connect(edit, SIGNAL(textChanged(QString)),
                this, SLOT(setEdgeWeight(QString)));
        connect(anEdge, SIGNAL(destroyed(QObject*)),
                this, SLOT(deletedLineEdit()));
        connect(anEdge, SIGNAL(destroyed(QObject*)),
                this, SLOT(deleteLater()));
    }
}

LabelController::LabelController(Node *aNode, QLineEdit *anEdit)
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

void LabelController::setEdgeWeight(QString string)
{
    if (edge != nullptr || edge != 0)
        edge->setWeight(string);
}


void LabelController::setNodeLabel(QString string)
{
    if (edge != nullptr || edge != 0)
        node->setNodeLabel(string);
}

void LabelController::deletedLineEdit()
{
    delete edit;
}

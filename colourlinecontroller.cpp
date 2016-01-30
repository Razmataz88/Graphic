#include "colourlinecontroller.h"

#include <QColorDialog>
#include <QtCore>

#define BUTTON_STYLE "border-style: outset; border-width: 2px; border-radius: 5px; border-color: beige; padding: 3px;"

ColorLineController::ColorLineController(Edge * anEdge, QPushButton * aButton)
{
    edge = anEdge;
    button = aButton;
    if (button != nullptr || button != 0)
    {
        QColor color = edge->getColour();
        QString s("background: #"
                  + QString(color.red() < 16? "0" : "")
                  + QString::number(color.red(),16)
                  + QString(color.green() < 16? "0" : "")
                  + QString::number(color.green(),16)
                  + QString(color.blue() < 16? "0" : "")
                  + QString::number(color.blue(),16) + ";"
                  BUTTON_STYLE);
        button->setStyleSheet(s);
        connect(button, SIGNAL (clicked(bool)),
                this, SLOT(setEdgeLineColor()));
        connect(anEdge, SIGNAL(destroyed(QObject*)),
                this, SLOT(deleteButton()));
        connect(anEdge, SIGNAL(destroyed(QObject*)),
                this, SLOT(deleteLater()));
    }
}

ColorLineController::ColorLineController(Node * aNode, QPushButton *aButton)
{
    node = aNode;
    button = aButton;
    if (button != nullptr || button != 0)
    {
        QColor color = node->getLineColour();
        QString s("background: #"
                  + QString(color.red() < 16? "0" : "")
                  + QString::number(color.red(),16)
                  + QString(color.green() < 16? "0" : "")
                  + QString::number(color.green(),16)
                  + QString(color.blue() < 16? "0" : "")
                  + QString::number(color.blue(),16) + ";"
                  BUTTON_STYLE);
        button->setStyleSheet(s);
        connect(button, SIGNAL (clicked(bool)),
                this, SLOT(setNodeOutlineColor()));
        connect(aNode, SIGNAL(destroyed(QObject*)),
                this, SLOT(deleteButton()));
        connect(aNode, SIGNAL(destroyed(QObject*)),
                this, SLOT(deleteLater()));
    }

}

void ColorLineController::setEdgeLineColor()
{
    QColor color = QColorDialog::getColor();
    QString s("background: #"
              + QString(color.red() < 16? "0" : "")
              + QString::number(color.red(),16)
              + QString(color.green() < 16? "0" : "")
              + QString::number(color.green(),16)
              + QString(color.blue() < 16? "0" : "")
              + QString::number(color.blue(),16) + ";"
              BUTTON_STYLE);
    button->setStyleSheet(s);
    if (edge != 0 || edge != nullptr)
    {
        edge->setColour(color);
    }
}

void ColorLineController::setNodeOutlineColor()
{
    QColor color = QColorDialog::getColor();
    QString s("background: #"
              + QString(color.red() < 16? "0" : "")
              + QString::number(color.red(),16)
              + QString(color.green() < 16? "0" : "")
              + QString::number(color.green(),16)
              + QString(color.blue() < 16? "0" : "")
              + QString::number(color.blue(),16) + ";"
              BUTTON_STYLE);
    button->setStyleSheet(s);
    if (node != 0 || node != nullptr)
    {
        node->setLineColour(color);
    }
}

void ColorLineController::deleteButton()
{
    delete button;
}




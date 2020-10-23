/*
 * File:    colourfillcontroller.cpp
 * Author:  Rachel Bood 100088769
 * Date:    2014 (?)
 * Version: 1.2
 *
 * Purpose:
 *
 * Modification history:
 * Jul 9, 2020 (IC V1.1)
 *  (a) BUTTON_STYLE moved to defuns.h.
 * Oct 18, 2020 (JD V1.2)
 *  (a) Code tidying, spelling corrections, code simplifications.
 */

#include "colourfillcontroller.h"
#include <QColorDialog>


ColourFillController::ColourFillController(Node * aNode, QPushButton * aButton)
{
    node = aNode;
    button = aButton;
    if (button != nullptr || button != 0)
    {
	QColor colour = node->getFillColour();
	QString s("background: " + colour.name() + "; " + BUTTON_STYLE);
	button->setStyleSheet(s);

	connect(button, SIGNAL (clicked(bool)),
		this, SLOT(setNodeFillColour()));
	connect(aNode, SIGNAL(destroyed(QObject*)),
		this, SLOT(deleteButton()));
	connect(aNode, SIGNAL(destroyed(QObject*)),
		this, SLOT(deleteLater()));
    }
}


// Original code set the button even if node == nullptr.  Do we want that?
void
ColourFillController::setNodeFillColour()
{
    if (node != 0 || node != nullptr)
    {
	QColor colour = QColorDialog::getColor();
	QString s("background: " + colour.name() + "; " + BUTTON_STYLE);
	button->setStyleSheet(s);
	node->setFillColour(colour);

    }
}



void
ColourFillController::deleteButton()
{
    delete button;
}

/*
 * File:    colourfillcontroller.h
 * Author:  Rachel Bood 100088769
 * Date:    2014 (?)
 * Version: 1.1
 *
 * Purpose:
 *
 * Modification history:
 * Jul 9, 2020 (IC V1.1)
 *  (a) #include defuns.h to get BUTTON_STYLE.
 */

#ifndef COLORFILLCONTROLLER_H
#define COLORFILLCONTROLLER_H
#include "node.h"
#include "defuns.h"

#include <QPushButton>
#include <QObject>

class ColorFillController : public QObject
{
    Q_OBJECT
public:
    ColorFillController(Node * aNode, QPushButton * aButton);

private slots:
    void setNodeFillColor();
    void deleteButton();

private:
    Node * node;
    QPushButton * button;
};

#endif // COLORFILLCONTROLLER_H

/*
 * File:    colourlinecontroller.h
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

#ifndef EDGECOLORCONTROLLER_H
#define EDGECOLORCONTROLLER_H
#include "edge.h"
#include "node.h"
#include "defuns.h"

#include <QPushButton>
#include <QObject>

class ColorLineController: public QObject
{
    Q_OBJECT

public:
    ColorLineController(Edge * anEdge, QPushButton * aButton);
    ColorLineController(Node * anNode, QPushButton * aButton);

private slots:
    void setEdgeLineColor();
    void setNodeOutlineColor();
    void deleteButton();

private:
    Edge * edge;
    Node * node;
    QPushButton * button;
};

#endif // EDGECOLORCONTROLLER_H

#ifndef CANVASSCENE_H
#define CANVASSCENE_H

/*
 * File:	canvasscene.h
 * Author:	Rachel Bood
 * Date:	?
 * Version:	1.0
 *
 * Purpose:
 *
 * Modification history:
 * Feb 3, 2016 (JD):
 *    Minor formatting changes/cleanups, add header comment.
 */

#include "node.h"
#include "mainwindow.h"

#include <QGraphicsScene>

class CanvasScene : public QGraphicsScene
{
public:

    typedef struct undoPositions
    {
        QPointF pos;
        Node * node;
    } undo_Node_Pos;

    CanvasScene();
    void isSnappedToGrid(bool snap);
    void getConnectionNodes();
    int getMode() const;
    void setCanvasMode(int mode);

protected:
    void dragMoveEvent (QGraphicsSceneDragDropEvent * event);
    void dropEvent (QGraphicsSceneDragDropEvent * event);
    void drawBackground(QPainter * painter, const QRectF &rect);
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    int numOfNodes, modeType;
    bool snapToGrid;
    const QSize mCellSize;		// The size of the cells in the grid.
    QGraphicsItem * mDragged;		// The item being dragged.
    Node * connectNode1a, * connectNode1b; // The first Nodes to be joined.
    Node * connectNode2a, * connectNode2b; // The second Nodes to be joined.
    QPointF mDragOffset;
    QList<undo_Node_Pos *> undoPositions;
    // The distance from the top left of the item to the mouse position.
};

#endif // CANVASSCENE_H

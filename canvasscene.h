
#include "node.h"
#include <QGraphicsScene>

#ifndef CANVASSCENE_H
#define CANVASSCENE_H

#include"mainwindow.h"


class CanvasScene : public QGraphicsScene
{
public:

    typedef struct undoPositions
    {
        QPointF pos;
        Node * node;
    }undo_Node_Pos;

    CanvasScene();
    void isSnappedToGrid(bool snap);
    void getConnectionNodes();
    int getMode() const;
    void setCanvasMode(int mode);

protected:
    void dragMoveEvent ( QGraphicsSceneDragDropEvent * event );
    void dropEvent ( QGraphicsSceneDragDropEvent * event );
    void drawBackground(QPainter * painter, const QRectF &rect);
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    int numOfNodes, modeType;
    //QRectF sRect;
    bool snapToGrid;
    const QSize mCellSize; // The size of the cells in the grid.
    QGraphicsItem *mDragged;  // The item being dragged.
    Node * connectNode1a, *connectNode1b; //first Node to be joined
    Node * connectNode2a, *connectNode2b; //second Node to be joined
    QPointF mDragOffset;
    QList<undo_Node_Pos *> undoPositions;
    // The distance from the top left of the item to the mouse position.
};

#endif // CANVASSCENE_H

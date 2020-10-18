/*
 * File:    cornergrabber.cpp
 * Author:  Rachel Bood
 * Date:    2014 (?)
 * Version: 1.1
 *
 * Purpose: If only we knew.
 *
 * Modification history:
 * Oct 18, 2020 (JD V1.1):
 *  (a) Add header comment.  Fix spelling.  Tidy code a bit.
 */

#ifndef CORNERGRABBER_H
#define CORNERGRABBER_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QPointF>



class CornerGrabber : public QGraphicsItem
{
  public:
    explicit CornerGrabber(QGraphicsItem * parent = 0,  int corner = 0);

    /// Allows the owner to find out which corner this is.
    int  getCorner();
    // Allows the owner to record the current mouse state.
    void setMouseState(int);
    // Allows the owner to get the current mouse state.
    int  getMouseState();

    qreal mouseDownX;
    qreal mouseDownY;

    // Define the mouse states.
    enum {kMouseReleased = 0, kMouseDown, kMouseMoving};

  private:
    // Must be re-implemented in this class to provide the dimensions
    // of the box to the QGraphicsView.
    virtual QRectF boundingRect() const;

    // Must be re-implemented here to paint the box on the paint-event.
    virtual void paint(QPainter * painter,
		       const QStyleOptionGraphicsItem * option,
		       QWidget * widget);
    // Must be re-implemented to handle mouse hover enter events.
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    // Must be re-implemented to handle mouse hover leave events.
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);

    // Once the hover event handlers are implemented in this class,
    // the mouse events must also be implemented because of
    // some linkage issue - apparently there is some connection
    // between the hover events and mouseMove/Press/Release
    // events which triggers a vtable issue.
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseMoveEvent(QGraphicsSceneDragDropEvent * event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mousePressEvent(QGraphicsSceneDragDropEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

    // The hover event handlers will toggle this between red and black.
    QColor _outerborderColour;
    // The pen is used to paint the red/black border.
    QPen _outerborderPen;

    qreal   _width;
    qreal   _height;

    // 0,1,2,3  - starting at x=0, y=0 and moving clockwise around the box.
    int _corner;

    int _mouseButtonState;
};

#endif // CORNERGRABBER_H

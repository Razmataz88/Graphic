#include "label.h"
#include <QTextCursor>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

class QEvent;
Label::Label(QGraphicsItem * parent)
{
    this->setParentItem(parent);
    setZValue(3);
    QFont font = this->font();
    QFont font1;

    font1.setFamily(QStringLiteral("cmmi10"));
    font1.setBold(false);
    font1.setWeight(50);


    this->setFont(font1);
    font = this->font();
    setTextInteractionFlags(Qt::TextEditorInteraction);

    if (parentItem() != nullptr)
        setPos(parentItem()->boundingRect().center().x() - boundingRect().width() / 2.,
               parentItem()->boundingRect().center().y() - boundingRect().height() / 2.);
}

void Label::setTextInteraction(bool on, bool selectAll)
{
    if(on && textInteractionFlags() == Qt::NoTextInteraction)
       {
           // switch on editor mode:
        qDebug() << textInteractionFlags();
           setTextInteractionFlags(Qt::TextEditorInteraction);
           qDebug() << textInteractionFlags();

           // manually do what a mouse click would do else:
           setFocus(Qt::MouseFocusReason); // this gives the item keyboard focus
           setSelected(true); // this ensures that itemChange() gets called when we click out of the item
           if(selectAll) // option to select the whole text (e.g. after creation of the TextItem)
           {
               QTextCursor c = textCursor();
               c.select(QTextCursor::Document);
               setTextCursor(c);
           }
       }
       else if(!on && textInteractionFlags() == Qt::TextEditorInteraction)
       {
           // turn off editor mode:
           setTextInteractionFlags(Qt::NoTextInteraction);
           // deselect text (else it keeps gray shade):
           QTextCursor c = this->textCursor();
           c.clearSelection();
           this->setTextCursor(c);
           clearFocus();
       }
   }


QVariant Label::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
//    if(change == QGraphicsItem::ItemSelectedChange) qDebug("itemChange '%s', selected=%s", this->toPlainText().toStdString().c_str(), value.toString().toStdString().c_str());
//    if(change == QGraphicsItem::ItemSelectedChange
//            && textInteractionFlags() != Qt::NoTextInteraction
//            && !value.toBool())
//    {
//        // item received SelectedChange event AND is in editor mode AND is about to be deselected:
//        setTextInteraction(false); // leave editor mode
//    }
    return QGraphicsTextItem::itemChange(change, value);
}

void Label::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "Double click on label";
    QGraphicsTextItem::mouseDoubleClickEvent(event);
}

void Label::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug("mouseClickEvent '%s'", this->toPlainText().toStdString().c_str());
//    if (event->button() == Qt::RightButton)
//    {
//        if(textInteractionFlags() == Qt::TextEditorInteraction)
//        {
//            // if editor mode is already on: pass double click events on to the editor:
//            QGraphicsTextItem::mousePressEvent(event);
//            return;
//        }

//        // if editor mode is off:
//        // 1. turn editor mode on and set selected and focused:
//        setTextInteraction(true);

//        // 2. send a single click to this QGraphicsTextItem (this will set the cursor to the mouse position):
//        // create a new mouse event with the same parameters as evt
//        QGraphicsSceneMouseEvent *click = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMousePress);
//        click->setButton(event->button());
//        click->setPos(event->pos());
//        QGraphicsTextItem::mousePressEvent(click);
//        delete click; // don't forget to delete the event
//    }
    QGraphicsTextItem::mousePressEvent(event);

}

void Label::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsTextItem::paint(painter, option, widget);
}


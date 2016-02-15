#include "label.h"
#include <QTextCursor>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QInputMethodEvent>

class QEvent;
Label::Label(QGraphicsItem * parent)
{
    this->setParentItem(parent);
    labelText = "";
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

void Label::setLabel(QString string)
{
    QRegExp re("\\d*");  // A digit (\d), zero or more times (*)

    labelText = string;
    QString htmlFormat = "";
    for (int i = 0; i < string.length(); i++)
    {
        if (re.exactMatch(string.at(i)))
            htmlFormat += "<font face=\"cmr10\">" + QString(string.at(i)) +"</font>";
        else
            htmlFormat += "<font face=\"cmmi10\">" + QString(string.at(i)) + "</font>";
    }
    this->setHtml(htmlFormat);
    if (parentItem() != nullptr)
        setPos(parentItem()->boundingRect().center().x() - boundingRect().width() / 2.,
               parentItem()->boundingRect().center().y() - boundingRect().height() / 2.);
}

void Label::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsTextItem::paint(painter, option, widget);
}




/*
 * File:	html-label.h	    formerly label.h
 * Author:	Rachel Bood
 * Date:	2014-??-??
 * Version:	1.1
 * 
 * Purpose:	Declare the functions relating to the HTML version of
 *		node and edge labels (i.e., the version of the strings
 *		displayed on the canvas).
 *
 * Modification history:
 * Nov 13, 2019 (JD V1.1)
 *  (a) Rename the class and files to emphasize that this is all about
 *	the HTML version of the label which is only used for display
 *	on the canvas (as opposed to the vertex and edge labels which
 *	are automatically assigned or input by the user, and saved to
 *	output .grhpc and .tikz files).    That is,
 *	Label -> HTML_Label , label.{h, cpp} -> html-label.{h, cpp}
 *	and labelText -> htmlLabelText .
 *  (b) Move my code to HTML-ize node labels from node.cpp to label.cpp.
 *	These labels may contain super- and sub-scripts.
 *  (c) Various and sundry minor formatting and comment additions.
 */

#ifndef HTML_LABEL_H
#define HTML_LABEL_H

#include <QGraphicsTextItem>

class HTML_Label : public QGraphicsTextItem
{
public:
    HTML_Label(QGraphicsItem * parent = 0);
    void setTextInteraction(bool on, bool selectAll = false);

    enum { Type = UserType + 4 };
    int type() const { return Type; }

    void setHtmlLabel(QString string);
    static QString strToHtml(QString str);

protected:
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
	       QWidget * widget);

private:
    QString htmlLabelText;
};

#endif // LABEL_H

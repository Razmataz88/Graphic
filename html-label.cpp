/*
 * File:	html-label.cpp	    Formerly label.cpp
 * Author:	Rachel Bood
 * Date:	2014-??-??
 * Version:	1.1
 * 
 * Purpose:	Implement the functions relating to node and edge labels.
 *		(Some places in the code use "weight" for "edge label".)
 *
 * Modification history:
 * Nov 13, 2019 (JD V1.1)
 *  (a) Rationalize the naming to clarify that this is the HTML
 *	version of a (node or edge) label which will be displayed on
 *	the canvas.  That is,
 *	Label -> HTML_Label , label.{h, cpp} -> html-label.{h, cpp}
 *	and labelText -> htmlLabelText .
 *  (b) Move (and improve) code to HTML-ize node labels, which may
 *	contain super- and sub-scripts, into this file (from node.cpp).
 *  (c) Various and sundry minor formatting and comment additions.
 */

#include "html-label.h"
#include <QTextCursor>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QInputMethodEvent>


//class QEvent;  JD: why was this here??


HTML_Label::HTML_Label(QGraphicsItem * parent)
{
    this->setParentItem(parent);
    htmlLabelText = "";
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
        setPos(parentItem()->boundingRect().center().x()
	       - boundingRect().width() / 2.,
               parentItem()->boundingRect().center().y()
	       - boundingRect().height() / 2.);
}



void
HTML_Label::setTextInteraction(bool on, bool selectAll)
{
    if (on && textInteractionFlags() == Qt::NoTextInteraction)
    {
	// Switch on editor mode:
        qDebug() << textInteractionFlags();
	setTextInteractionFlags(Qt::TextEditorInteraction);
	qDebug() << textInteractionFlags();

	// Manually do what a mouse click would do else:
	setFocus(Qt::MouseFocusReason); // This gives the item keyboard focus
	setSelected(true); // This ensures that itemChange() gets called when we click out of the item
	if(selectAll) // Option to select the whole text (e.g. after creation of the TextItem)
	{
	    QTextCursor c = textCursor();
	    c.select(QTextCursor::Document);
	    setTextCursor(c);
	}
    }
    else if (!on && textInteractionFlags() == Qt::TextEditorInteraction)
    {
	// Turn off editor mode:
	setTextInteractionFlags(Qt::NoTextInteraction);
	// Deselect text (else it keeps gray shade):
	QTextCursor c = this->textCursor();
	c.clearSelection();
	this->setTextCursor(c);
	clearFocus();
    }
}



void
HTML_Label::setHtmlLabel(QString string)
{
    QRegExp re("\\d*");  // A digit (\d), zero or more times (*)

    htmlLabelText = string;
    QString htmlFormat = "";
    for (int i = 0; i < string.length(); i++)
    {
        if (re.exactMatch(string.at(i)))  // Why not isdigit() ?
            htmlFormat += "<font face=\"cmr10\">" + QString(string.at(i))
		+ "</font>";
        else
            htmlFormat += "<font face=\"cmmi10\">" + QString(string.at(i))
		+ "</font>";
    }
    this->setHtml(htmlFormat);

    if (parentItem() != nullptr)
        setPos(parentItem()->boundingRect().center().x()
	       - boundingRect().width() / 2.,
               parentItem()->boundingRect().center().y()
	       - boundingRect().height() / 2.);
}



void
HTML_Label::paint(QPainter * painter,
		       const QStyleOptionGraphicsItem * option,
		       QWidget * widget)
{
    QGraphicsTextItem::paint(painter, option, widget);
}



/*
 * Name:	mathFontify()
 * Purpose:	Take a (non-HTMLized) string and create a new string
 *		with appropriate font tags to display the string on
 *		the Qt canvas in "math mode".
 * Arguments:	A QString to be fontified.
 * Outputs:	Nothing.
 * Modifies:	Nothing.
 * Returns:	A QString with the fontified version of the original.
 * Assumptions:	None.
 * Bugs:	?!
 * Notes:	When TeX displays a math formula it uses cmmi, but
 *		uses the cmr for digits (cmmi contains "old style"
 *		digits).  Unfortunately Qt's HTML display doesn't
 *		duplicate this, so this function handraulically
 *		fontifies each char of the string.
 */

#define     CMR	    "<font face=\"cmr10\">"
#define     CMMI    "<font face=\"cmmi10\">"
#define     FONTEND "</font>"

QString
mathFontify(QString str)
{
    QString htmlMathStr = "";
    int len = str.length();
    bool prevWasDigit;

    if (len == 0)
	return htmlMathStr;

    if (str[0].isDigit())
    {
	htmlMathStr += CMR + str[0];
	prevWasDigit = true;
    }
    else
    {
	htmlMathStr += CMMI + str[0];
	prevWasDigit = false;
    }

    for (int i = 1; i < len; i++)
    {
	bool thisIsDigit = str[i].isDigit();

	if (thisIsDigit == prevWasDigit)
	    htmlMathStr += str[i];
	else
	{
	    htmlMathStr += FONTEND;
	    if (thisIsDigit)
	    {
		htmlMathStr += CMR + str[i];
		prevWasDigit = true;
	    }
	    else
	    {
		htmlMathStr += CMMI + str[i];
		prevWasDigit = false;
	    }
	}
    }

    htmlMathStr += FONTEND;
#ifdef DEBUG
    printf("mathFontify(%s) -> /%s/\n", str.toLatin1().data(),
	   htmlMathStr.toLatin1().data());
#endif
    return htmlMathStr;
}



/*
 * Name:	strToHtml()
 * Purpose:	Parse the arg string, turn it into HTML, return that text.
 * Arguments:	A hopefully-correct TeX-ish vertex label string.
 * Outputs:	Nothing.
 * Modifies:	Nothing.
 * Returns:	On success, the HTML text.  On failure, the empty string.
 * Assumptions:	The label string is syntactically valid.
 * Bugs:	Should return a success indication should parsing fail.
 *		Does not correctly fontify parts with letters and digits.
 * Notes:	Unlike in TeX, where digits in formula are printed
 *		using cmr10, in Qt if the font is cmmi10, the digits
 *		are printed in "old-style" digits.  This is a pain to
 *		work around.
 * To Do:	Correctly fontify parts with both letters and digits.
 */

QString
HTML_Label::strToHtml(QString str)
{
    QString base, rest, sub, sup, retval;
    char other_one;
    int first;
    int brCount, pos;
    int firstUnderscore = str.indexOf('_');
    int firstCircumflex = str.indexOf('^');
    QRegExp re("\\d*");

#ifdef DEBUG
    printf("\nstrToHtml(%s) called\n", str.toLocal8Bit().data());
    printf("\t firstUnderscore() = %d", firstUnderscore);
    printf("  firstCircumflex() = %d\n", firstCircumflex);
#endif

    // Trivial case: no superscript or subscript:
    if (firstUnderscore == -1 && firstCircumflex == -1)
    {
#ifdef DEBUG
	printf("\tstrToHtml(): trivial case, returning\n");
#endif
	return mathFontify(str);
    }

    if (firstUnderscore == -1)
	first = firstCircumflex;
    else if (firstCircumflex == -1)
	first = firstUnderscore;
    else
	first = firstUnderscore < firstCircumflex
	    ? firstUnderscore : firstCircumflex;
#ifdef DEBUG
    printf("first sub/sup at index %d\n", first);
#endif

    base = str.left(first);
    rest = str.mid(first + 1);
#ifdef DEBUG
    printf("base part of str is /%s/\n", (char *)base.toLocal8Bit().data());
    printf("rest of str is /%s/\n", (char *)rest.toLocal8Bit().data());
#endif

    // Use cmr10 iff the base is all digits.
    // TODO: call myself recursively here once I handle mixed
    // digits/letters in the "trivial" case above.
    if (re.exactMatch(base))
	base = "<font face=\"cmr10\">" + base + "</font>";
    else
	base = "<font face=\"cmmi10\">" + base + "</font>";

    int rest_len = rest.count();
    if (rest_len < 3 || rest.at(0) != '{')
    {
	printf("BOGUS rest PART!  (too short or doesn't start with '{')\n");
	return "";
    }

    // Find and recursively process the subscript and/or superscript.
    // Below we check for nullptr to see if we found that part.
    sub = nullptr;
    // This empty sup makes it match (a bit better) an empty sup in TeX.
    sup = "<sup></sup>";

    // Initial values: count and skip over the '{'
    for (pos = 1, brCount = 1; brCount > 0 && pos < rest_len; pos++)
    {
	if (rest.at(pos) == '}')
	    brCount--;
	else if (rest.at(pos) == '{')
	    brCount++;
#ifdef DEBUG
	printf("\trest[%d] = %c\n", pos, (char)rest.at(pos).toLatin1());
#endif
    }
    // pos is now 1 more than the index of the '}'

    if (brCount != 0)
    {
	// More '{'s than '}'s
	printf("BOGUS braces in rest\n");
	return "";
    }

    // Recursive call on sub/sup; [0] is '{', [pos-1] is '}'
    if (str.at(first) == '_')
    {
	sub = "<sub>" + strToHtml(rest.mid(1, pos - 2)) + "</sub>";
	other_one = '^';
    }
    else
    {
	sup = "<sup>" + strToHtml(rest.mid(1, pos - 2)) + "</sup>";
	other_one = '_';
    }

    // We now have processed "rest" up to index "pos - 1".
    // Repeat the above code, +/-, if there is more.  (Refactor this??)
#ifdef DEBUG
    printf("aaa pos = %d, rest=/%s/, rest_len = %d\n",
	   pos, rest.toLocal8Bit().data(), rest_len);
#endif
    if (pos < rest_len)
    {
	// Trim the previous sub/sup from the start of "rest".
	rest = rest.mid(pos);
#ifdef DEBUG
	printf("bbb pos = %d, rest=/%s/\n", pos, rest.toLocal8Bit().data());
#endif
	if (rest.at(0) != other_one)
	{
	    printf("Expected '%c' but saw '%c' in vertex label\n",
		   other_one, rest.at(0).toLatin1());
	    return "";
	}
	if (rest.at(1) != '{')
	{
	    printf("BOGUS char after '%c' in vertex label\n", other_one);
	    return "";
	}
	rest_len = rest.count();
	if (rest_len < 4)
	{
	    printf("BOGUS second sub/sup (too short)!\n");
	    return "";
	}

	// Initial values: count and skip over the '{'
	for (pos = 2, brCount = 1; brCount > 0 && pos < rest_len; pos++)
	{
	    if (rest.at(pos) == '}')
		brCount--;
	    else if (rest.at(pos) == '{')
		brCount++;
	}
	if (brCount != 0 || pos != rest_len)
	{
	    // More '{'s than '}'s or got to matching '}' before end of string.
	    printf("BOGUS braces in second sub/sup\n");
	    return "";
	}

	// All OK (?)
	if (other_one == '_')
	    sub = "<sub>" + strToHtml(rest.mid(2, rest_len - 3))
		+ "</sub>";
	else
	    sup = "<sup>" + strToHtml(rest.mid(2, rest_len - 3))
		+ "</sup>";
    }

    retval = base + sup + sub;
#ifdef DEBUG
    printf("returning /%s/\n", (char *)retval.toLocal8Bit().data());
#endif
    return retval;
}

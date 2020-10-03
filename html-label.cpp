/*
 * File:	html-label.cpp	    Formerly label.cpp
 * Author:	Rachel Bood
 * Date:	2014-??-??
 * Version:	1.5
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
 * Nov 13, 2019 (JD V1.2)
 *   - in setHtmlLabel() (currently only called for edges) replace the
 *     fontification code with strToHtml() so that edge labels can have
 *     super- and subscripts as well.  Add function comment to setHtmlLabel().
 * Nov 18, 2019 (JD V1.3)
 *  (a) Set default font size for labels, since up until now vertex labels
 *      get font size 9 unless they had labels set during the graph creation.
 *  (b) Remove spurious font variable in HTML_Label().
 *  (c) Fix some comments.
 * Nov 30, 2019 (JD V1.4)
 *  (a) Add qDeb() / #ifdef DEBUG jazz and a few debug outputs.
 * Jul 9, 2020 (IC V1.5)
 *  (a) Change the Z value of the HTML label to 5.  (JD Q: why?)
 */

#include "html-label.h"
#include <QTextCursor>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QInputMethodEvent>


// Debugging aids (without editing the source file):
#ifdef DEBUG
static const bool debug = true;
#else
static const bool debug = false;
#endif

// Like qDebug(), but a little more literal, and turn-offable:
#define qDeb if (debug) \
        QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE,  \
                       QT_MESSAGELOG_FUNC).debug().noquote().nospace




//class QEvent;  JD: why was this here??


HTML_Label::HTML_Label(QGraphicsItem * parent)
{
    qDeb() << "HTML_Label constructor called";

    this->setParentItem(parent);
    htmlLabelText = "";
    setZValue(3);
    QFont font;

    font.setFamily(QStringLiteral("cmmi10"));
    font.setBold(false);
    font.setWeight(50);
    font.setPointSize(12);
    this->setFont(font);
    setTextInteractionFlags(Qt::TextEditorInteraction);

    if (parentItem() != nullptr)
        setPos(parentItem()->boundingRect().center().x()
	       - boundingRect().width() / 2.,
               parentItem()->boundingRect().center().y()
	       - boundingRect().height() / 2.);
}



// Is this function ever called??

void
HTML_Label::setTextInteraction(bool on, bool selectAll)
{
    qDeb() << "HL::setTextInteraction(" << on << ", " << selectAll << ") called";

    if (on && textInteractionFlags() == Qt::NoTextInteraction)
    {
	// Switch on editor mode:
        qDeb() << textInteractionFlags();
	setTextInteractionFlags(Qt::TextEditorInteraction);
	qDeb() << textInteractionFlags();

	// Manually do what a mouse click would do else:
	setFocus(Qt::MouseFocusReason); // This gives the item keyboard focus
	setSelected(true); // This ensures that itemChange() gets called when we click out of the item
	if (selectAll) // Option to select the whole text (e.g. after creation of the TextItem)
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



/*
 * Name:	setHtmlLabel()
 * Purpose:	Create the htmlLabel from the plain text version.
 * Arguments:	The plain text label.
 * Outputs:	Nothing.
 * Modifies:	The edge's htmlLabel.
 * Returns:	Nothing.
 * Assumptions:	None.
 * Bugs:	None.
 * Notes:	None.
 */

void
HTML_Label::setHtmlLabel(QString string)
{
    qDeb() << "H:setHtmlLabel(" << string << ") called";

    this->setHtml(strToHtml(string));

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

    qDeb() << "mathFontify(" << str << " -> /" << htmlMathStr << "/";

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
 * Notes:	Qt doesn't properly display 2nd-level sub/sups.
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
    QRegExp digitsRegexp("\\d*");

#ifdef DEBUG
    printf("\nstrToHtml(%s) called\n", str.toLocal8Bit().data());
    printf("\t firstUnderscore() = %d", firstUnderscore);
    printf("  firstCircumflex() = %d\n", firstCircumflex);
#endif

    // Trivial case: no superscript or subscript:
    if (firstUnderscore == -1 && firstCircumflex == -1)
    {
	qDeb() <<"\tstrToHtml(): trivial case, returning\n";
	return mathFontify(str);
    }

    if (firstUnderscore == -1)
	first = firstCircumflex;
    else if (firstCircumflex == -1)
	first = firstUnderscore;
    else
	first = firstUnderscore < firstCircumflex
	    ? firstUnderscore : firstCircumflex;

    qDeb() << "first sub/sup at index " << first;

    base = str.left(first);
    rest = str.mid(first + 1);
#ifdef DEBUG
    printf("base part of str is /%s/\n", (char *)base.toLocal8Bit().data());
    printf("rest of str is /%s/\n", (char *)rest.toLocal8Bit().data());
#endif

    // Use cmr10 iff the base is all digits.
    // TODO: call myself recursively here once I handle mixed
    // digits/letters in the "trivial" case above.
    if (digitsRegexp.exactMatch(base))
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

	qDeb() << "\trest[" << pos << "%d] = " << rest.at(pos);
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

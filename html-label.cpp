/*
 * File:	html-label.cpp	    Formerly label.cpp
 * Author:	Rachel Bood
 * Date:	2014-??-??
 * Version:	1.9
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
 * Jul 29, 2020 (IC V1.6)
 *  (a) Added eventFilter() to receive canvas events so we can identify
 *      the node/edge being edited/looked at in the edit tab list.
 * Aug 18, 2020 (JD V1.7)
 *  (a) Changed strToHtml() so that a label it can't parse is now
 *	returned as a raw string set in cmtt10 (so the user can see
 *      it), rather than returning an empty string.
 * Aug 19, 2020 (IC V1.8)
 *  (a) Changed the default label font from cmmi10 to cmtt10.  This
 *	causes the labels to be displayed in cmtt10 while the label is
 *	being edited.
 *  (b) Updated eventFilter to display the label text in TeX formatting while
 *      editing on the canvas and to signal the node/edge when when the label
 *      loses keyboard focus, either by clicking somewhere else on the canvas
 *      or pressing escape/enter.
 * Aug 20, 2020 (JD, V1.9)
 *  (a) *Major* rework of the code to HTML-ize the labels.  This code
 *	handles far more complex labels, although
 *	(i) it certainly doesn't handle many TeX math constructs, and
 *	(ii) the HTML renderer in Qt (at least to 5.15.0) does not
 *	     (seem to) allow either subscripts and superscripts to be
 *	     vertically stacked, or sub-sub/sup-sup scripts.  The
 *	     latter is supported by HTML4, the former maybe not.
 *  (b) Remove the function "setTextInteraction()" which has not been
 *	called in many a year, if ever.
 *  (c) #include defuns.h to get debugging defns, remove them from here.
 */

#include "defuns.h"
#include "html-label.h"

#include <QTextCursor>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QInputMethodEvent>




HTML_Label::HTML_Label(QGraphicsItem * parent)
{
    qDeb() << "HTML_Label constructor called";

    this->setParentItem(parent);
    texLabelText = "";
    setZValue(5);

    QFont font;
    // Set default font to tt so that it stands out while editing the label.
    font.setFamily(QStringLiteral("cmtt10"));
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

    editTabLabel = nullptr;
    installEventFilter(this);
}


/*
 * Name:        eventFilter()
 * Purpose:     Intercepts events related to canvas labels so we can
 *              identify the location of the item on the edit tab and
 *              transfer text data between the nodes/edges and the labels.
 * Arguments:
 * Output:	Nothing.
 * Modifies:	The "editability" of the label.
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:       Try using QEvent::HoverEnter and QEvent::HoverLeave
 */

bool
HTML_Label::eventFilter(QObject * obj, QEvent * event)
{
    if (event->type() == QEvent::FocusIn)
    {
        // Embolden the header for this item's entry on the edit tab
        QFont font = editTabLabel->font();
        font.setBold(true);
        editTabLabel->setFont(font);

        // Now update the editable text on the canvas temporarily
        QString text = "<font face=\"cmtt10\">" + texLabelText + "</font>";
        setHtml(text);
    }
    else if (event->type() == QEvent::FocusOut)
    {
	// Undo the bold text
        QFont font = editTabLabel->font();
        font.setBold(false);
        editTabLabel->setFont(font);

        // Let the parent know to update and reformat the label text.
        emit editDone(toPlainText());
    }
    else if (event->type() == QEvent::KeyPress)
    {
        // Trigger the focus out if any of these keys are pressed.
        QKeyEvent * keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape)
            clearFocus();
        else if (keyEvent->key() == Qt::Key_Enter
		 || keyEvent->key() == Qt::Key_Return)
        {
            clearFocus();
	    // Returning true prevents the event from being further processed.
	    // Without this the "Return" would go into the label field.
	    return true;
        }
    }

    return QObject::eventFilter(obj, event);
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
    qDeb() << "HL:setHtmlLabel(" << string << ") called";

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



// All of the following code is for outputting labels in a TeX-ish way.
// HTML4 and Qt can't handle all of TeX math, but the code below makes
// relatively simple things look realistic.

// These are the fonts we need to render letters, digits, '{' and '}'.
#define     CMR	    "<font face=\"cmr10\">"
#define     CMMI    "<font face=\"cmmi10\">"
#define     CMSY    "<font face=\"cmsy10\">"
#define     FONTEND "</font>"

typedef enum { none, cmr, cmmi, cmsy } fontChoices;



/*
 * Name:	fontChoice
 * Purpose:	Take a char and return the font TeX would set it in
 *		(in a math formula).
 * Argument:	The char of interest
 * Outputs:	Nothing.
 * Modifies:	Nothing.
 * Returns:	A member of the fontChoices enum.
 * Assumptions:	None.
 * Bugs:	There are probably valid chars it doesn't yet know about.
 * Notes:	None.
 */

static fontChoices
fontChoice(char c)
{
    if (isdigit(c))
	return cmr;

    switch (c)
    {
      case '[':
      case ']':
      case '(':
      case ')':
      case ' ':
      case ';':
      case ':':
      case '+':
      case '=':
      case '-':		// cmsy char 0, which I can't do.  Use n-dash.
	return cmr;

      case '{':
      case '}':
	return cmsy;

      default:
	return cmmi;
    }
}


/*
 * Name:	mathFontify()
 * Purpose:	Take a (non-HTMLized) string and create a new string
 *		with appropriate font tags to display the string on
 *		the Qt canvas in "math mode".
 *		THIS DOES NOT HANDLE _ or ^.  THAT IS DONE ELSEWHERE.
 * Arguments:	A QString to be fontified.
 * Outputs:	Nothing.
 * Modifies:	Nothing.
 * Returns:	A QString with the fontified version of the original.
 * Assumptions:	
 * Bugs:	Does not work for non-"Local8Bit" chars in the string.
 * Notes:	When TeX displays a math formula it uses cmmi, but
 *		uses the cmr for digits (cmmi contains "old style"
 *		digits).  Unfortunately Qt's HTML display doesn't
 *		duplicate this, so this function handraulically
 *		fontifies each char of the string.
 *		This function is a crude approximation to what TeX
 *		does, in that it only implements basic math formula
 *		typesetting.  One thing it handles is throwing away
 *		braces which are not preceded by a '\'.
 *		Qt bug?  If the formula has a \{ and/or \}, the HTML
 *		text is raised too high.  Font issue or Qt?
 */

static QString
mathFontify(QByteArray chars)
{
    QString htmlMathStr = "";
    fontChoices thisFont;
    bool prevWasBS = false;		// BackSlash
    int len = chars.length();
    fontChoices prevFont = none;

    qDebu("HL:mathFontify(\"%s\") called", chars.data());

    if (len == 0)
	return htmlMathStr;

    for (int i = 0; i < len; i++)
    {
	char c = chars[i];
	if (c == '\\' && ! prevWasBS)
	{
	    prevWasBS = true;
	    continue;
	}

	thisFont = fontChoice(c);
	// TeX doesn't display braces if they are not preceded with '\'
	// Ditto for spaces in math formulae.
	if (c == '{' || c == '}' || c == ' ')
	{
	    if (! prevWasBS)
		continue;
	    // These chars are not in the expected place in cmsy10!
	    if (c == '{')
		c = 'f';
	    else if (c == '}')
		c = 'g';
	}

	// minus is char 0 in cmmi10, which I can't easily get into
	// the string.  Use cmr n-dash.
	if (c == '-')
	    c = 0173;

	// These are in weird places in cmmi10
	if (c == ',')
	    c = 073;
	if (c == '.')
	    c = 072;

	// Change font, if necessary
	qDebu("  font(%c) = %d", c, thisFont);
	if (thisFont != prevFont)
	{
	    if (prevFont != none)
		htmlMathStr += FONTEND;
	    switch (thisFont)
	    {
	      case cmr:
		htmlMathStr += CMR;
		break;

	      case cmmi:
		htmlMathStr += CMMI;
		break;

	      case cmsy:
		htmlMathStr += CMSY;
		break;

	      default:
		fprintf(stderr, "unknown font %d in mathFontify()\n", thisFont);
		htmlMathStr += CMMI;
		break;
	    }
	    prevFont = thisFont;
	}
	if (c == '<')
	    htmlMathStr += "&lt;";
	else if (c == '>')
	    htmlMathStr += "&gt;";
	else if (c == ' ' && prevWasBS)
	    htmlMathStr += "&nbsp;";
	else
	    htmlMathStr += c;

	prevWasBS = false;
    }

    if (htmlMathStr.length() > 0)
	htmlMathStr += FONTEND;

    qDeb() << "mathFontify(" << chars << " -> /" << htmlMathStr << "/";

    return htmlMathStr;
}



/*
 * Name:	strToHtml2()
 * Purpose:	Parse the arg string, turn it into HTML, return that text.
 * Arguments:	A hopefully-correct TeX-ish vertex label string.
 * Outputs:	Nothing.
 * Modifies:	Nothing.
 * Returns:	If able to completely parse the arg string as proper TeX, 
 *		the HTML-ized text.  On failure, ideally return the
 *		empty string.
 * Assumptions:	The label string is not deviously invalid.
 * Bugs:	Should return a success indication should parsing fail.
 * Notes:	Algorithm:
 *		Case 1: no '^' or '_'
 *		        -> just call mathFontify()
 *		Case 2: '^' or '_' at position 0 (must be a recursive call)
 *		     2a: sub/sup is a single token
 *		        -> handle base directly, recursive call for rest
 *		     2b: sub/sub is a brace group
 *		        -> find & handle base directly, recursive call for rest
 *		Case 3: first '^' or '_' not at position 0, at depth 0
 *		        -> find text before ^/_, recursive call
 *			-> recursive call on rest (including the ^/_)
 *		Case 4: first '^' or '_' not at position 0, not at depth 0
 *			-> mathfontify() any text before first '{'
 *			-> find braced text, call strToHtml2() on
 *			   text inside braces
 *			-> call strToHtml2() on remaining text
 */

static QString
strToHtml2(QByteArray chars)
{
    QByteArray prefix, rest;
    int firstUnderscore = chars.indexOf('_');
    int firstCircumflex = chars.indexOf('^');
    int length = chars.length();
    int first, depth, end;
    QString result = "";
    QString closeTag;

    qDebu("HL:strToHtml2(%s) called", chars.data());
    qDebu("  firstUnderscore() = %d", firstUnderscore);
    qDebu("  firstCircumflex() = %d", firstCircumflex);
    
    // Case 1 (trivial): no superscript or subscript:
    if (firstUnderscore == -1 && firstCircumflex == -1)
    {
	qDeb() <<"  strToHtml2(): trivial case, returning mathFontify()";
	return mathFontify(chars);
    }

    if (firstUnderscore == -1)
	first = firstCircumflex;
    else if (firstCircumflex == -1)
	first = firstUnderscore;
    else
	first = firstUnderscore < firstCircumflex
	    ? firstUnderscore : firstCircumflex;
    qDebu("  .. first = %d", first);

    // Case 2: If first == 0, this means strToHtml2() has been called
    // recursively to deal with the rest of the string, after the text
    // before the ^/_ has already been deal with.  We must find the
    // sub/sup, HTMLize that, and then deal with the rest of the string.
    if (first == 0)
    {
	if (chars[1] != '{')
	{
	    // Case 2a: sub/sup is just a single token
	    // FOR NOW JUST HANDLE A SINGLE CHAR HERE
	    qDebu("   Case 2a: script is '%s'", chars.mid(1, 1).data());
	    qDebu("   Case 2a: rest is '%s'", chars.mid(2).data());
	    if (chars[0] == '^')
		result += "<sup>" + mathFontify(chars.mid(1, 1)) + "</sup>";
	    else
		result += "<sub>" + mathFontify(chars.mid(1, 1)) + "</sub>";
	    result += strToHtml2(chars.mid(2));
	    return result;
	}

	// Case 2b: Find the sub/sup section in braces, strip the
	// braces and send it off for processing.
	qDebu("   Case 2b:");
	depth = 1;
	for (end = 2; end < length && depth > 0; end++)
	{
	qDebu("  --- looking at '%c' where end = %d", (char)chars[end], end);
	    if (chars[end] == '{' && (end == 0 || chars[end - 1] != '\\'))
		depth++;
	    else if (chars[end] == '}' && (end == 0 || chars[end - 1] != '\\'))
		depth--;
	}
	end--;
	qDebu(" .. case 2b: end is %d, brace section is '%s'",
	      end, chars.mid(1, end).data());
	if (chars[0] == '^')
	    result += "<sup>" + strToHtml2(chars.mid(2, end - 2)) + "</sup>";
	else
	    result += "<sub>" + strToHtml2(chars.mid(2, end - 2)) + "</sub>";

	qDebu(" -.-. rest is '%s'", chars.mid(end + 1).data());
	if (end < length - 1)
	    result += strToHtml2(chars.mid(end + 1));

	return result;
    }

    // If we get here, there is at least one subscript or superscript.
    // Either the first ^/_ is at depth 0 or it is not.
    depth = 0;
    for (int i = 0; i < first; i++)
    {
	if (chars[i] == '{' && (i == 0 || chars[i - 1] != '\\'))
	    depth++;
	else if (chars[i] == '}' && (i == 0 || chars[i - 1] != '\\'))
	    depth--;
    }
    qDebu("  depth of first ^/_ is %d", depth);

    if (depth == 0)
    {
	// Case 3: look for any text before first ^/_, deal with it.
	//         Then do a recursive call on the rest.
	qDebu("  Case 3: first ^/_ at depth 0");
	qDebu("     text before ^/_ is '%s'", chars.left(first).data());
	result += strToHtml2(chars.left(first));
	result += strToHtml2(chars.mid(first));
	return result;
    }

    qDebu("  Case 4:  first ^/_ NOT at depth 0");
    // Break the string into three parts:
    // (a) prefix before first '{' (this could not be empty)
    // (b) the balanced {...} text
    // (c) possibly-empty suffix following the {...} text

    int firstBrace = 0;
    while (chars[firstBrace] != '{'
	   || (firstBrace != 0 && chars[firstBrace - 1] == '\\'))
	firstBrace++;
    qDebu("  ... firstBrace = %d", firstBrace);

    // (a) prefix
    if (firstBrace != 0)
	result += strToHtml2(chars.left(firstBrace));

    // (b) the balanced { ... } text
    depth = 1;
    for (end = firstBrace + 1; end < length && depth > 0; end++)
    {
	if (chars[end] == '{' && (end == 0 || chars[end - 1] != '\\'))
	    depth++;
	else if (chars[end] == '}' && (end == 0 || chars[end - 1] != '\\'))
	    depth--;
    }
    end--;
    qDebu(" .. case 4: end is %d, brace section is '%s'",
	      end, chars.mid(firstBrace + 1, end - firstBrace - 1).data());
    result += strToHtml2(chars.mid(firstBrace + 1, end - firstBrace - 1));

    // (c) The remaining text, if any.
    if (end < length - 1)
	result += strToHtml2(chars.mid(end + 1));

    return result;
}



/*
 * Name:	strToHtml()
 * Purpose:	Parse the arg string, turn it into HTML, return that text.
 * Arguments:	A hopefully-correct TeX-ish label string.
 * Outputs:	Nothing.
 * Modifies:	Nothing.
 * Returns:	If able to completely parse the arg string as proper TeX, 
 *		it returns the HTML-ized text.  On failure, the
 *		literal characters are set in cmtt10.
 * Assumptions:	The label string is not deviously invalid.
 * Bugs:	?
 * Notes:	Qt5 doesn't properly display 2nd-level sub/sups.
 */

QString
HTML_Label::strToHtml(QString str)
{
    QByteArray chars = str.toLocal8Bit();
    int length = chars.length();

    if (length == 0)
	return "";

    qDebu("\nHL:strToHtml(%s) called", chars.data());

    // Do some basic sanity checking
    bool bogusSubSup = false;
    int depth = 0;
    if (chars[0] == '{')
	depth = 1;
    else if (chars[0] == '}')
	depth = -1;
    if (chars[0] == '^' || chars[0] == '_')
	bogusSubSup = true;
    if (chars[length - 1] == '^' || chars[length - 1] == '_')
	bogusSubSup = true;
    for (int i = 1; i < length && depth >= 0 && !bogusSubSup; i++)
    {
	if (chars[i] == '{' && chars[i - 1] != '\\')
	    depth++;
	else if (chars[i] == '}' && chars[i - 1] != '\\')
	    depth--;

	if ((chars[i] == '^' || chars[i] == '_') && (chars[i - 1] == '{'))
	    bogusSubSup = true;
	if (i < length - 1 && (chars[i] == '^' || chars[i] == '_')
	    && (chars[i + 1] == '}'))
	    bogusSubSup = true;
    }
    if (depth != 0 || bogusSubSup)
	return "<font face=\"cmtt10\">" + str + "</font>";

    // The string wasn't empty. if strToHtml2() returns an empty
    // string, assume that Something Is Wrong.
    QString html = strToHtml2(chars);
    qDebu("  strToHtml() returns \"%s\"", html.toLocal8Bit().data());
    if (html.length() == 0)
	return "<font face=\"cmtt10\">" + str + "</font>";;

    // Success!
    return html;
}

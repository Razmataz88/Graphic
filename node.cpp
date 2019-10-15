/*
 * File:    node.cpp
 * Author:  Rachel Bood
 * Date:    2014/11/07
 * Version: 1.2
 *
 * Purpose: creates a node for the users graph
 *
 * Modification history:
 * Feb 8, 2016 (JD V1.1):
 *  (a) Modify setWeight(QString) so that it uses cmr10 if the string
 *      is numeric.  This function is (apparently) called by the
 *      callback in labelcontroller.cpp when a user edits a label.
 *      (Is it possible instead to test there, and, if so, should we?)
 *  (b) Drive-by typo and formatting corrections, general cleanup.
 *      Note that node.lSize is currently written to, never read.
 * Oct 13, 2019 (JD V1.2)
 *  (a) Remove unused lSize from here and node.h.
 *  (b) Add '_' and the subscript to label (when there is a subscript).
 *  (c) Formatting tweaking and comment improvements/additions.
 *      Deleted and/or commented out some spurious code.
 *  (d) Renamed "choose" to "penStyle".
 *  (e) Added code to parse vertex labels and create corresponding HTML
 *      for text->setHtml().  This code assumes (requires) that sub-
 *	and superscripts are enclosed in {}.
 *	TODO: the labels are now displayed on the canvas using cmmi10.
 *	      However, TeX outputs digits (at least sub and sup) in cmr,
 *	      so I need to go through this code and see if I can get
 *	      all the fonts correct in the HTML text.
 */

#include "edge.h"
#include "node.h"
#include "canvasview.h"
#include "preview.h"


#include <QTextDocument>
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QMessageBox>
#include <QDebug>
#include <QMimeData>
#include <QDrag>
#include <QtCore>

static const bool verbose = false;


/*
 * Name:        Node
 * Purpose:     Contructor for Node class.
 * Arguments:   canvas pointer, diameter of node, edge size
 * Output:      none
 * Modifies:    Private node variables.
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

Node::Node()
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    setZValue(2);
    nodeID = -1;
    penStyle = 0;	// What type of pen style to use when drawing outline.
    nodeDiameter = 1;
    edgeWeight = 1;     // UNUSED IN V 1.1.
    rotation = 0;
    text = new Label(this);
    setHandlesChildEvents(true);
    select = false;
    QScreen * screen = QGuiApplication::primaryScreen();
    logicalDotsPerInchX = screen->logicalDotsPerInchX();
    logicalDotsPerInchY = screen->logicalDotsPerInchY();
}



/*
 * Name:        addEdge
 * Purpose:     adds an Edge to the pointer QList of edges
 * Arguments:   an Edge pointer
 * Output:      Nothing.
 * Modifies:    The node's edgeList.
 * Returns:     Nothing.
 * Assumptions: edgeList is valid.
 * Bugs:        none...so far
 * Notes:       none
 */

void Node::addEdge(Edge * edge)
{
    edgeList << edge;
    edge->adjust();
}



/*
 * Name:        removeEdge()
 * Purpose:     Remove an edge from the edgelist.
 * Arguments:   Edge *
 * Output:      Nothing.
 * Modifies:    edgeList
 * Returns:     True if edge was removed, otherwise false.
 * Assumptions: edgeList is valid.
 * Bugs:        none
 * Notes:       none
 */

bool Node::removeEdge(Edge * edge)
{
    for (int i = 0; i < edgeList.length(); i++)
    {
        if (edgeList.at(i) == edge)
        {
            edgeList.removeAt(i);
            return true;
        }
    }

    return false;
}



/*
 * Name:        setDiameter()
 * Purpose:     Sets the size of the diameter of the node in "logical DPI".
 * Arguments:   qreal
 * Output:      Nothing.
 * Modifies:    nodeDiameter
 * Returns:     Nothing.
 * Assumptions: none
 * Bugs:        none
 * Notes:       The argument diameter is the diameter in inches, therefore
 *              the value must be converted back to pixels in order for the
 *              node to be drawn correctly.
 */

void Node::setDiameter(qreal diameter)
{
    nodeDiameter = diameter * logicalDotsPerInchX;
    update();
}



/*
 * Name:        getDiameter()
 * Purpose:     Returns diameter of the node (in inches)
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    Nothing.
 * Returns:     The node diameter, in inches.
 * Assumptions: none
 * Bugs:        none
 * Notes:       nodeDiamter is stored in pixels, it needs to be converted back
 *              to inches before it is returned.
 *              FUTURE WORK: create multiple getDiameter() functions to return
 *              different values....or create one function that will return
 *              the diameter in the desired unit of measurement.
 */

qreal Node::getDiameter()
{
    return nodeDiameter / logicalDotsPerInchX;
}



/*
 * Name:        setEdgeWeight()
 * Purpose:	TODO: ??? Why does a node have an edgeweight?
 * Arguments:
 * Output:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

void Node::setEdgeWeight(qreal aEdgeWeight)
{
    edgeWeight = aEdgeWeight;
    update();
}



/*
 * Name:        setRotation()
 * Purpose:     Sets the rotation of the node.
 * Arguments:   qreal
 * Output:      none
 * Modifies:    rotation
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void Node::setRotation(qreal aRotation)
{
   rotation = aRotation;
   QGraphicsItem::setRotation(aRotation);
}



/*
 * Name:        getRotation()
 * Purpose:     returns the rotation of the node
 * Arguments:   none
 * Output:      qreal
 * Modifies:    none
 * Returns:     rotation
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

qreal Node::getRotation()
{
    return rotation;
}



/*
 * Name:        setFillColour()
 * Purpose:     Sets the fill colour of the node.
 * Arguments:   QColor
 * Output:      Nothing.
 * Modifies:    nodeFill
 * Returns:     Nothing.
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void Node::setFillColour(QColor fColor)
{
    nodeFill = fColor;
    update();
}



/*
 * Name:        getFillColor()
 * Purpose:     Returns the fill colour of the node.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    Nothing.
 * Returns:     nodeFill.
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

QColor Node::getFillColour()
{
    return nodeFill;
}



/*
 * Name:        setLineColour()
 * Purpose:     Sets the outline colour of the node.
 * Arguments:   QColor
 * Output:      Nothing.
 * Modifies:    nodeLine
 * Returns:     Nothing.
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void Node::setLineColour(QColor lColor)
{
    nodeLine = lColor;
    update();
}



/*
 * Name:        getLineColor()
 * Purpose:     Returns the outline colour of the node.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    Nothing.
 * Returns:     nodeline
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

QColor Node::getLineColour()
{
    return nodeLine;
}



/*
 * Name:        findRootParent()
 * Purpose:     Finds the root parent of this node.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    Nothing.
 * Returns:     QGraphicsItem * root.
 * Assumptions: None.
 * Bugs:        none
 * Notes:       none
 */

QGraphicsItem * Node::findRootParent()
{
    QGraphicsItem * root = this->parentItem();
    while (root->parentItem() != 0 && root->parentItem() != nullptr)
        root = root->parentItem();

    return root;
}



/*
 * Name:        setID()
 * Purpose:     Sets the ID (i.e., the internal node number) of the node.
 * Arguments:   int
 * Output:      Nothing.
 * Modifies:    The nodeID of this node.
 * Returns:     Nothing.
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void Node::setID(int id)
{
    nodeID = id;
}



/*
 * Name:        getID()
 * Purpose:     Returns the ID of this node.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    Nothing.
 * Returns:     The int nodeID
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

int Node::getID()
{
    return nodeID;
}



/*
 * Name:        edges()
 * Purpose:     Returns the list of edges incident to this node.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    Nothing.
 * Returns:     QList<Edge *> edgeList
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

QList<Edge *> Node::edges() const
{
    return edgeList;
}



/*
 * Name:        setNodeLabel()
 * Purpose:     Sets the label of the node to a real number.
 * Arguments:   qreal
 * Output:      Nothing.
 * Modifies:    The text in the node label (both "text" and "label" fields).
 * Returns:     Nothing.
 * Assumptions: None.
 * Bugs:        None.
 * Notes:       None.
 */

void Node::setNodeLabel(qreal number)
{
    label = QString::number(number);
    Node::labelToHtml();
    text->setHtml("<font face=\"cmr10\">"
		  + QString::number(number) + "</font>");
}



/*
 * Name:        setNodeLabel(QString, qreal)
 * Purpose:     Sets the label of the node in the case where the label
 *		has a numeric subscript.
 * Arguments:   QString, qreal
 * Output:      Nothing.
 * Modifies:    The text in the node label (both "text" and "label" fields).
 * Returns:     Nothing.
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void Node::setNodeLabel(QString aLabel, qreal number)
{
    Node::setNodeLabel(aLabel, QString::number(number));
}



/*
 * Name:        setNodeLabel(QString, QString)
 * Purpose:     Sets the label of the node in the case where the label
 *		has a string subscript.
 * Arguments:   QString, QString
 * Output:      Nothing.
 * Modifies:    The text in the node label (both "text" and "label" fields).
 * Returns:     Nothing.
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void Node::setNodeLabel(QString aLabel, QString subscript)
{
    label = aLabel + "_{" + subscript + "}";//+ "^{a}";
    // qDebug() << "\nsetNodeLabel(QS, QS) set label to /" << label << "/";
    labelToHtml();
}



/*
 * Name:        setNodeLabel(QString)
 * Purpose:     Sets the label of the node.
 * Arguments:   QString
 * Output:      Nothing.
 * Modifies:    The text in the node label (both "text" and "label" fields).
 * Returns:     Nothing.
 * Assumptions: none
 * Bugs:        none
 * Notes:       This is (apparently) called from the labelcontroller.cpp
 *		callback, which doesn't distinguish between integer and
 *		string.  So do a test to choose the correct font.
 *		TODO: eh??
 */

void Node::setNodeLabel(QString aLabel)
{
    label = aLabel;
    labelToHtml();
}



/*
 * Name:	labelToHtml()
 * Purpose:	Call lbelToHtml2 to parse the label string, turn it
 *		into HTML, wrap it in cmmi10 font tags, and return that text.
 * Arguments:	None (uses this node's label).
 * Outputs:	Nothing.
 * Modifies:	This node's text field.
 * Returns:	Nothing.
 * Assumptions:
 * Bugs:	Should return a success indication.
 * Notes:	TeX outputs digits in math formula in cmr, and so if I
 *		want this to look extremely TeX-like I actually need to
 *		go around changing fonts depending on whether a char
 *		is a digit or a non-digit.  **sigh**
 *		TODO: something for another day; image exports will look
 *		
 */

void Node::labelToHtml()
{
    QString html = "<font face=\"cmmi10\">" + Node::strToHtml(label) + "</font>";
    text->setHtml(html);
    // printf("labelToHtml setting text to /%s/\n",
	//   (char *)html.toLocal8Bit().data());
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
 * Notes:
 */

QString Node::strToHtml(QString str)
{
    QString base, rest, sub, sup, retval;
    char other_one;
    int first;
    int brCount, pos;
    int firstUnderscore = str.indexOf('_');
    int firstCircumflex = str.indexOf('^');
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
	return str;
    }

    if (firstUnderscore == -1)
	first = firstCircumflex;
    else if (firstCircumflex == -1)
	first = firstUnderscore;
    else
	first = firstUnderscore < firstCircumflex
	    ? firstUnderscore : firstCircumflex;
#ifdef DEBUG
    printf("first = %d\n", first);
#endif

    base = str.left(first);
    rest = str.mid(first + 1);
#ifdef DEBUG
    printf("base part of str is /%s/\n", (char *)base.toLocal8Bit().data());
    printf("rest of str is /%s/\n", (char *)rest.toLocal8Bit().data());
#endif

    int rest_len = rest.count();
    if (rest_len < 3 || rest.at(0) != '{')
    {
	printf("BOGUS rest PART!  (too short or doesn't start with '{')\n");
	return "";
    }

    // Find and recursively process the subscript and/or superscript.
    // Below we check for nullptr to see if we found that part.
    sub = nullptr;
    sup = "<sup></sup>"; //TODO: see whether this makes it look more TeXish

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
	sub = "<sub>" + Node::strToHtml(rest.mid(1, pos - 2)) + "</sub>";
	other_one = '^';
    }
    else
    {
	sup = "<sup>" + Node::strToHtml(rest.mid(1, pos - 2)) + "</sup>";
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
	    printf("BOGUS repetition of '%c' in vertex label\n", other_one);
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
	    sub = "<sub>" + Node::strToHtml(rest.mid(2, rest_len - 3))
		+ "</sub>";
	else
	    sup = "<sup>" + Node::strToHtml(rest.mid(2, rest_len - 3))
		+ "</sup>";
    }

    retval = base + sup + sub;
#ifdef DEBUG
    printf("returning /%s/\n", (char *)retval.toLocal8Bit().data());
#endif
    return retval;
}



/*
 * Name:        setNodeLabel()
 * Purpose:     Sets the label of the node.
 * Arguments:   QString, qreal, QString
 * Output:      none
 * Modifies:    the text in the node's label
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       IS THIS CALLED FROM ANYWHERE?   NO?
 *		IS IT UP-TO-DATE?
 */

// Defunct?
//void Node::setNodeLabel(QString htmltext, qreal labelSize, QString aLabel)
//{
//    qDebug() << "setNodeLabel(QString, qreal, QString) called!";
//
//    label = aLabel;
//    text->setHtml(htmltext);
//    QFont font = text->font();
//    font.setPointSize(labelSize);
//    text->setFont(font);
//    lSize = labelSize;
//    update();
//}



/*
 * Name:        setNodeLabelSize()
 * Purpose:     Sets the font size of the node's label.
 * Arguments:   qreal
 * Output:      Nothing.
 * Modifies:    The node's text and fontsize.
 * Returns:     Nothing.
 * Assumptions: none
 * Bugs:        none
 * Notes:       Should this be renamed to be "setNodeFontSize()" ?
 */

void Node::setNodeLabelSize(qreal labelSize)
{
    QFont font = text->font();
    font.setPointSize(labelSize);
    text->setFont(font);
}



/*
 * Name:        getLabel()
 * Purpose:     Returns the label string for this node.
 * Arguments:   None.
 * Output:      QString
 * Modifies:    Nothing.
 * Returns:     The string contained in the label.
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

QString Node::getLabel() const
{
    return label;
}



/*
 * Name:        getLableSize()
 * Purpose:     Returns the font size of the label.
 * Arguments:   None.
 * Output:      Nothing/
 * Modifies:    Nothing.
 * Returns:     The font size as a qreal.
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

qreal Node::getLabelSize() const
{
    return text->font().pointSizeF();
}



/*
 * Name:        boundingRect()
 * Purpose:     Determines the bounding rectangle of the node.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    Nothing.
 * Returns:     QRectF
 * Assumptions: none
 * Bugs:        none
 * Notes:       TODO: Q: Is adjust some empirical fudge factor?
 */

QRectF Node::boundingRect() const
{
    qreal adjust = 2;

    return QRectF( (-1 * nodeDiameter / 2) - adjust,
                   (-1 * nodeDiameter / 2) - adjust,
                   nodeDiameter + 3 + adjust,
                   nodeDiameter + 3 + adjust);
}



/*
 * Name:        chosen()
 * Purpose:     An int used to determine how to draw the outline of the node.
 * Arguments:   integer
 * Output:      Nothing.
 * Modifies:    penStyle: the integer used in the Node::paint() function.
 * Returns:     Nothing.
 * Assumptions: none
 * Bugs:        none
 * Notes:       none.
 */

void Node::chosen(int pen_style)
{
    penStyle = pen_style;
    update();
}



/*
 * Name:        editLabel()
 * Purpose:     Change edit flags to specify if the label is editable.
 * Arguments:   boolean
 * Output:      Nothing.
 * Modifies:    ItemIsFocusable, ItemIsSelectable, setHandlesChildEvents flags
 * Returns:     Nothing.
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void Node::editLabel(bool edit)
{
    setHandlesChildEvents(!edit);
    text->setFlag(QGraphicsItem::ItemIsFocusable, edit);
    text->setFlag(ItemIsSelectable, edit);
}



/*
 * Name:        ~Node()
 * Purpose:     destructor function
 * Arguments:   none
 * Output:      none
 * Modifies:    none
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */
//Node::~Node()
//{
//    emit nodeDeleted();
//    foreach(Edge * edge, edgeList)
//       delete edge;

//    text->setParentItem(nullptr);
//    delete text;
//    text = nullptr;
//    setParentItem(nullptr);
//}



/*
 * Name:        nodeDeleted()
 * Purpose:     Originally: send a signal to controllers that the node
 *		is deleted.  Now, obviously none.
 * Arguments:   none
 * Output:      none
 * Modifies:    nothing
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void
Node::nodeDeleted()
{

}



/*
 * Name:        paint()
 * Purpose:     Paints a node.
 * Arguments:   QPainter *, QStyleOptionGraphicsItem *, QWidget *
 * Output:      A node drawing
 * Modifies:    The node drawing.
 * Returns:     Nothing.
 * Assumptions: none
 * Bugs:        none
 * Notes:       Currently only draws nodes as circles.
 */

void
Node::paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
	    QWidget * widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    QColor brushColor;

    brushColor = nodeFill;
    painter->setBrush(brushColor);

    QPen pen;

    if (penStyle == 1)
        pen.setStyle(Qt::DotLine);
    else if (penStyle == 2)
        pen.setStyle(Qt::DashLine);
    else
        pen.setStyle(Qt::SolidLine);

    pen.setColor(nodeLine);
    painter->setPen(pen);

    painter->drawEllipse(-1 * nodeDiameter / 2,
                         -1 * nodeDiameter / 2,
                         nodeDiameter, nodeDiameter);

    text->setPos(this->boundingRect().center().x()
		 - text->boundingRect().width() / 2.,
                 this->boundingRect().center().y()
                 - text->boundingRect().height() / 2.);
}



/*
 * Name:        itemChange()
 * Purpose:     Send a signal to the edge objects to re-adjust the location
 *              of the edges.
 * Arguments:   GraphicsItemChange, QVariant value
 * Output:      Nothing.
 * Modifies:    The node's edge list.
 * Returns:     A QVariant
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
      case ItemPositionHasChanged:
        if (parentItem() != 0)
        {
            if (parentItem()->type() == Graph::Type)
            {
                Graph * graph = qgraphicsitem_cast<Graph*>(parentItem());
                Graph * tempGraph = graph;
                graph = qgraphicsitem_cast<Graph*>(graph->getRootParent());
                this->setParentItem(nullptr);
                this->setParentItem(tempGraph);
            }
            if (verbose)
                qDebug() << "itemChange(): node does not have a graph "
			 << "item parent";
        }
        foreach (Edge * edge, edgeList)
            edge->adjust();
        break;

      case ItemRotationChange:
        foreach (Edge * edge, edgeList)
            edge->adjust();
        break;

      default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}


/*
 * Name:        mousePressEvent()
 * Purpose:
 * Arguments:	The event.
 * Output:
 * Modifies:
 * Returns:
 * Assumptions:
 */

void Node::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    select = true;
    QGraphicsItem::mousePressEvent(event);
}


/*
 * Name:       mouseReleaseEvent()
 * Purpose:
 * Arguments:
 * Output:
 * Modifies:
 * Returns:
 * Assumptions:
 */
void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    select = false;
    QGraphicsItem::mouseReleaseEvent(event);
}

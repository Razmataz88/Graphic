/*
 * File:    edge.cpp
 * Author:  Rachel Bood
 * Date:    2014/11/07
 * Version: 1.8
 *
 * Purpose: creates an edge for the users graph
 *
 * Modification history:
 * Feb 8, 2016 (JD):
 *  (a) Fix edge label font (cmmi and cmr were reversed).
 *  (b) Drive by formatting/typo/... cleanups.
 * Oct 11, 2019 (JD V1.2)
 *  (a) Minor formatting changes.
 * Oct 16, 2019 (JD V1.3)
 *  (a) setWeight() used to just set the label, but it also now sets
 *	the weight.  Even though setHtml() is never called, the label
 *	is showing up (on the canvas) in the appropriate font.  (How?)
 * Nov 13, 2019 (JD V1.4):
 *  (a) Clean up / add some comments.
 *  (b) Apply change of name Label -> HTML_Label.
 *  (c) Apply change of name setLabel() -> set_Html_Label().
 * Nov 13, 2019 (JD V1.5):
 *  (a) Rename (in this order!)
 *	       label -> htmlLabel,
 *	       setWeightLabelSize() -> setLabelSize(),
 *             getWeightLabelSize() -> getLabelSize(),
 *	       setWeight() -> setLabel(),
 *	       getWeight() -> getLabel(),
 *	       editWeight() -> editLabel(),
 *	       weight -> label,
 *	       esize -> labelSize,
 *     in order to rationalize the naming scheme.
 * Nov 30, 2019 (JD V1.6):
 *  (a) Add call to update() in setPenWidth() so that the canvas is
 *	updated with the new width.
 *	Ditto for setColour().  (These fix long-standing display bugs!)
 *  (b) Minor formatting tweaks and comment improvements.
 *  (c) Added #ifdef and friends so that debugging statement can be
 *	turned on and off without source code mods; just compile with
 *	DEBUG defined (e.g., env DEBUG=-DDEBUG make).
 *	NOTE NOTE NOTE: at this date there is a debug call to painter
 *	in Edge::paint() that draws a bounding box around edges,
 *	making it look like there are three edges instead of one.
 *  (d) Removed unused isDigits() function.
 *  (e) Made offset1 and offset2 local variables in
 *	createSelectionPolygon(), rather than having them as private
 *	class variables.  (Also changed edge.h).
 *  (f) Added bug notes in adjust() comment.
 *  (g) Removed some redundant calls to adjust().
 * Dec 1, 2019 (JD V1.7):
 *  (a) In the edge constructor set the radius values according to the
 *      given nodes.  Call adjust() at the end of the constructor,
 *	not before setting these (!).
 *  (b) Update the comment for adjust().
 * Dec 12, 2019 (JD V1.8):
 *  (a) The debug defns have now moved to defuns.h, which is now
 *	included by edge.h.
 */

#include "edge.h"
#include "node.h"
#include "canvasview.h"

#include <QTextDocument>
#include <math.h>
#include <QtCore>
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QMessageBox>
#include <QDebug>

static const double Pi = 3.14159265358979323846264338327950288419717;
static const double offset = 5;		// TO DO: what is this?




/*
 * Name:        Edge
 * Purpose:     Constructor for Edge class
 * Arguments:   two Nodes
 * Output:      Nothing.
 * Modifies:    private Edge variables
 * Returns:     A new edge.
 * Assumptions: The two args are valid nodes.
 * Bugs:        none
 * Notes:       none
 */

Edge::Edge(Node * sourceNode, Node * destNode)
{
    qDeb() << "Edge:Edge constructor called";

    setFlag(ItemIsSelectable);
    setFlag(ItemIsFocusable);
    setFlag(ItemSendsGeometryChanges);
    source = sourceNode;
    setZValue(0);
    dest = destNode;
    source->addEdge(this);
    dest->addEdge(this);
    penSize = 1;
    rotation = 0;
    label = "";
    destRadius = destNode->getDiameter() / 2.;
    sourceRadius = destNode->getDiameter() / 2.;
    setHandlesChildEvents(true);
    htmlLabel = new HTML_Label(this);
    htmlLabel->setPos((edgeLine.p2().rx() + edgeLine.p1().rx()) / 2.
		      - htmlLabel->boundingRect().width() / 2.,
		      (edgeLine.p2().ry() + edgeLine.p1().ry()) / 2.
		      - htmlLabel->boundingRect().height() / 2.);
    adjust();
}



/*
 * Name:        sourceNode()
 * Purpose:     Getter function for the sourceNode of the edge.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    Nothing.
 * Returns:     The "source" node of this edge.
 * Assumptions: None.
 * Bugs:        None.
 * Notes:       None.
 */

Node *
Edge::sourceNode() const
{
    return source;
}



/*
 * Name:        destNode()
 * Purpose:     Getter function for the destNode of an edge.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    Nothing.
 * Returns:     The "destination" node of this edge.
 * Assumptions: None.
 * Bugs:        None.
 * Notes:       None.
 */

Node *
Edge::destNode() const
{
    return dest;
}



/*
 * Name:        editLabel()
 * Purpose:     Sets flags so that the htmlLabel is editable.
 * Argument:    Boolean
 * Output:      Nothing.
 * Modifies:    ItemisFocusable flag, ItemIsFocusable flag,
 *		setHandlesChildEvents
 * Returns:     Nothign.
 * Assumptions: None.
 * Bugs:        None.
 * Notes:       None.
 */

void
Edge::editLabel(bool edit)
{
    qDeb() << "E::editLabel(" << edit << ") called";

    setHandlesChildEvents(!edit);
    htmlLabel->setFlag(QGraphicsItem::ItemIsFocusable, edit);
    htmlLabel->setFlag(ItemIsSelectable, edit);
}



/*
 * Name:        getRootParent()
 * Purpose:     Returns the root parent of the edge.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    Nothing.
 * Returns:     QGraphicsItem *
 * Assumptions: None.
 * Bugs:        None.
 * Notes:       None.
 */

QGraphicsItem *
Edge::getRootParent()
{
    QGraphicsItem * parent = this->parentItem();
    while (parent != nullptr)
        parent = parent->parentItem();
    return parent;
}



/*
 * Name:
 * Purpose:
 * Arguments:
 * Output:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:       *** edge destructor...work in progress****
 */
//Edge::~Edge()
//{
//    emit edgeDeleted();
//    this->sourceNode()->removeEdge(this);
//    this->destNode()->removeEdge(this);
//    htmlLabel->setParentItem(nullptr);
//    delete htmlLabel;
//    htmlLabel = nullptr;
//    setParentItem(nullptr);
//}



/*
 * Name:        setLabel()
 * Purpose:     Set the label and htmlLabel of an edge.
 * Argument:    QString
 * Output:      Nothing.
 * Modifies:    The edge's label and htmlLabel.
 * Returns:     Nothing.
 * Assumptions: None.
 * Bugs:        None.
 * Notes:       BOGUS ARCHAIC NOTES:
 *		Both the "unadorned" label and the HTML-ized htmlLabel are
 *		needed.  If the programmer tries to return the text in
 *		the htmlLabel it will return the text and QML/HTML tags
 *		used to style the text.
 */

void
Edge::setLabel(QString aLabel)
{
    htmlLabel->setHtmlLabel(aLabel);
    label = aLabel;

// TODO: why has this been commented out?  It seems we don't need it,
// at least if we are not using subscripts in edge labels/htmlLabels.
// Perhaps we don't need it for vertices either (in no sub/sup case).
//    QRegExp re("\\d*");  // A digit (\d), zero or more times (*)
//    label = aLabel;
//    if (re.exactMatch(aLabel))
//        htmlLabel->setHtml("<font face=\"cmr10\">" + aLabel + "</font>");
//    else
//        htmlLabel->setHtml("<font face=\"cmmi10\">" + aLabel + "</font>");
}



/*
 * Name:        getLabel()
 * Purpose:     Returns the (unadorned) edge label.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    None.
 * Returns:     QString label.
 * Assumptions: None.
 * Bugs:        None.
 * Notes:       None.
 */

QString
Edge::getLabel()
{
    return label;
}



/*
 * Name:        adjust()
 * Purpose:     Update the edge when (for example) the source or
 *		destination node changes location or size.  Also
 *		update its selection polygon, and notify Qt that
 *		its geometry has changed.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    Edge
 * Returns:     Nothing.
 * Assumptions: None.
 * Bugs:        BUG1?: TODO: Why do we compare length to 2*destRadius
 *		instead of destRadius + sourceRadius ?
 *		BUG2?: Debug statements show outputs like
 *		"l = 144.243 > dR*2 = 0.2"
 *		which suggests that the units are not the same.
 *		NOTE: until Dec 1/2019 destRadius (and maybe
 *		sourceRadius) were not updated when a node's size is
 *		changed via the "Edit Graph" tab (and maybe also when
 *		a "freestyle" edge is added to the graph), so those
 *		values may be irrelevant to the program anyway.  At
 *		least with BUG2 operational.  Was the purpose of the
 *		test to not draw an unseen (because it is "under" the
 *		nodes) edge?
 * Notes:       This function gets called a *lot*; thus its debug
 *		stmts might be mostly or wholly commented out.
 */

void
Edge::adjust()
{
    qDeb() << "E::adjust() called";

    if (!source || !dest)
        return;

    QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
    qreal length = line.length();

    prepareGeometryChange();

    if (length > destRadius * 2)
    {
        QPointF destEdgeOffset((line.dx() * destRadius) / length,
                               (line.dy() * destRadius) / length);
        QPointF sourceEdgeOffset((line.dx() * sourceRadius) / length,
                                 (line.dy() * sourceRadius) / length);
        sourcePoint = line.p1() + sourceEdgeOffset;
        destPoint = line.p2() - destEdgeOffset;
	//qDeb() << "    l = " << length << " > dR*2 = " << destRadius * 2
	//       << "; sP = " << sourcePoint << ", dP = " << destPoint;
    }
    else
    {
        sourcePoint = destPoint = line.p1();
	//qDeb() << "    l <= dR*2; sP = dP =" << sourcePoint;
    }
    edgeLine = line;
    createSelectionPolygon();
}



/*
 * Name:        setDestNode()
 * Purpose:     Stores the destination node to which the edge is incident,
 *		as well as the radius of that node.
 * Arguments:   Node *
 * Output:      Nothing.
 * Modifies:    The edge's destination node info.
 * Returns:     Nothing.
 * Assumptions: ?
 * Bugs:        ?
 * Notes:       None.
 */

void
Edge::setDestNode(Node * node)
{
    qDeb() << "E::setDestNode(node " << node->getLabel()
	   << ") setting dest rad to " << node->getDiameter() / 2;

    dest = node;
    setDestRadius(node->getDiameter() / 2.);
}



/*
 * Name:        setSourceNode()
 * Purpose:     Stores the source node to which the edge is incident,
 *		as well as the radius of that node.
 * Arguments:   Node *
 * Output:      Nothing.
 * Modifies:    The edge's source node info.
 * Returns:     Nothing.
 * Assumptions: ?
 * Bugs:        ?
 * Notes:       None.
 */

void
Edge::setSourceNode(Node * node)
{
    qDeb() << "E::setSourceNode(node " << node->getLabel()
	   << ") setting dest rad to " << node->getDiameter() / 2;

    source = node;
    setSourceRadius(node->getDiameter() / 2.);
}



/*
 * Name:        setDestRadius()
 * Purpose:     Stores the radius of the destination node.
 * Arguments:   A qreal.
 * Output:      Nothing.
 * Modifies:    The edge's destination node radius info.
 * Returns:     Nothing.
 * Assumptions: ?
 * Bugs:        ?
 * Notes:       None.
 */

void
Edge::setDestRadius(qreal aRadius)
{
    destRadius = aRadius;
    adjust();
}



/*
 * Name:        getDestRadius()
 * Purpose:     Returns the radius of the destination node.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    Nothing.
 * Returns:     A qreal, the destination node's radius.
 * Assumptions: ?
 * Bugs:        ?
 * Notes:       None.
 */

qreal
Edge::getDestRadius()
{
    return destRadius;
}



/*
 * Name:        setSourceRadius()
 * Purpose:     Stores the radius of the destination node.
 * Arguments:   A qreal.
 * Output:      Nothing.
 * Modifies:    The edge's source node radius info.
 * Returns:     Nothing.
 * Assumptions: ?
 * Bugs:        ?
 * Notes:       None.
 */

void
Edge::setSourceRadius(qreal aRadius)
{
    sourceRadius = aRadius;
    adjust();
}



/*
 * Name:        getSourceRadius()
 * Purpose:     Returns the radius of the source node.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    Nothing.
 * Returns:     A qreal, the destination node's radius.
 * Assumptions: ?
 * Bugs:        ?
 * Notes:       None.
 */

qreal
Edge::getSourceRadius()
{
    return sourceRadius;
}



/*
 * Name:        setPenWidth()
 * Purpose:     Sets the width (penSize) of the edge.
 * Arguments:   The new width.
 * Output:      Nothing.
 * Modifies:    The edge's penSize.
 * Returns:     Nothing.
 * Assumptions: ?
 * Bugs:        ?
 * Notes:       The method is labeled setPenWidth and not setEdgeWidth because
 *              penWidth is the naming convention used in Qt to draw a line.
 *              See paint() function for further details and implementation.
 */

void
Edge::setPenWidth(qreal aPenWidth)
{
    penSize = aPenWidth;
    update();
}



/*
 * Name:        getPenWidth()
 * Purpose:     Returns the width (penSize) of the edge.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    Nothing.
 * Returns:     A qreal, the penSize.
 * Assumptions: None.
 * Bugs:        None.
 * Notes:       The method is labeled getPenWidth and not getEdgeWidth because
 *              penWidth is the naming convention used in Qt to draw a line.
 *              See paint() function for further details and implementation.
 */

qreal
Edge::getPenWidth()
{
    return penSize;
}



/*
 * Name:	setRotation()
 * Purpose:	??
 * Arguments:
 * Output:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

void
Edge::setRotation(qreal aRotation)
{
    qDeb() << "E::setRotation(" << aRotation
	   << ") call on edge(" << source->getLabel() << ", "
	   << dest->getLabel() << ")";

    rotation = aRotation;
    QGraphicsItem::setRotation(aRotation);
}



/*
 * Name:	getRotation()
 * Purpose:	??
 * Arguments:
 * Output:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

qreal
Edge::getRotation()
{
    return rotation;
}



/*
 * Name:        setColour
 * Purpose:     Stores the colour of the edge in a QColor variable.
 * Arguments:   A QColor, the desired colour of this edge.
 * Output:      Nothing.
 * Modifies:    The node's edgeColour.
 * Returns:     Nothing.
 * Assumptions: None.
 * Bugs:        None?
 * Notes:       None.
 */

void
Edge::setColour(QColor colour)
{
    edgeColour = colour;
    update();
}



/*
 * Name:        getColour()
 * Purpose:     Returns the colour of an edge.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    Nothing.
 * Returns:     The edge's edgeColour.
 * Assumptions: None.
 * Bugs:        None.
 * Notes:       None.
 */

QColor
Edge::getColour()
{
    return edgeColour;
}



/*
 * Name:        setLabelSize()
 * Purpose:     Sets the font size of the edge label.
 * Arguments:   A qreal specifying the size, in points.
 * Output:      Nothing.
 * Modifies:    Both the attribute labelSize and the htmlLabel's font size.
 * Returns:     Nothing.
 * Assumptions: The font has been defined.
 * Bugs:        None.
 * Notes:       None.
 */

void
Edge::setLabelSize(qreal edgeLabelSize)
{
    QFont font = htmlLabel->font();
    font.setPointSize(edgeLabelSize);
    htmlLabel->setFont(font);
    labelSize = edgeLabelSize;
}



/*
 * Name:        getLabelSize()
 * Purpose:     Returns the font size of the edge label.
 * Arguments:   None
 * Output:      Nothing.
 * Modifies:    Nothing.
 * Returns:     The font size (in points) of the edge label.
 * Assumptions: None.
 * Bugs:        None.
 * Notes:       None/
 */

qreal
Edge::getLabelSize()
{
    return labelSize;
}



/*
 * Name:        boundingRect()
 * Purpose:     Sets the bounding rectangle of the edge.
 * Arguments:   none
 * Output:      none
 * Modifies:    boundingRect
 * Returns:     QRectF
 * Assumptions: ?
 * Bugs:        ?
 * Notes:       This is a function that MUST be implemented when creating a
 *              custom QGraphicsItem.  The issue with bounding
 *		Rectangles is that they do not rotate with the
 *		QGraphicsItem.  What I mean is, that should the user
 *		create an diagonal edge, the bounding rect WON'T be
 *		drawn with its length parallel to the edge.  This is
 *		an issue because the bounding rect represents the area
 *		that determines if the user has clicked and selected
 *		the edge.  Therefore a new function had to be created
 *		that updates the bounding rect so it is rotated and
 *		it's length is parallel to the edge length.
 *		Below is a *rough* drawing illustrating this idea.
 *              ISSUE:         SOLUTION:
 *              ------         -----
 *              | \  |          \ \ \
 *              |  \ |            \ \ \
 *              ------             -----
 */

QRectF
Edge::boundingRect() const
{
    if (!source || !dest)
        return QRectF();

    return selectionPolygon.boundingRect();
}



/*
 * Name:        shape()
 * Purpose:     Updates the drawing within the bounding rect.
 * Arguments:   None.
 * Output:      Nothing.
 * Modifies:    Nothing.
 * Returns:     A QPainterPath containing the selectionPolygon.
 * Assumptions: ?
 * Bugs:        ?
 * Notes:       This is an overloaded function provided by Qt.
 *		Although it is not explicitly called by any graph-ic
 *		code, it is nonetheless called every time the mouse
 *		moves within the preview graph's bounding box (as far
 *		as JD can tell this is the condition).  Not that JD
 *		sees why every mouse move should call this.
 *		It also is called sometimes when moving nodes around
 *		the canvas in "Edit" mode.
 */

QPainterPath
Edge::shape() const
{
    // qDeb() << "E::shape() called!";  // Way too much noise from this one!

    QPainterPath ret;
    ret.addPolygon(selectionPolygon);
    return ret;
}



/*
 * Name:        paint()
 * Purpose:     Paints an edge between two nodes.
 * Arguments:   QPainter * pointer, QStyleOptionGraphicsITem *, QWidget *
 * Output:      Renders an edge to canvasScene.
 * Modifies:    QGraphicsScene
 * Returns:     Nothing.
 * Assumptions: None.
 * Bugs:        None.
 * Notes:       QWidget * and QStyleOptionGraphicsItem * are not used in my
 *              implementation of this function.
 */

void
Edge::paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
	    QWidget * widget)
{
    // Q_UNUSED is used so compiler warnings won't pop up
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!source || !dest)
        return;

    QColor color = edgeColour;

    QLineF line(sourcePoint, destPoint);
    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;

    // Set the style and draw the line.
    QPen pen;
    pen.setColor(color);
    pen.setWidthF(penSize);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    painter->drawLine(line);
    edgeLine = line;

    // Debug statement to view the edge's bounding shape.
    if (debug)
        painter->drawPolygon(selectionPolygon);

    if (label.length() > 0)
    {
	htmlLabel->setPos((line.p2().rx() + line.p1().rx()) / 2.
			  - htmlLabel->boundingRect().width() / 2.,
			  (line.p2().ry() + line.p1().ry()) / 2.
			  - htmlLabel->boundingRect().height() / 2.);
    }
}



/*
 * Name:        edgeDeleted()
 * Purpose:     A signal emitted when an edge is deleted.
 * Arguments:   None
 * Output:      void
 * Modifies:    Nothing.
 * Returns:     Nothing.
 * Assumptions: None.
 * Bugs:        None.
 * Notes:       Currently signaling controllers to remove widgets from
 *		edit tab that were assigned to style this edge.  Since
 *		the edge is being deleted there is no reason to keep
 *		those widgets on the Edit tab.
 *		TO DO: should there be code here?
 */

void
Edge::edgeDeleted()
{

}



/*
 * Name:        createSelectionPolygon()
 * Purpose:     Constructs a boundingRect for edges where the length
 *              runs parallel and the width runs perpendicular to the edge.
 * Arguments:   QLineF
 * Output:      Nothing.
 * Modifies:    selectionPolygon
 * Returns:     Nothing.
 * Assumptions: None.
 * Bugs:        None.
 * Notes:       Called from E::adjust().
 *		Thus it is called so often than I commented out its
 *		debug output.
 */

void
Edge::createSelectionPolygon()
{
    // qDeb() << "E::createSelectionPolygon() called!";

    QPolygonF nPolygon;
    qreal radAngle = edgeLine.angle() * Pi / 180;

    qreal dx = offset * qSin(radAngle);
    qreal dy = offset * qCos(radAngle);

    QPointF offset1 = QPointF(dx, dy);
    QPointF offset2 = QPointF(-dx, -dy);
    nPolygon << edgeLine.p1() + offset1
             << edgeLine.p1() + offset2
             << edgeLine.p2() + offset2
             << edgeLine.p2() + offset1;

    selectionPolygon = nPolygon;
}

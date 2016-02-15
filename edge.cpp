/*
 * File:    edge.cpp
 * Author:  Rachel Bood
 * Date:    2014/11/07
 * Version: 1.1
 *
 * Purpose: creates an edge for the users graph
 * Modification history:
 * Feb 8, 2016 (JD):
 * (a) Fix edge label font (cmmi and cmr were reversed).
 * (b) Drive by formatting/typo/... cleanups.
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
static const bool verbose = false;

// TODO: need a better naming scheme so weights and labels aren't used
// synonymously.  Perhaps weight can only be for the thickness of
// edges and label refers to the text of the edge.



/*
 * Name:        Edge
 * Purpose:     Constructor for Edge class
 * Arguments:   two Nodes
 * Output:      none
 * Modifies:    private Edge variables
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

Edge::Edge(Node * sourceNode, Node * destNode)
{
    setFlag(ItemIsSelectable);
    setFlag(ItemIsFocusable);
    setFlag(ItemSendsGeometryChanges);
    source = sourceNode;
    setZValue(0);
    dest = destNode;
    source->addEdge(this);
    dest->addEdge(this);
    adjust();
    penSize = 1;
    rotation = 0;
    weight = "";
    destRadius = 1;     // Set arbitrarily
    sourceRadius = 1;
    setHandlesChildEvents(true);
    label = new Label(this);
    label->setPos((edgeLine.p2().rx() + edgeLine.p1().rx()) / 2.
		  - label->boundingRect().width() / 2.,
		  (edgeLine.p2().ry() + edgeLine.p1().ry()) / 2.
		  - label->boundingRect().height() / 2.);
}


/*
 * Name:        sourceNode()
 * Purpose:     Getter function for the sourceNode of the edge.
 * Arguments:   none
 * Output:      none
 * Modifies:    none
 * Returns:     Node *
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

Node * Edge::sourceNode() const
{
    return source;
}


/*
 * Name:        destNode
 * Purpose:     Getter function for the destNode.
 * Arguments:   none
 * Output:      none
 * Modifies:    none
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

Node * Edge::destNode() const
{
    return dest;
}


/*
 * Name:        editWeight()
 * Purpose:     Sets flags so that the label is editable.
 * Argument:    Boolean
 * Output:      none
 * Modifies:    ItemisFocusable flag, ItemIsFocusable flag,
 *		setHandlesChildEvents
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void Edge::editWeight(bool edit)
{
    setHandlesChildEvents(!edit);
    label->setFlag(QGraphicsItem::ItemIsFocusable, edit);
    label->setFlag(ItemIsSelectable, edit);
}


/*
 * Name:        getRootParent()
 * Purpose:     Returns the root parent of the edge.
 * Arguments:   none
 * Output:      QGraphicsItem *
 * Modifies:    none
 * Returns:     QGraphicsItem *
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

QGraphicsItem * Edge::getRootParent()
{
    QGraphicsItem * parent = this->parentItem();
    while (parent != nullptr && parent != 0)
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
//    label->setParentItem(nullptr);
//    delete label;
//    label = nullptr;
//    setParentItem(nullptr);
//}


/*
 * Name:        setWeight()
 * Purpose:     Set the weight and label of an edge.
 * Arguments:   QString
 * Output:      none
 * Modifies:    label, weight
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       Both the "unadorned" label and the HTML-ized label are
 *		needed.  If the programmer tries to return the text in
 *		the label it will return the text and QML/HTML tags
 *		used to style the text.
 */

void Edge::setWeight(QString aWeight)
{
    QRegExp re("\\d*");  // A digit (\d), zero or more times (*)

    weight = aWeight;
    if (re.exactMatch(aWeight))
        label->setHtml("<font face=\"cmr10\">" + aWeight + "</font>");
    else
        label->setHtml("<font face=\"cmmi10\">" + aWeight + "</font>");
}


/*
 * Name:        isDigits()
 * Purpose:     Checks if the string contains only digits.
 * Arguments:   String
 * Output:      Boolean
 * Modifies:    none
 * Returns:     True if string contains only digits otherwise false.
 * Assumptions: none
 * Bugs:        none
 * Notes:       TODO: IS THIS USED ANYWHERE?
 */

bool isDigits(const std::string &str)
{
    return str.find_first_not_of("0123456789") == std::string::npos;
}


/*
 * Name:        getWeight()
 * Purpose:     Returns the (unadorned) weight of the edge label.
 * Arguments:   none
 * Output:      none
 * Modifies:    none
 * Returns:     QString weight
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

QString Edge::getWeight()
{
    return weight;
}


/*
 * Name:        adjust()
 * Purpose:     Adjusts the QPointFs of the edge so it moves around with
 *              the node when the node is dragged.
 * Arguments:   none
 * Output:      none
 * Modifies:    Edge
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void Edge::adjust()
{
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
    }
    else
        sourcePoint = destPoint = line.p1();
    edgeLine = line;
    createSelectionPolygon();
}


/*
 * Name:        setDestNode()
 * Purpose:     Stores the destination node to which the edge is incident.
 * Arguments:   Node *
 * Output:      none
 * Modifies:    Node * dest
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void Edge::setDestNode(Node * node)
{
    dest = node;
    setDestRadius(node->getDiameter() / 2.);
    adjust();
}


/*
 * Name:        setSourceNode()
 * Purpose:     Stores the source Node to which the edge is incident.
 * Arguments:   Node *
 * Output:      none
 * Modifies:    Node * source
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void Edge::setSourceNode(Node * node)
{
    source = node;
    setSourceRadius(node->getDiameter() / 2.);
    adjust();
}


/*
 * Name:        setDestRadius()
 * Purpose:     Stores the radius of the destination node.
 * Arguments:   qreal
 * Output:      none
 * Modifies:    qreal destRadius
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void Edge::setDestRadius(qreal aRadius)
{
    destRadius = aRadius;
    adjust();
}


/*
 * Name:        getDestRadius()
 * Purpose:     Returns the radius of the destination node.
 * Arguments:   none
 * Output:      qreal
 * Modifies:    none
 * Returns:     qreal destRadius
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

qreal Edge::getDestRadius()
{
    return destRadius;
}


/*
 * Name:        setSourceRadius()
 * Purpose:     Stores the radius of the source node.
 * Arguments:   qreal
 * Output:      none
 * Modifies:    sourceRadius variable
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void Edge::setSourceRadius(qreal aRadius)
{
    sourceRadius = aRadius;
    adjust();
}


/*
 * Name:        getSourceRadius()
 * Purpose:     Returns the radius of the source node.
 * Arguments:   none
 * Output:      none
 * Modifies:    nothing
 * Returns:     qreal sourceRadius
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

qreal Edge::getSourceRadius()
{
    return sourceRadius;
}


/*
 * Name:        setPenWidth()
 * Purpose:     Sets the width (penSize) of the edge.
 * Arguments:   none
 * Output:      none
 * Modifies:    nothing
 * Returns:     qreal penSize
 * Assumptions: none
 * Bugs:        none
 * Notes:       The method is labeled setPenWidth and not setEdgeWidth because
 *              penWidth is the naming convention used in Qt to draw a line.
 *              See paint() function for further details and implementation.
 */

void Edge::setPenWidth(qreal aPenWidth)
{
    penSize = aPenWidth;
}


/*
 * Name:        getPenWidth()
 * Purpose:     Returns the width (penSize) of the edge.
 * Arguments:   none
 * Output:      none
 * Modifies:    nothing
 * Returns:     qreal penSize
 * Assumptions: none
 * Bugs:        none
 * Notes:       The method is labeled getPenWidth and not getEdgeWidth because
 *              penWidth is the naming convention used in Qt to draw a line.
 *              See paint() function for further details and implementation.
 */

qreal Edge::getPenWidth()
{
    return penSize;
}


/*
 * Name:     setRotation()
 * Purpose:
 * Arguments:
 * Output:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

void Edge::setRotation(qreal aRotation)
{
    rotation = aRotation;
    QGraphicsItem::setRotation(aRotation);
}


/*
 * Name:        getRotation()
 * Purpose:
 * Arguments:
 * Output:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

qreal Edge::getRotation()
{
    return rotation;
}


/*
 * Name:        setColour
 * Purpose:     Stores the colour of the edge in a QColor variable.
 * Arguments:   QColor
 * Output:      void
 * Modifies:    QColor edgeColour
 * Returns:     nothing
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void Edge::setColour(QColor colour)
{
    edgeColour = colour;
}


/*
 * Name:        getColour()
 * Purpose:     Returns the colour of an edge.
 * Arguments:   none
 * Output:      none
 * Modifies:    none
 * Returns:     QColor of the edge
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */
QColor Edge::getColour()
{
    return edgeColour;
}


/*
 * Name:        setWeightLabelSize()
 * Purpose:     Sets the font size of the edge label.
 * Arguments:   QLineF
 * Output:      void
 * Modifies:    selectionPolygon
 * Returns:     nothing
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void Edge::setWeightLabelSize(qreal edgeWeightLabelSize)
{
    QFont font = label->font();
    font.setPointSize(edgeWeightLabelSize);
    label->setFont(font);
    eSize = edgeWeightLabelSize;
}


/*
 * Name:        getWeightLabelSize()
 * Purpose:     Returns the font size of the edge label.
 * Arguments:   none
 * Output:      qreal
 * Modifies:    none
 * Returns:     The font size of the edge weight.
 * Assumptions: none
 * Bugs:        none
 * Notes:       FYI: Edge labels are often referred to as a weight...although
 *              the thickness of an edge is also referred to as a weight...
 */

qreal Edge::getWeightLabelSize()
{
    return eSize;
}


/*
 * Name:        boundingRect()
 * Purpose:     Sets the bounding rectangle of the edge.
 * Arguments:   none
 * Output:      none
 * Modifies:    boundingRect
 * Returns:     QRectF
 * Assumptions: none
 * Bugs:        none
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
 *              ISSUE:           SOLUTION:
 *              ------         -----
 *              | \  |          \ \ \
 *              |  \ |            \ \ \
 *              ------             -----
 */

QRectF Edge::boundingRect() const
{
    if (!source || !dest)
        return QRectF();

    return selectionPolygon.boundingRect();
}


/*
 * Name:        shape()
 * Purpose:     Updates the drawing within the bounding rect.
 * Arguments:   QLineF
 * Output:      nothing
 * Modifies:    The bounding rectangle of the edge.
 * Returns:     QPainterPath
 * Assumptions: none
 * Bugs:        none
 * Notes:       This is an overloaded function provided by Qt
 */

QPainterPath Edge::shape() const
{
    QPainterPath ret;
    ret.addPolygon(selectionPolygon);
    return ret;
}


/*
 * Name:        paint()
 * Purpose:     Paints an edge between two nodes.
 * Arguments:   QPainter pointer, QStyleOptionGraphicsITem *, QWidget*
 * Output:      Renders an edge to canvasScene.
 * Modifies:    QGraphicsScene
 * Returns:     nothing
 * Assumptions: none
 * Bugs:        none
 * Notes:       QWidget * and QStyleOptionGraphicsItem are not used in my
 *              implementation of this function.
 */

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem * option,
                 QWidget * widget)
{
    //Q_UNUSED is used so compiler warnings won't pop up
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
    if (verbose)
        painter->drawPolygon(selectionPolygon);

    if (weight.length() > 0)
    {
	label->setPos((line.p2().rx() + line.p1().rx()) / 2.
                      - label->boundingRect().width() / 2.,
		      (line.p2().ry() + line.p1().ry()) / 2.
                      - label->boundingRect().height() / 2.);
    }

}


/*
 * Name:        edgeDeleted()
 * Purpose:     A signal emitted when an edge is deleted.
 * Arguments:   None
 * Output:      void
 * Modifies:    none
 * Returns:     nothing
 * Assumptions: none
 * Bugs:        none
 * Notes:       Currently signaling controllers to remove widgets from
 *		edit tab that were assigned to style this edge.  Since
 *		the edge is being deleted there is no reason to keep
 *		those widgets on the Edit tab.
 *		TO DO: should there be code here?
 */

void Edge::edgeDeleted()
{

}


/*
 * Name:        createSelectionPoygon
 * Purpose:     Constructs a boundingRect for edges where the length
 *              runs parallel and the width runs perpendicular to the edge.
 * Arguments:   QLineF
 * Output:      void
 * Modifies:    selectionPolygon
 * Returns:     nothing
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

void Edge::createSelectionPolygon()
{
    QPolygonF nPolygon;
    qreal radAngle = edgeLine.angle() * Pi / 180;

    qreal dx = offset * qSin(radAngle);
    qreal dy = offset * qCos(radAngle);

    // TO DO: should offset1 and offset2 not be local variables here
    // and removed from edge.h?

    offset1 = QPointF(dx, dy);
    offset2 = QPointF(-dx, -dy);
    nPolygon << edgeLine.p1() + offset1
             << edgeLine.p1() + offset2
             << edgeLine.p2() + offset2
             << edgeLine.p2() + offset1;

    selectionPolygon = nPolygon;
}

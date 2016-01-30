/*
 * File:    node.cpp
 * Author:  Rachel Bood 100088769
 * Date:    2014/11/07
 * Version: 1.0
 *
 * Purpose: creates a node for the users graph
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

#define INIT_X  -10
#define INIT_Y  -10

static const bool verbose = false;

/*
 * Name:        Node
 * Purpose:     Contructor for Node class
 * Arguments:   canvas pointer, diameter of node, edge size
 * Output:      none
 * Modifies:    private node variables
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
    choose = 0;
    lSize = 12; //default font size
    nodeDiameter = 1;
    edgeWeight = 1;
    rotation = 0;
    lSize = 1;
    choose = 0;
    text = new Label(this);
    setHandlesChildEvents(true);
    select = false;
    QScreen *screen = QGuiApplication::primaryScreen();
    logicalDotsPerInchX = screen->logicalDotsPerInchX();
    logicalDotsPerInchY = screen->logicalDotsPerInchY();
}

/*
 * Name:        addEdge
 * Purpose:     adds an Edge to the pointer QList of edges
 * Arguments:   an Edge pointer
 * Output:      none
 * Modifies:    node
 * Returns:     none
 * Assumptions: none
 * Bugs:        none...so far
 * Notes:       none
 */
void Node::addEdge(Edge *edge)
{
    edgeList << edge;
    edge->adjust();
}

/*
 * Name:        removeEdge()
 * Purpose:     removes an edge from the edgelist
 * Arguments:   Edge *
 * Output:      boolean
 * Modifies:    edgeList
 * Returns:     true if edge was removed otherwise false
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */
bool Node::removeEdge(Edge *edge)
{
    for(int i = 0; i < edgeList.length(); i++)
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
 * Purpose:     sets the size of the diameter of the node
 * Arguments:   qreal
 * Output:      none
 * Modifies:    nodeDiameter
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       the argument diameter is the diameter in inches,  therefore
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
 * Purpose:     returns diameter of the node (in inches)
 * Arguments:   none
 * Output:      QColor
 * Modifies:    none
 * Returns:     nodeline
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
 * Purpose:
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
 * Purpose:     sets the rotation of the node
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
 * Purpose:     sets the fill colour of the node
 * Arguments:   QColor
 * Output:      none
 * Modifies:    nodeFill
 * Returns:     none
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
 * Purpose:     returns the fill colour of the node
 * Arguments:   none
 * Output:      QColor
 * Modifies:    none
 * Returns:     nodeline
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
 * Purpose:     sets the outline colour of the node
 * Arguments:   QColor
 * Output:      none
 * Modifies:    NodeLine
 * Returns:     none
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
 * Purpose:     returns the outline colour of the node
 * Arguments:   none
 * Output:      QColor
 * Modifies:    none
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
 * Purpose:     finds the root parent of the node
 * Arguments:   none
 * Output:      QGraphicsItem
 * Modifies:    none
 * Returns:     root
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */
QGraphicsItem *Node::findRootParent()
{
    QGraphicsItem * root = this->parentItem();
    while (root->parentItem() != 0 && root->parentItem() != nullptr)
        root = root->parentItem();

    return root;
}

/*
 * Name:        setID()
 * Purpose:     sets the ID of the node
 * Arguments:   int
 * Output:      none
 * Modifies:    none
 * Returns:     none
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
 * Purpose:     retuns the ID of the node
 * Arguments:   none
 * Output:      int
 * Modifies:    none
 * Returns:     nodeID
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
 * Purpose:     returns the list of edges incident to the node
 * Arguments:   none
 * Output:      QList<Edge *>
 * Modifies:    none
 * Returns:     edgeList
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
 * Purpose:     sets the label of the node
 * Arguments:   qreal
 * Output:      none
 * Modifies:    the text in the node label
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */
void Node::setNodeLabel(qreal number)
{
    text->setHtml("<font face=\"cmr10\">"
            + QString::number(number) + "</font>");
    label = QString::number(number);
}

/*
 * Name:        setNodeLabel()
 * Purpose:     sets the label of the node
 * Arguments:   QString, qreal,
 * Output:      none
 * Modifies:    the text in the node label
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */
void Node::setNodeLabel(QString aLabel, qreal number)
{
    label = aLabel;
    text->setHtml("<font face=\"cmmi10\">" + label
                  + "</font><sub><font face=\"cmr10\">"
                  + QString::number(number) + "</face></sub>");
}

/*
 * Name:        setNodeLabel()
 * Purpose:     sets the label of the node
 * Arguments:   QString,
 * Output:      none
 * Modifies:    the text in the node label
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */
void Node::setNodeLabel(QString aLabel)
{
    label = aLabel;
    text->setHtml("<font face=\"cmmi10\">" + label + "</font>");
}

/*
 * Name:        setNodeLabelSize()
 * Purpose:     sets the font size of the label node
 * Arguments:   QString, qreal, QString
 * Output:      none
 * Modifies:    the text in the node label
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */
void Node::setNodeLabelSize(qreal labelSize)
{
    QFont font = text->font();
    font.setPointSize(labelSize);
    text->setFont(font);
}

/*
 * Name:        setNodeLabel()
 * Purpose:     sets the label of the node
 * Arguments:   QString, qreal, QString
 * Output:      none
 * Modifies:    the text in the node's label
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */
void Node::setNodeLabel(QString htmltext, qreal labelSize, QString aLabel)
{
    label = aLabel;
    text->setHtml(htmltext);
    QFont font = text->font();
    font.setPointSize(labelSize);
    text->setFont(font);
    lSize = labelSize;
    update();
}

/*
 * Name:        getLabel()
 * Purpose:     retuns the string of the label
 * Arguments:   none
 * Output:      QString
 * Modifies:    none
 * Returns:     the string contained in the label
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
 * Purpose:     returns the font size of the label
 * Arguments:   none
 * Output:      qreal
 * Modifies:    none
 * Returns:     font size
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
 * Purpose:     determines the bounding rectangle of the node
 * Arguments:   none
 * Output:      QRectF
 * Modifies:    none
 * Returns:     QRectF
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
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
 * Purpose:     an int used to determine to draw the outline of the node
 * Arguments:   integer
 * Output:      none
 * Modifies:    choose
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       The integer used in the paint() function
 */
void Node::chosen(int group)
{
    choose = group;
    update();
}

/*
 * Name:        edtLabel()
 * Purpose:     a function that will change edit flags to determine if the
 *              label is editable
 * Arguments:   boolean
 * Output:      none
 * Modifies:    ItemIsFocusable, ItemIsSelectable, setHandlesChildEvents flags
 * Returns:     none
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
 * Name:        nodeDelted()
 * Purpose:     sends a signal to controllers that the node is deleted
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
 * Purpose:     paints a node
 * Arguments:   QPainter*, QStyleOptionGraphicsItem*, QWidget*
 * Output:      a node drawing
 * Modifies:    the node drawing
 * Returns:     nothing
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
    if (isSelected())
        pen.setStyle(Qt::DotLine);
    else
        pen.setStyle(Qt::SolidLine);

    if (choose == 1)
        pen.setStyle(Qt::DotLine);
    else if (choose == 2)
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
 * Purpose:     sends a signal to the edge objects to re-adjust the location
 *              of the edges
 * Arguments:   GraphicsItemChange, QVariant value
 * Output:      none
 * Modifies:    the edgelist
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */
QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
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
            {
                qDebug()<< "node does not have a graph item parent " << endl;
            }
        }
        foreach (Edge *edge, edgeList)
            edge->adjust();
        break;
    case ItemRotationChange	:
        foreach (Edge *edge, edgeList)
            edge->adjust();
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

/*
 * Name:        MousePressEvent()
 * Purpose:
 * Arguments:
 * Output:
 * Modifies:
 * Returns:
 * Assumptions:
 */
void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
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
void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
select = false;
QGraphicsItem::mouseReleaseEvent(event);
}

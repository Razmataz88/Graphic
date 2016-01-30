#include "basicgraphs.h"

#include <qmath.h>
#include "node.h"
#include "edge.h"
#include <QDebug>

static const double PI = 3.14159265358979323846264338327950288419717;

static const bool verbose = false; /* for debugging purposes */

//TODO: Use golden ratio in graphs with two cycles

BasicGraphs::BasicGraphs()
{
    Graph_Type_Name = { "None", "Antiprism", "Balanced Binary Tree", "Bipartite",
                        "Crown", "Cycle", "Dutch Windmill", "Gear (generalized)",
                         "Grid", "Helm", "Path", "Petersen (generalized)",
                        "Prism", "Round", "Star", "Wheel"};
}

/*
* Function:	generate_wheel()
* Paramters:	item: the graph
*		height: the desired height of the graph (note that
*		        many other functions get height / 2 as the
*			corresponding parameter)
*		width:the desired width of the graph
*		numOfNodes: the number of vertices in the graph
*		complete: if True, draw the edges
* Output:      none
* Modifies:    item
* Returns:     void
* Assumptions: none
* Bugs:        none
* Notes:       none
*/

void BasicGraphs::generate_wheel(Graph *item, qreal width, qreal height,
                                  int numOfNodes, bool complete)
{
    if (numOfNodes > 0)
    {
        item->nodes.cycle = create_cycle(item, width,height, numOfNodes - 1);
        Node * node = new Node();
        node->setPos(0, 0);
        item->nodes.center = node;
        node->setParentItem(item);
        if (complete)
            for (int i = 0; i < item->nodes.cycle.count(); i++)
            {
                Edge * edge = new Edge(node, item->nodes.cycle.at(i));
                edge->setParentItem(item);
                edge = new Edge(item->nodes.cycle.at(i),
                                item->nodes.cycle.at((i + 1)
                               % item->nodes.cycle.count()));
                edge->setParentItem(item);
            }
    }
    return;
}

void BasicGraphs::generate_bipartite(Graph * item, int topNodes,
                                      int bottomNodes, qreal height,
                                      qreal width, bool complete)
{
    /*
     * calculates the space between each of the nodes within
     * the user's desired width of the graph
     */
    qreal topSpacing = (topNodes >= bottomNodes) ?
                width / (topNodes - 1) :
                width / (topNodes );
    qreal bottomSpacing = (bottomNodes >= topNodes) ?
                width / (bottomNodes - 1) :
                width / (bottomNodes );
    /*
     * Calculates the y coordinate of the bottom partition of the nodes
     * Note:A QGraphicsScene coordinate system has the origin
     *      in the center of the scene
     */


    qreal x = (topNodes >= bottomNodes) ?  width / -2. : width / -2.
                                           + topSpacing / 2.;
    qreal y = height / -2.;

    for (int i = 0; i < topNodes; i++)
    {
        Node * node = new Node();
        node->setPos(x, y);
        item->nodes.bipartite_top.append(node);
        node->setParentItem(item);
        x += topSpacing;
    }

    x = (topNodes <= bottomNodes) ?  width / -2. : width / -2.
                                     + bottomSpacing / 2.;
    y = height / 2.;
    for (int i = 0; i < bottomNodes; i++)
    {

        Node * node = new Node();
        node->setPos(x, y);
        item->nodes.bipartite_bottom.append(node);
        node->setParentItem(item);
         x += bottomSpacing;
    }

    if (complete)
    {
        for (int i = 0; i < topNodes; i++)
            for (int j = 0; j < bottomNodes; j++)
            {
                Edge * edge = new Edge(item->nodes.bipartite_top.at(i),
                                       item->nodes.bipartite_bottom.at(j));
                edge->setParentItem(item);
            }
    }
}

void BasicGraphs::generate_cycle(Graph *item, qreal width, qreal height,
                                  int numOfNodes, bool complete)
{
    item->nodes.cycle = create_cycle(item, width, height, numOfNodes);
    if (complete)
    {
        for (int i = 0; i < item->nodes.cycle.count(); i++)
        {
            Edge * edge = new Edge(item->nodes.cycle.at(i),
                                   item->nodes.cycle.at( (i + 1)
                                                         % item->nodes.cycle.count()));
            edge->setParentItem(item);
        }
    }
}

void BasicGraphs::generate_star(Graph *item, qreal width, qreal height,
                                 int numOfNodes, bool complete)
{
    if (numOfNodes > 0)
    {
        item->nodes.cycle = create_cycle(item, width, height, numOfNodes - 1);
        Node * node = new Node();
        node->setPos(0, 0);
        item->nodes.center = node;
        node->setParentItem(item);
        if (complete)
            for (int i = 0; i < item->nodes.cycle.count(); i++)
            {
                Edge * edge = new Edge(node, item->nodes.cycle.at(i));
                edge->setParentItem(item);
            }
    }
}

void BasicGraphs::generate_complete(Graph *item, qreal width, qreal height,
                                     int numOfNodes, bool complete)
{
    item->nodes.cycle = create_cycle(item, width, height, numOfNodes);
    if (complete)
        for (int i = 0; i < item->nodes.cycle.count(); i++)
            for (int j = i + 1; j < item->nodes.cycle.count(); j++)
            {
                Edge * edge = new Edge(item->nodes.cycle.at(i),
                                       item->nodes.cycle.at(j));
                edge->setParentItem(item);
            }
}

void BasicGraphs::generate_petersen(Graph *item, qreal width,
                     qreal height, int numOfNodes,
                     int starSkip, bool complete)
{
    item->nodes.double_cycle.append(create_cycle(item, width, height,
                         numOfNodes));
    item->nodes.double_cycle.append(create_cycle(item, width/2, height/2,
                         numOfNodes));
    if (complete)
    {
        for (int i = 0; i < numOfNodes; i++)
        {
           Edge * edge = new Edge(item->nodes.double_cycle.at(0).at(i),
                                   item->nodes.double_cycle.at(0).at((i + 1)
                                   % item->nodes.double_cycle.at(0).count()));
           edge->setParentItem(item);

            if (starSkip % numOfNodes != 0)
            {
                edge = new Edge(item->nodes.double_cycle.at(1).at(i),
                                item->nodes.double_cycle.at(1).at((i + starSkip)
                                                                  % numOfNodes));
               edge->setParentItem(item);
            }
            Edge * connectEdge = new Edge(item->nodes.double_cycle.at(0).at(i),
                                          item->nodes.double_cycle.at(1).at(i));
           connectEdge->setParentItem(item);
        }
    }
}

void BasicGraphs::generate_gear(Graph *item, qreal width, qreal height,
                                 int numOfNodes, bool complete)
{
    if (numOfNodes % 2 != 0)
        item->nodes.cycle = create_cycle(item, width, height, numOfNodes - 1);
    else
        item->nodes.cycle = create_cycle(item, width, height, numOfNodes);

    qreal x1, y1,x2,y2;
    for (int i = 0; i < item->nodes.cycle.count(); i++)
    {
        if (i % 2 == 1)
        {
            x1 = item->nodes.cycle.at((i - 1) % item->nodes.cycle.count())->x();
            y1 = item->nodes.cycle.at((i - 1) % item->nodes.cycle.count())->y();
            x2 = item->nodes.cycle.at((i + 1) % item->nodes.cycle.count())->x();
            y2 = item->nodes.cycle.at((i + 1) % item->nodes.cycle.count())->y();

            item->nodes.cycle.at(i)->setPos((x1 + x2) / 2, (y1 + y2) / 2);
        }
    }

    Node * center = new Node();
    if (numOfNodes % 2 == 1)
    {
        center->setPos(0, 0);
        center->setParentItem(item);
        item->nodes.center = center;
    }
    if (complete)
    {
        for (int i = 0; i < item->nodes.cycle.count(); i++)
        {
            if (i % 2 == 0 && numOfNodes % 2 == 1)
            {
                Edge * edge = new Edge(center, item->nodes.cycle.at(i));
                edge->setParentItem(item);
            }
            Edge * edge = new Edge(item->nodes.cycle.at(i),
                                   item->nodes.cycle.at((i + 1)
                                   % item->nodes.cycle.count()));
            edge->setParentItem(item);
        }
    }
}

void BasicGraphs::generate_helm(Graph *item, qreal width, qreal height,
                                 int numOfNodes, bool complete)
{
    item->nodes.double_cycle.append(create_cycle(item, width, height, numOfNodes));
    item->nodes.double_cycle.append(create_cycle(item, width * 0.65,
                                                 height * 0.65, numOfNodes));

    Node * node = new Node();
    node->setPos(0,0);
    node->setParentItem(item);
    item->nodes.center = node;
    if (complete)
    {
        for (int i = 0; i < item->nodes.double_cycle.at(0).count(); i++)
        {
            Edge * edge = new Edge(item->nodes.double_cycle.at(1).at(i),
                                   item->nodes.center);
           edge->setParentItem(item);
            edge = new Edge(item->nodes.double_cycle.at(1).at(i),
                            item->nodes.double_cycle.at(0).at(i));
            edge->setParentItem(item);
            edge = new Edge(item->nodes.double_cycle.at(1).at(i),
                            item->nodes.double_cycle.at(1).at((i + 1)
                             % item->nodes.double_cycle.at(1).count()));
            edge->setParentItem(item);
        }
    }
}

void BasicGraphs::generate_crown(Graph *item, qreal width, qreal height,
                                  int numOfNodes, bool complete)
{
    item->nodes.double_cycle.append(create_cycle(item, width, height
                                                 , numOfNodes));
    item->nodes.double_cycle.append(create_cycle(item, width * 0.65, height
                                                 * 0.65, numOfNodes));
    if (complete)
    {
        for (int i = 0; i < item->nodes.double_cycle.at(1).count(); i++)
        {
            Edge * edge = new Edge(item->nodes.double_cycle.at(0).at(i),
                                   item->nodes.double_cycle.at(1).at(i));
            edge->setParentItem(item);
            edge = new Edge(item->nodes.double_cycle.at(1).at(i),
                            item->nodes.double_cycle.at(1).at((i + 1)
                            % item->nodes.double_cycle.at(1).count()));
           edge->setParentItem(item);
        }
    }
}

void BasicGraphs::generate_grid(Graph *item, qreal height, qreal width,
                                 int topNodes, int bottomNodes, bool complete)
{
    qreal xSpace = width / (topNodes - 1);
    qreal ySpace = height / (bottomNodes - 1);

    qreal  x = 0;
    qreal  y = 0;


    for (int i = 0; i < bottomNodes; i++)
    {
        for (int j = 0; j < topNodes; j++)
        {
            Node * node = new Node();
            qreal xcoord, ycoord;

            xcoord = topNodes > 1 ?  x - width / 2. : 0;
            ycoord = bottomNodes > 1 ? y - height / 2.: 0;

            node->setPos(xcoord, ycoord);
           node->setParentItem(item);
            item->nodes.grid.append(node);
            x += xSpace;
        }

        x = 0;
        y += ySpace;

    }
    if (complete)
    {
        for (int i = 0; i < item->nodes.grid.count(); i++)
        {
            if ((i + 1) % topNodes != 0)
            {
                Edge * edge = new Edge(item->nodes.grid.at(i),
                                       item->nodes.grid.at(i + 1));
                edge->setParentItem(item);
            }
            if (i + topNodes  < item->nodes.grid.count())
            {
                Edge * edge = new Edge(item->nodes.grid.at(i),
                                       item->nodes.grid.at(i + topNodes));
                edge->setParentItem(item);
            }
        }
    }
}

void BasicGraphs::generate_antiprism(Graph *item, qreal width,
                                      qreal height, int topNode, bool complete)
{
    item->nodes.cycle = create_cycle(item, width, height, topNode);

    qreal spacing =  (2 * PI) / topNode;
    qreal angle = 0;
    qreal y = (width / 4) * qCos(angle) * - 1;
    qreal x = (width / 4) * qSin(angle);
    for (int i = 0; i < item->nodes.cycle.count(); i++)
    {
        if (i % 2 == 0)
        {
            if (complete)
            {
                Edge * edge = new Edge(item->nodes.cycle.at(i),
                                       item->nodes.cycle.at((i + 2)
                                        % item->nodes.cycle.count()));
                edge->setParentItem(item);
            }
        }
        else
        {
            if (complete)
            {
                Edge * edge = new Edge(item->nodes.cycle.at(i),
                                       item->nodes.cycle.at((i+2)
                                        % item->nodes.cycle.count()));
                edge->setParentItem(item);
            }
            item->nodes.cycle.at(i)->setPos(x, y);
        }
        if (complete)
        {
            Edge * edge = new Edge(item->nodes.cycle.at(i),
                                   item->nodes.cycle.at((i + 1)
                                   % item->nodes.cycle.count()));
            edge->setParentItem(item);
        }
        angle += spacing;
        y = (width / 4) * qCos(angle) * - 1;
        x = (width / 4) * qSin(angle);
    }

}

void BasicGraphs::generate_path(Graph *item, qreal width, int numOfNodes,
                                 bool complete)
{
    if (numOfNodes > 1)
    {
        qreal x = 0;
        qreal spacing = width / (numOfNodes -1);
        for (int i = 0; i < numOfNodes; i++)
        {
            Node * node = new Node();
            node->setPos(x - width / 2., 0);
            node->setParentItem(item);
            item->nodes.path.append(node);
            x += spacing;
        }
        if (complete)
        {
            for (int i = 0; i < numOfNodes - 1; i++)
            {
                Edge * edge = new Edge(item->nodes.path.at(i),
                                       item->nodes.path.at((i + 1)
                                       % item->nodes.path.count()));
                edge->setParentItem(item);
            }
        }
    }
    else if (numOfNodes == 1)
    {
        Node* node = new Node();
        node->setPos(0,0);
        node->setParentItem(item);
    }

}

void BasicGraphs::generate_prism(Graph *item, qreal width, qreal height,
                                  int numOfNodes, bool complete)
{
    item->nodes.double_cycle.append(create_cycle(item, width, height, numOfNodes));
    item->nodes.double_cycle.append(create_cycle(item, width / 2., height / 2.,
                                                 numOfNodes));

    if (complete)
    {
        for (int i = 0; i < item->nodes.double_cycle.at(0).count(); i++)
        {
            Edge * edge = new Edge(item->nodes.double_cycle.at(0).at(i),
                                   item->nodes.double_cycle.at(0).at((i + 1)
                                   % item->nodes.double_cycle.at(0).count()));
            edge->setParentItem(item);

            edge = new Edge(item->nodes.double_cycle.at(0).at(i),
                            item->nodes.double_cycle.at(1).at(i));
            edge->setParentItem(item);
        }
        for (int i = 0; i < item->nodes.double_cycle.at(1).count(); i++)
        {
            Edge * edge = new Edge(item->nodes.double_cycle.at(1).at(i),
                                   item->nodes.double_cycle.at(1).at((i + 1)
                                   % item->nodes.double_cycle.at(1).count()));
            edge->setParentItem(item);
        }
    }
}

void
BasicGraphs::generate_balanced_binary_tree(Graph * item,
                                            qreal height, qreal width,
                                            int numOfNodes, bool complete)
{
    item->nodes.binaryHeap.resize(numOfNodes);
    qreal treeDepth = floor(log2(numOfNodes));
    recursive_binary_tree(item, 0, 0, height, width, treeDepth);

    if (verbose)
    {
        Node * node00 = new Node();
        node00->setPos(0, 0);
        node00->setParentItem(item);
        Node * nodew0 = new Node();
        nodew0->setPos(width, 0);
        Node * nodewh = new Node();
        nodewh->setPos(width, height);
        Node * node0h = new Node();
        node0h->setPos(0, height);
    }

    if (complete)
    {
        for (int i = 0; i < item->nodes.binaryHeap.count() ; i++)
        {
            if (2 * i + 1 < item->nodes.binaryHeap.count())
            {
                Edge * edge = new Edge(item->nodes.binaryHeap.at(i),
                                       item->nodes.binaryHeap.at(2 * i + 1));
                edge->setParentItem(item);
            }
            if (2 * i + 2 < item->nodes.binaryHeap.count())
            {
                Edge * edge = new Edge(item->nodes.binaryHeap.at(i),
                                       item->nodes.binaryHeap.at(2 * i + 2 ));
                edge->setParentItem(item);
            }
        }
    }
}


// The vertical spacing is evenly distributed from 0 (root) to height
// (leaves).
// The horizontal spacing is defined as follows:
// - The leaves on a non-full tree are in the same positions they
//   would be if the bottom row was full.
// - The leaves of a full tree are evenly distributed from 0 (left)
//   to width (right).  This means the inter-leaf spacing is width(N - 1)
//   where N is the number of leaves in the full tree.
// - Since the X coord of the parents of the leaves are half way
//   between two leaves, we actually use a spacing value of width/2*(N-1).

void
BasicGraphs::recursive_binary_tree(Graph * item, int depth, int index,
				    qreal height, qreal width, int treeDepth)
{
    if (index >= item->nodes.binaryHeap.size())
	return;

    int leftChildIndex = index * 2 + 1;
    int rightChildIndex = index * 2 + 2;
    qreal x;
    qreal y;

    if (treeDepth == 0)
	y = height / 2;
    else
	y = depth * height / treeDepth;

    if (depth == 0)
	x = width / 2;
    else
    {
    int spaceFactor = 1 << (treeDepth - depth + 1);
	int spaceOffset = 0;
	if (treeDepth != depth)
        spaceOffset = (1 << (treeDepth - depth)) - 1;

    int startIndexThisLevel = (1 << depth) - 1;
	x = ((index - startIndexThisLevel) * spaceFactor + spaceOffset)
        * (qreal)width / (2 * ((1 << treeDepth) - 1));
    }
    if (verbose)
        qDebug() << "Node" << index << "at depth" << depth
                 << "is at (" << x << "," << y << ") where treeDepth is"
                 << treeDepth;

    Node * node = new Node();
    node->setPos(x - width/2., y - height/2.);
    item->nodes.binaryHeap[index] = node;
    node->setParentItem(item);

    if (leftChildIndex < item->nodes.binaryHeap.size())
        recursive_binary_tree(item, depth + 1, leftChildIndex, height,
			      width, treeDepth);
    if (rightChildIndex < item->nodes.binaryHeap.size())
        recursive_binary_tree(item, depth + 1, rightChildIndex, height,
			      width, treeDepth);
    return;
}

QList<Node *> BasicGraphs::create_cycle(Graph * item, qreal width,
                                         qreal height, int numOfNodes,
                                         qreal radians)
{
    QList<Node *> nodes;

    qreal spacing =  (2 * PI) / numOfNodes;
    for (int i = 0; i < numOfNodes; i++)
    {
        qreal y =  height * -qCos(radians);
        qreal x = width * qSin(radians);
        Node * node = new Node();
        node->setPos(x, y);
        nodes.append(node);
        node->setParentItem(item);
        radians += spacing;
    }

    return nodes;
}

QString BasicGraphs::getGraphName(int enumValue)
{
     return Graph_Type_Name[enumValue];
}


/*
* Function:	generate_dutch_windmill()
* Paramters:	g: the graph
*		height: the desired height of the graph (note that
*		        many other functions get height / 2 as the
*			corresponding parameter)
*		blades: the number of blades in the windmill
*		bladeSize: the number of vertices in each blade
*		complete: if True, draw the edges, else don't.
* Idea:	Give each blade and its adjacent air space an angle
*		of 2 * Pi / blades, regardless of how many vertices
*		per blade.
*		That angle is shared amongst the blade and the empty
*		space as follows: the blade is given
*		    0.9 - c * exp(-kn)
*		percent of the share; c = 0.786 and k = 0.135
*		were chosen to have 70% space in a 2-bladed windmill
*		and 50% space in a 5-bladed windmill.
* Assumption:	if # nodes is odd, create_cycle() creates the cycle
*		with a horizontal line at the bottom.
* To do:	make the edges from the center node to the adjacent
*		nodes a bit longer, at least for bladeSize = 5 and
*		maybe above.  Otherwise the nodes adjacent to the
*		center are too close, even when the node size is very
*		small.
*		Perhaps we need to take the node size into the
*		calculations when computing bladeWidth.
*/

void
BasicGraphs::generate_dutch_windmill(Graph * g, qreal height,
                      int blades, int bladeSize,
                      bool complete)
{
   qreal cycleHeight, cycleWidth;
   qreal bladeSpacing = 2 * PI / blades;
   qreal bladeWidth = bladeSpacing * (0.9 - 0.786 * qExp(-0.135 * blades));

   qreal angle = 0;

   if (verbose)
   {
       qDebug() << "bladeSpacing is " << bladeSpacing << " radians";
       qDebug() << "bladeWidth is " << bladeWidth << "radians";
       qDebug() << "height is " << height << ", blades is " << blades
                << ", bladeSize is " << bladeSize;
       qDebug() << "bladeWidth * #verts / (#v -2) / Pi = "
                << bladeWidth * bladeSize / (bladeSize - 2) / PI;
   }
   g->nodes.center = new Node();
   g->nodes.center->setPos(0, 0);
   g->nodes.center->setParentItem(g);

   // cycleHeight is the vertical "radius", not "diameter"
   cycleHeight = height / 4;
   // To get the cycle "width", multiply the height by the ratio of
   //        the desired base angle (bladeWidth)
   //        to the corner angle in a regular bladeSize-gon.
   // Note that the angles of the corners of a bladeSize-gon
   // sum to (bladeSize - 2) * Pi.
   cycleWidth = cycleHeight * bladeWidth * bladeSize / ((bladeSize - 2) * PI);

   if (verbose)
   {
       qDebug() << "cycleWidth will be " << cycleWidth;
       qDebug() << "cycleHeight will be " << cycleHeight;
   }

   QList <QList <Node *>> list_of_cycles = g->nodes.list_of_cycles;

   for (int i = 0; i < blades; i++)
   {
       list_of_cycles.append(create_cycle(g, cycleWidth, cycleHeight,
                       bladeSize, PI));

    // Create a cycle with a single vertex at the bottom
    // and then remove that vertex.  It's neighbours will
    // eventually be joined to the g->node.center vertex.
    QList <Node *> i_cycle = list_of_cycles.at(i);
    i_cycle.at(0)->setParentItem(nullptr);
       i_cycle.removeFirst();

       for (int j = 0; j < i_cycle.count(); j++)
       {
        // Translate the cycle up so that the center node is where
        // the deleted vertex should have been:
           i_cycle.at(j)->setY(i_cycle.at(j)->y() - cycleHeight);

        // Now rotate the cycle around (0, 0):
           qreal x = i_cycle.at(j)->x();
           qreal y = i_cycle.at(j)->y();

           i_cycle.at(j)->setY(x * qSin(angle) + y * qCos(angle));
           i_cycle.at(j)->setX(x * qCos(angle) - y * qSin(angle));
           i_cycle.at(j)->setParentItem(g);

           if (complete && j != i_cycle.count() - 1)
           {
               Edge * edge = new Edge(i_cycle.at(j),
                                      i_cycle.at((j + 1) % i_cycle.count()));
               edge->setParentItem(g);
           }
       }
       angle += bladeSpacing;

       if (complete)
       {
           Edge * edge = new Edge(i_cycle.at(0), g->nodes.center);
           edge->setParentItem(g);
           edge = new Edge(i_cycle.at(i_cycle.count() - 1),
                g->nodes.center);
           edge->setParentItem(g);
       }
   }
}

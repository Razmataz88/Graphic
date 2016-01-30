#include "simplegraphs.h"

#include <qmath.h>
#include "node.h"
#include "edge.h"
#include <QDebug>

static const double PI = 3.14159265358979323846264338327950288419717;



SimpleGraphs::SimpleGraphs()
{
    Graph_Type_Name = { "None", "Antiprism", "Balanced Binary Tree", "Bipartite",
                        "Crown", "Cycle", "Dutch Windmill", "Gear", "Petersen",
                        "Grid", "Helm", "Path", "Prism", "Round", "Star", "Wheel"};
}

void SimpleGraphs::generate_wheel(GraphItem *item, qreal radius, int numOfNodes, bool complete)
{
    if (numOfNodes > 0)
    {
        item->nodes.cycle = create_cycle(item, radius, numOfNodes - 1);
        //generate_cycle(item, radius, numOfNodes - 1, complete);
        Node * node = new Node();
        node->setPos(0, 0);
        item->nodes.center = node;
        item->addToGroup(node);
        if (complete)
            for (int i = 0; i < item->nodes.cycle.count(); i++)
            {
                Edge * edge = new Edge(node, item->nodes.cycle.at(i));
                item->addToGroup(edge);
                edge = new Edge(item->nodes.cycle.at(i),
                                item->nodes.cycle.at((i + 1) % item->nodes.cycle.count()));
                item->addToGroup(edge);
            }
    }
}

void SimpleGraphs::generate_bipartite(GraphItem * item, int topNodes, int bottomNodes, qreal height, qreal width, bool complete)
{
    /*
     * calculates the space between each of the nodes within
     * the user's desired width of the graph
     */
    qreal topSpacing = (topNodes >= bottomNodes) ?
                width / (topNodes - 1) :
                width / (topNodes + 1);
    qreal bottomSpacing = (bottomNodes >= topNodes) ?
                width / (bottomNodes - 1) :
                width / (bottomNodes + 1);
    /*
     * Calculates the y coordinate of the bottom partition of the nodes
     * Note:A QGraphicsScene coordinate system has the origin
     *      in the center of the scene
     */

    qreal bottomY = height / 2;

    int spaceNum;
    for (int i = 0; i < topNodes; i++)
    {
        spaceNum = (topNodes >= bottomNodes) ? i : i + 1;
        Node * node = new Node();
        node->setPos(spaceNum * topSpacing, height);
        item->nodes.bipartite_top.append(node);
        item->addToGroup(node);
    }

    for (int i = 0; i < bottomNodes; i++)
    {
        spaceNum = (bottomNodes >= topNodes) ? i : i + 1;
        Node * node = new Node();
        node->setPos(spaceNum * bottomSpacing, bottomY);
        item->nodes.bipartite_bottom.append(node);
        item->addToGroup(node);
    }

    if (complete)
    {
        for (int i = 0; i < topNodes; i++)
            for (int j = 0; j < bottomNodes; j++)
            {
                Edge * edge = new Edge(item->nodes.bipartite_top.at(i),
                                       item->nodes.bipartite_bottom.at(j));
                item->addToGroup(edge);
            }
    }
}

void SimpleGraphs::generate_cycle(GraphItem *item, qreal radius, int numOfNodes, bool complete)
{
    item->nodes.cycle = create_cycle(item, radius, numOfNodes);
    if (complete)
    {
        for (int i = 0; i < item->nodes.cycle.count(); i++)
        {
            //item->addToGroup(item->nodes.cycle.at(i));
            Edge * edge = new Edge(item->nodes.cycle.at(i), item->nodes.cycle.at( (i + 1) % item->nodes.cycle.count()));
            item->addToGroup(edge);
        }
    }
}

void SimpleGraphs::generate_star(GraphItem *item, qreal radius, int numOfNodes, bool complete)
{
    if (numOfNodes > 0)
    {
        item->nodes.cycle = create_cycle(item, radius, numOfNodes - 1);
        Node * node = new Node();
        node->setPos(0, 0);
        item->nodes.center = node;
        item->addToGroup(node);
        if (complete)
            for (int i = 0; i < item->nodes.cycle.count(); i++)
            {
                Edge * edge = new Edge(node, item->nodes.cycle.at(i));
                item->addToGroup(edge);
            }
    }
}

void SimpleGraphs::generate_complete(GraphItem *item, qreal radius, int numOfNodes, bool complete)
{
    item->nodes.cycle = create_cycle(item, radius, numOfNodes);
   // for (int i = 0; i < item->nodes.cycle.count(); i++)
     //   item->addToGroup(item->nodes.cycle.at(i));
    if (complete)
        for (int i = 0; i < item->nodes.cycle.count(); i++)
            for (int j = i + 1; j < item->nodes.cycle.count(); j++)
            {
                Edge * edge = new Edge(item->nodes.cycle.at(i), item->nodes.cycle.at(j));
                item->addToGroup(edge);
            }

}

void SimpleGraphs::generate_petersen(GraphItem *item, qreal height, int numOfNodes, bool complete)
{
    item->nodes.double_cycle.append(create_cycle(item, height, numOfNodes));
    item->nodes.double_cycle.append(create_cycle(item, height / 2, numOfNodes));

    if (complete)
    {
        for (int i = 0; i < numOfNodes; i++)
        {
          //  item->addToGroup(item->nodes.double_cycle.at(0).at(i));
          //  item->addToGroup(item->nodes.double_cycle.at(1).at(i));

            Edge * edge = new Edge(item->nodes.double_cycle.at(0).at(i),
                                   item->nodes.double_cycle.at(0).at((i + 1) % item->nodes.double_cycle.at(0).count()));
            item->addToGroup(edge);

            edge = new Edge(item->nodes.double_cycle.at(1).at(i),
                                   item->nodes.double_cycle.at(1).at((i + 2) % numOfNodes));
            item->addToGroup(edge);
            Edge * connectEdge = new Edge(item->nodes.double_cycle.at(0).at(i),
                                          item->nodes.double_cycle.at(1).at(i));
            item->addToGroup(connectEdge);
        }
    }
}

void SimpleGraphs::generate_gear(GraphItem *item, qreal radius, int numOfNodes, bool complete)
{
    item->nodes.cycle = create_cycle(item, radius, numOfNodes - 1);
    qreal x1, y1,x2,y2;
    for (int i = 0; i < item->nodes.cycle.count(); i++)
    {
        //item->addToGroup(item->nodes.cycle.at(i));
        if (i % 2 == 1)
        {
            x1 = item->nodes.cycle.at((i - 1) % item->nodes.cycle.count())->x();
            y1 = item->nodes.cycle.at((i - 1) % item->nodes.cycle.count())->y();
            x2 = item->nodes.cycle.at((i + 1) % item->nodes.cycle.count())->x();
            y2 = item->nodes.cycle.at((i + 1) % item->nodes.cycle.count())->y();

            item->nodes.cycle.at(i)->setPos((x1 + x2) / 2, (y1 + y2) / 2);
        }
    }
    Node * node = new Node();
    node->setPos(0, 0);
    item->addToGroup(node);
    item->nodes.center = node;
    if (complete)
    {
        for (int i = 0; i < item->nodes.cycle.count(); i++)
        {
            if (i % 2 == 0)
            {
                Edge * edge = new Edge(node, item->nodes.cycle.at(i));
                item->addToGroup(edge);
            }
            Edge * edge = new Edge(item->nodes.cycle.at(i),
                                   item->nodes.cycle.at((i + 1) % item->nodes.cycle.count()));
            item->addToGroup(edge);
        }
    }
}

void SimpleGraphs::generate_helm(GraphItem *item, qreal radius, int numOfNodes, bool complete)
{
    item->nodes.double_cycle.append(create_cycle(item, radius, numOfNodes));
    item->nodes.double_cycle.append(create_cycle(item, radius / 2, numOfNodes));

    Node * node = new Node();
    node->setPos(0,0);
    item->addToGroup(node);
    item->nodes.center = node;
    if (complete)
    {
        for (int i = 0; i < item->nodes.double_cycle.at(0).count(); i++)
        {
           // item->addToGroup(item->nodes.double_cycle.at(0).at(i));
           // item->addToGroup(item->nodes.double_cycle.at(1).at(i));
            Edge * edge = new Edge(item->nodes.double_cycle.at(1).at(i), item->nodes.center);
            item->addToGroup(edge);
            edge = new Edge(item->nodes.double_cycle.at(1).at(i), item->nodes.double_cycle.at(0).at(i));
            item->addToGroup(edge);
            edge = new Edge(item->nodes.double_cycle.at(1).at(i),
                            item->nodes.double_cycle.at(1).at((i + 1) % item->nodes.double_cycle.at(1).count()));
            item->addToGroup(edge);
        }
    }
}

void SimpleGraphs::generate_crown(GraphItem *item, qreal radius, int numOfNodes, bool complete)
{
    item->nodes.double_cycle.append(create_cycle(item, radius, numOfNodes));
    item->nodes.double_cycle.append(create_cycle(item, radius / 2, numOfNodes));

    if (complete)
    {
       // for (int i = 0; i < item->nodes.double_cycle.at(0).count(); i++)
        //    item->addToGroup(item->nodes.double_cycle.at(0).at(i));

        for (int i = 0; i < item->nodes.double_cycle.at(1).count(); i++)
        {
          //  item->addToGroup(item->nodes.double_cycle.at(1).at(i));

            Edge * edge = new Edge(item->nodes.double_cycle.at(0).at(i),
                                   item->nodes.double_cycle.at(1).at(i));
            item->addToGroup(edge);
            edge = new Edge(item->nodes.double_cycle.at(1).at(i),
                            item->nodes.double_cycle.at(1).at((i + 1) % item->nodes.double_cycle.at(1).count()));
            item->addToGroup(edge);
        }
    }
}

void SimpleGraphs::generate_grid(GraphItem *item, qreal height, qreal width, int topNodes, int bottomNodes, bool complete)
{
    qreal xSpace = width / topNodes;
    qreal ySpace = height / bottomNodes;

    qreal  x = width / 2;
    qreal  y = height / 2 * -1;

    for (int i = 0; i < bottomNodes; i++)
    {
        for (int j = 0; j < topNodes; j++)
        {
            Node * node = new Node();
            node->setPos(x, y);
            item->addToGroup(node);
            item->nodes.grid.append(node);
            x += xSpace;
        }
        x = width / 2;
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
                item->addToGroup(edge);
            }
            if (i + topNodes  < item->nodes.grid.count())
            {
                Edge * edge = new Edge(item->nodes.grid.at(i),
                                       item->nodes.grid.at(i + topNodes));
                item->addToGroup(edge);
            }
        }
    }
}

void SimpleGraphs::generate_antiprism(GraphItem *item, qreal radius, int topNode, bool complete)
{
    item->nodes.cycle = create_cycle(item, radius, topNode);
    if (complete)
    {
        qreal spacing =  (2 * PI) / topNode;
        qreal angle = 0;
        qreal y = (radius / 4) * qCos(angle) * - 1;
        qreal x = (radius / 4) * qSin(angle);
        for (int i = 0; i < item->nodes.cycle.count(); i++)
        {
           // item->addToGroup(item->nodes.cycle.at(i));
            if (i % 2 == 0)
            {
                Edge * edge = new Edge(item->nodes.cycle.at(i),
                                       item->nodes.cycle.at((i + 2) % item->nodes.cycle.count()));
                item->addToGroup(edge);
            }
            else
            {
                Edge * edge = new Edge(item->nodes.cycle.at(i),
                                       item->nodes.cycle.at((i+2) % item->nodes.cycle.count()));
                item->addToGroup(edge);
                item->nodes.cycle.at(i)->setPos(x, y);
            }
            Edge * edge = new Edge(item->nodes.cycle.at(i),
                                   item->nodes.cycle.at((i + 1) % item->nodes.cycle.count()));
            item->addToGroup(edge);
            angle += spacing;
            y = (radius / 4) * qCos(angle) * - 1;
            x = (radius / 4) * qSin(angle);
        }
    }
}

void SimpleGraphs::generate_path(GraphItem *item, qreal width, int numOfNodes, bool complete)
{
    qreal x = width / 2;
    qreal spacing = width / numOfNodes;
    for (int i = 0; i < numOfNodes; i++)
    {
        Node * node = new Node();
        node->setPos(x, 0);
        item->addToGroup(node);
        item->nodes.path.append(node);
        x += spacing;
    }
    if (complete)
    {
        for (int i = 0; i < numOfNodes; i++)
        {
            Edge * edge = new Edge(item->nodes.path.at(i),
                                   item->nodes.path.at((i + 1) % item->nodes.path.count()));
            item->addToGroup(edge);
        }
    }
}

void SimpleGraphs::generate_prism(GraphItem *item, qreal radius, int numOfNodes, bool complete)
{
    item->nodes.double_cycle.append(create_cycle(item, radius, numOfNodes));
    item->nodes.double_cycle.append(create_cycle(item, radius / 2, numOfNodes));

    if (complete)
    {
        for (int i = 0; i < item->nodes.double_cycle.at(0).count(); i++)
        {
            Edge * edge = new Edge(item->nodes.double_cycle.at(0).at(i), item->nodes.double_cycle.at(0).at((i + 1) % item->nodes.double_cycle.at(0).count()));
            item->addToGroup(edge);

            edge = new Edge(item->nodes.double_cycle.at(0).at(i), item->nodes.double_cycle.at(1).at(i));
            item->addToGroup(edge);
        }
        for (int i = 0; i < item->nodes.double_cycle.at(1).count(); i++)
        {
            Edge * edge = new Edge(item->nodes.double_cycle.at(1).at(i), item->nodes.double_cycle.at(1).at((i + 1) % item->nodes.double_cycle.at(1).count()));
            item->addToGroup(edge);
        }
    }
}

void SimpleGraphs::generate_balanced_binary_tree(GraphItem *item, qreal height, qreal width,int numOfNodes, bool complete)
{
    item->nodes.binaryHeap.resize(numOfNodes);
    qreal treeDepth = floor(log2(numOfNodes));
    recursive_binary_tree(item, 0, 0, height, width, treeDepth );
    if (complete)
    {
        for (int i = 0; i < item->nodes.binaryHeap.count() ; i++)
        {
            if (2 * i + 1 < item->nodes.binaryHeap.count())
            {
                Edge * edge = new Edge(item->nodes.binaryHeap.at(i),
                                       item->nodes.binaryHeap.at(2 * i + 1));
                item->addToGroup(edge);
            }
            if (2 * i + 2 < item->nodes.binaryHeap.count())
            {
                Edge* edge = new Edge(item->nodes.binaryHeap.at(i),
                                      item->nodes.binaryHeap.at(2 * i + 2 ));
                item->addToGroup(edge);
            }
        }
    }
}

void SimpleGraphs::recursive_binary_tree(GraphItem * item, int depth, int index, qreal height, qreal width, int treeDepth)
{
    int leftChildIndex = index * 2 + 1;
    int rightChildIndex = index * 2 + 2;
    if (index < item->nodes.binaryHeap.size())
    {
        qreal x = index * width / qPow(2, depth) + width;
        qreal y = (depth * height / treeDepth) - height;
        Node * node = new Node();
        node->setPos(x, y);
        item->nodes.binaryHeap[index] = node;
        item->addToGroup(node);
    }
    else
        return;
    if (leftChildIndex < item->nodes.binaryHeap.size())
        recursive_binary_tree(item, depth + 1, leftChildIndex, height, width, treeDepth);
    if (rightChildIndex < item->nodes.binaryHeap.size())
        recursive_binary_tree(item, depth + 1, rightChildIndex, height, width, treeDepth);
    return;
}

QList<Node *> SimpleGraphs::create_cycle(GraphItem * item, qreal radius, int numOfNodes, qreal angle)
{
    QList<Node *> nodes;

    qreal spacing =  (2 * PI) / numOfNodes;
    for (int i = 0; i < numOfNodes; i++)
    {
        qreal y =  radius * qCos(angle) * - 1;
        qreal x = radius * qSin(angle);
        Node * node = new Node();
        node->setPos(x, y);
        nodes.append(node);
        item->addToGroup(node);
        angle += spacing;
    }

    return nodes;
}

QString SimpleGraphs::getGraphName(int enumValue)
{
     return Graph_Type_Name[enumValue];
}

void SimpleGraphs::generate_dutch_windmill(GraphItem *item, qreal height, int topNodes, int bottomNodes, bool complete)
{
    qreal bladeSpace = (2 * PI) / bottomNodes;
    qreal cycleSpace = (2 * PI) / topNodes;
    qreal angle = 0;

    item->nodes.center = new Node();
    item->nodes.center->setPos(0, 0);
    item->addToGroup(item->nodes.center);

    for (int i = 0; i < bottomNodes; i++)
    {
        item->nodes.list_of_cycles.append(create_cycle(item, height / 2, topNodes, cycleSpace * (topNodes + 1) / 2 ));
        for (int j = 0; j < item->nodes.list_of_cycles.at(i).count(); j++)
        {
            item->nodes.list_of_cycles.at(i).at(j)->setY(item->nodes.list_of_cycles.at(i).at(j)->y() -
                                                         height * 3 / 5);

            qreal x = item->nodes.list_of_cycles.at(i).at(j)->x();
            qreal y = item->nodes.list_of_cycles.at(i).at(j)->y();

            item->nodes.list_of_cycles.at(i).at(j)->setY(x * qSin(angle) + y * qCos(angle));
            item->nodes.list_of_cycles.at(i).at(j)->setX(x * qCos(angle) - y * qSin(angle));
            item->addToGroup(item->nodes.list_of_cycles.at(i).at(j));

            if (complete && j != item->nodes.list_of_cycles.at(i).count() - 1)
            {
                Edge * edge = new Edge(item->nodes.list_of_cycles.at(i).at(j),
                                       item->nodes.list_of_cycles.at(i).at((j + 1) 
                                                                           % item->nodes.list_of_cycles.at(i).count()));
                item->addToGroup(edge);
            }

        }
        angle += bladeSpace;

        if (complete)
        {
            Edge * edge = new Edge(item->nodes.list_of_cycles.at(i).at(0),
                                   item->nodes.center);
            item->addToGroup(edge);
            edge = new Edge(item->nodes.list_of_cycles.at(i).at(item->nodes.list_of_cycles.at(i).count() - 1),
                            item->nodes.center);
            item->addToGroup(edge);
        }

    }
}




























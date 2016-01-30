#ifndef SIMPLEGRAPHS_H
#define SIMPLEGRAPHS_H


#include <graph.h>

class BasicGraphs

{
public:  

    BasicGraphs();
    void generate_wheel(Graph * item, qreal width, qreal height, int numOfNodes, bool complete);
    void generate_bipartite(Graph * item, int topNodes,int bottomNodes,qreal height,qreal width, bool complete);
    void generate_cycle(Graph * item, qreal width, qreal height, int numOfNodes, bool complete);
    void generate_star(Graph * item, qreal width, qreal height, int numOfNodes, bool complete);
    void generate_complete(Graph * item, qreal width, qreal height, int numOfNodes, bool complete);
    void generate_petersen(Graph * item, qreal width, qreal height,
                  int numOfNodes, int starSkip, bool complete);
    void generate_gear(Graph * item, qreal width, qreal height, int numOfNodes, bool complete);
    void generate_helm(Graph * item, qreal width, qreal height, int numOfNodes, bool complete);
    void generate_crown(Graph * item, qreal width, qreal height, int numOfNodes, bool complete);
    void generate_grid(Graph * item, qreal height, qreal width, int topNodes, int bottomNodes, bool complete);
    void generate_antiprism(Graph * item, qreal width, qreal height, int topNode, bool complete);
    void generate_path(Graph * item, qreal width, int numOfNodes, bool complete);
    void generate_prism(Graph * item, qreal width, qreal height, int numOfNodes, bool complete);
    void generate_balanced_binary_tree(Graph * item, qreal height, qreal width, int numOfNodes, bool complete);
    void recursive_binary_tree(Graph * item, int depth, int index, qreal height, qreal width, int treeDepth);
    void generate_dutch_windmill(Graph * g, qreal height,
                          int blades, int bladeSize,
                          bool complete);
    QList<Node *> create_cycle(Graph * item, qreal width, qreal height, int numOfNodes, qreal radians = 0);

    enum Graph_Type {Antiprism = 1, BBTree, Bipartite, Crown, Cycle, Windmill, Gear,
                      Grid, Helm, Path, Petersen, Prism, Complete, Star, Wheel, Count};
    QString getGraphName(int enumValue);
private:
    QVector<QString> Graph_Type_Name;
};

#endif // SIMPLEGRAPHS_H

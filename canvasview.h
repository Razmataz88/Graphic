#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include "canvasscene.h"
#include "graph.h"

#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>

class Node;
class Edge;

class CanvasView: public QGraphicsView
{
    Q_OBJECT
public:

    enum mode {none, join, del, edit, freestyle};

    typedef struct nParams
    {
        qreal diameter;
        bool isNumbered;
        QString label;
        qreal labelSize;
        QColor fillColour;
        QColor outlineColour;
    }Node_Params;

    typedef struct eParams
    {
        qreal size;
        QString label;
        qreal LabelSize;
        QColor color;
    }Edge_Params;

    CanvasView(QWidget *parent = 0);
    QList<Node *> selectedNodes;
    QList<Edge *> selectedEdges;

    void isSnappedToGrid(bool snap);
    void setUpNodeParams(qreal nodeDiameter, bool numberedLabels, QString label,
                     qreal nodeLabelSize, QColor nodeFillColor,
                     QColor nodeOutLineColor);
    void setUpEdgeParams(qreal edgeSize, QString edgeLabel, qreal edgeLabelSize,
                    QColor edgeLineColor);

    Node * createNode(QPointF pos);
    Edge * createEdge(Node * source, Node * destination);
    Edge * addEdgeToScene(Node * source, Node * destination);
    int getMode() const;
    void setMode(int m);

public slots:
    void snapToGrid(bool snap);

signals:
    void setKeyStatusLabelText(QString text);
    void resetNoMode();

protected:
    void dragEnterEvent(QDragEnterEvent * event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    int modeType;
    int timerId;
    CanvasScene *aScene;
    Graph * freestyleGraph;
    Node_Params * nodeParams;
    Edge_Params * edgeParams;
    Node * node1, *node2;
};

#endif // CANVASVIEW_H

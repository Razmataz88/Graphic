#ifndef PREVIEW_H
#define PREVIEW_H

#include "graph.h"

#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>

class Node;
class Edge;

class PreView: public QGraphicsView
{
    Q_OBJECT
public:
    PreView(QWidget * parent = 0);
    bool isComplete();

public slots:
    void zoomIn();
    void zoomOut();
    Graph * Create_Graph(int graph, int topNodes, int bottomNodes,
                             qreal height, qreal width,
                             bool complet);


    void Style_Graph(Graph * item, int graphType, qreal nodeDiameter,
                     QString topNodeLabels, QString bottomNodeLabels,
                     bool numberedLabels, qreal nodeLabelSize, qreal edgeSize,
                     QString edgeLabel, qreal edgeLabelSize, qreal rotation,
                     QColor nodeFillColor, QColor nodeOutlineColor,
                     QColor edgeLineColor);
protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void scaleView(qreal scaleFactor);
    virtual void mousePressEvent(QMouseEvent *event);


private:
    QGraphicsScene * aScene;
    bool modified;
    QImage image;
    QColor edgeLine, nodeLine, nodeFill;
};

#endif // PREVIEW_H

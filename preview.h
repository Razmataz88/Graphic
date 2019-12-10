/*
 * File:    preview.h
 * Author:  Rachel Bood 100088769
 * Date:    2014/11/07 (?)
 * Version: 1.1
 *
 * Purpose: define the fields of the preview class.
 *
 * Modification history:
 * Dec 7, 2019 (JD V1.1)
 *  (a) Add the currentGraphTypeIndex field (& 3 similar friends).
 *  (b) Remove the return type of Create_Graph() since it wasn't used.
 */

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
      void Create_Graph(int graphType, int topNodes, int bottomNodes,
			bool drawEdges);

      void Style_Graph(Graph * item, int graphType, qreal nodeDiameter,
		       QString topNodeLabels, QString bottomNodeLabels,
		       bool numberedLabels, qreal nodeLabelSize,
		       QColor nodeFillColor, QColor nodeOutlineColor,
		       qreal edgeSize, QString edgeLabel, qreal edgeLabelSize,
		       QColor edgeLineColor,
		       qreal width, qreal height, qreal rotation);

  protected:
    virtual void keyPressEvent(QKeyEvent * event);
    virtual void scaleView(qreal scaleFactor);
    virtual void mousePressEvent(QMouseEvent *event);


  private:
    QGraphicsScene * aScene;
    bool modified;
    QImage image;
    QColor edgeLine, nodeLine, nodeFill;
    int currentGraphTypeIndex, currentTopNodes, currentBottomNodes,
	currentDrawEdges;
};

#endif // PREVIEW_H

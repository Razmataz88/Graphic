#ifndef EDGEWEIGHTCONTROLLER_H
#define EDGEWEIGHTCONTROLLER_H
#include "edge.h"
#include "node.h"

#include <QLineEdit>
#include <QObject>

class LabelController : public QObject
{
    Q_OBJECT
public:
    LabelController(Edge * anEdge, QLineEdit * anEdit);
    LabelController(Node * aNode, QLineEdit * anEdit);

private slots:
    void setEdgeWeight(QString string);
    void setNodeLabel(QString string);
    void deletedLineEdit();

private:
    Edge * edge;
    Node * node;
    QLineEdit * edit;
};

#endif // EDGEWEIGHTCONTROLLER_H

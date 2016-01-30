
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "GraphSettings.h"
#include "edge.h"
#include "basicgraphs.h"
#include "colourlinecontroller.h"
#include "sizecontroller.h"
#include "labelcontroller.h"
#include "labelsizecontroller.h"
#include "colourfillcontroller.h"

#include <QDesktopWidget>
#include <QColorDialog>
#include <QGraphicsItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QShortcut>
#include <qmath.h>
#include <QtSvg/QSvgGenerator>


#define GRAPHICS_FILE_EXTENSION ".grphc"
#define GRAPHICS_SAVE_FILE "Graph-ic (*.grphc)"
#define TIKZ_SAVE_FILE "TikZ (*.tikz)"
#define EDGES_SAVE_FILE "Edge list (*.edges)"
#define SVG_SAVE_FILE "SVG (*.svg)"

#define BUTTON_STYLE "border-style: outset; border-width: 2px; border-radius: 5px; border-color: beige; padding: 3px;"

#define PIXELS_IN_CM  37.795276
#define MM_IN_INCH  25.4

#define TITLE_SIZE  20
#define SUB_TITLE_SIZE  18
#define SUB_SUB_TITLE_SIZE  12


/*
 * Name:        MainWindow
 * Purpose:     Main window contructor
 * Arguments:   QWidget *
 * Output:      none
 * Modifies:    private MainWindow variables
 * Returns:     none
 * Assumptions: none
 * Bugs:        none...so far
 * Notes:       This is a cpp file used in with the mainwindow.ui file
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{        
    fileDirectory = QDir::currentPath().append("/graph-ic");
    QDir dir(fileDirectory);

    if (!dir.exists())
        dir.mkdir(fileDirectory);

    ui->setupUi(this);
    this->generate_Combobox_Titles();

    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save_Graph()));
    connect(ui->actionOpen_File, SIGNAL(triggered()), this, SLOT(load_Graphic_File()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this, SLOT(close()));

    // Quits application via CTRL and Q keys
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_O), this, SLOT(load_Graphic_File()));
    // Save dialog pops up via CTRL and S keys
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this, SLOT(save_Graph()));

    QObject::connect(ui->nodeSize, SIGNAL(valueChanged(double)),
                     this, SLOT(generate_Graph()));
    QObject::connect(ui->edgeSize, SIGNAL(valueChanged(double)),
                     this, SLOT(generate_Graph()));
    QObject::connect(ui->NodeLabel1, SIGNAL(textChanged(QString)),
                     this, SLOT(generate_Graph()));
    QObject::connect(ui->NodeLabel2, SIGNAL(textChanged(QString)),
                     this, SLOT(generate_Graph()));
    QObject::connect(ui->EdgeLabel, SIGNAL(textChanged(QString)),
                     this, SLOT(generate_Graph()));
    QObject::connect(ui->NodeLabelSize, SIGNAL(valueChanged(double)),
                     this, SLOT(generate_Graph()));
    QObject::connect(ui->NumLabelCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(generate_Graph()));
    QObject::connect(ui->NodeFillColor, SIGNAL(clicked(bool)),
                     this, SLOT(generate_Graph()));
    QObject::connect(ui->NodeOutlineColor, SIGNAL(clicked(bool)),
                     this, SLOT(generate_Graph()));
    QObject::connect(ui->EdgeLineColor, SIGNAL(clicked(bool)),
                     this, SLOT(generate_Graph()));


    QObject::connect(ui->nodeSize, SIGNAL(valueChanged(double)),
                     this, SLOT(generate_Freestyle_Nodes()));
    QObject::connect(ui->edgeSize, SIGNAL(valueChanged(double)),
                     this, SLOT(generate_Freestyle_Edges()));
    QObject::connect(ui->NodeLabel1, SIGNAL(textChanged(QString)),
                     this, SLOT(generate_Freestyle_Nodes()));
    QObject::connect(ui->NodeLabel2, SIGNAL(textChanged(QString)),
                     this, SLOT(generate_Freestyle_Nodes()));
    QObject::connect(ui->EdgeLabel, SIGNAL(textChanged(QString)),
                     this, SLOT(generate_Freestyle_Edges()));
    QObject::connect(ui->NodeLabelSize, SIGNAL(valueChanged(double)),
                     this, SLOT(generate_Freestyle_Nodes()));
    QObject::connect(ui->NumLabelCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(generate_Freestyle_Nodes()));
    QObject::connect(ui->NodeFillColor, SIGNAL(clicked(bool)),
                     this, SLOT(generate_Freestyle_Nodes()));
    QObject::connect(ui->NodeOutlineColor, SIGNAL(clicked(bool)),
                     this, SLOT(generate_Freestyle_Nodes()));
    QObject::connect(ui->EdgeLineColor, SIGNAL(clicked(bool)),
                     this, SLOT(generate_Freestyle_Edges()));


    QObject::connect(ui->graphRotation, SIGNAL(valueChanged(double)),
                     this, SLOT(generate_Graph()));
    QObject::connect(ui->complete_checkBox, SIGNAL(clicked(bool)),
                     this, SLOT(generate_Graph()));
    QObject::connect(ui->graphHeight, SIGNAL(valueChanged(double)),
                     this, SLOT(generate_Graph()));
    QObject::connect(ui->graphWidth, SIGNAL(valueChanged(double)),
                     this, SLOT(generate_Graph()));
    QObject::connect(ui->numOfNodes1, SIGNAL(valueChanged(int)),
                     this, SLOT(generate_Graph()));
    QObject::connect(ui->numOfNodes2, SIGNAL(valueChanged(int)),
                     this, SLOT(generate_Graph()));
    QObject::connect(ui->graphType_ComboBox, SIGNAL(activated(int)),
                     this, SLOT(generate_Graph()));

    QObject::connect(ui->EdgeLabelSize, SIGNAL(valueChanged(double)),
                     this, SLOT(generate_Graph()));

    QObject::connect(ui->snapToGrid_checkBox, SIGNAL(clicked(bool)),
                    ui->canvas, SLOT(snapToGrid(bool)));

    QObject::connect(ui->canvas, SIGNAL(setKeyStatusLabelText(QString)),
                     ui->keyPressStatus_label, SLOT(setText(QString)));

    QObject::connect(ui->canvas, SIGNAL(resetNoMode()),
                     ui->noMode_radioButton, SLOT(click()));

    ui->noMode_radioButton->click();

    //initialize color buttons
    QString s("background: #000000;" BUTTON_STYLE);
    ui->EdgeLineColor->setStyleSheet(s);
    ui->NodeOutlineColor->setStyleSheet(s);

    s = "background: #ffffff;" BUTTON_STYLE;
    ui->NodeFillColor->setStyleSheet(s);

    generate_Freestyle_Edges();
    generate_Freestyle_Nodes();

    // initialize the canvas to enable snapToGrid feature when loaded
    ui->canvas->snapToGrid(ui->snapToGrid_checkBox->isChecked());

    ui->splitter->setStretchFactor(0, 1); /* show different modes at start up */
    set_Label_Font_Sizes();
    // initialize font sizes for ui labels (Linux fix)
    gridLayout = new QGridLayout();

    //ui->editGraph->setLayout(gridLayout);
    ui->scrollAreaWidgetContents->setLayout(gridLayout);
}

/*
 * Name:        ~MainWindow
 * Purpose:     frees the memory of mainwindow
 * Arguments:   none
 * Output:      none
 * Modifies:    mainwindow
 * Returns:     none
 * Assumptions: none
 * Bugs:        none...so far
 * Notes:       none
 */
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setKeyStatusLabel(QString text)
{
    ui->keyPressStatus_label->setText(text);
}

void MainWindow::generate_Combobox_Titles()
{
    BasicGraphs * simpleG = new BasicGraphs();
    int i = 1;
    while (i < BasicGraphs::Count)
    {
        ui->graphType_ComboBox->addItem(simpleG->getGraphName(i));
        i++;
    }
    ui->graphType_ComboBox->insertSeparator(BasicGraphs::Count);
    this->load_Grapha_Library();
}


/*
 * Name:        save_Graph
 * Purpose:     saves an image/tikz/grphcs of the Canvas
 * Arguments:   QByteArray
 * Output:      true to save file,false to not save file
 * Modifies:    none
 * Returns:     none
 * Assumptions: none
 * Bugs:        none
 * Notes:       none
 */

bool MainWindow::save_Graph()
{
    QString fileTypes = "";

    foreach (QByteArray format, QImageWriter::supportedImageFormats())
    {
        // Remove offensive file types.
        // Even with these, there still may a confusing number of choices.
        if (QString(format).toUpper() == "BMP")
            continue;
        if (QString(format).toUpper() == "WBMP")
            continue;
        if (QString(format).toUpper() == "DDS")
            continue;
        fileTypes += tr("%1 (*.%2);;").arg(QString(format).toUpper(),
                                           QString(format).toLower());
    }

    fileTypes += EDGES_SAVE_FILE ";;"
             GRAPHICS_SAVE_FILE  ";;"
             TIKZ_SAVE_FILE  ";;"
            SVG_SAVE_FILE ";;"
            "All Files (*)";

    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Save graph",
                                                    fileDirectory,
                                                    fileTypes,
                                                    &selectedFilter);


#ifdef __linux__

    // Stupid, stupid Qt file browser works differently on different OSes.
    // Append the extension if there isn't already one ...
    QFileInfo fi(fileName);
    QString ext = fi.suffix();
    if (ext.isNull())
    {
        int start = selectedFilter.indexOf("*") + 1;
        int end = selectedFilter.indexOf(")");

        if (start < 0 || end < 0)
        {
            qDebug() << "?? could not find extension in " << selectedFilter;
            return false;
        }

        QString extension = selectedFilter.mid(start, end - start);
        qDebug() << "computed extension is" << extension;
        fileName += extension;
        qDebug() << "computed filename is" << fileName;
    }
#endif


    //qDebug() << "fileName is /" << fileName << "/";
    if (fileName.isNull())
        return false;

    // Handle all image (i.e., non-text) outputs here;
    // check for all known text-file types.
    // TODO: should we use QFileInfo(fileName).extension().lower();
    bool saveStatus = ui->snapToGrid_checkBox->isChecked();
    if (ui->snapToGrid_checkBox->isChecked())
        ui->canvas->snapToGrid(false);

    if (selectedFilter != GRAPHICS_SAVE_FILE
            && selectedFilter != TIKZ_SAVE_FILE
            && selectedFilter != EDGES_SAVE_FILE
            && selectedFilter != SVG_SAVE_FILE)
    {
        ui->canvas->scene()->clearSelection();
        ui->canvas->scene()->invalidate(ui->canvas->scene()
                                        ->itemsBoundingRect(),
                                        ui->canvas->scene()->BackgroundLayer);

        QPixmap * image = new QPixmap(ui->canvas->scene()
                            ->itemsBoundingRect().size().toSize());

        image->fill(Qt::transparent);
        QPainter painter(image);
        painter.setRenderHints(QPainter::Antialiasing |
                               QPainter::TextAntialiasing |
                               QPainter::HighQualityAntialiasing |
                               QPainter::NonCosmeticDefaultPen, true);
        ui->canvas->scene()->setBackgroundBrush(Qt::transparent);
        ui->canvas->
                scene()->render(&painter,
                                QRectF(0, 0, ui->canvas->scene()
                                       ->itemsBoundingRect().width(),
                                       ui->canvas->scene()
                                       ->itemsBoundingRect().height()),
                                ui->canvas->scene()->itemsBoundingRect(),
                                Qt::IgnoreAspectRatio);
        image->save(fileName); // requires file extension or it won't save :-/
        ui->canvas->snapToGrid(saveStatus);
        ui->canvas->update();
        return true;
    }

    // Common code for text files:
    int numOfNodes = 0;
    QVector<Node *> nodes;
    QString edges = "";

    QFile outputFile(fileName);
    outputFile.open(QIODevice::WriteOnly);
    if (!outputFile.isOpen())
    {
        qDebug() << "- Error, unable to open" << fileName << "for output";
        return false;
    }
    QTextStream outStream(&outputFile);

    foreach (QGraphicsItem * item, ui->canvas->scene()->items())
    {
        if (item->type() == Node::Type)
        {
            Node * node = qgraphicsitem_cast<Node *>(item);
            node->setID(numOfNodes);
            numOfNodes++;
            nodes.append(node);
        }
    }

    if (selectedFilter == GRAPHICS_SAVE_FILE)
    {
        QString nodeStyles = QString::number(numOfNodes) + "\n";

        for (int i = 0; i < nodes.count(); i++)
        {
            Node * node = nodes.at(i);
            nodeStyles += QString::number(node->scenePos().rx()) + ","
                    + QString::number(node->scenePos().ry()) + ","
                    + QString::number(node->getDiameter()) + ","
                    + QString::number(node->getRotation()) + ","
                    + QString::number(node->getFillColour().redF()) + ","
                    + QString::number(node->getFillColour().greenF()) + ","
                    + QString::number(node->getFillColour().blueF()) + ","
                    + QString::number(node->getLineColour().redF()) + ","
                    + QString::number(node->getLineColour().greenF()) + ","
                    + QString::number(node->getLineColour().blueF()) +"\n";
        }

        for (int i = 0; i < nodes.count(); i++)
        {
            for (int j = 0; j < nodes.at(i)->edgeList.count(); j++)
            {
                Edge * edge = nodes.at(i)->edgeList.at(j);
                if (edge->sourceNode()->getID() == i
                        && edge->destNode()->getID() > i)
                {
                    edges += QString::number(edge->sourceNode()->getID()) + ","
                            + QString::number(edge->destNode()->getID()) + ","
                            + QString::number(edge->getDestRadius()) + ","
                            + QString::number(edge->getSourceRadius()) + ","
                            + QString::number(edge->getRotation()) + ","
                            + QString::number(edge->getPenWidth()) + ","
                            + QString::number(edge->getColour().redF()) + ","
                            + QString::number(edge->getColour().greenF()) + ","
                            + QString::number(edge->getColour().blueF())  + "\n";
                }
                else if (edge->destNode()->getID() == i
                         && edge->sourceNode()->getID() > i)
                {
                    edges += QString::number(edge->destNode()->getID()) + ","
                            + QString::number(edge->sourceNode()->getID()) + ","
                            + QString::number(edge->getDestRadius()) + ","
                            + QString::number(edge->getSourceRadius()) + ","
                            + QString::number(edge->getRotation()) + ","
                            + QString::number(edge->getPenWidth()) + ","
                            + QString::number(edge->getColour().redF()) + ","
                            + QString::number(edge->getColour().greenF()) + ","
                            + QString::number(edge->getColour().blueF())  + "\n";
                }
            }
        }

        outStream << nodeStyles + edges ;
        outputFile.close();
        ui->canvas->snapToGrid(saveStatus);
        ui->canvas->update();
        QFileInfo fi(fileName);
        ui->graphType_ComboBox->insertItem(ui->graphType_ComboBox->count(),
                                           fi.baseName());
        return true;
    }

    if (selectedFilter == EDGES_SAVE_FILE)
    {
       // qDebug() << "A .edges file with " << nodes.count() << " nodes...";

        for (int i = 0; i < nodes.count(); i++)
        {
            for (int j = 0; j < nodes.at(i)->edgeList.count(); j++)
            {
                Edge * edge = nodes.at(i)->edgeList.at(j);
                //qDebug() << "i is " << i << " and j is " << j;
                if (edge->sourceNode()->getID() == i
                        && edge->destNode()->getID() > i)
                {
                   // qDebug() << "IF: source == i and dest > i";
                    edges += QString::number(edge->sourceNode()->getID()) + ","
                            + QString::number(edge->destNode()->getID()) + "\n";
                }
                else if (edge->destNode()->getID() == i
                         && edge->sourceNode()->getID() > i)
                {
                  //  qDebug() << "ELSE: dest == i and source > i";
                    edges += QString::number(edge->destNode()->getID()) + ","
                            + QString::number(edge->sourceNode()->getID()) + "\n";
                }
            }
        }
        outStream << nodes.count() << "\n";
        outStream << edges;
        outputFile.close();
        ui->canvas->snapToGrid(saveStatus);
        ui->canvas->update();
        return true;
    }

    if (selectedFilter == TIKZ_SAVE_FILE)
    {
        QScreen *screen = QGuiApplication::primaryScreen();

        QString nodeStyles = "";
        QString edgeStyles = "";
        QString begin = "\\begin{tikzpicture} [x=1.0in, y=1.0in, xscale=1, yscale=1]\n";
        // Nodes
        for (int i = 0; i < nodes.count(); i++)
        {
            Node * node = nodes.at(i);
            // create the variable name to refer to the node's fill and line color
            QString fillColor = "node" + QString::number(i) + "fillColor";
            QString lineColor = "node" + QString::number(i) + "lineColor";

            //define the Node's fill color using RGB format
            nodeStyles += "\\definecolor{" + fillColor + "} {RGB} {"
                    + QString::number(node->getFillColour().red())
                    + "," + QString::number(node->getFillColour().green())
                    + "," + QString::number(node->getFillColour().blue()) + "}\n";
            //define the Node's line color using RGB format
            nodeStyles += "\\definecolor{" + lineColor + "}{RGB}{"
                    + QString::number(node->getLineColour().red())
                    + "," + QString::number(node->getLineColour().green())
                    + "," + QString::number(node->getLineColour().blue()) + "}\n";

            // Uses x,y coordinate system.
            nodeStyles += "\\node (v" + QString::number(i) + ") at ("
                    +  QString::number(node->scenePos().rx() / screen->logicalDotsPerInchX()) // x
                    + "," +  QString::number(node->scenePos().ry() / - screen->logicalDotsPerInchX()) // y
                    +  ") " + "[scale=1.00, inner sep=0, font=\\fontsize{"
                    + QString::number(node->getLabelSize()) // font size
                    + "}{1}\\selectfont, fill=" + fillColor
                    + ", shape=circle,minimum size="
                    +  QString::number(node->getDiameter()) // node size
                    +  "in,draw=" + lineColor + "]";
            //set the node label if there is one
            if (node->getLabel().length() > 0)
            {
                bool check;
                node->getLabel().toInt(&check);
                if (check)
                    nodeStyles += "{$" + node->getLabel() + "$};\n";
                else
                    nodeStyles += "{$" + node->getLabel()
                            + "^{}_{" + QString::number(node->getID()) + "}$};\n";
            }
            else
                nodeStyles += "{$$};\n";
        }

        //Edges
        for (int i = 0; i < nodes.count(); i++)
        {
            for (int j = 0; j < nodes.at(i)->edgeList.count(); j++)
            {
                Edge * edge = nodes.at(i)->edgeList.at(j);
                //set Edge Color variable name
                QString edgeColor = "edge" + QString::number(i + j) +"edgeColor";
                //set color to use for Edge format
                nodeStyles += "\\definecolor{" + edgeColor + "}{RGB}{"
                        + QString::number(edge->getColour().red())
                        + "," + QString::number(edge->getColour().green())
                        + "," + QString::number(edge->getColour().blue()) + "}\n";

                if (edge->sourceNode()->getID() == i
                        && edge->destNode()->getID() > i)
                {
                    edgeStyles += "\\path (v" + QString::number(edge->sourceNode()->getID())
                            + ") edge[draw=" + edgeColor +",line width=" +
                            QString::number(edge->getPenWidth() / screen->logicalDotsPerInchX())
                            + "in] node [font=\\fontsize{"
                            + QString::number(edge->getWeightLabelSize())
                            + "}{1}\\selectfont]";
                    //set Edge weight if it hasone
                    if (edge->getWeight().length() > 0)
                            edgeStyles += "{$" + edge->getWeight() + "$} ";
                    else
                        edgeStyles += "{$$} ";
                    edgeStyles += "(v" + QString::number(edge->destNode()->getID())
                            + ");\n";

                }
                else if (edge->destNode()->getID() == i
                         && edge->sourceNode()->getID() > i)
                {
                    edgeStyles += "\\path (v"
                            + QString::number(edge->sourceNode()->getID())
                            + ") edge[draw="+ edgeColor +",line width="
                            + QString::number(edge->getPenWidth() / screen->logicalDotsPerInchX())
                            + "in] node [font=\\fontsize{" + QString::number(edge->getWeightLabelSize())
                            + "}{1}\\selectfont]";

                    if (edge->getWeight().length() > 0)
                        edgeStyles += "{$" + edge->getWeight() + "$} ";
                    else
                        edgeStyles += "{$$} ";
                    edgeStyles+=" (v" + QString::number(edge->destNode()->getID()) + ");\n";
                }
            }
        }

        outStream << begin << nodeStyles << edgeStyles << "\\end{tikzpicture}";
        outputFile.close();
        ui->canvas->snapToGrid(saveStatus);
        ui->canvas->update();
        return true;
    }
    if (selectedFilter == SVG_SAVE_FILE)
    {
        QSvgGenerator svgGen;

        svgGen.setFileName(fileName);
        svgGen.setSize(ui->canvas->scene()
                       ->itemsBoundingRect().size().toSize());
        QPainter painter( &svgGen );
        ui->canvas->scene()->render( &painter,
                                     QRectF(0, 0, ui->canvas->scene()
                                            ->itemsBoundingRect().width(),
                                            ui->canvas->scene()
                                            ->itemsBoundingRect().height()),
                                     ui->canvas->scene()->itemsBoundingRect(),
                                     Qt::IgnoreAspectRatio);
        ui->canvas->snapToGrid(saveStatus);
        ui->canvas->update();
        return true;
    }

    // ? Should not get here!
    return false;
}



bool MainWindow::load_Graphic_File()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Load Graph-ics File",
                                                    fileDirectory,
                                                    GRAPHICS_SAVE_FILE);
    select_Custom_Graph(fileName);
    return true;
}

void MainWindow::load_Grapha_Library()
{
    QDirIterator dirIt(fileDirectory, QDirIterator::Subdirectories);
    while (dirIt.hasNext())
    {
        dirIt.next();
        if (QFileInfo(dirIt.filePath()).isFile())
            qDebug() << QFileInfo(dirIt.filePath()).suffix() << endl;

            if (QFileInfo(dirIt.filePath()).suffix() == "grphc")
            {
                QFileInfo fileInfo(dirIt.filePath());
                ui->graphType_ComboBox->addItem(fileInfo.baseName());
            }
    }
}

void MainWindow::select_Custom_Graph(QString graphName)
{
    qDebug() << graphName;
    if (!graphName.isNull())
    {
        QFile file(graphName);
        if(!file.open(QIODevice::ReadOnly))
            QMessageBox::information(0,
                                     "Error",
                                     "File: "
                                     + graphName + file.errorString());

        QTextStream in(&file);
        int check = 0;
        int i = 0;
        QVector<Node *> nodes;
        int numOfNodes = 0;
        Graph * graph = new Graph();


        while(!in.atEnd())
        {
            QString line = in.readLine();
            if (check == 0)
            {
               // qDebug() << line.toInt();
                numOfNodes = line.toInt();
                check += 1;
            }
            else if (i < numOfNodes)
            {
                QStringList fields = line.split(",");
                Node * node = new Node();
                node->setPos(fields.at(0).toDouble(),
                             fields.at(1).toDouble());
                node->setID(i);
                node->setDiameter(fields.at(2).toDouble());
                node->setRotation(fields.at(3).toDouble());
                QColor fillColor;
                fillColor.setRedF(fields.at(4).toDouble());
                fillColor.setGreenF(fields.at(5).toDouble());
                fillColor.setBlueF(fields.at(6).toDouble());
                node->setFillColour(fillColor);
                QColor lineColor;
                lineColor.setRedF(fields.at(7).toDouble());
                lineColor.setGreenF(fields.at(8).toDouble());
                lineColor.setBlueF(fields.at(9).toDouble());
                node->setLineColour(lineColor);
                nodes.append(node);
                //item->addToGroup(node);
                node->setParentItem(graph);
                //qDebug() << fields;
                i++;
            }
            else
            {
                QStringList fields = line.split(",");
               // qDebug() << fields << endl;
                Edge * edge = new Edge(nodes.at(fields.at(0).toInt()),
                                       nodes.at(fields.at(1).toInt()));
                edge->setDestRadius(fields.at(2).toDouble());
                edge->setSourceRadius(fields.at(3).toDouble());
                edge->setRotation(fields.at(4).toDouble());
                edge->setPenWidth(fields.at(5).toDouble());
                QColor lineColor;
                lineColor.setRedF(fields.at(6).toDouble());
                lineColor.setGreen(fields.at(7).toDouble());
                lineColor.setBlue(fields.at(8).toDouble());
                edge->setColour(lineColor);
            //    item->addToGroup(edge);
                edge->setParentItem(graph);
            }
        }
        file.close();
        qDebug() << graph->childItems().length();
        graph->setRotation(-1 * ui->graphRotation->value());
        ui->preview->scene()->clear();
        ui->preview->scene()->addItem(graph);
    }
}


void MainWindow::style_Graph()
{
    foreach(QGraphicsItem * item, ui->preview->scene()->items())
    {
        if (item->type() == Graph::Type)
        {
            Graph * graphItem =  qgraphicsitem_cast<Graph *>(item);
            ui->preview->Style_Graph(graphItem,
                                     ui->graphType_ComboBox->currentIndex(),
                                     ui->nodeSize->value(),
                                     ui->NodeLabel1->text(),
                                     ui->NodeLabel2->text(),
                                     ui->NumLabelCheckBox->isChecked(),
                                     ui->NodeLabelSize->value(),
                                     ui->edgeSize->value(),
                                     ui->EdgeLabel->text(),
                                     ui->EdgeLabelSize->value(),
                                     ui->graphRotation->value(),
                                     ui->NodeFillColor->palette().background().color(),
                                     ui->NodeOutlineColor->palette().background().color(),
                                     ui->EdgeLineColor->palette().background().color());

        }
    }
}

void MainWindow::generate_Graph()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    ui->preview->scene()->clear();
    if (ui->graphType_ComboBox->currentIndex() < BasicGraphs::Count)
        ui->preview->Create_Graph(ui->graphType_ComboBox->currentIndex(),
                                  ui->numOfNodes1->value(),
                                  ui->numOfNodes2->value(),
                                  ui->graphHeight->value() * screen->logicalDotsPerInchY()
                                  - ui->nodeSize->value() * screen->logicalDotsPerInchX(),
                                  ui->graphWidth->value() * screen->logicalDotsPerInchX()
                                  - ui->nodeSize->value() * screen->logicalDotsPerInchX(),
                                  ui->complete_checkBox->isChecked());

    else
        select_Custom_Graph(fileDirectory + "/" + ui->graphType_ComboBox
                            ->currentText()
                            + GRAPHICS_FILE_EXTENSION);

    this->style_Graph();
}
void MainWindow::on_NodeOutlineColor_clicked()
{

    QColor color = QColorDialog::getColor();
    QString s("background: #"
              + QString(color.red() < 16? "0" : "")
              + QString::number(color.red(),16)
              + QString(color.green() < 16? "0" : "")
              + QString::number(color.green(),16)
              + QString(color.blue() < 16? "0" : "")
              + QString::number(color.blue(),16) + ";"
              BUTTON_STYLE);
    ui->NodeOutlineColor->setStyleSheet(s);
    ui->NodeOutlineColor->update();
}

void MainWindow::on_NodeFillColor_clicked()
{
    QColor color = QColorDialog::getColor();

    QString s("background: #"
              + QString(color.red() < 16? "0" : "")
              + QString::number(color.red(),16)
              + QString(color.green() < 16? "0" : "")
              + QString::number(color.green(),16)
              + QString(color.blue() < 16? "0" : "")
              + QString::number(color.blue(),16) + ";"
              BUTTON_STYLE);
    ui->NodeFillColor->setStyleSheet(s);
    ui->NodeFillColor->update();
}

void MainWindow::on_EdgeLineColor_clicked()
{
    QColor color = QColorDialog::getColor();

    QString s("background: #"
              + QString(color.red() < 16? "0" : "")
              + QString::number(color.red(),16)
              + QString(color.green() < 16? "0" : "")
              + QString::number(color.green(),16)
              + QString(color.blue() < 16? "0" : "")
              + QString::number(color.blue(),16) + ";"
              BUTTON_STYLE);
    ui->EdgeLineColor->setStyleSheet(s);
    ui->EdgeLineColor->update();
}

void MainWindow::on_NumLabelCheckBox_clicked(bool checked)
{
    if (checked)
    {
        ui->NodeLabel1->setDisabled(true);
        ui->NodeLabel2->setDisabled(true);
    }
    else
    {
        ui->NodeLabel1->setDisabled(false);
        ui->NodeLabel2->setDisabled(false);
    }
}


void MainWindow::set_Label_Font_Sizes()
{
    QFont font;
    font = ui->graphLabel->font();
    font.setPixelSize(TITLE_SIZE);
    ui->graphLabel->setFont(font);

    font = ui->edgeLabel->font();
    font.setPixelSize(TITLE_SIZE - 1);
    ui->edgeLabel->setFont(font);

    font = ui->nodeLabel->font();
    font.setPixelSize(TITLE_SIZE - 1);
    ui->nodeLabel->setFont(font);

    font = ui->sizeLabel->font();
    font.setPixelSize(SUB_TITLE_SIZE);
    ui->sizeLabel->setFont(font);

    font = ui->partitionLabel->font();
    font.setPixelSize(SUB_TITLE_SIZE);
    ui->partitionLabel->setFont(font);

    font = ui->labelLabel->font();
    font.setPixelSize(SUB_TITLE_SIZE);
    ui->labelLabel->setFont(font);

    font = ui->sizeLabelEN->font();
    font.setPixelSize(SUB_TITLE_SIZE);
    ui->sizeLabelEN->setFont(font);

    font = ui->colorLabel->font();
    font.setPixelSize(SUB_TITLE_SIZE);
    ui->colorLabel->setFont(font);

    font = ui->rotationLabel->font();
    font.setPixelSize(SUB_TITLE_SIZE);
    ui->rotationLabel->setFont(font);

    font = ui->widthLabel->font();
    font.setPixelSize(SUB_SUB_TITLE_SIZE);
    ui->widthLabel->setFont(font);

    font = ui->heightLabel->font();
    font.setPixelSize(SUB_SUB_TITLE_SIZE);
    ui->heightLabel->setFont(font);

    font = ui->textInputLabel->font();
    font.setPixelSize(SUB_SUB_TITLE_SIZE);
    ui->textInputLabel->setFont(font);

    font = ui->textSizeLabel->font();
    font.setPixelSize(SUB_SUB_TITLE_SIZE);
    ui->textSizeLabel->setFont(font);

    font = ui->fillLabel->font();
    font.setPixelSize(SUB_SUB_TITLE_SIZE);
    ui->fillLabel->setFont(font);

    font = ui->outlineLabel->font();
    font.setPixelSize(SUB_SUB_TITLE_SIZE);
    ui->outlineLabel->setFont(font);

    font = ui->ptLabel->font();
    font.setPixelSize(SUB_SUB_TITLE_SIZE);
    ui->ptLabel->setFont(font);

    font = ui->inchesLabel->font();
    font.setPixelSize(SUB_SUB_TITLE_SIZE);
    ui->inchesLabel->setFont(font);

    font = ui->complete_checkBox->font();
    font.setPixelSize(SUB_SUB_TITLE_SIZE - 1);
    ui->complete_checkBox->setFont(font);

}

void MainWindow::on_graphType_ComboBox_currentIndexChanged(int index)
{
    switch (index) {
    case BasicGraphs::Crown:
    case BasicGraphs::Cycle:
    case BasicGraphs::Gear:
    case BasicGraphs::Helm:
    case BasicGraphs::Prism:
    case BasicGraphs::Complete:
    case BasicGraphs::Star:
    case BasicGraphs::Wheel:
    case BasicGraphs::BBTree:
    {
        ui->partitionLabel->setText("Node Count");
        ui->numOfNodes2->hide();
        ui->numOfNodes1->setSingleStep(1);
        ui->numOfNodes1->setMinimum(0);
        ui->numOfNodes2->setMinimum(0);
        ui->graphWidth->show();
        ui->widthLabel->show();
        ui->heightLabel->show();
        ui->graphHeight->show();
        break;
    }
    case BasicGraphs::Path:
    {
        ui->partitionLabel->setText("Node Count");
        ui->numOfNodes2->hide();
        ui->numOfNodes1->setSingleStep(1);
        ui->numOfNodes1->setMinimum(0);
        ui->numOfNodes2->setMinimum(0);
        ui->graphWidth->show();
        ui->widthLabel->show();
        ui->heightLabel->hide();
        ui->graphHeight->hide();
        break;
    }
    case BasicGraphs::Antiprism:
    {
        ui->partitionLabel->setText("Node Count");
        ui->numOfNodes2->hide();
        ui->numOfNodes1->setMinimum(6);
        if (ui->numOfNodes1->value() % 2 == 1)
            ui->numOfNodes1->setValue( ui->numOfNodes1->value() - 1);
        ui->numOfNodes1->setSingleStep(2);
        ui->graphWidth->show();
        ui->widthLabel->show();
        ui->heightLabel->show();
        ui->graphHeight->show();
        break;
    }
    case BasicGraphs::Bipartite:
    {
        ui->partitionLabel->setText("Partitions");
        ui->numOfNodes2->show();
        ui->numOfNodes1->setSingleStep(1);
        ui->numOfNodes2->setSingleStep(1);
        ui->numOfNodes1->setMinimum(0);
        ui->numOfNodes2->setMinimum(0);
        ui->graphWidth->show();
        ui->widthLabel->show();
        ui->heightLabel->show();
        ui->graphHeight->show();
        break;
    }
    case BasicGraphs::Grid:
    {
        ui->partitionLabel->setText("Rows v. Columns");
        ui->numOfNodes2->show();
        ui->numOfNodes2->setSingleStep(1);
        ui->numOfNodes1->setSingleStep(1);
        ui->numOfNodes1->setMinimum(0);
        ui->numOfNodes2->setMinimum(0);
        ui->graphHeight->show();
        ui->graphWidth->show();
        ui->widthLabel->show();
        ui->heightLabel->show();
        break;
    }
    case BasicGraphs::Petersen:
    {
        ui->partitionLabel->setText("N, k");
        ui->numOfNodes1->setSingleStep(1);
        ui->numOfNodes2->show();
        ui->numOfNodes2->setValue(2);
        ui->numOfNodes1->setMinimum(3);
        ui->graphWidth->show();
        ui->graphHeight->show();
        ui->heightLabel->show();
        ui->widthLabel->show();
        break;

    }
    case BasicGraphs::Windmill:
    {
        ui->partitionLabel->setText("Nodes and Blades");
        ui->graphWidth->hide();
        ui->widthLabel->hide();
        ui->numOfNodes2->show();
        ui->heightLabel->hide();
        ui->numOfNodes2->setMinimum(3);
        if (ui->numOfNodes2->value() < 3)
            ui->numOfNodes2->setValue(3);
        ui->numOfNodes1->setMinimum(0);
        break;
    }
    default:
      qDebug() << "Error Occured when Styling Graph-cs" << endl;
    }
}

void MainWindow::on_numOfNodes2_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    if (ui->graphType_ComboBox->currentIndex() == BasicGraphs::Petersen)
    {
        if (ui->numOfNodes2->value() > floor((ui->numOfNodes1->value() - 1) / 2))
            ui->numOfNodes2->setValue(1);
    }
}

void MainWindow::generate_Freestyle_Nodes()
{
    //QScreen *screen = QGuiApplication::primaryScreen();
    ui->canvas->setUpNodeParams(
                           ui->nodeSize->value(),
                           ui->NumLabelCheckBox->isChecked(),
                           ui->nodeLabel->text(),
                           ui->nodeSize->value(),
                           ui->NodeFillColor->palette().background().color(),
                           ui->NodeOutlineColor->palette().background().color());
}

void MainWindow::generate_Freestyle_Edges()
{
   ui->canvas->setUpEdgeParams(ui->edgeSize->value(),
                          ui->EdgeLabel->text(),
                          ui->EdgeLabelSize->value(),
                          ui->EdgeLineColor->palette().background().color());
}

void MainWindow::on_deleteMode_radioButton_clicked()
{
    ui->canvas->setMode(CanvasView::del);
}

void MainWindow::on_joinMode_radioButton_clicked()
{
    ui->canvas->setMode(CanvasView::join);

}

void MainWindow::on_editMode_radioButton_clicked()
{
    ui->canvas->setMode(CanvasView::edit);

}


void MainWindow::on_noMode_radioButton_clicked()
{
    ui->canvas->setMode(CanvasView::none);

}

void MainWindow::on_freestyleMode_radioButton_clicked()
{
    ui->canvas->setMode(CanvasView::freestyle);

}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    switch(index)
    {
    case 0:
    {
       QLayoutItem * wItem;
       while ((wItem = ui->scrollAreaWidgetContents->layout()->takeAt(0)) != 0)
       {
           if (wItem->widget())
               wItem->widget()->setParent(NULL);
           delete wItem;
       }
       break;
    }
    case 1:
    {
        int i = 0;
        foreach(QGraphicsItem * item, ui->canvas->scene()->items())
        {
            if (item != 0 || item != nullptr)
            {
                if (item->type() == Graph::Type)
                {
                    Graph * graph =
                            qgraphicsitem_cast<Graph*>(item);
                    QLabel * graphLabel = new QLabel("Graph");
                    gridLayout->addWidget(graphLabel, i, 1);
                    i++;

                    gridLayout->addWidget(new QLabel("size"), i, 2);
                    gridLayout->addWidget(new QLabel("Text"), i, 3);
                    gridLayout->addWidget(new QLabel("Text Size"), i , 4);
                    gridLayout->addWidget(new QLabel("Outline Color"), i, 5);
                    gridLayout->addWidget(new QLabel("Fill Color"), i, 6);
                    i++;

                    foreach(QGraphicsItem * gItem, graph->childItems())
                    {

                        if (gItem != nullptr || gItem != 0)
                        {
                            if (gItem->type() == Node::Type)
                            {
                                Node * node = qgraphicsitem_cast<Node*>(gItem);
                                QLineEdit * nodeEdit = new QLineEdit();
                                nodeEdit->setText("Node\n");
                                gridLayout->addWidget(nodeEdit);

                                QLabel * label = new QLabel("Node");
                                QPushButton * lineColorButton = new QPushButton();
                                QPushButton * fillColorButton = new QPushButton();

                                QDoubleSpinBox * sizeBox = new QDoubleSpinBox();
                                QDoubleSpinBox * fontSizeBox = new QDoubleSpinBox();

                                SizeController * sizeController = new SizeController(node, sizeBox);
                                ColorLineController * colorLineController = new ColorLineController(node, lineColorButton);
                                LabelController * weightController = new LabelController(node, nodeEdit);
                                LabelSizeController *weightSizeController = new LabelSizeController(node, fontSizeBox);
                                ColorFillController * colorFillController = new ColorFillController(node, fillColorButton);

                                gridLayout->addWidget(label, i, 1);
                                gridLayout->addWidget(sizeBox, i, 2);
                                gridLayout->addWidget(nodeEdit,  i, 3);
                                gridLayout->addWidget(fontSizeBox, i, 4);
                                gridLayout->addWidget(lineColorButton, i, 5);
                                gridLayout->addWidget(fillColorButton, i, 6);
                                Q_UNUSED(sizeController);
                                Q_UNUSED(colorLineController);
                                Q_UNUSED(colorFillController);
                                Q_UNUSED(weightController);
                                Q_UNUSED(weightSizeController);
                            }

                            else if (gItem->type() == Edge::Type)
                            {
                                Edge * edge = qgraphicsitem_cast<Edge*>(gItem);
                                QLineEdit * editEdge = new QLineEdit();
                                editEdge->setText("Edge\n");
                                gridLayout->addWidget(editEdge);

                                QLabel * label = new QLabel("Edge");
                                QPushButton * button = new QPushButton();
                                QDoubleSpinBox * sizeBox = new QDoubleSpinBox();
                                QDoubleSpinBox * fontSizeBox = new QDoubleSpinBox();

                                SizeController * sizeController = new SizeController(edge, sizeBox);
                                ColorLineController * colorController = new ColorLineController(edge, button);
                                LabelController * weightController = new LabelController(edge, editEdge);
                                LabelSizeController *weightSizeController = new LabelSizeController(edge, fontSizeBox);

                                gridLayout->addWidget(label, i, 1);
                                gridLayout->addWidget(sizeBox, i, 2);
                                gridLayout->addWidget(editEdge,  i, 3);
                                gridLayout->addWidget(fontSizeBox, i, 4);
                                gridLayout->addWidget(button,  i, 5);

                                Q_UNUSED(sizeController);
                                Q_UNUSED(colorController);
                                Q_UNUSED(weightController);
                                Q_UNUSED(weightSizeController);


                            }
                        }
                        i++;
                    }
                }
            }
        }
        break;
    }
    default:
           break;
    }
}

/*
 * File:	mainwindow.cpp
 * Author:	Rachel Bood
 * Date:	January 25, 2015.
 * Version:	1.1
 *
 * Purpose:
 *
 * Modification history:
 * Jan 25, 2016 (JD):
 *  (a) Don't include TIFF and JPEG as file types to make the list shorter;
 *	assume (!) that the list also includes TIF and JPG.
 *  (b) TikZ: only output edge label font size if there is an edge label.
 *  (c) TikZ: only output edge thickness to ET_PREC_TIKZ digits.
 *  (d) TikZ: Only output vertex positions to VP_PREC_TIKZ digits.
 *  (e) TikZ: Merge the two (currently identical) code branches for drawing
 *	edges.
 *  (f) Factored out the "default" features in
 *	on_graphType_ComboBox_currentIndexChanged()) while fixing a
 *	bug where one or two weren't set when they should be.
 *  (g) Changed the minimum number of nodes to 1 for known graph types;
 *	tweaked some other similar parameters as well.
 *	For graphs read in from .grphc files, only display the UI control
 *	that (currently) do anything.
 *  (h) Changed some of the widget labels for the different types of
 *	graphs in on_graphType_ComboBox_currentIndexChanged().
 *  (i) Re-ordered file type drop-down list so to put the "text" outputs
 *	at the top.
 * Feb 3, 2016 (JD)
 *  (a) Removed "GraphSettings.h" since it is not used.
 *  (b) Minor formatting cleanups.
 *  (c) Changed "Grapha" to "Graphic".
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
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
#define GRAPHICS_SAVE_FILE	"Graph-ic (*.grphc)"
#define TIKZ_SAVE_FILE		"TikZ (*.tikz)"
#define EDGES_SAVE_FILE		"Edge list (*.edges)"
#define SVG_SAVE_FILE		"SVG (*.svg)"

#define BUTTON_STYLE "border-style: outset; border-width: 2px; " \
	     "border-radius: 5px; border-color: beige; padding: 3px;"

// The unit of these is points:
#define TITLE_SIZE	    20
#define SUB_TITLE_SIZE	    18
#define SUB_SUB_TITLE_SIZE  12

// The precision (number of digits after the decimal place) with which
// vertex positions and edge thicknesses, respectively, are written in
// TikZ output:
#define VP_PREC_TIKZ  4
#define ET_PREC_TIKZ  4



/*
 * Name:	MainWindow
 * Purpose:	Main window contructor
 * Arguments:	QWidget *
 * Output:	none
 * Modifies:	private MainWindow variables
 * Returns:	none
 * Assumptions: none
 * Bugs:	none...so far
 * Notes:	This is a cpp file used in with the mainwindow.ui file
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
    connect(ui->actionOpen_File, SIGNAL(triggered()),
	    this, SLOT(load_Graphic_File()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this, SLOT(close()));

    // Ctrl-Q quits.
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_O),
		  this, SLOT(load_Graphic_File()));
    // Save dialog pops up via Ctrl-S.
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

    // Initialize color buttons.
    QString s("background: #000000;" BUTTON_STYLE);
    ui->EdgeLineColor->setStyleSheet(s);
    ui->NodeOutlineColor->setStyleSheet(s);

    s = "background: #ffffff;" BUTTON_STYLE;
    ui->NodeFillColor->setStyleSheet(s);

    generate_Freestyle_Edges();
    generate_Freestyle_Nodes();

    // Initialize the canvas to enable snapToGrid feature when loaded.
    ui->canvas->snapToGrid(ui->snapToGrid_checkBox->isChecked());

    ui->splitter->setStretchFactor(0, 1); // Show different modes at start up.
    set_Label_Font_Sizes();
    // Initialize font sizes for ui labels (Linux fix).
    gridLayout = new QGridLayout();

    //ui->editGraph->setLayout(gridLayout);
    ui->scrollAreaWidgetContents->setLayout(gridLayout);
}


/*
 * Name:	~MainWindow
 * Purpose:	frees the memory of mainwindow
 * Arguments:	none
 * Output:	none
 * Modifies:	mainwindow
 * Returns:	none
 * Assumptions: none
 * Bugs:	none...so far
 * Notes:	none
 */

MainWindow::~MainWindow()
{
    delete ui;
}


/*
 * Name:
 * Purpose:
 * Arguments:
 * Outputs:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

void MainWindow::setKeyStatusLabel(QString text)
{
    ui->keyPressStatus_label->setText(text);
}


/*
 * Name:
 * Purpose:
 * Arguments:
 * Outputs:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

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
    this->load_Graphic_Library();
}


/*
 * Name:	save_Graph
 * Purpose:	saves an image/tikz/grphcs of the Canvas
 * Arguments:	QByteArray
 * Output:	true to save file,false to not save file
 * Modifies:	none
 * Returns:	none
 * Assumptions: none
 * Bugs:	none
 * Notes:	none
 */

bool MainWindow::save_Graph()
{
    QString fileTypes = "";

    fileTypes += GRAPHICS_SAVE_FILE  ";;"
	EDGES_SAVE_FILE ";;"
	TIKZ_SAVE_FILE	";;";

    foreach (QByteArray format, QImageWriter::supportedImageFormats())
    {
	// Remove offensive and redundant file types.
	// Even with these, there still may a confusing number of choices.
	if (QString(format).toUpper() == "BMP")
	    continue;
	if (QString(format).toUpper() == "WBMP")
	    continue;
	if (QString(format).toUpper() == "DDS")
	    continue;
	if (QString(format).toUpper() == "TIFF")  // Just list "tif"
	    continue;
	if (QString(format).toUpper() == "JPEG")  // Just list "jpg"
	    continue;
	fileTypes += tr("%1 (*.%2);;").arg(QString(format).toUpper(),
					   QString(format).toLower());
    }

    fileTypes += SVG_SAVE_FILE ";;"
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
	painter.setRenderHints(QPainter::Antialiasing
			       | QPainter::TextAntialiasing
			       | QPainter::HighQualityAntialiasing
			       | QPainter::NonCosmeticDefaultPen, true);
	ui->canvas->scene()->setBackgroundBrush(Qt::transparent);
	ui->canvas->
	    scene()->render(&painter,
			    QRectF(0, 0, ui->canvas->scene()
				   ->itemsBoundingRect().width(),
				   ui->canvas->scene()
				   ->itemsBoundingRect().height()),
			    ui->canvas->scene()->itemsBoundingRect(),
			    Qt::IgnoreAspectRatio);
	image->save(fileName); // Requires file extension or it won't save :-/
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
	// TO DO: need to pop up an error window here, the average person
	// won't see this error message!
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
		+ QString::number(node->getLineColour().blueF()) + "\n";
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
			+ QString::number(edge->getColour().blueF()) + "\n";
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
			+ QString::number(edge->getColour().blueF()) + "\n";
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
	for (int i = 0; i < nodes.count(); i++)
	{
	    for (int j = 0; j < nodes.at(i)->edgeList.count(); j++)
	    {
		Edge * edge = nodes.at(i)->edgeList.at(j);

		if (edge->sourceNode()->getID() == i
		    && edge->destNode()->getID() > i)
		{
		    edges += QString::number(edge->sourceNode()->getID()) + ","
			+ QString::number(edge->destNode()->getID()) + "\n";
		}
		else if (edge->destNode()->getID() == i
			 && edge->sourceNode()->getID() > i)
		{
		    edges += QString::number(edge->destNode()->getID()) + ","
			+ QString::number(edge->sourceNode()->getID())
			+ "\n";
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
	// TODO: only define a given colour once.
	// (Hash the known colours, and use the name if already defined?)
	QScreen * screen = QGuiApplication::primaryScreen();

	QString nodeStyles = "";
	QString edgeStyles = "";
	QString begin = "\\begin{tikzpicture} "
	    "[x=1in, y=1in, xscale=1, yscale=1]\n";
	// Nodes
	for (int i = 0; i < nodes.count(); i++)
	{
	    Node * node = nodes.at(i);
	    // Create the variable name to refer to the node's fill
	    // and line colours.
	    QString fillColour = "node" + QString::number(i) + "fillColour";
	    QString lineColour = "node" + QString::number(i) + "lineColour";

	    // Define the Node's fill colour using RGB format.
	    nodeStyles += "\\definecolor{" + fillColour + "} {RGB} {"
		+ QString::number(node->getFillColour().red())
		+ "," + QString::number(node->getFillColour().green())
		+ "," + QString::number(node->getFillColour().blue())
		+ "}\n";
	    // Define the Node's line colour using RGB format.
	    nodeStyles += "\\definecolor{" + lineColour + "}{RGB}{"
		+ QString::number(node->getLineColour().red())
		+ "," + QString::number(node->getLineColour().green())
		+ "," + QString::number(node->getLineColour().blue())
		+ "}\n";

	    // Use (x,y) coordinate system for node positions.
	    nodeStyles += "\\node (v" + QString::number(i) + ") at ("
		+  QString::number(node->scenePos().rx()
				   / screen->logicalDotsPerInchX(),
				   'f', VP_PREC_TIKZ)
		+ ","
		+ QString::number(node->scenePos().ry()
				  / -screen->logicalDotsPerInchX(),
				  'f', VP_PREC_TIKZ)
		+  ") " + "[scale=1, inner sep=0,\n\t"
		+ "shape=circle, minimum size="
		+ QString::number(node->getDiameter()) // Node size
		+ "in,\n\t"
		+ "fill=" + fillColour
		+ ", draw=" + lineColour;
	    // Output the node label and its font size if and only if
	    // there is a node label.
	    if (node->getLabel().length() > 0)
	    {
		bool check;

		nodeStyles += ", \n\tfont=\\fontsize{"
		    + QString::number(node->getLabelSize()) // Font size
		    + "}{1}\\selectfont";

		node->getLabel().toInt(&check);
		if (check)
		    nodeStyles += "] {$" + node->getLabel() + "$};\n";
		else
		    nodeStyles += "] {$" + node->getLabel()
			+ "^{}_{" + QString::number(node->getID())
			+ "}$};\n";
	    }
	    else
		nodeStyles += "] {$$};\n";
	}

	// Edges
	for (int i = 0; i < nodes.count(); i++)
	{
	    for (int j = 0; j < nodes.at(i)->edgeList.count(); j++)
	    {
		Edge * edge = nodes.at(i)->edgeList.at(j);
		QString edgeColour = "edge" + QString::number(i + j)
		    + "edgeColour";
		// Output
		nodeStyles += "\\definecolor{" + edgeColour + "}{RGB}{"
		    + QString::number(edge->getColour().red())
		    + "," + QString::number(edge->getColour().green())
		    + "," + QString::number(edge->getColour().blue())
		    + "}\n";

		if ((edge->sourceNode()->getID() == i
		     && edge->destNode()->getID() > i)
		    || (edge->destNode()->getID() == i
			&& edge->sourceNode()->getID() > i))
		{
		    edgeStyles += "\\path (v"
			+ QString::number(edge->sourceNode()->getID())
			+ ") edge[draw=" + edgeColour
			+ ", line width="
			+ QString::number(edge->getPenWidth()
					  / screen->logicalDotsPerInchX(),
					  'f', ET_PREC_TIKZ)
			+ "in]\n\tnode[";
		    // Output edge weight (and the selected font info)
		    // if and only if the edge has a weight.
		    if (edge->getWeight().length() > 0)
			edgeStyles += "font=\\fontsize{"
			    + QString::number(edge->getWeightLabelSize())
			    + "}{1}\\selectfont"
			    + "] {$" + edge->getWeight() + "$}";
		    else
			edgeStyles += "] {$$}";

		    // Finally, output the other end of the edge:
		    edgeStyles += " (v"
			+ QString::number(edge->destNode()->getID())
			+ ");\n";
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
	ui->canvas->scene()->render(&painter,
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
    qDebug() << "Unexpected output filter in MainWindow::save_Graph()!";
    return false;
}


/*
 * Name:
 * Purpose:
 * Arguments:
 * Outputs:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

bool MainWindow::load_Graphic_File()
{
    QString fileName = QFileDialog::getOpenFileName(this,
						    "Load Graph-ics File",
						    fileDirectory,
						    GRAPHICS_SAVE_FILE);
    select_Custom_Graph(fileName);
    return true;
}


/*
 * Name:
 * Purpose:
 * Arguments:
 * Outputs:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

void MainWindow::load_Graphic_Library()
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


/*
 * Name:
 * Purpose:
 * Arguments:
 * Outputs:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

void MainWindow::select_Custom_Graph(QString graphName)
{
    qDebug() << graphName;
    if (!graphName.isNull())
    {
	QFile file(graphName);
	if (!file.open(QIODevice::ReadOnly))
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
		numOfNodes = line.toInt();
		check++;
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
		// item->addToGroup(node);
		node->setParentItem(graph);
		i++;
	    }
	    else
	    {
		QStringList fields = line.split(",");
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
		// item->addToGroup(edge);
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


/*
 * Name:
 * Purpose:
 * Arguments:
 * Outputs:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

void MainWindow::style_Graph()
{
    foreach (QGraphicsItem * item, ui->preview->scene()->items())
    {
	if (item->type() == Graph::Type)
	{
	    Graph * graphItem =	 qgraphicsitem_cast<Graph *>(item);
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


/*
 * Name:
 * Purpose:
 * Arguments:
 * Outputs:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

void MainWindow::generate_Graph()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    ui->preview->scene()->clear();
    if (ui->graphType_ComboBox->currentIndex() < BasicGraphs::Count)
	ui->preview->Create_Graph(ui->graphType_ComboBox->currentIndex(),
				  ui->numOfNodes1->value(),
				  ui->numOfNodes2->value(),
				  ui->graphHeight->value()
				  * screen->logicalDotsPerInchY()
				  - ui->nodeSize->value()
				  * screen->logicalDotsPerInchX(),
				  ui->graphWidth->value()
				  * screen->logicalDotsPerInchX()
				  - ui->nodeSize->value()
				  * screen->logicalDotsPerInchX(),
				  ui->complete_checkBox->isChecked());
    else
	select_Custom_Graph(fileDirectory + "/"
			    + ui->graphType_ComboBox->currentText()
			    + GRAPHICS_FILE_EXTENSION);

    this->style_Graph();
}


/*
 * Name:
 * Purpose:
 * Arguments:
 * Outputs:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

void MainWindow::on_NodeOutlineColor_clicked()
{
    QColor color = QColorDialog::getColor();
    QString s("background: #"
	      + QString(color.red() < 16 ? "0" : "")
	      + QString::number(color.red(), 16)
	      + QString(color.green() < 16 ? "0" : "")
	      + QString::number(color.green(), 16)
	      + QString(color.blue() < 16 ? "0" : "")
	      + QString::number(color.blue(), 16) + ";"
	      BUTTON_STYLE);
    ui->NodeOutlineColor->setStyleSheet(s);
    ui->NodeOutlineColor->update();
}


/*
 * Name:
 * Purpose:
 * Arguments:
 * Outputs:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

void MainWindow::on_NodeFillColor_clicked()
{
    QColor color = QColorDialog::getColor();

    QString s("background: #"
	      + QString(color.red() < 16 ? "0" : "")
	      + QString::number(color.red(), 16)
	      + QString(color.green() < 16 ? "0" : "")
	      + QString::number(color.green(), 16)
	      + QString(color.blue() < 16 ? "0" : "")
	      + QString::number(color.blue(), 16) + ";"
	      BUTTON_STYLE);
    ui->NodeFillColor->setStyleSheet(s);
    ui->NodeFillColor->update();
}


/*
 * Name:
 * Purpose:
 * Arguments:
 * Outputs:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

void MainWindow::on_EdgeLineColor_clicked()
{
    QColor color = QColorDialog::getColor();

    QString s("background: #"
	      + QString(color.red() < 16 ? "0" : "")
	      + QString::number(color.red(), 16)
	      + QString(color.green() < 16 ? "0" : "")
	      + QString::number(color.green(), 16)
	      + QString(color.blue() < 16 ? "0" : "")
	      + QString::number(color.blue(), 16) + ";"
	      BUTTON_STYLE);
    ui->EdgeLineColor->setStyleSheet(s);
    ui->EdgeLineColor->update();
}


/*
 * Name:
 * Purpose:
 * Arguments:
 * Outputs:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:	Simplified by JD on Jan 25/2016 to use less lines of code.
 *		(In honour of Robbie Burns?)
 */

void MainWindow::on_NumLabelCheckBox_clicked(bool checked)
{
    ui->NodeLabel1->setDisabled(checked);
    ui->NodeLabel2->setDisabled(checked);
}


/*
 * Name:
 * Purpose:
 * Arguments:
 * Outputs:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

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


/*
 * Name:	MainWindow::on_graphType_ComboBox_currentIndexChanged()
 * Purpose:
 * Arguments:	the index of the selected graph from the drop-down list.
 * Outputs:	nothing.
 * Modifies:	Various and sundry UI parameters.
 * Returns:	Nothing.
 * Assumptions:
 * Bugs:	Doesn't know what to do with graphs loaded from .grphc files.
 * Notes:
 */

void MainWindow::on_graphType_ComboBox_currentIndexChanged(int index)
{
    // Here are the default settings.  Over-ride as needed below.
    ui->numOfNodes1->setSingleStep(1);
    ui->numOfNodes1->setMinimum(1);
    ui->numOfNodes1->show();

    ui->numOfNodes2->setSingleStep(1);
    ui->numOfNodes2->setMinimum(1);
    ui->numOfNodes2->hide();

    ui->partitionLabel->setText("Nodes");

    ui->graphHeight->show();
    ui->heightLabel->show();
    ui->graphWidth->show();
    ui->widthLabel->show();

    ui->complete_checkBox->show();

    switch (index)
    {
      case BasicGraphs::Crown:
      case BasicGraphs::Cycle:
      case BasicGraphs::Helm:
      case BasicGraphs::Prism:
      case BasicGraphs::Complete:
      case BasicGraphs::Star:
      case BasicGraphs::Wheel:
      case BasicGraphs::BBTree:
      {
	  break;
      }
      case BasicGraphs::Antiprism:
      {
	  ui->numOfNodes1->setMinimum(6);
	  if (ui->numOfNodes1->value() % 2 == 1)
	      ui->numOfNodes1->setValue(ui->numOfNodes1->value() - 1);
	  ui->numOfNodes1->setSingleStep(2);
	  break;
      }
      case BasicGraphs::Bipartite:
      {
	  ui->partitionLabel->setText("Partitions");
	  ui->numOfNodes2->show();
	  break;
      }
      case BasicGraphs::Gear:
      {
	  ui->numOfNodes1->setMinimum(6);
	  break;
      }
      case BasicGraphs::Grid:
      {
	  ui->partitionLabel->setText("Rows & Cols");
	  ui->numOfNodes2->show();
	  break;
      }
      case BasicGraphs::Path:
      {
	  ui->graphHeight->hide();
	  ui->heightLabel->hide();
	  break;
      }
      case BasicGraphs::Petersen:
      {
	  ui->partitionLabel->setText("Nodes & Step");
	  ui->numOfNodes1->setMinimum(3);
	  ui->numOfNodes2->setValue(2);
	  ui->numOfNodes2->show();
	  break;

      }
      case BasicGraphs::Windmill:
      {
	  ui->partitionLabel->setText("Nodes & Blades");
	  ui->numOfNodes1->setMinimum(2);
	  ui->numOfNodes2->setMinimum(3);
	  if (ui->numOfNodes2->value() < 3)
	      ui->numOfNodes2->setValue(3);
	  ui->graphWidth->hide();
	  ui->widthLabel->hide();
	  break;
      }
      default:
	// ToDo: may need to change grphc file format to add
	qDebug() << "Unknown " << endl;
	ui->numOfNodes1->hide();
	ui->numOfNodes2->hide();
	ui->graphHeight->hide();
	ui->graphWidth->hide();
	ui->heightLabel->hide();
	ui->widthLabel->hide();
	ui->complete_checkBox->hide();
    }
}


/*
 * Name:
 * Purpose:
 * Arguments:
 * Outputs:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

void MainWindow::on_numOfNodes2_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
    if (ui->graphType_ComboBox->currentIndex() == BasicGraphs::Petersen)
    {
	if (ui->numOfNodes2->value()
	    > floor((ui->numOfNodes1->value() - 1) / 2))
	    ui->numOfNodes2->setValue(1);
    }
}


/*
 * Name:
 * Purpose:
 * Arguments:
 * Outputs:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

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


/*
 * Name:
 * Purpose:
 * Arguments:
 * Outputs:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

void MainWindow::generate_Freestyle_Edges()
{
    ui->canvas->setUpEdgeParams(ui->edgeSize->value(),
				ui->EdgeLabel->text(),
				ui->EdgeLabelSize->value(),
				ui->EdgeLineColor->
				palette().background().color());
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
	  while ((wItem = ui->scrollAreaWidgetContents->layout()->takeAt(0))
		 != 0)
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
	  foreach (QGraphicsItem * item, ui->canvas->scene()->items())
	  {
	      // Q: when would item be a 0 or nullptr?
	      if (item != 0 || item != nullptr)
	      {
		  if (item->type() == Graph::Type)
		  {
		      Graph * graph = qgraphicsitem_cast<Graph*>(item);
		      QLabel * graphLabel = new QLabel("Graph");
		      gridLayout->addWidget(graphLabel, i, 1);
		      i++;

		      gridLayout->addWidget(new QLabel("size"), i, 2);
		      gridLayout->addWidget(new QLabel("Text"), i, 3);
		      gridLayout->addWidget(new QLabel("Text Size"), i , 4);
		      gridLayout->addWidget(new QLabel("Outline Color"), i, 5);
		      gridLayout->addWidget(new QLabel("Fill Color"), i, 6);
		      i++;

		      foreach (QGraphicsItem * gItem, graph->childItems())
		      {
			  if (gItem != nullptr || gItem != 0)
			  {
			      if (gItem->type() == Node::Type)
			      {
				  Node * node
				      = qgraphicsitem_cast<Node*>(gItem);
				  QLineEdit * nodeEdit = new QLineEdit();
				  nodeEdit->setText("Node\n");
				  gridLayout->addWidget(nodeEdit);

				  QLabel * label = new QLabel("Node");
				  QPushButton * lineColorButton
				      = new QPushButton();
				  QPushButton * fillColorButton
				      = new QPushButton();

				  QDoubleSpinBox * sizeBox
				      = new QDoubleSpinBox();
				  QDoubleSpinBox * fontSizeBox
				      = new QDoubleSpinBox();

				  SizeController * sizeController
				      = new SizeController(node, sizeBox);
				  ColorLineController * colorLineController
				      = new ColorLineController(node,
								lineColorButton);
				  LabelController * weightController
				      = new LabelController(node, nodeEdit);
				  LabelSizeController *weightSizeController
				      = new LabelSizeController(node,
								fontSizeBox);
				  ColorFillController * colorFillController
				      = new ColorFillController(node,
								fillColorButton);

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
				  Edge * edge
				      = qgraphicsitem_cast<Edge*>(gItem);
				  QLineEdit * editEdge = new QLineEdit();
				  editEdge->setText("Edge\n");
				  gridLayout->addWidget(editEdge);

				  QLabel * label = new QLabel("Edge");
				  QPushButton * button = new QPushButton();
				  QDoubleSpinBox * sizeBox
				      = new QDoubleSpinBox();
				  QDoubleSpinBox * fontSizeBox
				      = new QDoubleSpinBox();

				  SizeController * sizeController
				      = new SizeController(edge, sizeBox);
				  ColorLineController * colorController
				      = new ColorLineController(edge, button);
				  LabelController * weightController
				      = new LabelController(edge, editEdge);
				  LabelSizeController * weightSizeController
				      = new LabelSizeController(edge,
								fontSizeBox);

				  gridLayout->addWidget(label, i, 1);
				  gridLayout->addWidget(sizeBox, i, 2);
				  gridLayout->addWidget(editEdge,  i, 3);
				  gridLayout->addWidget(fontSizeBox, i, 4);
				  gridLayout->addWidget(button,	 i, 5);

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

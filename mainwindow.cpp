/*
 * File:	mainwindow.cpp
 * Author:	Rachel Bood
 * Date:	January 25, 2015.
 * Version:	1.18
 *
 * Purpose:	Implement the main window and functions called from there.
 *
 * Modification history:
 * Jan 25, 2016 (JD V1.2):
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
 * Feb 3, 2016 (JD V1.3)
 *  (a) Removed "GraphSettings.h" since it is not used.
 *  (b) Minor formatting cleanups.
 *  (c) Changed "Grapha" to "Graphic".
 * Oct 11, 2019 (JD V1.4)
 *  (a) Allow blank lines and comments ([ \t]*#.*) in .grphc files.
 *  (b) Add some comments.  Reformat a bit.  Improve debug stmts.
 *  (c) Output a more informative and human-readable .grhpc file.
 *      Output the information as we compute it, rather than slowly
 *      constructing two big honkin' strings piece by piece and
 *	then outputting them.
 *  (d) Check for some error conditions and pop up a QErrorMessage in
 *      such cases.
 * Oct 13, 2019 (JD V1.5)
 *  (a) Before this version, node labels and corresponding font sizes
 *	were not stored in the .grphc file.  Go figure.
 *  (b) Look up TikZ-known colours (for TikZ output) and use those
 *	names rather than defining new colours for every coloured item.
 *  (c) When saving the graph, only do Linux-specific filename checks
 *	*after* confirming the selected filename is non-null.
 *  (d) Added some error checking when selecting files as possible
 *      graph-ic files, as well as checking contents of a graph-ic file.
 *  (e) Since node.{h,cpp} have been updated to allow superscripts as
 *	well as subscripts, and this allows the "edit" function to change
 *	vertex labels with subs/supers, the TikZ export has been changed
 *	to add '^{}' iff there is no '^' in the label.  This adds a
 *	spurious (but apparently harmless) empty superscript to labels
 *	which have neither sub nor super.
 *  (f) Added a number of function comments.
 * Oct 16, 2019 (JD V1.6)
 *  (a) When creating .grphc file, center graph on (0,0) so that when
 *      it is read back in it is centered (and thus accessible) in the
 *	preview pane.  Ditto
 *  (b) Output coords (to .grphc file) using a %.<VP_PREC_GRPHC>f
 *	format, rather than the default %6g format, to avoid numbers
 *	like -5.68434e-14 (DAMHIKT) in the output file.
 * Oct 16, 2019 (JD V1.7)
 *  (a) The original code was outputting (TikZ case) too many edge
 *      colour definitions, and in some cases they could be wrong.
 *	Fix that, and also add in the "well known colour" stuff from
 *	the nodes, as described in V1.5(b) above.
 *  (b) Only output the edge label and size if there is one.
 *  (c) Read edges from .grphc file with and without label info.
 * Nov 10, 2019 (JD V1.8)
 *  (a) Output the label font size before label so that a space before the
 *      label doesn't need to be trimmed.
 * Nov 10, 2019 (JD V1.9)
 *  (a) Fix apparently-erroneous use of logicalDotsPerInchX in the Y
 *	part of Create_Graph() (called in generate_Graph()).
 *  (b) Add some comments and some qDebug() statements.
 *  (c) Rename getWeightLabelSize() -> getLabelSize().
 *  (d) Rename "Weight" to "Label" for edge function names.
 *  (e) Do not call style_Graph() in generate_Graph() for "library"
 *	graphs, since doing so removes colour and size info.  This is
 *	only a partial fix to the problem, since this mod takes away
 *	the ability to style a library graph.  style_Graph() needs to
 *	be entirely rethought.
 *  (f) Add the rest of the "named" colours to lookupColour().
 *  (g) Remove a bunch of uninteresting output file types from the
 *	save dialog menu.
 * Nov 16, 2019 (JD V1.10)
 *  (a) Refactor save_graph() because it is unwieldy large
 *	into itself + saveEdgelist() (step 1 of many!).
 *  (b) Formatting tweaks.
 * Nov 17, 2019 (JD V1.11)
 *  (a) Move lookupColour() above where it is used and make it a
 *	non-class function.
 * Nov 18, 2019 (JD V1.12)
 *  (a) Fixed the edit window so that it looks nicer and behaves
 *	(vis-a-vis vertical spacing) much nicer (changes in
 *	on_tabWidget_currentChanged()).
 *	Modify code which outputs "Edit Graph" tab to make the Label
 *	box wider, since that is the one which needs the space (at
 *	least when using sub- or superscripts).
 *	Also put stretch at the bottom so that the layout manager
 *	doesn't distribute extra space in ugly places.
 *  (b) Add connections between a node (or edge) and its label in the
 *	edit tab, so that when a node (or edge) is deleted (in delete
 *	mode) the label in its row in the Edit Graph tab is also
 *	deleted.  (Everything else used to go, may as well do that
 *	too.)
 *  (c) Change "weight" to "label" in .grphc output edge comment.
 *  (d) Comment & formatting tweaking.
 * Nov 19, 2019 (JD V1.13)
 *  (a) Factor out saveTikZ() from save_Graph().
 *  (b) Add findDefaults() as a first step to improving the TikZ
 *	output (which will set default styles at the top of each graph
 *	and only output differences for nodes/edges that are different).
 * Nov 24, 2019 (JD V1.14)
 *  (a) Add a call on_graphType_ComboBox_currentIndexChanged(-1) the
 *      mainwindow constructor to initialize the "Create Graph" pane
 *	to a self-consistent shape.  Modify on_graphType_...() to not
 *	whine when its arg is < 0.
 * Nov 28, 2019 (JD V1.15)
 *  (a) Add dumpTikZ() and dumpGraphIc() functions and shortcuts (^T
 *	and ^G) to call them.  dumpGraphIc() not yet implemented.
 *  (b) findDefaults(): changed default node diameter from 0.2 to
 *	(qreal)0.2 to correctly match the data type.  Changed the
 *	floats to qreals in the hashes for the same reason.
 *      Also only change struct values from the initial defaults if
 *	there were actually observations in the corresponding hash table.
 *  (c) Improve saveTikZ() so that "nicer" TikZ code is output.
 *      Specifically, output default styles for nodes, edge lines and
 *	edge labels and when outputting individual nodes and edges
 *	only specify differences from the defaults.
 *	Also output things as we go, rather than creating a big
 *	honkin' string and outputting it at the end (can you say O(n^2)?).
 * Nov 28, 2019 (JD V1.16)
 *  (a) Factor out saveGraphIc() from save_Graph().
 *      Use this to implement dumpGraphIc().
 * Nov 29, 2019 (JD V1.17)
 *  (a) Rename "none" mode to "drag" mode, for less confusion.
 * Dec 1, 2019 (JD V1.18)
 *  (a) Add qDeb() / DEBUG stuff.
 *  (b) Print out more screen resolution & size info.
 *  (c) Add comments, minor code clean-ups.
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

#include <unordered_map>

#include <QDesktopWidget>
#include <QColorDialog>
#include <QGraphicsItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QShortcut>
#include <qmath.h>
#include <QtSvg/QSvgGenerator>
#include <QErrorMessage>
#include <QDate>


// Debugging aids (without editing the source file):
#ifdef DEBUG
static const bool debug = true;
#else
static const bool debug = false;
#endif

// Like qDebug(), but a little more literal, and turn-offable:
#define qDeb if (debug) \
        QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE,  \
                       QT_MESSAGELOG_FUNC).debug().noquote().nospace


#define GRAPHiCS_FILE_EXTENSION "grphc"
#define GRAPHiCS_SAVE_FILE	"Graph-ic (*." GRAPHiCS_FILE_EXTENSION ")"
#define GRAPHiCS_SAVE_SUBDIR	"graph-ic"
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
// Similar for vertex precision in .grphc output:
#define VP_PREC_GRPHC  4

/*
 * Name:	MainWindow
 * Purpose:	Main window constructor
 * Arguments:	QWidget *
 * Output:	Nothing.
 * Modifies:	private MainWindow variables
 * Returns:	Nothing.
 * Assumptions: ?
 * Bugs:	None known... so far.
 * Notes:	This is a cpp file used with the mainwindow.ui file
 */

MainWindow::MainWindow(QWidget * parent) :
QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    fileDirectory = QDir::currentPath().append("/" GRAPHiCS_SAVE_SUBDIR);
    QDir dir(fileDirectory);

    if (!dir.exists())
	if (!dir.mkdir(fileDirectory))
	{
	    QMessageBox::information(0, "Error", 
				     "Unable to create the subdirectory ./"
				     GRAPHiCS_SAVE_SUBDIR
				     " (where the graphs you create are "
				     "stored); I will boldly carry on anyway.  "
				     "Perhaps you can fix that problem from "
				     "a terminal or file manager before you "
				     "try to save a graph.");
	}

    ui->setupUi(this);
    this->generate_Combobox_Titles();

    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save_Graph()));
    connect(ui->actionOpen_File, SIGNAL(triggered()),
	    this, SLOT(load_Graphic_File()));

    // Ctrl-Q quits.
    // TODO: we should ask whether to save an unsaved graph or not
    //       before quitting.
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this, SLOT(close()));

    // Ctrl-O pops up the open file dialog.
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_O),
		  this, SLOT(load_Graphic_File()));

    // Save dialog pops up via Ctrl-S.
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this, SLOT(save_Graph()));

    // DEBUG HELP:
    // Dump TikZ to stdout
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_T), this, SLOT(dumpTikZ()));
    // Dump graph-ic code to stdout
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_G), this,
		  SLOT(dumpGraphIc()));

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

    QObject::connect(ui->canvas, SIGNAL(resetDragMode()),
		     ui->dragMode_radioButton, SLOT(click()));

    ui->dragMode_radioButton->click();

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

    // Initialize Create Graph pane to default values
    on_graphType_ComboBox_currentIndexChanged(-1);

#ifdef DEBUG
    // Info to help with dealing with HiDPI issues
    QScreen * screen = QGuiApplication::primaryScreen();
    printf("Logical DPI: (%.3f, %.3f)\nPhysical DPI: (%.3f, %.3f)\n",
	   screen->logicalDotsPerInchX(), screen->logicalDotsPerInchY(),
	   screen->physicalDotsPerInchX(), screen->physicalDotsPerInchY());
    printf("Physical size (mm): ht %.1f, wd %.3f\n",
	   screen->physicalSize().height(), screen->physicalSize().width());
    printf("Pixel resolution:  %d, %d\n",
	   screen->size().height(), screen->size().width());
    printf("devicePixelRatio: %.3f\n", screen->devicePixelRatio());
#endif
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
 * Name:	setKeyStatusLabel()
 * Purpose:	Set the help text to the given string.
 * Arguments:   The new help text (or empty string).
 * Outputs:	Nothing.
 * Modifies:	The help area of the main window.
 * Returns:	Nothing.
 * Assumptions:	?
 * Bugs:	None(?).
 * Notes:	Is this a confusing name?
 */

void
MainWindow::setKeyStatusLabel(QString text)
{
    ui->keyPressStatus_label->setText(text);
}



/*
 * Name:	generate_Combobox_Titles()
 * Purpose:	Populate the list of graph types with the defined
 *		basic types, then add a separator, then call
 *		load_Graphic_Library() to load the local graph library
 *		(if any).
 * Arguments:	None.
 * Outputs:	Nothing.
 * Modifies:	The ui->graphType_ComboBox
 * Returns:	Nothing.
 * Assumptions:	ui->graphType_ComboBox is set up.
 * Bugs:	?
 * Notes:	None.
 */

void
MainWindow::generate_Combobox_Titles()
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
 * Name:	saveEdgelist()
 * Purpose:	Save the current graph as an edgelist.
 * Arguments:	A file pointer to write to and the node list.
 * Outputs:	An edge list of the graph to the file.
 * Modifies:	Nothing.
 * Returns:	True on success.
 * Assumptions:	Args are valid.
 * Bugs:	?!
 * Notes:	Currently always returns T, but maybe in the future ...
 */

bool
saveEdgelist(QTextStream &outfile, QVector<Node *> nodes)
{
    QString edges = "";

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
    outfile << nodes.count() << "\n";
    outfile << edges;

    return true;
}



/*
 * Name:	lookupColour()
 * Purpose:	Given an RGB colour, see if this is a colour known to
 *		TikZ by a human-friendly name; if so, return the name.
 * Arguments:	A QColor.
 * Outputs:	Nothing.
 * Modifies:	Nothing.
 * Returns:	A TikZ colour name (as a QString) or nullptr.
 * Assumptions:	None.
 * Bugs:	This is shamefully unsophisticated.
 *		The colours known to TikZ may be a moving target.
 * Notes:	At time of writing (Oct 2019), the following are
 *		(allegedly) the known colours (in RGB):
 *		red, green, blue, cyan, magenta, yellow, black,
 *		gray (128,128, 128), darkgray (64,64,64),
 *		lightgray (191,191,191), brown (191,128,64), lime (191,255,0),
 *		olive (127,127,0), orange (255,128,0), pink (255,191,191),
 *		purple (191,0,64), teal (0,128,128), violet (128,0,128)
 *		and white (modulo the fact that 127~=128, 63~=64, and so on).
 *		To get the RGB values from a PDF file with cmyk colours, I used
 *		    gs -dUseFastColor file.pdf
 *		which does a direct mapping of cmyk to RGB without
 *		using any ICC colour profiles, and then used xmag.
 *		Not knowing what numbers to turn to what names, I will
 *		only map the subset of the above names found in
 *		.../texmf-dist/tex/generic/pgf/utilities/pgfutil-plain.def,
 *		as well as lightgray and darkgray.
 *		Note that some of these are quite different than X11's rgb.txt.
 */

// Allow a bit of slop in some cases (see noted examples below).
#define     CLOSE(x, c)	    (((x) == (c)) || ((x) == ((c) + 1)))

QString
lookupColour(QColor color)
{
    int r = color.red();
    int g = color.green();
    int b = color.blue();

    if (r == 0)
    {
	if (g == 0 && b == 0)
	    return "black";
	if (g == 255 && b == 0)
	    return "green";
	if (g == 0 && b == 255)
	    return "blue";
	if (g == 255 && b == 255)
	    return "cyan";
	if (CLOSE(g, 127) && CLOSE(b, 127))
	    return "teal";
	return nullptr;
    }

    if (CLOSE(r, 63) && CLOSE(g, 63) && CLOSE(b, 63))
	return "darkgray";

    if (CLOSE(r, 127))			    // 0.5 -> 127.5
    {
	if (CLOSE(g, 127) && CLOSE(b, 127))
	    return "gray";
	if (CLOSE(g, 127) && b == 0)
	    return "olive";
	if (g == 0 && CLOSE(b, 127))
	    return "violet";
	return nullptr;
    }

    if (CLOSE(r, 191))			    // 0.75 -> 191.25
    {
	if (g == 0 && CLOSE(b, 63))	    // 0.25 -> 63.75
	    return "purple";
	if (CLOSE(g, 127) && CLOSE(b, 63))
	    return "brown";
	if (g == 255 && b == 0)
	    return "lime";
	if (CLOSE(g, 191) && CLOSE(b, 191))
	    return "lightgray";
	return nullptr;
    }

    if (r == 255)
    {
	if (g == 255 && b == 255)
	    return "white";
	if (g == 0 && b == 0)
	    return "red";
	if (g == 0 && b == 255)
	    return "magenta";
	if (g == 255 && b == 0)
	    return "yellow";
	if (CLOSE(g, 127) && b == 0)
	    return "orange";
	if (CLOSE(g, 191) && CLOSE(b, 191))
	    return "pink";
	return nullptr;
    }
    return nullptr;
}



typedef struct
{
    int fillR, fillG, fillB;
    int lineR, lineG, lineB;
    qreal nodeDiameter;		// inches
    qreal labelSize;		// points; See Node::setNodeLabelSize()
} nodeInfo;

typedef struct
{
    int lineR, lineG, lineB;
    qreal penSize;		// pixels (!); thickness of line.
    qreal labelSize;		// points
} edgeInfo;



/*
 * Name:	findDefaults()
 * Purpose:	Find the most common line colours, fill colours, pen widths,
 *		and so on, of the set of nodes and edges in the graph.
 * Arguments:	The list of nodes and int *'s to hold the R, G and B values.
 * Outputs:	Nothing.
 * Modifies:	Nothing.
 * Returns:	The two sets of R, G and B.
 * Assumptions:	None.
 * Bugs:	?
 * Notes:	Returns (0,0,0) in the case there are no edges or vertices.
 */

void
findDefaults(QVector<Node *> nodes,
	     nodeInfo * nodeDefaults_p, edgeInfo * edgeDefaults_p)
{
    // Set the default defaults (sic).
    // TODO: These values should really be #defines somewhere.
    *nodeDefaults_p = {255, 255, 255, 0, 0, 0, (qreal)0.2, 12};
    *edgeDefaults_p = {0, 0, 0, (qreal)1., (qreal)12.};

    if (nodes.count() == 0)
	return;

    int max_count, result, colour, R, G, B;
    qreal fresult;
    std::unordered_map<int, int> vFillColour;
    std::unordered_map<int, int> vLineColour;
    std::unordered_map<qreal, int> vNodeDiam;
    std::unordered_map<qreal, int> vLabelSize;
    std::unordered_map<int, int> eLineColour;
    std::unordered_map<qreal, int> ePenSize;
    std::unordered_map<qreal, int> eLabelSize;

    // Populate all the node hashes.
    for (int i = 0; i < nodes.count(); i++)
    {
	Node * node = nodes.at(i);
	R = node->getFillColour().red();
	G = node->getFillColour().green();
	B = node->getFillColour().blue();
	colour = R << 16 | G << 8 | B;
	vFillColour[colour]++;

	R = node->getLineColour().red();
	G = node->getLineColour().green();
	B = node->getLineColour().blue();
	colour = R << 16 | G << 8 | B;
	vLineColour[colour]++;

	vNodeDiam[node->getDiameter()]++;
	vLabelSize[node->getLabelSize()]++;
    }

    max_count = 0;
    result = nodeDefaults_p->fillR << 16 | nodeDefaults_p->fillG << 8
	| nodeDefaults_p->fillB;
    for (auto item : vFillColour)
    {
        if (max_count < item.second)
	{
            result = item.first;
            max_count = item.second;
        }
    }
    nodeDefaults_p->fillR = result >> 16;
    nodeDefaults_p->fillG = (result >> 8) & 0xFF;
    nodeDefaults_p->fillB = result & 0xFF;
    // printf("nodeFill: (%d,%d,%d) count = %d\n", nodeDefaults_p->fillR,
    //   nodeDefaults_p->fillG, nodeDefaults_p->fillB, max_count);

    max_count = 0;
    result = nodeDefaults_p->lineR << 16 | nodeDefaults_p->lineG << 8
	| nodeDefaults_p->lineB;
    for (auto item : vLineColour)
    {
        if (max_count < item.second)
	{
            result = item.first;
            max_count = item.second;
        }
    }
    nodeDefaults_p->lineR = result >> 16;
    nodeDefaults_p->lineG = (result >> 8) & 0xFF;
    nodeDefaults_p->lineB = result & 0xFF;

    // printf("nodeLine: (%d,%d,%d) count = %d\n", nodeDefaults_p->lineR,
    //   nodeDefaults_p->lineG, nodeDefaults_p->lineB, max_count);

    max_count = 0;
    fresult = nodeDefaults_p->nodeDiameter;
    for (auto item : vNodeDiam)
    {
        if (max_count < item.second)
	{
            fresult = item.first;
            max_count = item.second;
        }
    }
    nodeDefaults_p->nodeDiameter = fresult;
    printf("nodeDiam: %.4f count = %d\n", fresult, max_count);

    max_count = 0;
    fresult = nodeDefaults_p->labelSize;
    for (auto item : vLabelSize)
    {
        if (max_count < item.second)
	{
            fresult = item.first;
            max_count = item.second;
        }
    }
    nodeDefaults_p->labelSize = fresult;
    printf("nodeLabelSize: %.4f count = %d\n", fresult, max_count);

    // printf("nodes.count() = %d\n", nodes.count());
    for (int i = 0; i < nodes.count(); i++)
    {
	// printf("node %d has %d edges\n", i, nodes.at(i)->edgeList.count());
	for (int j = 0; j < nodes.at(i)->edgeList.count(); j++)
	{
	    // TODO: see TODO in Edge section of saveTikZ().
	    Edge * edge = nodes.at(i)->edgeList.at(j);
	    int sourceID = edge->sourceNode()->getID();
	    int destID = edge->destNode()->getID();
	    if ((sourceID == i && destID > i)
		|| (destID == i && sourceID > i))
	    {
		R = edge->getColour().red();
		G = edge->getColour().green();
		B = edge->getColour().blue();
		colour = R << 16 | G << 8 | B;
		eLineColour[colour]++;
		// Don't count 0's, they are likely bogus.
		if (edge->getPenWidth() > 0)
		    ePenSize[edge->getPenWidth()]++;
		// Only count the label sizes for edges which have a label.
		if (edge->getLabel().length() > 0)
		{
		    printf("i=%d, j=%d, label=/%s/, size=%.1f\n",
			   i, j, edge->getLabel().toLatin1().data(),
			   edge->getLabelSize());
		    if (edge->getLabelSize() >= 1)
			eLabelSize[edge->getLabelSize()]++;
		}
	    }
	}
    }

    max_count = 0;
    result = edgeDefaults_p->lineR << 16 | edgeDefaults_p->lineG << 8
	| edgeDefaults_p->lineB;
    for (auto item : eLineColour)
    {
        if (max_count < item.second)
	{
            result = item.first;
            max_count = item.second;
        }
    }
    edgeDefaults_p->lineR = result >> 16;
    edgeDefaults_p->lineG = (result >> 8) & 0xFF;
    edgeDefaults_p->lineB = result & 0xFF;
    printf("edgeColour: (%d,%d,%d) count = %d\n", edgeDefaults_p->lineR,
	   edgeDefaults_p->lineG, edgeDefaults_p->lineB, max_count);

    max_count = 0;
    fresult = edgeDefaults_p->penSize;
    for (auto item : ePenSize)
    {
        if (max_count < item.second)
	{
            fresult = item.first;
            max_count = item.second;
        }
    }
    edgeDefaults_p->penSize = fresult;
    printf("edgePenSize: %.4f count = %d\n", fresult, max_count);

    max_count = 0;
    fresult = edgeDefaults_p->labelSize;
    for (auto item : eLabelSize)
    {
        if (max_count < item.second)
	{
            fresult = item.first;
            max_count = item.second;
        }
    }
    edgeDefaults_p->labelSize = fresult;
    printf("edgeLabelSize: %.4f count = %d\n", fresult, max_count);
}



/*
 * Name:	saveTikZ()
 * Purpose:	Save the current graph as a (LaTeX) TikZ file.
 * Arguments:	A file pointer to write to and the node list.
 * Outputs:	A TikZ picture (in LaTeX syntax) which draws the graph.
 * Modifies:	Nothing.
 * Returns:	True on success.
 * Assumptions:	Args are valid.
 * Bugs:	This is grotesquely long.
 * Notes:	Currently always returns T, but maybe in the future ...
 * 		Idea: to minimize the amount of TikZ code, the most
 *		common vertex and edge attributes are found and stored
 *		in the styles v/.style, e/.style and l/.style.  Then
 *		when drawing a particular vertex or edge, anything not
 *		matching the default is output, over-riding the style.
 */

bool
saveTikZ(QTextStream &outfile, QVector<Node *> nodes)
{
    printf("saveTikZ() called!\n");
    // TODO: only define a given colour once.
    // (Hash the known colours, and use the name if already defined?)
    QScreen * screen = QGuiApplication::primaryScreen();

    nodeInfo nodeDefaults;
    edgeInfo edgeDefaults;

    // Output the boilerplate TikZ picture code
    outfile << "\\begin{tikzpicture}[x=1in, y=1in, xscale=1, yscale=1,\n";

    // Now find and output the default node and edge details
    findDefaults(nodes, &nodeDefaults, &edgeDefaults);

    // Define the default styles.
    // If the line or fill colour is a TikZ "known" colour,
    // use the name.  Otherwise define a colour.
    // Q: why did Rachel output in RGB, as opposed to rgb?
    // Note: drawings may need to be tweaked by hand if they are to be
    //	     printed, due to the RGB/rgb <-> cmyk conversion nightmare.
    // Note: TikZ for plain TeX does not support the cmyk colourspace
    //	     nor (without JD's addition) the RGB colourspace.
    bool defineDefNodeFillColour = false;
    QColor defNodeFillColour
	= QColor(nodeDefaults.fillR, nodeDefaults.fillG, nodeDefaults.fillB);
    QString defNodeFillColourName = lookupColour(defNodeFillColour);
    if (defNodeFillColourName == nullptr)
    {
	defineDefNodeFillColour = true;
	outfile << "    n/.style={fill=defNodeFillColour, "; 
    }
    else
	outfile << "    n/.style={fill=" << defNodeFillColourName << ", "; 

    bool defineDefNodeLineColour = false;
    QColor defNodeLineColour
	= QColor(nodeDefaults.lineR, nodeDefaults.lineG, nodeDefaults.lineB);
    QString defNodeLineColourName = lookupColour(defNodeLineColour);
    if (defNodeLineColourName == nullptr)
    {
	defineDefNodeLineColour = true;
	outfile << "draw=defNodeLineColour, shape=circle,\n";
    }
    else
	outfile << "draw=" << defNodeLineColourName << ", shape=circle,\n"; 

    outfile << "\tminimum size=" << nodeDefaults.nodeDiameter << "in, "
	    << "inner sep=0, "
	    << "font=\\fontsize{" << nodeDefaults.labelSize
	    << "}{1}\\selectfont},\n";


    // e style gets 'draw=<colour>' and 'line width=<stroke width>' options
    // l style gets 'font=<spec>' options.  If we want to change the
    // label text colour to something else, use 'text=<colour>';
    // if we want to draw a box around the label text, use 'draw=<colour>'
    // (and "circle" to get a circle instead of a box).
    bool defineDefEdgeLineColour = false;
    QColor defEdgeLineColour
	= QColor(edgeDefaults.lineR, edgeDefaults.lineG, edgeDefaults.lineB);
    QString defEdgeLineColourName = lookupColour(defEdgeLineColour);
    if (defEdgeLineColourName == nullptr)
    {
	defineDefEdgeLineColour = true;
	outfile << "    e/.style={draw=defEdgeLineColour";
    }
    else
	outfile << "    e/.style={draw=" << defEdgeLineColourName;

    outfile << ", line width="
	    << QString::number(edgeDefaults.penSize
			       / screen->logicalDotsPerInchX(),
			       'f', ET_PREC_TIKZ) << "in},\n";
    outfile << "    l/.style={font=\\fontsize{" << edgeDefaults.labelSize
	    << "}{1}\\selectfont}]\n";

    // We have now finished the generic style.
    // Output default colours, if needed.
    if (defineDefNodeFillColour)
    {
	outfile << "\\definecolor{defNodeFillColour} {RGB} {"
		<< QString::number(defNodeFillColour.red())
		<< "," << QString::number(defNodeFillColour.green())
		<< "," << QString::number(defNodeFillColour.blue())
		<< "}\n";
    }
    if (defineDefNodeLineColour)
    {
	outfile << "\\definecolor{defNodeLineColour} {RGB} {"
		<< QString::number(defNodeLineColour.red())
		<< "," << QString::number(defNodeLineColour.green())
		<< "," << QString::number(defNodeLineColour.blue())
		<< "}\n";
    }
    if (defineDefEdgeLineColour)
    {
	outfile << "\\definecolor{defEdgeLineColour} {RGB} {"
		<< QString::number(defEdgeLineColour.red())
		<< "," << QString::number(defEdgeLineColour.green())
		<< "," << QString::number(defEdgeLineColour.blue())
		<< "}\n";
    }

    QString edgeStyles = "";

    // Nodes: find center of graph, output graph centered on (0, 0)
    qreal minx = 0, maxx = 0, miny = 0, maxy = 0;
    if (nodes.count() > 0)
    {
	Node * node = nodes.at(0);
	minx = maxx = node->scenePos().rx();
	miny = maxy = node->scenePos().ry();
    }
    for (int i = 1; i < nodes.count(); i++)
    {
	Node * node = nodes.at(i);
	qreal x = node->scenePos().rx();
	qreal y = node->scenePos().ry();
	if (x > maxx)
	    maxx = x;
	else if (x < minx)
	    minx = x;
	if (y > maxy)
	    maxy = y;
	else if (y < minx)
	    miny = y;
    }
    qreal midx = (maxx + minx) / 2.;
    qreal midy = (maxy + miny) / 2.;

    // Sample output for a node:
    //  \definecolor{n<n>lineClr} {RGB} {R,G,B}   (if not default)
    //  \definecolor{n<n>fillClr} {RGB} {R,G,B}   (if not default)
    //	\node (v<n>) at (x,y) [n ,diffs from defaults] {$<node label>$};
    // Note that to change the text colour we could add (e.g.) "text=red"
    // to the \node options (or to the 'n' style above).
    // Note that TikZ is OK with a spurious ',' at the end of the options;
    // this fact is used to simplify the code below.
    for (int i = 0; i < nodes.count(); i++)
    {
	QString fillColour = "";
	QString lineColour = "";
	Node * node = nodes.at(i);
	bool doNewLine = false;

	if (node->getFillColour() != defNodeFillColour)
	{
	    // printf("Node %d: fill colour non-default; ", i);
	    fillColour = lookupColour(node->getFillColour());
	    if (fillColour == nullptr)
	    {
		// Output the colour defn:
		fillColour = "n" + QString::number(i) + "fillClr";
		outfile << "\\definecolor{" << fillColour << "} {RGB} {"
			<< QString::number(node->getFillColour().red())
			<< "," << QString::number(node->getFillColour().green())
			<< "," << QString::number(node->getFillColour().blue())
			<< "}\n";
	    }
	    // Wrap the fillColour with the TikZ syntax for later consumption:
	    fillColour = ", fill=" + fillColour;
	    doNewLine = true;
	    //printf("fillColour = /%s/\n", fillColour.toLatin1().data());
	}
	if (node->getLineColour() != defNodeLineColour)
	{
	    //printf("Node %d: line colour non-default\n", i);
	    lineColour = lookupColour(node->getLineColour());
	    if (lineColour == nullptr)
	    {
		lineColour = "n" + QString::number(i) + "lineClr";
		outfile << "\\definecolor{" << lineColour << "}{RGB}{"
			<< QString::number(node->getLineColour().red())
			<< "," << QString::number(node->getLineColour().green())
			<< "," << QString::number(node->getLineColour().blue())
			<< "}\n";
	    }
	    lineColour = ", draw=" + lineColour;
	    doNewLine = true;
	    //printf("SETTING lineColour = /%s/\n", lineColour.toLatin1().data());
	}

	// Use (x,y) coordinate system for node positions.
	outfile << "\\node (v" << QString::number(i) << ") at ("
		<< QString::number((node->scenePos().rx() - midx)
				   / screen->logicalDotsPerInchX(),
				   'f', VP_PREC_TIKZ)
		<< ","
		<< QString::number((node->scenePos().ry() - midy)
				   / -screen->logicalDotsPerInchX(),
				   'f', VP_PREC_TIKZ)
		<< ") [n";
//	printf("HERE: lineColour = /%s/\n", lineColour.toLatin1().data());
	outfile << fillColour << lineColour;
	if (node->getDiameter() != nodeDefaults.nodeDiameter)
	{
	    outfile << ", minimum size=" << QString::number(node->getDiameter())
		    << "in";
	    doNewLine = true;
	}

	// Output the node label and its font size if and only if
	// there is a node label.
	if (node->getLabel().length() > 0)
	{
	    if (node->getLabelSize() != nodeDefaults.labelSize)
	    {
		if (doNewLine)
		    outfile << ",\n\t";
		else
		    outfile << ", ";
		outfile << "font=\\fontsize{"
			<< QString::number(node->getLabelSize()) // Font size
			<< "}{1}\\selectfont";
	    }

	    QString thisLabel = node->getLabel();
	    // TODO: this check just checks for a '^', but
	    // if a subscript itself has a superscript
	    // and there is no (top-level) superscript, we would
	    // fail to add the "^{}" text.
	    // printf("thisLabel = /%s/\n", thisLabel.toLatin1().data());
	    if (thisLabel.indexOf('^') != -1 || thisLabel.indexOf('_') == -1)
		outfile << "] {$" << thisLabel << "$};\n";
	    else
		outfile << "] {$" << thisLabel << "^{}$};\n";
	}
	else
	    outfile << "] {$$};\n";
    }

    // Sample output for an edge:
    //  \definecolor{e<n>_<m>lineClr} {RGB} {R,G,B}   (if not default)
    //  \path (v<n>) edge[e, diff from defaults] node[l, diff from defaults]
    //		{$<edge label>} (v_<m>);
    for (int i = 0; i < nodes.count(); i++)
    {
	bool wroteExtra = false;
	printf("Node %d has %d edges\n", i, nodes.at(i)->edgeList.count());
	for (int j = 0; j < nodes.at(i)->edgeList.count(); j++)
	{
	    // TODO: is it possible that with various and sundry
	    // operations on graphs neither the sourceID nor the
	    // destID of an edge in nodes.at(i)'s list is equal to
	    // i, and thus some edge won't be printed?  If so,
	    // should we just test "sourceID < destID in the if
	    // test immediately below?
	    Edge * edge = nodes.at(i)->edgeList.at(j);
	    int sourceID = edge->sourceNode()->getID();
	    int destID = edge->destNode()->getID();
	    if ((sourceID == i && destID > i)
		|| (destID == i && sourceID > i))
	    {
		printf("i %d j %d srcID %d dstID %d", i, j, sourceID, destID);
		printf("\tlabel = /%s/\n", edge->getLabel().toLatin1().data());
		QString lineColour = "";
		if (edge->getColour() != defEdgeLineColour)
		{
		    printf("E %d,%d: colour non-default\n", sourceID, destID);
		    lineColour = lookupColour(edge->getColour());
		    if (lineColour == nullptr)
		    {
			lineColour = "e" + QString::number(sourceID) + "_"
			    + QString::number(destID) + "lineClr";
			outfile << "\\definecolor{" << lineColour << "}{RGB}{"
				<< QString::number(edge->getColour().red())
				<< ","
				<< QString::number(edge->getColour().green())
				<< ","
				<< QString::number(edge->getColour().blue())
				<< "}\n";
		    }
		    lineColour = ", draw=" + lineColour;
		    wroteExtra = true;
		    printf("SETTING lineColour = /%s/\n", lineColour.toLatin1().data());
		}

		outfile << "\\path (v"
			<< QString::number(sourceID)
			<< ") edge[e" << lineColour;
		if (edge->getPenWidth() != edgeDefaults.penSize)
		{
		    outfile << ", line width="
			    << QString::number(edge->getPenWidth()
					       / screen->logicalDotsPerInchX(),
					       'f', ET_PREC_TIKZ)
			    << "in";
		    wroteExtra = true;
		}

		// Output a \n iff we have both a non-default line
		// width and a non-default label size.
		if (edge->getLabel().length() > 0
		    && edge->getLabelSize() != edgeDefaults.labelSize
		    && wroteExtra)
		    outfile << "]\n\tnode[l";
		else
		    outfile << "] node[l";

		// Output edge label size (and the "select font" info)
		// if and only if the edge has a label.
		if (edge->getLabel().length() > 0)
		{
		    if (edge->getLabelSize() != edgeDefaults.labelSize)
		    {
			outfile << ", font=\\fontsize{"
				<< QString::number(edge->getLabelSize())
				<< "}{1}\\selectfont";
		    }
		    outfile << "] {$" << edge->getLabel() << "$}";
		}
		else
		    outfile << "] {$$}";

		// Finally, output the other end of the edge:
		outfile << " (v"
			<< QString::number(destID)
			<< ");\n";
	    }
	}
    }

    outfile << "\\end{tikzpicture}\n";

    return true;
}



/*
 * Name:	saveGraphIc()
 * Purpose:	Output the description of the graph in "graph-ic" format.
 * Arguments:	A file pointer to write to and the node list.
 * Outputs:	The graph-ic description of the current graph.
 * Modifies:	Nothing.
 * Returns:	True on success.
 * Assumptions:	Args are valid.
 * Bugs:	
 * Notes:	Currently always returns T, but maybe in the future ...
 *		Normally vertex and edge label info is not output if
 *		the label is empty, but if outputExtra = T these
 *		are output (this is a debugging aid), as well as some
 *		extra info.
 */

bool
saveGraphIc(QTextStream &outfile, QVector<Node *> nodes, bool outputExtra)
{
    printf("saveGraphIc() called!\n");
    // Use some painful Qt constructs to output the node and edge
    // information with a more readable format.
    // Note that tests with explicitly setting the format to 'g'
    // and the precision to 6 indicate that Qt5.9.8 (on S64-14.2)
    // will do whatever it damn well pleases, vis-a-vis the number
    // of chars printed.
    outfile << "# graph-ic graph definition created ";
    QDateTime dateTime = dateTime.currentDateTime();
    outfile << dateTime.toString("yyyy-MM-dd hh:mm:ss") << "\n\n";

    outfile << "# The number of nodes in this graph:\n";
    outfile << nodes.count() << "\n\n";

    QString nodeInfo = QString::number(nodes.count()) + "\n\n";

    outfile << "# The node descriptions; the format is:\n";
    outfile << "# x,y, diameter, rotation, fill r,g,b,\n";
    outfile << "#      outline r,g,b[, label font size,label]\n";

    // In some cases I have created a graph where all the
    // coordinates are negative and "large", and the graph is not
    // visible in the preview pane when I load it, which also
    // means (as of time of writing) that I can't drag it to the
    // canvas.  Thus the graph is effectively lost.  Avoid this by
    // centering the graph on (0, 0) when writing it out.
    qreal minx = 0, maxx = 0, miny = 0, maxy = 0;
    if (nodes.count() > 0)
    {
	Node * node = nodes.at(0);
	minx = maxx = node->scenePos().rx();
	miny = maxy = node->scenePos().ry();
    }
    for (int i = 1; i < nodes.count(); i++)
    {
	Node * node = nodes.at(i);
	qreal x = node->scenePos().rx();
	qreal y = node->scenePos().ry();
	if (x > maxx)
	    maxx = x;
	else if (x < minx)
	    minx = x;
	if (y > maxy)
	    maxy = y;
	else if (y < minx)
	    miny = y;
    }

    qreal midx = (maxx + minx) / 2.;
    qreal midy = (maxy + miny) / 2.;
    for (int i = 0; i < nodes.count(); i++)
    {
	// TODO: s/,/\\/ before writing out label.  Undo this when reading.
	Node * node = nodes.at(i);
	outfile << "# Node " + QString::number(i) + ":\n";
	outfile << QString::number(node->scenePos().rx() - midx,
				   'f', VP_PREC_GRPHC) << ","
		<< QString::number(node->scenePos().ry() - midy,
				   'f', VP_PREC_GRPHC) << ", "
		<< QString::number(node->getDiameter()) << ", "
		<< QString::number(node->getRotation()) << ", "
		<< QString::number(node->getFillColour().redF()) << ","
		<< QString::number(node->getFillColour().greenF()) << ","
		<< QString::number(node->getFillColour().blueF()) << ", "
		<< QString::number(node->getLineColour().redF()) << ","
		<< QString::number(node->getLineColour().greenF()) << ","
		<< QString::number(node->getLineColour().blueF());
	// Output the node label and its font size if and only if
	// there is a node label.
	if (node->getLabel().length() > 0 || outputExtra)
	{
	    outfile << ", "
		    << QString::number(node->getLabelSize())
		    << ","
		    << node->getLabel();
	}
	outfile << "\n";
    }

    outfile << "\n# Edge descriptions; the format is:\n"
	    << "# u, v, dest_radius, source_radius, rotation, pen_width,\n"
	    << "#       line r,g,b[, label font size, label]\n";
	    
    for (int i = 0; i < nodes.count(); i++)
    {
	for (int j = 0; j < nodes.at(i)->edgeList.count(); j++)
	{
	    Edge * edge = nodes.at(i)->edgeList.at(j);
	    if (outputExtra)
	    {
		outfile << "# Looking at i, j = "
			<< QString::number(i) << ", " << QString::number(j)
			<< "  ->  src, dst = "
			<< QString::number(edge->sourceNode()->getID())
			<< ", "
			<< QString::number(edge->destNode()->getID())
			<< "\n";
	    }

	    int printThisOne = 0;
	    int sourceID = edge->sourceNode()->getID();
	    int destID = edge->destNode()->getID();
	    if (sourceID == i && destID > i)
	    {
		printThisOne++;
		outfile << QString("%1").arg(sourceID, 2, 10, QChar(' '))
			<<  ","
			<< QString("%1").arg(destID, 2, 10, QChar(' '));
	    }
	    else if (destID == i && sourceID > i)
	    {
		printThisOne++;
		outfile << QString("%1").arg(destID, 2, 10, QChar(' '))
			<<  ","
			<< QString("%1").arg(sourceID, 2, 10, QChar(' '));
	    }
	    if (printThisOne)
	    {
		outfile << ", " << QString::number(edge->getDestRadius())
			<< ", " << QString::number(edge->getSourceRadius())
			<< ", " << QString::number(edge->getRotation())
			<< ", " << QString::number(edge->getPenWidth())
			<< ", "
			<< QString::number(edge->getColour().redF())
			<< ","
			<< QString::number(edge->getColour().greenF())
			<< ","
			<< QString::number(edge->getColour().blueF());
		if (edge->getLabel().length() > 0 || outputExtra)
		{
		    // TODO: check for ',' in the label and deal with it.
		    outfile << ", "
			    << edge->getLabelSize()
			    << ","
			    << edge->getLabel();
		}
		outfile << "\n";
	    }
	}
    }

    return true;
}



/*
 * Name:	save_Graph()
 * Purpose:	Saves an image/tikz/grphc/edgelist version of the canvas.
 * Arguments:	None.
 * Output:	A file corresponding to the graph.
 * Modifies:	Nothing.
 * Returns:	True on file successfully saved, false otherwise.
 * Assumptions: ?
 * Bugs:	This function is too long.
 * Notes:	none
 */

bool
MainWindow::save_Graph()
{
    QString fileTypes = "";

    fileTypes += GRAPHiCS_SAVE_FILE ";;"
	TIKZ_SAVE_FILE ";;"
	EDGES_SAVE_FILE	";;";

    foreach (QByteArray format, QImageWriter::supportedImageFormats())
    {
	// Remove offensive and redundant file types.
	// Even with these, there still may a confusing number of choices.
	if (QString(format).toUpper() == "BMP")	    // Winblows bitmap
	    continue;
	if (QString(format).toUpper() == "CUR")	    // Winblows cursor
	    continue;
	if (QString(format).toUpper() == "DDS")	    // Winblows directdraw sfc
	    continue;
	if (QString(format).toUpper() == "ICO")	    // Winblows icon
	    continue;
	if (QString(format).toUpper() == "ICNS")    // Apple icon
	    continue;
	if (QString(format).toUpper() == "PBM")	    // Portable bitmap
	    continue;
	if (QString(format).toUpper() == "PGM")	    // Portable gray map
	    continue;
	if (QString(format).toUpper() == "PPM")	    // Portable pixmap
	    continue;
	if (QString(format).toUpper() == "XBM")	    // X bitmap 
	    continue;
	if (QString(format).toUpper() == "XBM")	    // X bitmap 
	    continue;
	if (QString(format).toUpper() == "XPM")	    // X pixmap
	    continue;
	if (QString(format).toUpper() == "WBMP")    // wireless bitmap
	    continue;
	if (QString(format).toUpper() == "TIFF")    // Just list "tif"
	    continue;
	if (QString(format).toUpper() == "JPEG")    // Just list "jpg"
	    continue;
	fileTypes += tr("%1 (*.%2);;").arg(QString(format).toUpper(),
					   QString(format).toLower());
    }

    fileTypes += SVG_SAVE_FILE ";;"
	"All Files (*)";

    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(this, "Save graph",
						    fileDirectory, fileTypes,
						    &selectedFilter);
    if (fileName.isNull())
	return false;

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
	    qDebug() << "?? save_Graph() could not find extension in "
		     << selectedFilter;
	    return false;
	}

	QString extension = selectedFilter.mid(start, end - start);
	qDebug() << "save_Graph(): computed extension is" << extension;
	fileName += extension;
	qDebug() << "save_Graph(): computed filename is" << fileName;
    }
#endif

    // Handle all image (i.e., non-text) outputs here;
    // check for all known text-file types.
    // TODO: should we use QFileInfo(fileName).extension().lower();
    bool saveStatus = ui->snapToGrid_checkBox->isChecked();
    if (saveStatus)
	ui->canvas->snapToGrid(false);

    if (selectedFilter != GRAPHiCS_SAVE_FILE
	&& selectedFilter != TIKZ_SAVE_FILE
	&& selectedFilter != EDGES_SAVE_FILE
	&& selectedFilter != SVG_SAVE_FILE)
    {
	ui->canvas->scene()->clearSelection();
	ui->canvas->scene()->invalidate(ui->canvas->scene()->itemsBoundingRect(),
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
			    QRectF(0, 0,
				   ui->canvas->scene()
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
	QMessageBox::information(0, "Error",
				 "Unable to open " + fileName + " for output!");
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

    if (selectedFilter == GRAPHiCS_SAVE_FILE)
    {
	bool success = saveGraphIc(outStream, nodes, false);
	outputFile.close();
	ui->canvas->snapToGrid(saveStatus);
	ui->canvas->update();
	QFileInfo fi(fileName);
	ui->graphType_ComboBox->insertItem(ui->graphType_ComboBox->count(),
					   fi.baseName());
	return true && success;
    }

    if (selectedFilter == EDGES_SAVE_FILE)
    {
	bool success = saveEdgelist(outStream, nodes);
	outputFile.close();
	ui->canvas->snapToGrid(saveStatus);
	ui->canvas->update();
	return true && success;
    }

    if (selectedFilter == TIKZ_SAVE_FILE)
    {
	bool success = saveTikZ(outStream, nodes);
	outputFile.close();
	ui->canvas->snapToGrid(saveStatus);
	ui->canvas->update();
	return true && success;
    }

    if (selectedFilter == SVG_SAVE_FILE)
    {
	QSvgGenerator svgGen;

	svgGen.setFileName(fileName);
	svgGen.setSize(ui->canvas->scene()
		       ->itemsBoundingRect().size().toSize());
	QPainter painter(&svgGen);
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
    qDebug() << "save_Graph(): Unexpected output filter in "
	     << "MainWindow::save_Graph()!";
    return false;
}



/*
 * Name:	load_Graphic_File()
 * Purpose:	Ask the user for a file to load.  If we get a name,
 *		call a function to read in the alleged graph.
 * Arguments:	None.
 * Outputs:	Displays a file chooser.
 * Modifies:	Nothing.
 * Returns:	True.  (TODO: Why?)
 * Assumptions:	None.
 * Bugs:
 * Notes:
 */

bool
MainWindow::load_Graphic_File()
{
    QString fileName = QFileDialog::getOpenFileName(this,
						    "Load Graph-ics File",
						    fileDirectory,
						    GRAPHiCS_SAVE_FILE);
    if (! fileName.isNull())
	select_Custom_Graph(fileName);

    return true;
}



/*
 * Name:	load_Graphic_Library()
 * Purpose:	Append all of the graph-ic files in the library
 *		directory to the graphType menu.
 * Arguments:	None.
 * Outputs:	Nothing.
 * Modifies:	ui->graphType_ComboBox
 * Returns:	Nothing.
 * Assumptions:	fileDirectory has been initialized.
 *		This assumes that if a file has a GRAPHiCS_FILE_EXTENSION
 *		extension then it is a graph-ic file.
 * Bugs:
 * Notes:
 */

void
MainWindow::load_Graphic_Library()
{
    QDirIterator dirIt(fileDirectory, QDirIterator::Subdirectories);
    while (dirIt.hasNext())
    {
	dirIt.next();
#ifdef DEBUG2
	if (QFileInfo(dirIt.filePath()).isFile())
	    qDebug() << "load_Graphic_Library(): suffix of"
		     << QFileInfo(dirIt.filePath()).fileName()
		     << "is"
		     << QFileInfo(dirIt.filePath()).suffix();
#endif

	if (QFileInfo(dirIt.filePath()).suffix() == GRAPHiCS_FILE_EXTENSION
	    && QFileInfo(dirIt.filePath()).isFile())
	{
	    QFileInfo fileInfo(dirIt.filePath());
	    ui->graphType_ComboBox->addItem(fileInfo.baseName());
	}
    }
}



/*
 * Name:	    select_Custom_Graph()
 * Purpose:	    Read in a graph-ic file.
 * Arguments:	    The name of the file to read from.
 * Outputs:	    None.
 * Modifies:	    Creates a graph object.
 * Returns:	    Nothing.
 * Assumptions:	    The input file is valid.
 * Bugs:	    May (almost certainly will) crash and burn on invalid input.
 * Notes:	    JD added "comment lines" capability Oct 2019.
 */

void
MainWindow::select_Custom_Graph(QString graphName)
{
    if (!graphName.isNull())
    {
	qDebug() << "select_Custom_Graph(): graphName is" << graphName;

	QFile file(graphName);

	if (!file.open(QIODevice::ReadOnly))
	    QMessageBox::information(0,
				     "Error",
				     "File: " + graphName
				     + ": " + file.errorString());

	QTextStream in(&file);
	int i = 0;
	QVector<Node *> nodes;
	int numOfNodes = -1;		// < 0 ==> haven't read it yet
	Graph * graph = new Graph();

	while (!in.atEnd())
	{
	    QString line = in.readLine();
	    QString simpLine = line.simplified();
	    if (simpLine.isEmpty())
	    {
		// Allow visually blank lines
	    }
	    else if (simpLine.at(0).toLatin1() == '#')
	    {
		// Allow comments where first non-white is '#'.
		// TODO: Should we save these comments somewhere?
	    }
	    else if (numOfNodes < 0)
	    {
		bool ok;
		numOfNodes = line.toInt(&ok);
		// TODO: do we want to allow 0-node graphs?
		// Theoretically yes, but practically, no.
		if (! ok || numOfNodes < 0)
		{
		    QMessageBox::information(0, "Error",
					     "The file " + graphName
					     + " has an invalid number of "
					     "nodes.  Thus I can not read "
					     "this file.");
		    return;
		}
	    }
	    else if (i < numOfNodes)
	    {
		QStringList fields = line.split(",");

		// Nodes may or may not have label info.  Accept both.
		if (fields.count() != 10 && fields.count() != 12)
		{
		    QMessageBox::information(0, "Error",
					     "Node " + QString::number(i)
					     + " of file "
					     + graphName
					     + " has an invalid number of "
					     "fields.  Thus I can not read "
					     "this file.");
		    // TODO: do I need to free any storage?
		    return;
		}

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
		if (fields.count() == 12)
		{
		    node->setNodeLabelSize(fields.at(10).toFloat());
		    node->setNodeLabel(fields.at(11));
		}
		nodes.append(node);
		node->setParentItem(graph);
		i++;
	    }
	    else
	    {
		QStringList fields = line.split(",");

		// Edges may or may not have label info.  Accept both.
		if (fields.count() != 9 && fields.count() != 11)
		{
		    QMessageBox::information(0, "Error",
					     "Edge "
					     + QString::number(i - numOfNodes)
					     + "of file "
					     + graphName
					     + " has an invalid number of "
					     "fields.  Thus I can not read "
					     "this file.");
		    // TODO: do I need to free any storage?
		    return;
		}
		Edge * edge = new Edge(nodes.at(fields.at(0).toInt()),
				       nodes.at(fields.at(1).toInt()));
		edge->setDestRadius(fields.at(2).toDouble());
		edge->setSourceRadius(fields.at(3).toDouble());
		edge->setRotation(fields.at(4).toDouble());
		edge->setPenWidth(fields.at(5).toDouble());
		QColor lineColor;
		lineColor.setRedF(fields.at(6).toDouble());
		lineColor.setGreenF(fields.at(7).toDouble());
		lineColor.setBlueF(fields.at(8).toDouble());
		// printf("setting edge (%d, %d) colour to %.3f, %.3f, %.3f\n",
		   //    fields.at(0).toInt(), fields.at(1).toInt(),
		   //    fields.at(6).toDouble(), fields.at(7).toDouble(),
		   //    fields.at(8).toDouble());
		edge->setColour(lineColor);
		if (fields.count() == 11)
		{
		    edge->setLabelSize(fields.at(9).toFloat());
		    edge->setLabel(fields.at(10));
		}
		edge->setParentItem(graph);
		i++;
		for (int i = 0; i < nodes.count(); i++)
		{
		    for (int j = 0; j < nodes.at(i)->edgeList.count(); j++)
		    {
			Edge * e = nodes.at(i)->edgeList.at(j);
			// int sourceID = e->sourceNode()->getID();
			int destID = e->destNode()->getID();
			if (i < destID)
			{
			    // printf("node[%d]'s %d-th edge has dst = %d",
				//   i, j, destID);
			    QString wt = e->getLabel();
			    // QColor col = e->getColour();
			    // printf(", wt /%s/, rgb (%.2f,%.2f,%.2f)\n",
				//   wt.toLatin1().data(),
				//   col.redF(), col.greenF(), col.blueF());
			}
		    }
		}
		// printf("\n");
	    }
	}
	file.close();
	qDebug() << "select_Custom_Graph: graph->childItems().length() ="
		 << graph->childItems().length();
	graph->setRotation(-1 * ui->graphRotation->value());
	ui->preview->scene()->clear();
	ui->preview->scene()->addItem(graph);
    }
    else
	qDebug() << "select_Custom_Graph(): graphName is NULL!! ??";
}



/*
 * Name:	style_Graph()
 * Purpose:
 * Arguments:	None.
 * Outputs:	Nothing.
 * Modifies:	The graph in the preview scene.
 * Returns:	Nothing.
 * Assumptions: ?
 * Bugs:	?
 * Notes:	Do not call this on a saved graph, otherwise the
 *		colours, edge thicknesses and node sizes are lost,
 *		since everything will be set to the current values of
 *		the UI boxes/sliders.
 */

void
MainWindow::style_Graph()
{
#ifdef DEBUG
    printf("style_Graph() called\n");
#endif
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
 * Name:	generate_Graph()
 * Purpose:	Load a new graph into the preview pane.
 * Arguments:	None.
 * Outputs:	Nothing.
 * Modifies:	The preview pane.
 * Returns:	Nothing.
 * Assumptions: ?
 * Bugs:	?
 * Notes:	?
 */

void
MainWindow::generate_Graph()
{
    QScreen * screen = QGuiApplication::primaryScreen();

    ui->preview->scene()->clear();
    if (ui->graphType_ComboBox->currentIndex() < BasicGraphs::Count)
    {
	qDebug() << "generate_Graph(): making a basic graph";
	ui->preview->Create_Graph(ui->graphType_ComboBox->currentIndex(),
				  ui->numOfNodes1->value(),
				  ui->numOfNodes2->value(),
				  ui->graphHeight->value()
				  * screen->logicalDotsPerInchY()
				  - ui->nodeSize->value()
				  * screen->logicalDotsPerInchY(),
				  ui->graphWidth->value()
				  * screen->logicalDotsPerInchX()
				  - ui->nodeSize->value()
				  * screen->logicalDotsPerInchX(),
				  ui->complete_checkBox->isChecked());
	this->style_Graph();
    }
    else
    {
	qDebug() << "generate_Graph() making a "
		 << ui->graphType_ComboBox->currentText()
		 << " graph";
	select_Custom_Graph(fileDirectory + "/"
			    + ui->graphType_ComboBox->currentText()
			    + "." + GRAPHiCS_FILE_EXTENSION);
    }
}



/*
 * Name:	on_NodeOutlineColor_clicked()
 * Purpose:	
 * Arguments:	None.
 * Outputs:	Nothing.
 * Modifies:	ui->NodeOutlineColor.
 * Returns:	Nothing.
 * Assumptions:	???
 * Bugs:	???
 * Notes:	???
 */

void
MainWindow::on_NodeOutlineColor_clicked()
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
    qDebug() << "on_NodeOutlineColor_clicked(): outline colour set to" << s;
    ui->NodeOutlineColor->setStyleSheet(s);
    ui->NodeOutlineColor->update();
}



/*
 * Name:	on_NodeFillColor_clicked()
 * Purpose:	
 * Arguments:	None.
 * Outputs:	Nothing.
 * Modifies:	ui->NodeFillColor
 * Returns:	Nothing.
 * Assumptions: ???
 * Bugs:	???
 * Notes:	???
 */

void
MainWindow::on_NodeFillColor_clicked()
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
    qDebug() << "on_NodeFillColor_clicked(): fill colour set to" << s;
    ui->NodeFillColor->setStyleSheet(s);
    ui->NodeFillColor->update();
}



/*
 * Name:	on_EdgeLineColor_clicked()
 * Purpose:
 * Arguments:	None.
 * Outputs:	Nothing.
 * Modifies:	ui->EdgeLineColor
 * Returns:	Nothing.
 * Assumptions: ???
 * Bugs:	???
 * Notes:	???
 */

void
MainWindow::on_EdgeLineColor_clicked()
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
    qDebug() << "on_EdgeLineColor_clicked(): edge line colour set to" << s;
    ui->EdgeLineColor->setStyleSheet(s);
    ui->EdgeLineColor->update();
}


/*
 * Name:	on_NumLabelCheckBox_clicked()
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

void
MainWindow::on_NumLabelCheckBox_clicked(bool checked)
{
    ui->NodeLabel1->setDisabled(checked);
    ui->NodeLabel2->setDisabled(checked);
}


/*
 * Name:	MainWindow::set_Label_Font_Sizes()
 * Purpose:
 * Arguments:
 * Outputs:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

void
MainWindow::set_Label_Font_Sizes()
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
 * Name:	on_graphType_ComboBox_currentIndexChanged()
 * Purpose:
 * Arguments:	the index of the selected graph from the drop-down list.
 * Outputs:	nothing.
 * Modifies:	Various and sundry UI parameters.
 * Returns:	Nothing.
 * Assumptions:
 * Bugs:	Doesn't know what to do with graphs loaded from .grphc files.
 * Notes:
 */

void
MainWindow::on_graphType_ComboBox_currentIndexChanged(int index)
{
    qDebug() << "on_graphType_ComboBox_currentIndexChanged("
	     << index << ") called";
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
	  ui->numOfNodes2->show();
	  ui->numOfNodes2->setMinimum(3);
	  if (ui->numOfNodes2->value() < 3)
	      ui->numOfNodes2->setValue(3);
	  ui->graphWidth->hide();
	  ui->widthLabel->hide();
	  break;
      }
      default:
	// ToDo: may need to change grphc file format to add
	if (index > 0)
	{
	    // Calling this with an index <= 0 has the effect of
	    // setting the pane to its default state.  Before this
	    // change it started with the bipartite choices there,
	    // which is sort of confusing/misleading.
	    qDebug() << "on_graphType_ComboBox_currentIndexChanged()"
		     << "Unknown index " << index;
	}
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
 * Name:	on_numOfNodes2_valueChanged()
 * Purpose:	
 * Arguments:	(Unused)
 * Outputs:	Nothing.
 * Modifies:	Possibly ui->numOfNodes2.
 * Returns:	Nothing.
 * Assumptions:	???
 * Bugs:	???
 * Notes:	???
 */

void
MainWindow::on_numOfNodes2_valueChanged(int arg1)
{
    qDebug() << "on_numOfNodes2_valueChanged() called";
    Q_UNUSED(arg1);
    if (ui->graphType_ComboBox->currentIndex() == BasicGraphs::Petersen)
    {
	if (ui->numOfNodes2->value()
	    > floor((ui->numOfNodes1->value() - 1) / 2))
	{
	    qDebug() << "\tchanging ui->numOfNodes2 to 1 from "
		     << ui->numOfNodes2->value();
	    ui->numOfNodes2->setValue(1);
	}
    }
}



/*
 * Name:	generate_Freestyle_Nodes()
 * Purpose:
 * Arguments:
 * Outputs:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

void
MainWindow::generate_Freestyle_Nodes()
{
    //QScreen * screen = QGuiApplication::primaryScreen();
    ui->canvas->setUpNodeParams(
	ui->nodeSize->value(),
	ui->NumLabelCheckBox->isChecked(),
	ui->nodeLabel->text(),
	ui->nodeSize->value(),
	ui->NodeFillColor->palette().background().color(),
	ui->NodeOutlineColor->palette().background().color());
}



/*
 * Name:	generate_Freestyle_Edges()
 * Purpose:
 * Arguments:
 * Outputs:
 * Modifies:
 * Returns:
 * Assumptions:
 * Bugs:
 * Notes:
 */

void
MainWindow::generate_Freestyle_Edges()
{
    ui->canvas->setUpEdgeParams(ui->edgeSize->value(),
				ui->EdgeLabel->text(),
				ui->EdgeLabelSize->value(),
				ui->EdgeLineColor->
				palette().background().color());
}


void
MainWindow::on_deleteMode_radioButton_clicked()
{
    ui->canvas->setMode(CanvasView::del);
}


void
MainWindow::on_joinMode_radioButton_clicked()
{
    ui->canvas->setMode(CanvasView::join);

}


void
MainWindow::on_editMode_radioButton_clicked()
{
    ui->canvas->setMode(CanvasView::edit);
}


void
MainWindow::on_dragMode_radioButton_clicked()
{
    ui->canvas->setMode(CanvasView::drag);
}


void
MainWindow::on_freestyleMode_radioButton_clicked()
{
    ui->canvas->setMode(CanvasView::freestyle);
}


/*
 * Name:	on_tabWidget_currentChanged()
 * Purpose:	Redraw the UI for the tabbed section at the left of
 *		the main UI window.
 * Arguments:	The tab index.
 * Outputs:	Nothing.
 * Modifies:	The user view.
 * Returns:	Nothing.
 * Assumptions:	?
 * Bugs:	?
 * Notes:	JD Q: what actually draws the UI for tab 0?  ui_mainwindow?
 */

void
MainWindow::on_tabWidget_currentChanged(int index)
{
    qDebug() << "on_tabWidget_currentChanged(" << index << ")";
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

		      gridLayout->addWidget(new QLabel("N Diam"), i, 2);
		      gridLayout->addWidget(new QLabel("E width"), i+1, 2);
		      gridLayout->addWidget(new QLabel("Label"), i, 3);
		      gridLayout->addWidget(new QLabel("Text"), i, 4);
		      gridLayout->addWidget(new QLabel("Size"), i+1, 4);
		      gridLayout->addWidget(new QLabel("Line"), i, 5);
		      gridLayout->addWidget(new QLabel("Color"), i+1, 5);
		      gridLayout->addWidget(new QLabel("Fill"), i, 6);
		      gridLayout->addWidget(new QLabel("Color"), i+1, 6);
		      i += 2;

		      foreach (QGraphicsItem * gItem, graph->childItems())
		      {
			  if (gItem != nullptr)
			  {
			      if (gItem->type() == Node::Type)
			      {
				  Node * node = qgraphicsitem_cast<Node*>(gItem);
				  QLineEdit * nodeEdit = new QLineEdit();
				  // Q: what was the point of this?
				  // nodeEdit->setText("Node\n");
				  // gridLayout->addWidget(nodeEdit);

				  QLabel * label = new QLabel("Node");
				  // When this node is deleted, also
				  // delete its label in the edit tab.
				  connect(node, SIGNAL(destroyed(QObject*)),
					  label, SLOT(deleteLater()));

				  QDoubleSpinBox * sizeBox
				      = new QDoubleSpinBox();

				  QPushButton * lineColorButton
				      = new QPushButton();
				  QPushButton * fillColorButton
				      = new QPushButton();

				  QDoubleSpinBox * fontSizeBox
				      = new QDoubleSpinBox();

				  SizeController * sizeController
				      = new SizeController(node, sizeBox);
				  ColorLineController * colorLineController
				      = new ColorLineController(node,
								lineColorButton);
				  LabelController * weightController
				      = new LabelController(node, nodeEdit);
				  LabelSizeController * weightSizeController
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
				  // Q: what were these for??
				  // editEdge->setText("Edge\n");
				  // gridLayout->addWidget(editEdge);

				  QLabel * label = new QLabel("Edge");
				  // When this edge is deleted, also
				  // delete its label in the edit tab.
				  connect(edge, SIGNAL(destroyed(QObject*)),
					  label, SLOT(deleteLater()));

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
				  gridLayout->addWidget(editEdge, i, 3);
				  gridLayout->addWidget(fontSizeBox, i, 4);
				  gridLayout->addWidget(button,	i, 5);

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
	  // TODO: the setRowStretch() was added 2019/11/18 because
	  // the extra vertical space was being distributed *between*
	  // the rows, rather than at the end.
	  // But by itself it only "works" after something was deleted
	  // from the graph and then we switch into the edit pane (or
	  // out & back in).  After that it is OK.  Go figure.
	  // Dumps core if setRowStretch(i - 1, 40) is called when i == 0.
	  // So to get it to work I added a label with a blank at the
	  // end of the section, and then it works.  Kludge.
	  if (i > 0)
	  {
	      // printf("Setting row stretch for row %d to 40\n", i - 1);
	      // gridLayout->setRowStretch(i - 1, 40);
	      // TODO: JD could not figure out any way to get a spacer
	      // in here.
	      // QSpacerItem * verticalSpacer;
	      //QSpacerItem verticalSpacer = QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
	      //auto widget = new QWidget();
	      //widget->setLayout( verticalSpacer );
	      //gridLayout->addWidget(widget, 0, 1);
	      //QLineEdit * editEdge = new QLineEdit();
	      //gridLayout->addWidget(editEdge, i, 1);
	      //gridLayout->addItem(verticalSpacer, i/2, 1, 1, 1, Qt::AlignTop);
	      //gridLayout->addItem(verticalSpacer);//, 12, 1, 0, 0, 0);
	      //gridLayout->addItem(verticalSpacer);
// compile error	      gridLayout->addItem(new QSpacerItem((1, 1, QSizePolicy::Expanding, QSizePolicy::Preferred), 3,1,1,4, Qt::AlignTop);
	      // This is a horrible kludge that makes the stretch work
	      // before anything is deleted from the Edit Graph tab.
	      // With this the setRowStretch() for i-1 is not needed.
	      // The horror, the horror.
	      QLabel * label = new QLabel(" ");
	      gridLayout->addWidget(label, i, 1);
	      gridLayout->setRowStretch(i, 40);
	  }
	  break;
      }
      default:
	break;
    }
}



void
MainWindow::dumpTikZ()
{
    QVector<Node *> nodes;
    int numOfNodes = 0;

    foreach (QGraphicsItem * item, ui->canvas->scene()->items())
    {
	if (item->type() == Node::Type)
	{
	    Node * node = qgraphicsitem_cast<Node *>(item);
	    node->setID(numOfNodes++);
	    nodes.append(node);
	}
    }
    
    printf("%%========== TikZ dump of current graph follows: ============\n");
    QTextStream tty(stdout);
    saveTikZ(tty, nodes);
}


void
MainWindow::dumpGraphIc()
{
    printf("dumpGraphIc() called\n");
    QVector<Node *> nodes;
    int numOfNodes = 0;

    foreach (QGraphicsItem * item, ui->canvas->scene()->items())
    {
	if (item->type() == Node::Type)
	{
	    Node * node = qgraphicsitem_cast<Node *>(item);
	    node->setID(numOfNodes++);
	    nodes.append(node);
	}
    }
    
    printf("%%========= graphIc dump of current graph follows: ===========\n");
    QTextStream tty(stdout);
    saveGraphIc(tty, nodes, true);
}

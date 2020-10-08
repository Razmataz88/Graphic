/*
 * File:	defuns.h
 * Author:	Jim Diamond
 * Date:	2019-12-10
 * Version:	1.4
 *
 * Purpose:	Hold definitions that are needed by multiple classes
 *		and yet don't seem to meaningfully fit anywhere else.
 *		OO fanatics will be horrified by this expeditiousness.
 *
 * Modification history:
 * Dec 10, 2019
 *  (a) Initial version.  Define the widget_ID enum, needed to pass
 *	information (about which graph parameter widget changed,
 *	causing a (re)draw) between MainWindow and PreView.
 *	"ALL_WGT" is a special value that indicates all styles should
 *	be applied, rather than just one corresponding to a particular
 *	widget that changed; used when loading a "basic" graph.
 *	"NO_WGT" indicates that no styles should be applied; used when
 * May 25, 2020 (IC V1.1)
 *  (a) Added numLabelStart_WGT.
 * Jun 9, 2020 (IC V1.2)
 *  (a) Moved BUTTON_STYLE here so it is not repeated across 3 files.
 * Jun 3, 2020 (IC V1.3)
 *  (a) Added nodeThickness_WGT.
 * Aug 7, 2020 (IC V1.4)
 *  (a) Declare settings here.  (Hmmm...)
 *  (b) Change nodeSize_WGT to nodeDiam_WGT and edgeSize_WGT to
 *	edgeThickness_WGT.
 */

#ifndef DEFUNS_H
#define DEFUNS_H

#include <QSettings>

// Use qDeb() and qDebu() for debugging, and then the statements can
// be turned on and off with a re-compile but no source-code editing.
#ifdef DEBUG
static const bool debug = true;
#define qDeb() qDebug().nospace().noquote() << fixed \
    << qSetRealNumberPrecision(4)
#define qDebu(...) qDebug(__VA_ARGS__)
#else
static const bool debug = false;
#define qDeb() if (false) qDebug()
#define qDebu(...) 
#endif

#define BUTTON_STYLE "border-style: outset; border-width: 2px; " \
	     "border-radius: 5px; border-color: beige; padding: 3px;"

extern QSettings settings;

enum widget_ID {NO_WGT, ALL_WGT, nodeDiam_WGT, nodeLabel1_WGT, nodeLabel2_WGT,
		nodeLabelSize_WGT, numLabelCheckBox_WGT, nodeFillColour_WGT,
		nodeOutlineColour_WGT, edgeThickness_WGT, edgeLabel_WGT,
		edgeLabelSize_WGT, edgeLineColour_WGT, graphRotation_WGT,
		completeCheckBox_WGT, graphHeight_WGT, graphWidth_WGT,
		numOfNodes1_WGT, numOfNodes2_WGT, graphTypeComboBox_WGT,
		numLabelStart_WGT, nodeThickness_WGT};

#endif

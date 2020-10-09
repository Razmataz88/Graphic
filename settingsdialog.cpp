/*
 * File:    settingsdialog.cpp
 * Author:  Ian Cathcart
 * Date:    2020/08/05
 * Version: 1.2
 *
 * Purpose: Implements the settings dialog.
 *
 * Modification history:
 * Aug 5, 2020 (IC V1.1)
 *  (a) Fix spelling of "colour".  Add "Bg" into background colour var names.
 *  (b) Lots of tweaks to saveSettings() and loadSettings() to make sure
 *      they properly save and load custom DPI settings.
 *  (c) Added a signal to tell mainWindow that the user OK'd the dialog.
 * Aug 7, 2020 (IC V1.2)
 *  (a) Added background colour buttons to settingsdialog.ui so their code
 *      is reflected here.  They should prompt the user to select a colour
 *      for saved graph backgrounds.  TODO: Allow a transparent background
 *      option for non-JPEG image types.
 */

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "defuns.h"

#include <QColorDialog>

SettingsDialog::SettingsDialog(QWidget * parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Settings");

    // Initialize colour buttons.
    QString s("background: #ffffff;" BUTTON_STYLE);
    ui->jpgBgColour->setStyleSheet(s);
    ui->otherImageBgColour->setStyleSheet(s);

    loadSettings();

    connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));
}



SettingsDialog::~SettingsDialog()
{
    delete ui;
}



void
SettingsDialog::loadSettings()
{
    // Always set this label to defaultResolution
    ui->defaultLabel->setText(settings.value("defaultResolution").toString()
                              + " pixels/inch");

    // If no settings founds, initialize to defaults
    if (!settings.contains("useDefaultResolution"))
    {
        ui->customSpinBox->setValue(settings.value("defaultResolution").toInt());
    }
    else // load saved settings
    {
        if (settings.value("useDefaultResolution").toBool() == true)
            ui->defaultButton->setChecked(true);
        else
            ui->customButton->setChecked(true);

        ui->customSpinBox->setValue(settings.value("customResolution").toInt());

        if (settings.contains("jpgBgColour"))
            ui->jpgBgColour->setStyleSheet("background: "
                                        + settings.value("jpgBgColour").toString()
                                        + ";" + BUTTON_STYLE);
        if (settings.contains("otherImageBgColour"))
            ui->otherImageBgColour->setStyleSheet("background: "
                                        + settings.value("otherImageBgColour").toString()
                                        + ";" + BUTTON_STYLE);
    }
}



void
SettingsDialog::saveSettings()
{
    settings.setValue("useDefaultResolution", ui->defaultButton->isChecked());
    settings.setValue("customResolution", ui->customSpinBox->value());

    emit saveDone();
}



void
SettingsDialog::on_jpgBgColour_clicked()
{
    QColor colour = QColorDialog::getColor();

    if (!colour.isValid())
	return;

    QString s("background: #"
	      + QString(colour.red() < 16 ? "0" : "")
	      + QString::number(colour.red(), 16)
	      + QString(colour.green() < 16 ? "0" : "")
	      + QString::number(colour.green(), 16)
	      + QString(colour.blue() < 16 ? "0" : "")
	      + QString::number(colour.blue(), 16) + ";"
	      BUTTON_STYLE);
    qDeb() << "MW::on_jpgBgColour_clicked(): background colour set to" << s;
    settings.setValue("jpgBgColour", colour.name());
    ui->jpgBgColour->setStyleSheet(s);
    ui->jpgBgColour->update();
}



void
SettingsDialog::on_otherImageBgColour_clicked()
{
    QColor colour = QColorDialog::getColor();

    if (!colour.isValid())
	return;

    QString s("background: #"
	      + QString(colour.red() < 16 ? "0" : "")
	      + QString::number(colour.red(), 16)
	      + QString(colour.green() < 16 ? "0" : "")
	      + QString::number(colour.green(), 16)
	      + QString(colour.blue() < 16 ? "0" : "")
	      + QString::number(colour.blue(), 16) + ";"
	      BUTTON_STYLE);
    qDeb() << "MW::on_otherImageBgColour_clicked(): BG colour set to" << s;
    settings.setValue("otherImageBgColour", colour.name());
    ui->otherImageBgColour->setStyleSheet(s);
    ui->otherImageBgColour->update();
}

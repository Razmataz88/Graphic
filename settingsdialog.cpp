/*
 * File:    settingsdialog.cpp
 * Author:  Ian Cathcart
 * Date:    2020/08/07
 * Version: 1.0
 *
 * Purpose: Implements the settings dialog.
 *
 * Modification history:
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
    ui->jpgColor->setStyleSheet(s);
    ui->otherColor->setStyleSheet(s);

    ui->defaultLabel->setText(settings.value("systemPhysicalDpi").toString()
			      + " pixels/inch");
    ui->customSpinBox->setValue(settings.value("systemPhysicalDpi").toInt());

    if (settings.contains("useDefaultResolution"))
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
    // First one redundant? Perhaps...
    ui->defaultButton->setChecked(
	settings.value("useDefaultResolution").toBool());
    ui->customButton->setChecked(
	!settings.value("useDefaultResolution").toBool());

    ui->customSpinBox->setValue(settings.value("customResolution").toInt());

    if (settings.contains("jpgColor"))
	ui->jpgColor->setStyleSheet("background: "
				    + settings.value("jpgColor").toString()
				    + ";" + BUTTON_STYLE);
    if (settings.contains("otherColor"))
	ui->otherColor->setStyleSheet("background: "
				      + settings.value("otherColor").toString()
				      + ";" + BUTTON_STYLE);
}



void
SettingsDialog::saveSettings()
{
    settings.setValue("useDefaultResolution", ui->defaultButton->isChecked());

    settings.setValue("customResolution", ui->customSpinBox->value());
}



void
SettingsDialog::on_jpgColor_clicked()
{
    QColor color = QColorDialog::getColor();

    if (!color.isValid())
	return;

    QString s("background: #"
	      + QString(color.red() < 16 ? "0" : "")
	      + QString::number(color.red(), 16)
	      + QString(color.green() < 16 ? "0" : "")
	      + QString::number(color.green(), 16)
	      + QString(color.blue() < 16 ? "0" : "")
	      + QString::number(color.blue(), 16) + ";"
	      BUTTON_STYLE);
    qDeb() << "MW::on_jpgColor_clicked(): background color set to" << s;
    settings.setValue("jpgColor", color.name());
    ui->jpgColor->setStyleSheet(s);
    ui->jpgColor->update();
}



void
SettingsDialog::on_otherColor_clicked()
{
    QColor color = QColorDialog::getColor();

    if (!color.isValid())
	return;

    QString s("background: #"
	      + QString(color.red() < 16 ? "0" : "")
	      + QString::number(color.red(), 16)
	      + QString(color.green() < 16 ? "0" : "")
	      + QString::number(color.green(), 16)
	      + QString(color.blue() < 16 ? "0" : "")
	      + QString::number(color.blue(), 16) + ";"
	      BUTTON_STYLE);
    qDeb() << "MW::on_otherColor_clicked(): background color set to" << s;
    settings.setValue("otherColor", color.name());
    ui->otherColor->setStyleSheet(s);
    ui->otherColor->update();
}

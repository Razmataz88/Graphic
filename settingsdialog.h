/*
 * File:    settingsdialog.h
 * Author:  Ian Cathcart
 * Date:    2020/08/07
 * Version: 1.0
 *
 * Purpose: Definitions for the settings dialog.
 *
 * Modification history:
 */

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QtCore>

namespace Ui
{
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget * parent = nullptr);
    ~SettingsDialog();

private slots:
    void on_jpgColor_clicked();
    void on_otherColor_clicked();

public slots:
    void saveSettings();
    void loadSettings();

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H

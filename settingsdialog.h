/*
 * File:    settingsdialog.h
 * Author:  Ian Cathcart
 * Date:    2020/08/07
 * Version: 1.1
 *
 * Purpose: Definitions for the settings dialog.
 *
 * Modification history:
 * Aug 7, 2020 (IC V1.1)
 *  (a) Rename background colour fields.
 *  (b) Add saveDone() signal.
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
    void on_jpgBgColour_clicked();
    void on_otherImageBgColour_clicked();

public slots:
    void saveSettings();
    void loadSettings();

signals:
    void saveDone();

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H

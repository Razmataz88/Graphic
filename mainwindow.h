#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QGridLayout>
#include <QScrollArea>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setKeyStatusLabel(QString text);

private slots:
    bool save_Graph();
    bool load_Graphic_File();
    void load_Grapha_Library();
    void select_Custom_Graph(QString graphName);
    void generate_Graph();
    void style_Graph();
    void generate_Combobox_Titles();

    void set_Label_Font_Sizes();
    void on_NodeOutlineColor_clicked();
    void on_NodeFillColor_clicked();
    void on_EdgeLineColor_clicked();

    void on_NumLabelCheckBox_clicked(bool checked);
    void on_graphType_ComboBox_currentIndexChanged(int index);
    void on_numOfNodes2_valueChanged(int arg1);

    void generate_Freestyle_Nodes();
    void generate_Freestyle_Edges();

    void on_deleteMode_radioButton_clicked();

    void on_joinMode_radioButton_clicked();

    void on_editMode_radioButton_clicked();

    void on_noMode_radioButton_clicked();

    void on_freestyleMode_radioButton_clicked();

    void on_tabWidget_currentChanged(int index);

private:
    Ui::MainWindow *ui;
    QDir dir;
    QString fileDirectory;
    QGridLayout * gridLayout;
    QScrollArea * scroll;
};

#endif // MAINWINDOW_H

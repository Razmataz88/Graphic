#ifndef GRAPHSETTINGS_H
#define GRAPHSETTINGS_H

#include <QDialog>

namespace Ui {
class GraphSettings;
}

class GraphSettings : public QDialog
{
    Q_OBJECT

public:
    explicit GraphSettings(QWidget *parent = 0);
    ~GraphSettings();

private:
    Ui::GraphSettings *ui;
};

#endif // GRAPHSETTINGS_H

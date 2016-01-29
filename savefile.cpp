#include "savefile.h"
#include "ui_savefile.h"

Savefile::Savefile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Savefile)
{
    ui->setupUi(this);
}

Savefile::~Savefile()
{
    delete ui;
}

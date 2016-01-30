#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QWidget>

namespace Ui {
class toolbar;
}

class toolbar : public QWidget
{
    Q_OBJECT

public:
    explicit toolbar(QWidget *parent = 0);
    ~toolbar();

private:
    Ui::toolbar *ui;
};

#endif // TOOLBAR_H

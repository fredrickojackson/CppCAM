#ifndef DLGCUTTER_H
#define DLGCUTTER_H

#include <QDialog>

namespace Ui {
class dlgCutter;
}

class dlgCutter : public QDialog
{
    Q_OBJECT

public:
    explicit dlgCutter(QWidget *parent = 0);
    ~dlgCutter();

private slots:
    void on_buttonBox_accepted();

public:
    Ui::dlgCutter *ui;
};

#endif // DLGCUTTER_H

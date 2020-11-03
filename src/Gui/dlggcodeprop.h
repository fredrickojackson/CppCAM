#ifndef DLGGCODEPROP_H
#define DLGGCODEPROP_H

#include <QDialog>

namespace Ui {
class dlgGcodeProp;
}

class dlgGcodeProp : public QDialog
{
    Q_OBJECT

public:
    explicit dlgGcodeProp(QWidget *parent = nullptr);
    ~dlgGcodeProp();

    Ui::dlgGcodeProp *ui;

private slots:
    void on_pbToggle_clicked();

private:
};

#endif // DLGGCODEPROP_H

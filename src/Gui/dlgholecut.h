#ifndef DLGHOLECUT_H
#define DLGHOLECUT_H

#include <QDialog>
#include "ui_dlgholecut.h"


namespace Ui {
class dlgHoleCut;
}

class dlgHoleCut : public QDialog , public Ui::dlgHoleCut
{
    Q_OBJECT

public:
    explicit dlgHoleCut(QWidget *parent = nullptr);
    ~dlgHoleCut();
    Ui::dlgHoleCut *ui;

    int myresult=0;

private slots:
    void on_pbDone_clicked();
    void on_pbApply_clicked();
};

#endif // DLGHOLECUT_H

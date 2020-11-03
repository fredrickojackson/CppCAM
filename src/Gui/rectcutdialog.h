#ifndef RECTCUTDIALOG_H
#define RECTCUTDIALOG_H

#include <QDialog>

#include "Stock.h"
#include "Model.h"
#include "Cutter.h"

#include "ui_rectcutdialog.h"

namespace Ui {
class RectCutDialog;
}

class RectCutDialog :
        public QDialog, public Ui::RectCutDialog
{
    Q_OBJECT

public:
    explicit RectCutDialog(QWidget *parent = 0);
    ~RectCutDialog();
    Ui::RectCutDialog *ui;
    void updSize();
    double m_droundcorner;

    Stock* m_stock;
    Model* m_model;
    Cutter* m_cutter;



private:
//    Ui::RectCutDialog *ui;
protected:
    void showEvent(QShowEvent *event) override;
private slots:
    void on_buttonBox_accepted();
    void on_cbroundcorner_stateChanged(int arg1);
    void on_cbInside_stateChanged(int arg1);
    void on_cbOutside_stateChanged(int arg1);
    void on_dsbRoundCorner_valueChanged(double arg1);
    void on_dsbminX_valueChanged(double arg1);
    void on_dsbminY_valueChanged(double arg1);
    void on_dsbminZ_valueChanged(double arg1);
    void on_dsbmaxX_valueChanged(double arg1);
    void on_dsbmaxY_valueChanged(double arg1);
    void on_dsbmaxZ_valueChanged(double arg1);
    void on_cbOutside_clicked();
};

#endif // RECTCUTDIALOG_H

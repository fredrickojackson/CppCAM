#ifndef RECTCUTDIALOG_H
#define RECTCUTDIALOG_H

#include <QDialog>

#include "Stock.h"
#include "Model.h"

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
    double dminX();
    double dminY();
    double dminZ();
    double dmaxX();
    double dmaxY();
    double dmaxZ();
    double dSzX();
    double dSzY();
    double dSzZ();
    double dStepDown();
    void updSize();
    double m_droundcorner;

    Stock* m_stock;
    Model* m_model;



private:
//    Ui::RectCutDialog *ui;
protected:
    void showEvent(QShowEvent *event) override;
private slots:
    void on_dblminX_valueChanged(const QString &arg1);
    void on_dblminY_valueChanged(const QString &arg1);
    void on_dblminZ_valueChanged(const QString &arg1);
    void on_dblmaxX_valueChanged(const QString &arg1);
    void on_dblmaxY_valueChanged(const QString &arg1);
    void on_dblmaxZ_valueChanged(const QString &arg1);
    void on_buttonBox_accepted();
    void on_dsbroundcorner_valueChanged(const QString &arg1);
    void on_cbroundcorner_stateChanged(int arg1);
};

#endif // RECTCUTDIALOG_H

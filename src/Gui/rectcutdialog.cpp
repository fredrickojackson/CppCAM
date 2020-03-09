/* added by fredrick.o.jackson@gmail.com */
#include "rectcutdialog.h"
#include "ui_rectcutdialog.h"
#include <QMessageBox>

RectCutDialog::RectCutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RectCutDialog)
{
    ui->setupUi(this);

}

void RectCutDialog::showEvent(QShowEvent *event)
{
//    QMessageBox::StandardButton reply = QMessageBox::question(this,
//                                       "My Title", "This is my custom message",
//                              QMessageBox::Yes | QMessageBox::No);
//        if(reply == QMessageBox::Yes) {
//            QApplication::quit();
//        }
        QString qsr;
        qsr="stock\n      x:";
        qsr.append(qsr.number(m_stock->min_x()));
        qsr.append(",");
        qsr.append(qsr.number(m_stock->max_x()));
        qsr.append("\n      y:");
        qsr.append(qsr.number(m_stock->min_y()));
        qsr.append(",");
        qsr.append(qsr.number(m_stock->max_y()));
        qsr.append("\n      z:");
        qsr.append(qsr.number(m_stock->min_z()));
        qsr.append(",");
        qsr.append(qsr.number(m_stock->max_z()));
        ui->lblStock->setText(qsr);

        qsr="model\n      x:";
        qsr.append(qsr.number(m_model->min_x()));
        qsr.append(",");
        qsr.append(qsr.number(m_model->max_x()));
        qsr.append("\n      y:");
        qsr.append(qsr.number(m_model->min_y()));
        qsr.append(",");
        qsr.append(qsr.number(m_model->max_y()));
        qsr.append("\n      z:");
        qsr.append(qsr.number(m_model->min_z()));
        qsr.append(",");
        qsr.append(qsr.number(m_model->max_z()));
        ui->lblModel->setText(qsr);
}


RectCutDialog::~RectCutDialog()
{
    delete ui;
}

double RectCutDialog::dminX()
{
    return ui->dblminX->value();
}

double RectCutDialog::dminY()
{
    return ui->dblminY->value();
}

double RectCutDialog::dminZ()
{
    return ui->dblminZ->value();
}
double RectCutDialog::dmaxX()
{
    return ui->dblmaxX->value();
}

double RectCutDialog::dmaxY()
{
    return ui->dblmaxY->value();
}

double RectCutDialog::dmaxZ()
{
    return ui->dblmaxZ->value();
}

double RectCutDialog::dSzX()
{
    return ui->dblSizeX->value();
}

double RectCutDialog::dSzY()
{
    return ui->dblSizeY->value();
}

double RectCutDialog::dSzZ()
{
    return ui->dblSizeZ->value();
}

double RectCutDialog::dStepDown()
{
    return ui->dblStepDown->value();
}

void RectCutDialog::on_pbUsestock_clicked()
{
    ui->dblminX->setValue(m_stock->min_x());
    ui->dblminY->setValue(m_stock->min_y());
    ui->dblminZ->setValue(m_stock->min_z());
    ui->dblmaxX->setValue(m_stock->max_x());
    ui->dblmaxY->setValue(m_stock->max_y());
    ui->dblmaxZ->setValue(m_stock->max_z());
}

void RectCutDialog::on_dblminX_valueChanged(const QString &arg1)
{
    updSize();
}


void RectCutDialog::on_dblminY_valueChanged(const QString &arg1)
{
    updSize();
}

void RectCutDialog::on_dblminZ_valueChanged(const QString &arg1)
{
    updSize();
}

void RectCutDialog::on_dblmaxX_valueChanged(const QString &arg1)
{
    updSize();
}

void RectCutDialog::on_dblmaxY_valueChanged(const QString &arg1)
{
    updSize();
}

void RectCutDialog::on_dblmaxZ_valueChanged(const QString &arg1)
{
    updSize();
}

void RectCutDialog::updSize()
{
    ui->dblSizeX->setValue(ui->dblmaxX->value()-ui->dblminX->value());
    ui->dblSizeY->setValue(ui->dblmaxY->value()-ui->dblminY->value());
    ui->dblSizeZ->setValue(ui->dblmaxZ->value()-ui->dblminZ->value());
}




void RectCutDialog::on_buttonBox_accepted()
{

}

void RectCutDialog::on_dsbroundcorner_valueChanged(const QString &arg1)
{
    //if(dsbroundcorner->value() > 0.0)cbroundcorner->setChecked(true);
    //double x = dsbroundcorner->value();
    m_droundcorner=arg1.toDouble();
    //m_droundcorner=dsbroundcorner->value();
}

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
        ui->dsbminX->setValue(m_stock->min_x());
        ui->dsbminY->setValue(m_stock->min_y());
        ui->dsbminZ->setValue(m_stock->min_z());
        ui->dsbmaxX->setValue(m_stock->max_x());
        ui->dsbmaxY->setValue(m_stock->max_y());
        ui->dsbmaxZ->setValue(m_stock->max_z());

}


RectCutDialog::~RectCutDialog()
{
    delete ui;
}




void RectCutDialog::updSize()
{
    ui->dsbsizeX->setValue(ui->dsbmaxX->value()-ui->dsbminX->value());
    ui->dsbsizeY->setValue(ui->dsbmaxY->value()-ui->dsbminY->value());
    ui->dsbsizeZ->setValue(ui->dsbmaxZ->value()-ui->dsbminZ->value());
}




void RectCutDialog::on_buttonBox_accepted()
{

}




void RectCutDialog::on_cbroundcorner_stateChanged(int istate)
{
    if(ui->cbroundcorner->isChecked())
        ui->dsbRoundCorner->setValue(m_cutter->radius());
    else
        ui->dsbRoundCorner->setValue(0.0);
}

void RectCutDialog::on_cbInside_stateChanged(int arg1)
{
    if(arg1 == 2)ui->cbOutside->setCheckState(Qt::Unchecked);
}

void RectCutDialog::on_cbOutside_stateChanged(int arg1)
{
    if(arg1 == 2)ui->cbInside->setCheckState(Qt::Unchecked);
}


void RectCutDialog::on_dsbRoundCorner_valueChanged(double arg1)
{
    m_droundcorner=arg1;
}

void RectCutDialog::on_dsbminX_valueChanged(double arg1)
{
    updSize();
}

void RectCutDialog::on_dsbminY_valueChanged(double arg1)
{
    updSize();
}

void RectCutDialog::on_dsbminZ_valueChanged(double arg1)
{
    updSize();
}

void RectCutDialog::on_dsbmaxX_valueChanged(double arg1)
{
    updSize();
}

void RectCutDialog::on_dsbmaxY_valueChanged(double arg1)
{
    updSize();
}

void RectCutDialog::on_dsbmaxZ_valueChanged(double arg1)
{
    updSize();
}

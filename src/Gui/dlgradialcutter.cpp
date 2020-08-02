
#include "dlgradialcutter.h"

#include <cstdio>


dlgRadialCutter::dlgRadialCutter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgRadialCutter)
{
    ui->setupUi(this);
    flag_f=0.0;
    m_xmin=0.0;
    m_ymin=0.0;
    m_zmin=0.0;
    m_xmax=0.0;
    m_ymax=0.0;
    m_zmax=0.0;
    m_comp=0.0;
    m_smooth=0;
}

void dlgRadialCutter::showEvent(QShowEvent *event){
    if(m_comp){
        cbLeaveMargin->setChecked(true);
        leMargin->setText(QString::number(m_comp));
    }
    if(m_smooth){
        leSmooth->setText(QString::number(m_smooth));
    }
}


dlgRadialCutter::~dlgRadialCutter()
{
    delete ui;
}

void dlgRadialCutter::on_lineEdit_points_editingFinished()
{
    double res = 0.0;
    QString qs;
    if(radioButton_xdir->isChecked())
        res=(m_xmax-m_xmin)/(lineEdit_points->text().toInt()-1);
    else
        res=(m_ymax-m_ymin)/(lineEdit_points->text().toInt()-1);
    qs="resolution=";
    qs.append(QString::number(res)/*.append("\n")*/);
    textEditStatus->append(qs);
    le_resolution->setText(QString::number(res));
}

void dlgRadialCutter::on_lineEdit_lines_editingFinished()
{
    double step = 0.0;
    QString qs;
    if (!isNear((m_ymax - m_ymin),0.0))
    {

        if(radioButton_xdir->isChecked())
            step=(m_ymax-m_ymin)/(lineEdit_lines->text().toInt()-1);
        else
            step=(m_xmax-m_xmin)/(lineEdit_lines->text().toInt()-1);
        qs="stepover=";
        qs.append(QString::number(step));
        textEditStatus->append(qs);
        le_stepover->setText(QString::number(step));
    }
}

void dlgRadialCutter::on_lineEdit_layers_editingFinished()
{
    double step = 0.0;
    QString qs;
    if(lineEdit_layers->text().toInt()==1){
        step=(m_zmax-m_zmin);
    }else{
        step=(m_zmax-m_zmin)/(lineEdit_layers->text().toInt()-1);
    }
    qs="stepdown=";
    qs.append(QString::number(step));
    textEditStatus->append(qs);
    le_stepdown->setText(QString::number(step));
}

void dlgRadialCutter::on_leUseLine_textChanged(const QString &arg1)
{
    if(leUseLine->text().toInt() == 1)lblUseline->setText("Line");
    if(leUseLine->text().toInt() == 2)lblUseline->setText("nd Line");
    if(leUseLine->text().toInt() == 3)lblUseline->setText("rd Line");
    if(leUseLine->text().toInt() >= 4)lblUseline->setText("th Line");
}


void dlgRadialCutter::on_leMargin_editingFinished()
{
    if(cbLeaveMargin->isChecked())
        m_comp=leMargin->text().toDouble();
    else
        m_comp=0.0;
}

void dlgRadialCutter::on_cbLeaveMargin_stateChanged(int arg1)
{
    if(arg1)
        m_comp=leMargin->text().toDouble();
    else
        m_comp=0.0;
}

void dlgRadialCutter::on_buttonBox_accepted()
{

}

#include "rectcutdialog.h"
#include "ui_rectcutdialog.h"

RectCutDialog::RectCutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RectCutDialog)
{
    ui->setupUi(this);
}

RectCutDialog::~RectCutDialog()
{
    delete ui;
}

double RectCutDialog::dPosX()
{
    return ui->dblPosX->value();
}

double RectCutDialog::dPosY()
{
    return ui->dblPosY->value();
}

double RectCutDialog::dPosZ()
{
    return ui->dblPosZ->value();
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

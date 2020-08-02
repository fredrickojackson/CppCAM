#ifndef DLGRADIALCUTTER_H
#define DLGRADIALCUTTER_H

#include <QDialog>
#include "Geometry/Geometry.h"

#include "ui_dlgradialcutter.h"

namespace Ui {
class dlgRadialCutter;
}

class dlgRadialCutter : public QDialog, public Ui::dlgRadialCutter
{
    Q_OBJECT

public:
    explicit dlgRadialCutter(QWidget *parent = nullptr);
    ~dlgRadialCutter();

    double flag_f;
    double m_xmin;
    double m_ymin;
    double m_zmin;
    double m_xmax;
    double m_ymax;
    double m_zmax;
    int m_smooth=0;
    double m_comp=0;
    void showEvent(QShowEvent *event);

private slots:

    void on_lineEdit_points_editingFinished();
    void on_lineEdit_lines_editingFinished();
    void on_lineEdit_layers_editingFinished();
    void on_leUseLine_textChanged(const QString &arg1);
    void on_leMargin_editingFinished();
    void on_cbLeaveMargin_stateChanged(int arg1);
    void on_buttonBox_accepted();


public:
    Ui::dlgRadialCutter *ui;
};

#endif // DLGRADIALCUTTER_H

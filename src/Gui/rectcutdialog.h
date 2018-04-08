#ifndef RECTCUTDIALOG_H
#define RECTCUTDIALOG_H

#include <QDialog>

namespace Ui {
class RectCutDialog;
}

class RectCutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RectCutDialog(QWidget *parent = 0);
    ~RectCutDialog();
    Ui::RectCutDialog *ui;
    double dPosX();
    double dPosY();
    double dPosZ();
    double dSzX();
    double dSzY();
    double dSzZ();
    double dStepDown();

private:
//    Ui::RectCutDialog *ui;
};

#endif // RECTCUTDIALOG_H

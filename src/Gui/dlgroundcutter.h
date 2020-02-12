#ifndef DLGROUNDCUTTER_H
#define DLGROUNDCUTTER_H

#include <QDialog>

namespace Ui {
class dlgRoundCutter;
}

class dlgRoundCutter : public QDialog
{
    Q_OBJECT

public:
    explicit dlgRoundCutter(QWidget *parent = nullptr);
    ~dlgRoundCutter();

private:
    Ui::dlgRoundCutter *ui;
};

#endif // DLGROUNDCUTTER_H

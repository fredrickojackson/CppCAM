#ifndef RACKCUTDLG_H
#define RACKCUTDLG_H

#include <QDialog>
#include "ui_rackcutdlg.h"

namespace Ui {
class rackcutdlg;
}

class rackcutdlg : public QDialog
{
    Q_OBJECT

public:
    explicit rackcutdlg(QWidget *parent = nullptr);
    ~rackcutdlg();
    Ui::rackcutdlg *ui;

private:
};

#endif // RACKCUTDLG_H

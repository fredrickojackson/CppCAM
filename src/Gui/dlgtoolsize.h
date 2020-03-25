#ifndef DLGTOOLSIZE_H
#define DLGTOOLSIZE_H

#include <QDialog>

namespace Ui {
class dlgToolSize;
}

class dlgToolSize : public QDialog
{
    Q_OBJECT

public:
    explicit dlgToolSize(QWidget *parent = 0);
    bool isSph();
    double m_cutterSize;
    QString m_ts();
    ~dlgToolSize();

private slots:
    void on_lineEditSize_editingFinished();

private:
    Ui::dlgToolSize *ui;
};

#endif // DLGTOOLSIZE_H

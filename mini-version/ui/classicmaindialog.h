#ifndef CLASSICMAINDIALOG_H
#define CLASSICMAINDIALOG_H

#include <QDialog>

namespace Ui {
class ClassicMainDialog;
}

class ClassicMainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClassicMainDialog(QWidget *parent = 0);
    ~ClassicMainDialog();

private:
    Ui::ClassicMainDialog *ui;
};

#endif // CLASSICMAINDIALOG_H

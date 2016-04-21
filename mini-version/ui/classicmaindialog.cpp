#include "classicmaindialog.h"
#include "ui_classicmaindialog.h"

ClassicMainDialog::ClassicMainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClassicMainDialog)
{
    ui->setupUi(this);
}

ClassicMainDialog::~ClassicMainDialog()
{
    delete ui;
}

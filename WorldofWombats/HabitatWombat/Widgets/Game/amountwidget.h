#ifndef AMOUNTWIDGET_H
#define AMOUNTWIDGET_H

#include <QWidget>
#include "winventorywidget.h"
#include "storagewidget.h"
#include <QIntValidator>

class WInventoryWidget;
class StorageWidget;

namespace Ui {
class AmountWidget;
}

class AmountWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AmountWidget(QWidget *parent = 0);
    ~AmountWidget();
    WInventoryWidget *wInventoryWidget;
    StorageWidget *storageWidget;
    bool parent;
    void setup(int amount, bool parent);

private slots:
    void on_close_clicked();
    void on_horizontalSlider_valueChanged(int value);

    void on_accept_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::AmountWidget *ui;
    QIntValidator *validator;
};

#endif // AMOUNTWIDGET_H

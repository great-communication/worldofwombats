#ifndef DELETEITEMWIDGET_H
#define DELETEITEMWIDGET_H

#include <QWidget>
#include "winventorywidget.h"
#include "storagewidget.h"

class WInventoryWidget;
class StorageWidget;

namespace Ui {
class DeleteItemWidget;
}

class DeleteItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeleteItemWidget(QWidget *parent = 0);
    ~DeleteItemWidget();
    WInventoryWidget *wInventoryWidget;
    StorageWidget *storageWidget;
    bool parent;
    void setup(bool parent);

private slots:
    void on_accept_clicked();
    void on_close_clicked();

private:
    Ui::DeleteItemWidget *ui;
};

#endif // DELETEITEMWIDGET_H

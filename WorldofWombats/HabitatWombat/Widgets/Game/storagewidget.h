// Storage Inventory Widget
#ifndef STORAGEWIDGET_H
#define STORAGEWIDGET_H

#include <QWidget>
#include "../../gui.h"
#include "amountwidget.h"
#include "deleteitemwidget.h"
#include <QGridLayout>
#include <QListWidgetItem>

class HW_Global;
class GUI;
class AmountWidget;
class DeleteItemWidget;

namespace Ui {
class StorageWidget;
}

class StorageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StorageWidget(QWidget *parent = 0);
    ~StorageWidget();
    bool eventFilter(QObject *obj, QEvent *event);
    GUI *gui;
    int storageId;
    int burrowId;
    bool movingLabel;    
    bool amountWidgetOpen;
    bool deleteItemWidgetOpen;
    QVector<bool> acceptedResources;
    void setup();
    void prepareUpdate();
    void update(const QVector<InventoryItem> &inventory, int percentageFull);
    QRect getStorageWidgetDropAreaPos();
    void closeDeleteItemWidget();
    void deleteDragLabel();
    void closeAmountWidget();
    void moveCancelled();
    void moveFinished(int amount);
    void deleteItem(int amount);

private slots:
    void on_storageClose_clicked();

    void on_acceptedResourcesButton_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::StorageWidget *ui;
    QGridLayout *layout;
    QVector<QLabel*> labels;
    QPoint offset;
    QPoint cursorPos;
    QLabel *draggedLabel;
    QLabel *dragSource;
    AmountWidget *amountWidget;
    DeleteItemWidget *deleteItemWidget;
    bool splitStack;   
    int index;    
    QVector<InventoryItem> inventory;
    QVector<QListWidgetItem*> resourceList;
    void moveWidget(QWidget *widget);
    void showDeleteItemWidget();
    void showAmountWidget();
    void closeAllDialogsInWInventoryWidget();
    void setAcceptedResourcesButtonText(QString text);
    void checkAcceptedResources();
    void setAcceptedResourcesInRegistry();
};

#endif // STORAGEWIDGET_H

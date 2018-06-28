// Wombat Inventory Widget
#ifndef WINVENTORYWIDGET_H
#define WINVENTORYWIDGET_H

#include <QWidget>
#include "../../gui.h"
#include "amountwidget.h"
#include "deleteitemwidget.h"
#include <QGridLayout>
#include <QLabel>

class GUI;
class AmountWidget;
class DeleteItemWidget;

namespace Ui {
class WInventoryWidget;
}

class WInventoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WInventoryWidget(QWidget *parent = 0);
    ~WInventoryWidget();
    bool eventFilter(QObject *obj, QEvent *event);
    GUI *gui;
    int wombatId;
    bool movingLabel;
    bool amountWidgetOpen;
    bool deleteItemWidgetOpen;
    void setup();
    void prepareUpdate();
    void update(const QVector<InventoryItem> &inventory, int selectedWombat);
    QRect getWInventoryWidgetDropAreaPos();
    void closeDeleteItemWidget();
    void deleteDragLabel();
    void closeAmountWidget();
    void moveCancelled();
    void moveFinished(int amount);

    void deleteItem(int amount);

private slots:
    void on_inventoryClose_clicked();

private:
    Ui::WInventoryWidget *ui;
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
    void moveWidget(QWidget *widget);    
    void showDeleteItemWidget();
    void showAmountWidget();
    void closeAllDialogsInStorageWidget();

};

#endif // WINVENTORYWIDGET_H

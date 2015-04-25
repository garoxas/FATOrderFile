#include "treewidget.h"

TreeWidget::TreeWidget(QWidget *parent) :
    QTreeWidget(parent)
{
    qRegisterMetaType<QList<QTreeWidgetItem *>>("QTreeWidgetItem");

    connect(this, SIGNAL(itemDropped(QList<QTreeWidgetItem*>)), SLOT(selectItems(QList<QTreeWidgetItem*>)), Qt::QueuedConnection);
}

TreeWidget::~TreeWidget()
{

}

void TreeWidget::selectItems(const QList<QTreeWidgetItem *> items)
{
    foreach (QTreeWidgetItem *item, items) {
        item->setFlags(item->flags() & ~Qt::ItemIsDropEnabled);

        setItemSelected(item, true);
    }

    if (items.size() > 0) {
        setCurrentItem(items.back());
    }
}

void TreeWidget::dropEvent(QDropEvent *event)
{
    QList<QTreeWidgetItem *> items = selectedItems();

    QTreeWidget::dropEvent(event);

    emit itemDropped(items);
}

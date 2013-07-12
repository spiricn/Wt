#include "stdafx.h"

#include "wte/ResourceTreeWidget.h"

ResourceTreeWidget::ResourceTreeWidget(QWidget* parent) : QTreeWidget(parent){
	
	connect(this, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
		this, SLOT(onItemActivated(QTreeWidgetItem*,int)));

	connect(this, SIGNAL(customContextMenuRequested(QPoint)),
		this, SLOT(onContextMenuRequest(QPoint)));
}


ResourceTreeWidget::~ResourceTreeWidget(){
}

void unhideItemChildren(QTreeWidgetItem* item){
	item->setHidden(false);
	for(int i=0; i<item->childCount(); i++){
		unhideItemChildren(item->child(i));
	}
}

void unhideItemParents(QTreeWidgetItem* item){
	item->setHidden(false);

	if(item->parent() != NULL){
		unhideItemParents(item->parent());
	}

	if(item->childCount() > 0){
		unhideItemChildren(item);
	}
}

void ResourceTreeWidget::setFilter(const QString& param){
	QList<QTreeWidgetItem*> items = findItems(param, Qt::MatchRecursive|Qt::MatchContains);
	  
	QTreeWidgetItemIterator it(this);
	while(*it){
		(*it)->setHidden(true);
		++it;
	}

	for(QList<QTreeWidgetItem*>::Iterator i=items.begin(); i!=items.end(); i++){
		unhideItemParents(*i);
	}
}


void ResourceTreeWidget::onContextMenuRequest(QPoint point){
	emit contextMenuRequest(getTreeitem(point), point);
}

void ResourceTreeWidget::onItemActivated(QTreeWidgetItem* item, int col){
	emit itemActivated( getTreeItem(item) );
}

QTreeWidgetItem* ResourceTreeWidget::createTreeItem(RTWItem* parent, RTWItem::Id id, const QString& name, RTWItem::Type type){
	QTreeWidgetItem* treeItem;

	if(parent == NULL){
		treeItem = new QTreeWidgetItem(this);
	}
	else{
		treeItem = new QTreeWidgetItem(parent->widgetItem);
	}

	treeItem->setIcon(0, QIcon(
		type==RTWItem::GROUP?":/icons/resource_group":":/icons/resource"));
	treeItem->setText(0, name);
	treeItem->setData(0, Qt::UserRole, QVariant(id));


	if(parent == NULL){
		this->addTopLevelItem(treeItem);
	}
	else{
		this->expandItem(parent->widgetItem);
		parent->widgetItem->addChild(treeItem);
	}
	
	this->clearSelection();
	treeItem->setSelected(true);

	return treeItem;
}

RTWItem* ResourceTreeWidget::getTreeitem(const QPoint& point){
	QTreeWidgetItem* widgetItem = this->itemAt(point.x(), point.y());

	return widgetItem != NULL?getTreeItem(widgetItem):NULL;
}

RTWItem* ResourceTreeWidget::getTreeItem(QTreeWidgetItem* widgetItem){
	return mItems[ widgetItem->data(0, Qt::UserRole).toInt() ];
}

RTWItem* ResourceTreeWidget::createItem(RTWItem* parent, RTWItem::Type type, const QString& name, void* userData){
	RTWItem::Id id = mItems.size();

	QTreeWidgetItem* widgetItem = createTreeItem(parent, id, name, type);

	RTWItem* item = new RTWItem(id, type, widgetItem, parent, userData);

	mItems[id] = item;

	return item;
}
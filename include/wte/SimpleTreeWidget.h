#ifndef WTE_SIMPLETREEWIDGET_H
#define WTE_SIMPLETREEWIDGET_H

#include "stdafx.h"

#include <wt/Log.h>

class SimpleTreeWidget : public QTreeWidget{
Q_OBJECT;


public:

	struct Item{
		QTreeWidgetItem* treeItem;
		void* userData;
		int id;

		Item() : treeItem(NULL), userData(NULL), id(0){
		}
	};

	typedef std::map<int, Item*> ItemMap;

private:
	ItemMap mItems;

public:
	SimpleTreeWidget(QWidget* parent) : QTreeWidget(parent){
		connect(this, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
		this, SLOT(onItemActivated(QTreeWidgetItem*,int)));
	}

	~SimpleTreeWidget(){
		for(ItemMap::iterator i=mItems.begin(); i!=mItems.end(); i++){
			delete i->second;
		}

		mItems.clear();
	}

	void* getSelectedItem(){
		return mItems[currentItem()->data(0, Qt::UserRole).toInt()]->userData;
	}

	void addItem(const QString& text, void* userData){
		Item* item = new Item;

		item->treeItem = new QTreeWidgetItem(this);
		item->userData = userData;
		item->id = mItems.size();

		item->treeItem->setText(0, text);
		item->treeItem->setData(
			0, Qt::UserRole, QVariant(item->id));

		mItems[item->id] = item;

		addTopLevelItem(item->treeItem);
	}

	void deleteItem(void* data){
		Item* item = findItemByData(data);
		if(item==NULL){
			LOGE("SimpleTreeWidget", "Unexisting tree item %p", data);
		}

		removeItemWidget(item->treeItem, 0);

		mItems.erase(item->id);

		delete item;
	}

	Item* findItemByData(void* data){
		for(ItemMap::iterator i=mItems.begin(); i!=mItems.end(); i++){
			if(i->second->userData == data){
				return i->second;
			}
		}

		return NULL;
	}


signals:
	void onItemActivated(void* data);

protected slots:
	void onItemActivated(QTreeWidgetItem* item, int col){
		int id = item->data(0, Qt::UserRole).toInt();

		emit onItemActivated( mItems[id]->userData );
	}

}; // </SimpleTreeWidget>

#endif // </WTE_SIMPLETREEWIDGET_H>
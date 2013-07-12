#ifndef WTE_RESOURCETREEWIDGET_H
#define WTE_RESOURCETREEWIDGET_H

#include "stdafx.h"

#include <qtreewidget.h>
#include <qaction.h>

#include <wt/ImageManager.h>
#include <wt/DevilImageLoader.h>

class RTWItem{
public:
	enum Type {RESOURCE, GROUP};

	typedef unsigned int Id;
	Id id;
	Type type;
	QTreeWidgetItem* widgetItem;
	RTWItem* parent;
	void* userData;

	Type getType() const{
		return type;
	}

	RTWItem(Id _id, Type _type, QTreeWidgetItem* _widgetItem, RTWItem* _parent, void* _userData=NULL)
	: id(_id), type(_type), widgetItem(_widgetItem), parent(_parent), userData(_userData){
	}
};

class RTWItemAction : public QAction{
	Q_OBJECT
private:
	RTWItem* mItem;

  public:
    RTWItemAction(const QIcon& icon, const QString& text, QObject* parent, RTWItem* item)
      : mItem(item), QAction(icon, text, parent){

      connect(this, SIGNAL(triggered()),
              this, SLOT(onTriggered()));
    }

  signals:
    void triggered(RTWItem*);

  private slots:
    void onTriggered() {
      emit triggered(mItem);
    }
};


class ResourceTreeWidget : public QTreeWidget{
	Q_OBJECT;
public:
	typedef std::map<unsigned int, RTWItem*> ItemMap;
private:
	ItemMap mItems;
	RTWItem::Id newId() const{
		return mItems.size();
	}

	RTWItem* createItem(RTWItem* parent, const QString& name, RTWItem::Type type);
	QTreeWidgetItem* ResourceTreeWidget::createTreeItem(RTWItem* parent, RTWItem::Id id, const QString& name, RTWItem::Type type);

public:
	RTWItem* getTreeitem(const QPoint&);
	RTWItem* getTreeItem(QTreeWidgetItem*);

	RTWItem* createItem(RTWItem* parent, RTWItem::Type type, const QString& name, void* userData);

	void deleteItem(RTWItem* item){
	//QTreeWidget::removeItemWidget(item->widgetItem, 0);
		QTreeWidgetItem* wi = item->widgetItem;
		QTreeWidgetItem* parent;
		if((parent=wi->parent()) == NULL){
			delete takeTopLevelItem( indexOfTopLevelItem(wi) );
		}
		else{
			delete parent->takeChild( parent->indexOfChild(wi) );
		}

		mItems.erase(item->id);
	}

	void setFilter(const QString& param);

	template<class T>
	void buildTree(RTWItem* parent, wt::AResourceGroup<T>* group){
		RTWItem* item=NULL;
	
		if(group->getParent() != NULL){
			item = createItem(parent, RTWItem::GROUP, group->getName().c_str(), group);
		}

		// Create resource items
		for( wt::AResourceGroup<T>::ResourceMap::iterator i=group->getResources().begin();
			i!=group->getResources().end(); i++){
				createItem(item, RTWItem::RESOURCE, i->second->getName().c_str(), i->second)->widgetItem->setToolTip(
					0, i->second->getUri().c_str());
		}

		// Create child group items
		for(wt::AResourceGroup<T>::GroupMap::iterator i=group->getChildren().begin(); i!=group->getChildren().end(); i++){
			buildTree<T>(item, i->second);
		}
	}

	ResourceTreeWidget(QWidget* parent);

	~ResourceTreeWidget();

private slots:
	void onItemActivated(QTreeWidgetItem* item, int col);

	void onContextMenuRequest(QPoint point);

signals:
	void itemActivated(RTWItem*);

	void contextMenuRequest(RTWItem*,QPoint);

}; // </ResourceTreeWidget>

#endif // </WTE_RESOURCETREEWIDGET_H>
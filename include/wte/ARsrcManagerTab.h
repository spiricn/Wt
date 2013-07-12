#ifndef WTE_ARSRCMANAGERTAB_H
#define WTE_ARSRCMANAGERTAB_H

#include <qmenu.h>
#include <qmessagebox.h>

#include <wt/Assets.h>
#include "wte/ResourceTreeWidget.h"

class ARsrcManagerTab : public QWidget{
Q_OBJECT;

private:
	ResourceTreeWidget* mTree;
	QWidget* mParent;
	void* mManager;
	bool mUriSettable;
	wt::Assets* mAssets;
public:
	ARsrcManagerTab(QWidget* parent, wt::Assets* assets, void* manager, bool uriSettable=false) : mParent(parent),
		mTree(NULL), mManager(manager), QWidget(parent), mUriSettable(uriSettable), mAssets(assets){
	}

	wt::Assets* getAssets() const{
		return mAssets;
	}

	void setTreeWidget(ResourceTreeWidget* tree){
		mTree = tree;

		connect(mTree, SIGNAL(itemActivated(RTWItem*)),
			this, SLOT(onTreeItemActivated(RTWItem*)));

		connect(mTree, SIGNAL(contextMenuRequest(RTWItem*,QPoint)),
			this, SLOT(onTreeContextMenu(RTWItem*,QPoint)));
	}



	template<class T>
	T* createResource(RTWItem* item, const QString& rsrcName=""){
		wt::AResourceGroup<T>* parent;

		if(item==NULL){
			parent = static_cast<wt::AResourceManager<T>*>(mManager);
		}
		else{
			parent = static_cast<wt::AResourceGroup<T>*>(item->userData);
		}

		
		QString name;
		if(rsrcName.isEmpty()){
			bool ok;
			name = QInputDialog::getText(mParent, "Input",
						"Resource name: ", QLineEdit::Normal, "", &ok);
			if(!ok || name.isEmpty()){
				return NULL;
			}
		}

		T* rsrc = parent->create(name.toStdString());
		mTree->createItem(item, RTWItem::RESOURCE, name, rsrc);

		return rsrc;
	}

	virtual void destroyAll() = 0;

	virtual void refreshAll() = 0;

	template<class T>
	void deleteItem(RTWItem* item, bool askConfirm=true){
		int res = QMessageBox::question(
			mParent, "Confirmation", "Delete resource?", QMessageBox::Yes, QMessageBox::No);

		if(res != QMessageBox::Yes){
			return;
		}

		wt::AResourceGroup<T>* parentGroup = item->parent==NULL ? (wt::AResourceManager<T>*)mManager : (wt::AResourceGroup<T>*)item->parent->userData;

		if(item->type == RTWItem::GROUP){	
			parentGroup->deleteChild( ((wt::AResourceGroup<T>*)item->userData)->getName() );
		}
		else{
			parentGroup->deleteResource( ((wt::AResourceGroup<T>*)item->userData)->getName() );
		}

		mTree->deleteItem(item);
	}

	template<class T>
	void destroyAll(){
		((wt::AResourceManager<T>*)mManager)->destroy();

		mTree->clear();
	}

	template<class T>
	void createResourceGroup(RTWItem* item){
		wt::AResourceGroup<T>* parent;

		if(item==NULL){
			parent = static_cast<wt::AResourceManager<T>*>(mManager);
		}
		else{
			parent = static_cast<wt::AResourceGroup<T>*>(item->userData);
		}

		bool ok;
		QString name = QInputDialog::getText(mParent, "Input",
			"Resource group name: ", QLineEdit::Normal, "", &ok);
		if(ok){
			wt::AResourceGroup<T>* group = parent->createGroup(name.toStdString());
			mTree->createItem(item, RTWItem::GROUP, name, group);
		}
	}

	virtual void createContextMenu(QMenu& menu, RTWItem* item){
		if(item == NULL || item->type == RTWItem::GROUP){
			RTWItemAction* setUri = new RTWItemAction(QIcon(":/icons/placeholder"), "Create group", this, item);
			connect(setUri, SIGNAL(triggered(RTWItem*)),
				this, SLOT(onCtxMenuCreateGroup(RTWItem*)));
			menu.addAction(setUri);
		}

		if((item != NULL && item->type == RTWItem::GROUP) || item == NULL){
			RTWItemAction* createImage = new RTWItemAction(QIcon(":/icons/placeholder"), "Create resource", this, item);
				connect(createImage, SIGNAL(triggered(RTWItem*)),
					this, SLOT(onCtxMenuCreateResource(RTWItem*)));
				menu.addAction(createImage);
		}

		if(item != NULL){
			if(mUriSettable){
				RTWItemAction* setUri = new RTWItemAction(QIcon(":/icons/placeholder"), "Set URI", this, item);
				connect(setUri, SIGNAL(triggered(RTWItem*)),
					this, SLOT(onCtxMenuSetUri(RTWItem*)));
				menu.addAction(setUri);
			}

			RTWItemAction* deleteItem = new RTWItemAction(QIcon(":/icons/placeholder"), "Delete ", this, item);
			connect(deleteItem, SIGNAL(triggered(RTWItem*)),
				this, SLOT(onCtxMenuDelete(RTWItem*)));
			menu.addAction(deleteItem);
		}

	}

protected slots:
	virtual void onTreeItemActivated(RTWItem* item){
		if(mUriSettable && item->type == RTWItem::RESOURCE){
			onCtxMenuSetUri(item);
		}
	}

	virtual void onTreeContextMenu(RTWItem* item, QPoint point){
		QMenu menu(this);

		createContextMenu(menu, item);

		menu.exec(mTree->mapToGlobal(point));
	}

	virtual void onCtxMenuCreateResource(RTWItem*){
	}

	virtual void onCtxMenuCreateGroup(RTWItem*){
	}

	virtual void onCtxMenuDelete(RTWItem*){
	}

	virtual void onCtxMenuSetUri(RTWItem*){
	}

}; // </ARsrcManagerTab>

#endif // </WTE_ARSRCMANAGERTAB_H>
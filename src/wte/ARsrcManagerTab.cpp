#include "stdafx.h"


#include "wte/ARsrcManagerTab.h"

ARsrcManagerTab::ARsrcManagerTab(QWidget* parent, wt::AResourceSystem* assets, void* manager, bool uriSettable) : mParent(parent),
	mTree(NULL), mManager(manager), QWidget(parent), mUriSettable(uriSettable), mAssets(assets), mImpl(NULL){


	connect(&WTE_CTX, SIGNAL(assetsLoaded()),
		this, SLOT(onAssetsLoaded()));

	connect(&WTE_CTX, SIGNAL(assetsUnloaded()),
		this, SLOT(onAssetsUnloaded()));

}

wt::AResourceSystem* ARsrcManagerTab::getAssets() const{
	return mAssets;
}

void ARsrcManagerTab::setTreeWidget(ResourceTreeWidget* tree){
	mTree = tree;

	connect(mTree, SIGNAL(itemActivated(RTWItem*)),
		this, SLOT(onTreeItemActivated(RTWItem*)));

	connect(mTree, SIGNAL(contextMenuRequest(RTWItem*,QPoint)),
		this, SLOT(onTreeContextMenu(RTWItem*,QPoint)));
}

void ARsrcManagerTab::destroyAll(){
	mTree->clear();
}

void ARsrcManagerTab::refreshAll(){
	if(mImpl){
		mImpl->refreshAll();
	}
}

void ARsrcManagerTab::createContextMenu(QMenu& menu, RTWItem* item){
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



void ARsrcManagerTab::onTreeItemActivated(RTWItem* item){
	if(mUriSettable && item->type == RTWItem::RESOURCE){
		onCtxMenuSetUri(item);
	}
}

void ARsrcManagerTab::onTreeContextMenu(RTWItem* item, QPoint point){
	QMenu menu(this);

	createContextMenu(menu, item);

	menu.exec(mTree->mapToGlobal(point));
}

void ARsrcManagerTab::onCtxMenuCreateResource(RTWItem* item){
	if(mImpl){
		mImpl->onCtxMenuCreateResource(item);
	}
}

void ARsrcManagerTab::onCtxMenuCreateGroup(RTWItem* item){
	if(mImpl){
		mImpl->onCtxMenuCreateGroup(item);
	}
}

void ARsrcManagerTab::onCtxMenuDelete(RTWItem* item){
	if(mImpl){
		mImpl->onCtxMenuDelete(item);
	}
}

void ARsrcManagerTab::onCtxMenuSetUri(RTWItem*){
}

void ARsrcManagerTab::onAssetsUnloaded(){
	destroyAll();
}

void ARsrcManagerTab::onAssetsLoaded(){
	refreshAll();
}

ResourceTreeWidget* ARsrcManagerTab::getTreeWidget(){
	return mTree;
}

void* ARsrcManagerTab::getResourceManager(){
	return mManager;
}

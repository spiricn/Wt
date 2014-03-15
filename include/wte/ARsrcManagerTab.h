#ifndef WTE_ARSRCMANAGERTAB_H
#define WTE_ARSRCMANAGERTAB_H

#include <qmenu.h>
#include <qmessagebox.h>

#include <wt/Assets.h>

#include "wte/ResourceTreeWidget.h"
#include "wte/WtEditorContext.h"

class IResourceImpl{
public:
	virtual void onCtxMenuCreateResource(RTWItem* item) = 0;

	virtual void onCtxMenuCreateGroup(RTWItem* item) = 0;

	virtual void onCtxMenuDelete(RTWItem* item) = 0;
	
	virtual void refreshAll() = 0;
}; // </IResourceImpl>

class ARsrcManagerTab;

// IResourceImpl factory function
template <typename T>
IResourceImpl* createResourceImplementation(ARsrcManagerTab*);

class ARsrcManagerTab : public QWidget{
Q_OBJECT;

public:
	ARsrcManagerTab(QWidget* parent, wt::Assets* assets, void* manager, bool uriSettable=false);

	wt::Assets* getAssets() const;

	void setTreeWidget(ResourceTreeWidget* tree);

	virtual void destroyAll();

	virtual void refreshAll();

	virtual void createContextMenu(QMenu& menu, RTWItem* item);

	ResourceTreeWidget* getTreeWidget();

	void* getResourceManager();

	template<class T>
	T* createResource(RTWItem* item, const QString& rsrcName="");

	template<class T>
	void deleteItem(RTWItem* item, bool askConfirm=true);

	template<class T>
	void createResourceGroup(RTWItem* item);

	template<class T>
	void initialize(ResourceTreeWidget* treeWidget);

protected slots:
	virtual void onTreeItemActivated(RTWItem* item);

	virtual void onTreeContextMenu(RTWItem* item, QPoint point);

	virtual void onCtxMenuCreateResource(RTWItem* item);

	virtual void onCtxMenuCreateGroup(RTWItem* item);

	virtual void onCtxMenuDelete(RTWItem* item);

	virtual void onCtxMenuSetUri(RTWItem*);

	void onAssetsUnloaded();

	void onAssetsLoaded();

private:
	ResourceTreeWidget* mTree;
	QWidget* mParent;
	void* mManager;
	bool mUriSettable;
	wt::Assets* mAssets;
	IResourceImpl* mImpl;

}; // </ARsrcManagerTab>

#include <wte/ARsrcManagerTab.impl.h>

template<class T>
class ResourceImpl : public IResourceImpl{
public:
	ResourceImpl(ARsrcManagerTab* tab) : mTab(tab){
	}

	void onCtxMenuCreateResource(RTWItem* item){
		mTab->createResource<T>(item);
	}

	void onCtxMenuCreateGroup(RTWItem* item){
		mTab->createResourceGroup<T>(item);
	}

	void onCtxMenuDelete(RTWItem* item){
		mTab->deleteItem<T>(item);
	}

	void refreshAll(){
		mTab->getTreeWidget()->buildTree<T>(NULL, static_cast<wt::AResourceManager<T>*>(mTab->getResourceManager()));
	}

private:
	ARsrcManagerTab* mTab;
}; // </ResourceImpl>

template <typename T>
IResourceImpl* createResourceImplementation(ARsrcManagerTab* tab){
	return new ResourceImpl<T>(tab);
}


#endif // </WTE_ARSRCMANAGERTAB_H>
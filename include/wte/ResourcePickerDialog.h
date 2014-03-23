#ifndef WTE_RESOURCEPICKER_H
#define WTE_RESOURCEPICKER_H

#include <qdialog.h>

#include <wt/AResourceManager.h>

#include "ui_ResourcePickerDialog.h"

class ResourcePickerDialog : public QDialog{
Q_OBJECT
public:
	template<class T>
	static T* pickResource(QWidget* parent, wt::AResourceManager<T>* manager, const QString& title="");

	template<class T>
	static bool pickMultiResources(QWidget* parent, wt::AResourceManager<T>* manager, QList<T*>& resources, const QString& title="");

	template<class T>
	static wt::AResourceGroup<T>* pickGroup(QWidget* parent, wt::AResourceManager<T>* manager, const QString& title="");

protected slots:
	void onItemActivated(RTWItem* item);

	void onSearch(QString param);

	void onOk();

private:
	ResourcePickerDialog(QWidget* parent, bool groupMode=false, bool multiSelection=false);

private:
	Ui::ResourcePickerDialog ui;
	bool mGroupMode;
	QList<void*> mPickedResources;
}; // </ResourcePickerDialog>


template<class T>
T* ResourcePickerDialog::pickResource(QWidget* parent, wt::AResourceManager<T>* manager, const QString& title){
	ResourcePickerDialog dlg(parent, false);

	dlg.ui.treeWidget->buildTree<T>(NULL, manager);

	dlg.setWindowTitle(title);

	dlg.exec();

	if(dlg.mPickedResources.empty()){
		return NULL;
	}
	else{
		return static_cast<T*>(*dlg.mPickedResources.begin());
	}
}

template<class T>
bool ResourcePickerDialog::pickMultiResources(QWidget* parent, wt::AResourceManager<T>* manager, QList<T*>& resources, const QString& title){
	ResourcePickerDialog dlg(parent, false);

	dlg.ui.treeWidget->buildTree<T>(NULL, manager);

	dlg.ui.treeWidget->setSelectionMode( QAbstractItemView::ExtendedSelection );

	dlg.setWindowTitle(title);

	dlg.exec();

	for(QList<void*>::Iterator iter=dlg.mPickedResources.begin(); iter!=dlg.mPickedResources.end(); iter++){
		resources.push_back( static_cast<T*>(*iter) );
	}

	return !resources.empty();
}

template<class T>
wt::AResourceGroup<T>* ResourcePickerDialog::pickGroup(QWidget* parent, wt::AResourceManager<T>* manager, const QString& title){
	ResourcePickerDialog dlg(parent, true);

	dlg.ui.treeWidget->buildTree<T>(NULL, manager);

	dlg.setWindowTitle(title);

	dlg.exec();

	if(dlg.mPickedResources.empty()){
		return NULL;
	}
	else{
		return static_cast<wt::AResourceGroup<T>*>(*dlg.mPickedResources.begin());
	}
}


#endif // </WTE_RESOURCEPICKER_H>
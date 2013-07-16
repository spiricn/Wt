#ifndef WTE_RESOURCEPICKER_H
#define WTE_RESOURCEPICKER_H

#include <qdialog.h>

#include <wt/AResourceManager.h>

#include "ui_ResourcePickerDialog.h"

class ResourcePickerDialog : public QDialog{
Q_OBJECT
private:
	Ui::ResourcePickerDialog ui;
	void* mResult;
	bool mGroupMode;

public:
	ResourcePickerDialog(QWidget* parent, bool groupMode=false) : QDialog(parent),
		mResult(NULL), mGroupMode(groupMode){

		ui.setupUi(this);

		connect(ui.treeWidget, SIGNAL(itemActivated(RTWItem*)),
			this, SLOT(onItemActivated(RTWItem*)));
	}

	template<class T>
	static T* pickResource(QWidget* parent, wt::AResourceManager<T>* manager, const QString& title=""){
		ResourcePickerDialog dlg(parent, false);

		dlg.ui.treeWidget->buildTree<T>(NULL, manager);

		dlg.setWindowTitle(title);

		dlg.exec();

		return static_cast<T*>(dlg.mResult);
	}

	template<class T>
	static wt::AResourceGroup<T>* pickGroup(QWidget* parent, wt::AResourceManager<T>* manager, const QString& title=""){
		ResourcePickerDialog dlg(parent, true);

		dlg.ui.treeWidget->buildTree<T>(NULL, manager);

		dlg.setWindowTitle(title);

		dlg.exec();

		return static_cast<wt::AResourceGroup<T>*>(dlg.mResult);
	}

protected slots:
	void onItemActivated(RTWItem* item){
		if((mGroupMode && item->type == RTWItem::GROUP) || (!mGroupMode && item->type == RTWItem::RESOURCE)){
			mResult = item->userData;
			close();
		}
	}

	void onSearch(QString param){
		ui.treeWidget->setFilter(param);
	}

}; // </ResourcePickerDialog>

#endif // </WTE_RESOURCEPICKER_H>
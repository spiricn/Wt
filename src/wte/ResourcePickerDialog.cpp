#include "stdafx.h"
#include "wte/ResourcePickerDialog.h"

#define TD_TRACE_TAG "ResourcePickerDialog"

ResourcePickerDialog::ResourcePickerDialog(QWidget* parent, bool groupMode, bool multiSelection) : QDialog(parent),
	mGroupMode(groupMode){

	ui.setupUi(this);

	connect(ui.treeWidget, SIGNAL(itemActivated(RTWItem*)),
		this, SLOT(onItemActivated(RTWItem*)));
}

void ResourcePickerDialog::onItemActivated(RTWItem* item){
	if((mGroupMode && item->type == RTWItem::GROUP) || (!mGroupMode && item->type == RTWItem::RESOURCE)){
		mPickedResources.push_back(item->userData);
		close();
	}
}

void ResourcePickerDialog::onSearch(QString param){
	ui.treeWidget->setFilter(param);
}

void ResourcePickerDialog::onOk(){
	QList<RTWItem*> items = ui.treeWidget->getSelectedItems();

	for(QList<RTWItem*>::Iterator iter=items.begin(); iter!=items.end(); iter++){
		mPickedResources.push_back((*iter)->userData);
	}

	close();

}
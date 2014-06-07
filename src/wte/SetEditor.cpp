#include "stdafx.h"
#include "wte/SetEditor.h"
#include "wte/FilePicker.h"
#include "wte/WtEditorContext.h"

#define TD_TRACE_TAG "SetEditor"

SetEditor::SetEditor() : QDialog() {
	ui.setupUi(this);
}

SetEditor::~SetEditor(){
}

void SetEditor::onSetAdd(){
	bool ok = false;

	QString name = QInputDialog::getText(this, "Input",
		"Set name: ", QLineEdit::Normal, "", &ok);

	if(!ok){
		return;
	}

	SetEntry* entry = createEntry();

	entry->name = name.toStdString();

	update(entry);
}

void SetEditor::onSetDelete(){
	SetEntry* entry = getSelection();

	if(!entry){
		return;
	}

}

SetEditor::SetEntry* SetEditor::getSelection(){
	QTreeWidgetItem* item = ui.treeWidget->currentItem();

	if(item){
		return reinterpret_cast<SetEntry*>( item->data(0, Qt::UserRole).toInt() );
	}
	else{
		return NULL;
	}
}

SetEditor::SetEntry* SetEditor::createEntry(){
	SetEntry* res = new SetEntry();

	res->treeItem = new QTreeWidgetItem(ui.treeWidget);

	res->treeItem->setData(0, Qt::UserRole, QVariant( (int)(res) ) );

	res->treeItem->setText(0, "set_name");
	res->treeItem->setText(1, "");

	mSets.insert(res);

	return res;
}

void SetEditor::onSetActivated(){
	SetEntry* entry = getSelection();

	QString res = FilePicker::getFile(this, WTE_CTX.getAssets()->getFileSystem()->getRoot().c_str(), "*.wtr");

	if(!res.size()){
		return;
	}

	entry->path = WTE_CTX.getAssets()->getRelativeURI(res.toStdString());

	update(entry);
}

void SetEditor::update(SetEntry* entry){
	entry->treeItem->setText(0, entry->name.c_str());
	entry->treeItem->setText(1, entry->path.c_str());
}
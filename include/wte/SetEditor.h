#ifndef WTE_SETEDITOR_H
#define WTE_SETEDITOR_H

#include "ui_SetEditor.h"

#include "stdafx.h"

#include <wt/Serializator.h>

class SetEditor : public QDialog{
Q_OBJECT;

public:
	SetEditor();

	~SetEditor();

private slots:

	void onSetAdd();

	void onSetDelete();

	void onSetActivated();

private:
	struct SetEntry : public wt::ASerializator<SetEntry>{
		wt::String name;
		wt::String path;
		QTreeWidgetItem* treeItem;

		BEG_FIELDS
			FIELD( name )
			FIELD( path )
		END_FIELDS
	};

	typedef std::set<SetEntry*> SetList;

private:

	SetEntry* createEntry();

	SetEntry* getSelection();

	void update(SetEntry* entry);

private:
	SetList mSets;

	Ui::SetEditor ui;
	
}; // </SetEditor>

#endif // </WTE_SETEDITOR_H>

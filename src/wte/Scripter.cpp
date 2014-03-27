#include "stdafx.h"
#include "wte/Scripter.h"

#include <qtextstream.h>
#include "wte/Utils.h"
#include "wte/WtEditorContext.h"

#define TD_TRACE_TAG "Scripter"

Scripter::Scripter() : QDialog(), mScript(NULL) {
	ui.setupUi(this);
}

void Scripter::open(wt::ScriptResource* rsrc){
	ui.textEdit->clear();

	ui.textEdit->setText( rsrc->getSource().c_str() );

	mScript = rsrc;

	setWindowTitle(QString("%1 ( %2 )").arg(mScript->getPath().c_str()).arg(mScript->getUri().c_str()));
}

void Scripter::onOpen(){
	
	ui.textEdit->clear();

	FILE* file = fopen("test_script.lua", "rb");
    QString src;

    QTextStream s1(file, QIODevice::ReadOnly);
    src.append(s1.readAll());
	
	fclose(file);

	ui.textEdit->setText(src);
}

void Scripter::onSave(){
	if(mScript == NULL){
		return;
	}

	if(!askYesNo(this, QString("Save script ? %1 ( %2 )").arg(mScript->getPath().c_str()).arg(mScript->getUri().c_str()))){
		return;
	}

	// Update source
	mScript->setSource( ui.textEdit->toPlainText().toStdString().c_str() );

	// Save to file
	wt::StreamPtr stream = WTE_CTX.getAssets()->getFileSystem()->open(mScript->getUri(), wt::AIOStream::eMODE_WRITE);

	WTE_CTX.getAssets()->getScriptManager()->getLoader()->save(stream, mScript);
}

void Scripter::onRun(){
	if(mScript){
		WTE_CTX.getLuaState()->getStateOwner()->DoString(mScript->getSource().c_str());
	}
}

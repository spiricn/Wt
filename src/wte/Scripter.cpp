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
	mScript = rsrc;

	setWindowTitle(QString("%1 ( %2 )").arg(mScript->getPath().c_str()).arg(mScript->getUri().c_str()));

	reload();
}

void Scripter::reload(){
	ui.textEdit->clear();

	ui.textEdit->setText( mScript->getSource().c_str() );
}

void Scripter::onReload(){
	if(mScript){
		try{
			WTE_CTX.getAssets()->getScriptManager()->load(mScript);
			reload();
		}catch(...){
			LOGE("Error reloading script");
		}
	}
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
	WTE_CTX.getAssets()->getScriptManager()->save(mScript);
}

void Scripter::onRun(){
	if(mScript){
		WTE_CTX.getLuaState()->getStateOwner()->DoString( ui.textEdit->toPlainText().toStdString().c_str()  );
	}
}

#include "stdafx.h"
#include "wte/Scripter.h"

#include <qtextstream.h>

#include "wte/WtEditorContext.h"

#define TD_TRACE_TAG "Scripter"

Scripter::Scripter() : QDialog() {
	ui.setupUi(this);
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
	FILE* file = fopen("test_script.lua", "wb");

	fwrite(ui.textEdit->toPlainText().toStdString().c_str(), strlen(ui.textEdit->toPlainText().toStdString().c_str()), 1, file);

	fclose(file);
}

void Scripter::onRun(){
	WTE_CTX.getLuaState()->getStateOwner()->DoString(ui.textEdit->toPlainText().toStdString().c_str());
}

Scripter::~Scripter(){
}

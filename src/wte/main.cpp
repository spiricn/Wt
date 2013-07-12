#include "stdafx.h"

#include <QtGui/QApplication>

#include "wte/WtEditor.h"

#pragma comment(lib, "glew32.lib")

int main(int argc, char** argv){
	QDir::setCurrent("d:/Documents/prog/c++/workspace/wt/");

	QApplication a(argc, argv);
	WtEditor w;
	w.show();

	return a.exec();
}

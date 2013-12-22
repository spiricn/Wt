#include "stdafx.h"

#include <QtGui/QApplication>

#include "wte/WtEditor.h"

#pragma comment(lib, "glew32.lib")

int main(int argc, char** argv){
	// TODO hardcoded
	QDir::setCurrent("d:/Documents/prog/c++/workspace/wt/");

	QApplication app(argc, argv);
	WtEditor w(NULL, 0, argc, argv);
	w.show();

	return app.exec();
}

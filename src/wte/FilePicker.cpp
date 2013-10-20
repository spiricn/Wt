#include "stdafx.h"

#include "wte/FilePicker.h"

#include <qstack.h>


#define TD_TRACE_TAG "FilePicker"


FilePicker::FilePicker(QWidget* parent, const QString& root, const QString& filter) : QDialog(parent), mRoot(root), mFilter(filter){
    ui.setupUi(this);

	connect(ui.treeWidget, SIGNAL(onItemActivated(void*)),
		this, SLOT(onItemPicked(void*)));


	setWindowTitle("Browsing " + root + " ( filter " + filter + " )");

	// Root directory
	QDir dir(mRoot);

	// Stack of unchecked paths
	QStack<QString> stack;

	// Add initial root directory to the stack
	stack.push(dir.absolutePath());

	while (!stack.isEmpty()) {
		// Get a directory from the stack
		QString subDirPath = stack.pop();
		QDir subDir(subDirPath);

		// Check for the files.
		QStringList entries = subDir.entryList(QStringList() << filter, QDir::Files);
		for (int i = 0; i < entries.size(); i++) {
			FileItem* item;
			mFiles.insert( item = new FileItem(entries[i], subDir.absolutePath() ) );

			ui.treeWidget->addItem(subDir.absolutePath() + "/" + entries[i], item);
		}

		// Append all directories in the current one to the stack
		QFileInfoList infoEntries = subDir.entryInfoList(QStringList(), QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
		for (int i = 0; i < infoEntries.size(); i++) {
			QFileInfo& item = infoEntries[i];
			stack.push(item.absoluteFilePath());
		}
	}
}

FilePicker::~FilePicker(){
	ui.treeWidget->clear();

	for(FileItemSet::iterator i=mFiles.begin(); i!=mFiles.end(); i++){
		delete *i;
	}
}

void FilePicker::onItemPicked(void* data){
	FileItem* item = static_cast<FileItem*>(data);

	mResult = item->dir + "/" + item->name;

	close();
}

QString FilePicker::getFile(QWidget* parent, const QString& rootDir, const QString& filter){
	FilePicker picker(parent, rootDir, filter);

	picker.exec();

	return picker.mResult;
}
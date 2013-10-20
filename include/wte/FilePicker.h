#ifndef WTE_FILEPICKER_H
#define WTE_FILEPICKER_H

#include "ui_FilePicker.h"

class FilePicker : public QDialog{
Q_OBJECT

private:
    Ui::FilePicker ui;

	FilePicker(QWidget* parent, const QString& root, const QString& filter);

	~FilePicker();

	struct FileItem{
		QString name;
		QString dir;

		FileItem(const QString& name, const QString& dir) : name(name), dir(dir){
		}
	};


	typedef std::set<FileItem*> FileItemSet;

	FileItemSet mFiles;
	QString mRoot, mFilter, mResult;
public:

	static QString getFile(QWidget* parent, const QString& rootDir, const QString& filter);

protected slots:
	void onItemPicked(void* item);

}; // </FilePicker>

#endif // </WTE_FILEPICKER_H>
#ifndef WTE_UTILS_H
#define WTE_UTILS_H

#include "stdafx.h"

#include <qmessagebox.h>

inline bool askYesNo(QWidget* parent, const QString& text){
	return QMessageBox::question(parent, "", text, QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes;
}

#endif
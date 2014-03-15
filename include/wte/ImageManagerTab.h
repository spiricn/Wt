#ifndef WTE_IMAGEMANAGERTAB_H
#define WTE_IMAGEMANAGERTAB_H

#include "stdafx.h"

#include <qmenu.h>

#include "ui_ImageManager.h"

#include "wte/ARsrcManagerTab.h"

class ImageManagerTab : public ARsrcManagerTab{
Q_OBJECT;

public:
	ImageManagerTab(QWidget* parent, wt::Assets* assets);

private:
	void onCtxMenuSetUri(RTWItem*);

	Ui::ImageManager ui;

}; // </ImageManagerTab>

#endif // </WTE_IMAGEMANAGERTAB_H>
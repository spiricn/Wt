#ifndef WTE_TEXTUREMANAGERTAB_H
#define WTE_TEXTUREMANAGERTAB_H

#include <qmenu.h>

#include <Wt/Texture2D.h>

#include "stdafx.h"
#include "ui_TextureManager.h"
#include "wte/ARsrcManagerTab.h"

class TextureManagerTab : public ARsrcManagerTab{
Q_OBJECT;

public:
	TextureManagerTab(QWidget* parent, wt::Assets* assets);

private:
	void onCtxMenuSetUri(RTWItem*);

	Ui::TextureManager ui;

}; // </TextureManagerTab >

#endif // </WTE_TEXTUREMANAGERTAB_H>
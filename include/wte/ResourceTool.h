#ifndef WTE_RESOURCETOOL_H
#define WTE_RESOURCETOOL_H

#include "ui_ResourceTool.h"
#include "wte/ARsrcManagerTab.h"

#include "wte/ATool.h"
#include "stdafx.h"

class ResourceTool : public QWidget, public ATool{
Q_OBJECT;

public:
	ResourceTool(QWidget* parent, AToolManager* manager);

	~ResourceTool();

private:
	void addTab(ARsrcManagerTab* tab, const QString& name);

private:
	Ui::ResourceTool ui;
	
}; // </ResourceTool>

#endif // </WTE_RESOURCETOOL_H>

#include "stdafx.h"

#include <Wt/Model.h>
#include <Wt/ModelManager.h>

#include "wte/ModelManagerTab.h"
#include "wte/ModelEditDialog.h"

ModelManagerTab::ModelManagerTab(QWidget* parent, wt::Assets* assets) : ARsrcManagerTab(parent, assets, assets->getModelManager(), true){
	ui.setupUi(this);

	setTreeWidget(ui.treeWidget);
}

void ModelManagerTab::onCtxMenuSetUri(RTWItem* item){
	QString path = QFileDialog::getOpenFileName(this,
		tr("Open model"), "", tr("Wt model file(*.wtm)"));

	((wt::Model*)item->userData)->setUri( getAssets()->getRelativeURI( path.toStdString() ) );
}


void ModelManagerTab::onTreeItemActivated(RTWItem* item){
	if(item->type == RTWItem::RESOURCE){
		wt::Model* model = ((wt::Model*)item->userData);

		ModelEditDialog::EditResult res = ModelEditDialog::edit(this, getAssets(), model);
	}
}
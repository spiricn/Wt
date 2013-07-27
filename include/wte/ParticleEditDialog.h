#ifndef WTE_PARTICLEEDITDIALOG_H
#define WTE_PARTICLEEDITDIALOG_H

#include "ui_ParticleEditDialog.h"

#include <wt/ParticleEffectResource.h>

class ParticleEditDialog : public QDialog{
Q_OBJECT

public:
    ParticleEditDialog(QWidget* parent, wt::ParticleEffectResource*);

	void setEffect(wt::ParticleEffectResource* effect);

protected slots:

	void onParamChanged();

	void onNewLayer();

	void onDeleteLayer();

	void onPickTexture();

	void onLayerActivated(void*);

private:
	Ui::ParticleEditDialog ui;
	wt::ParticleEffectResource* mEffect;
	wt::ParticleLayerResource* mCurrentLayer;
	bool mLoadingLayer;

	void loadLayer(wt::ParticleLayerResource* layer);

	void loadEffect(wt::ParticleEffectResource* effect);

}; // </ParticleEditDialog>

#endif // </WTE_PARTICLEEDITDIALOG_H>
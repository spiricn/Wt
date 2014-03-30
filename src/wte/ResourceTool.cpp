#include "stdafx.h"
#include "wte/ResourceTool.h"

#include "wte/HeightmapManagerTab.h"
#include "wte/ScriptManagerTab.h"
#include "wte/ParticleManagerTab.h"
#include "wte/ImageManagerTab.h"
#include "wte/TextureManagerTab.h"
#include "wte/SkyboxManagerTab.h"
#include "wte/SoundManagerTab.h"
#include "wte/ModelManagerTab.h"
#include "wte/AnimationManagerTab.h"

#define TD_TRACE_TAG "ResourceTool"

ResourceTool::ResourceTool(QWidget* parent, AToolManager* manager) : QWidget(parent), ATool(manager){
	ui.setupUi(this);

	// Image
	addTab(new ImageManagerTab(this, WTE_CTX.getAssets()), "Image");

	// Texture
	addTab(new TextureManagerTab(this, WTE_CTX.getAssets()), "Texture");

	// Model
	addTab(new ModelManagerTab(this, WTE_CTX.getAssets()), "Model");

	// Animation 
	addTab(new AnimationManagerTab(this, WTE_CTX.getAssets()), "Animation");

	// SkyBox
	addTab(new SkyBoxManagerTab(this, WTE_CTX.getAssets()), "Skybox");

	// Sound
	addTab(new SoundManagerTab(this, WTE_CTX.getAssets()), "Sound");

	// Particles
	addTab(new ParticleManagerTab(this, WTE_CTX.getAssets()), "Particle");

	// Heightmaps
	addTab(new HeightmapManagerTab(this, WTE_CTX.getAssets()), "Heightmap");

	// Scripts
	addTab(new ScriptManagerTab(this, WTE_CTX.getAssets()), "Script");
}

void ResourceTool::addTab(ARsrcManagerTab* tab, const QString& name){
	ui.tabWidget->addTab(tab, name);
}

ResourceTool::~ResourceTool(){
}


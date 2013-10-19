#ifndef WTE_PROJECT_H
#define WTE_PROJECT_H

#include "wte/ProjectScene.h"

class Project{
public:
	typedef std::set<ProjectScene*> SceneSet;

    Project();

	ProjectScene* addScene(const QString& path);

	void removeScene(ProjectScene::Id sceneId);

	void save(const QString& path);

	void load(const QString& path);

	const SceneSet& getSceneSet() const;

	const QString& getAssetsRoot() const;

	// TODO assets root type?

private:	
	typedef std::map<ProjectScene::Id, ProjectScene*> SceneMap;
	QString mAssetsRoot;
	SceneSet mSceneSet;
	SceneMap mScenes;
}; // </Project>

#endif // </WTE_PROJECT_H>
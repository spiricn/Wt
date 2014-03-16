#ifndef WTE_WTEDITORCONTEXT_H
#define WTE_WTEDITORCONTEXT_H

#include <wt/Assets.h>
#include <wt/lua/State.h>
#include <wt/Singleton.h>
#include <wt/Scene.h>
#include <wt/Renderer.h>

class WtEditorContext : public QObject, public wt::Singleton<WtEditorContext>{
Q_OBJECT;

public:
	WtEditorContext();

	wt::Assets* getAssets();

	wt::lua::State* getLuaState();

	void loadScene(const QString&);

	void saveScene(const QString&);

	void unloadScene();

	void loadAssets(const QString&);

	void saveAssets(const QString&);

	void unloadAssets();

	wt::EventManager* getEventManager();

	wt::Scene* getScene();

	wt::Renderer* getRenderer();

	bool isAssetsLoaded() const;

	bool isSceneLoaded() const;

	QString getAssetsFilePath() const;

	QString getSceneFilePath() const;

	void reloadAssets();

	void createNewScene(QString path);

	void createNewAssets(QString path);

	wt::LuaObject getSettingsTable();

	void clearScene();

	void clearAssets();

	void save();

	void createWorkspace(QString rootDir, QString filePath);

	void loadWorkspace(QString filePath);

	void saveWorkspace(QString filePath);

	void unloadWorkspace();

	QString getWorkspaceFilePath() const;

	QString getWorkspaceRootDir() const;

signals:
	void workspaceUnloaded();

	void workspaceLoaded();

	void fileSystemCreated();

	// All the assets have been unloaded from memory
	void assetsUnloaded();

	// Scene has been unloaded from memory
	void sceneUnloaded();

	// All the assets have been loaded to memory
	void assetsLoaded();

	// New scene has been loaded
	void sceneLoaded();

	void saveRequest();

protected:
	void setAssets(wt::Assets* assets);

private:
	wt::Assets mAssets;
	wt::lua::State* mLuaState;
	QString mAssetsFilePath;
	QString mSceneFilePath;
	QString mWorkspaceRoot;
	QString mWorkspaceFilePath;
	bool mAssetsLoaded;
	bool mSceneLoaded;
	wt::EventManager mEventManager;
	wt::Scene mScene;
	wt::Renderer mRenderer;

friend class WtEditor;

}; // </WtEditorContext>

#define WTE_CTX WtEditorContext::getSingleton()

#endif // </WTE_WTEDITORCONTEXT_H>

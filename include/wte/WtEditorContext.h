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

	void loadAssets(const QString&);

	void switchWorkspace(const QString&);

	void saveScene(const QString&);

	void saveAssets(const QString&);

	void unloadAssets();

	void unloadScene();

	wt::EventManager* getEventManager();

	wt::Scene* getScene();

	wt::Renderer* getRenderer();

	bool isAssetsLoaded() const;

	bool isSceneLoaded() const;

	QString getAssetsFilePath() const;

	QString getSceneFilePath() const;

	QString getWorkspaceFilePath() const;

	void reloadAssets();

	void createNewScene(QString path);

	void createNewAssets(QString path);

	wt::LuaObject getSettingsTable();

signals:
	void workspaceSwitched();

	void fileSystemCreated();

	void assetsUnloaded();

	void sceneUnloaded();

	void assetsLoaded();

	void sceneLoaded();

	void clearScene();

protected:
	void setAssets(wt::Assets* assets);

private:
	wt::Assets mAssets;
	wt::lua::State mLuaState;
	QString mAssetsFilePath;
	QString mSceneFilePath;
	QString mWorkspacePath;
	bool mAssetsLoaded;
	bool mSceneLoaded;
	wt::EventManager mEventManager;
	wt::Scene mScene;
	wt::Renderer mRenderer;
	wt::LuaObject mSettingsTable;

friend class WtEditor;

}; // </WtEditorContext>

#define WTE_CTX WtEditorContext::getSingleton()

#endif // </WTE_WTEDITORCONTEXT_H>

#ifndef WTE_PROJECTSCENE_H
#define WTE_PROJECTSCENE_H

#include <wt/ASerializable.h>

class Project;

class ProjectScene : public wt::lua::ASerializable{
friend class Project;
public:
	typedef int32_t Id;

	Project* getProject() const;

	Id getId() const;

	const QString& getPath() const;

	void serialize(wt::lua::State* luaState, LuaPlus::LuaObject& dst) const;

	void deserialize(wt::lua::State* luaState, const LuaPlus::LuaObject& src);

protected:
	ProjectScene(Project* project, Id id=-1);

	void setPath(const QString& path);
	
protected slots:

private:
	ProjectScene(const ProjectScene&);

	Id mId;
	Project* mProject;
	QString mPath;

}; // </ProjectScene>

#endif // </WTE_PROJECTSCENE_H>
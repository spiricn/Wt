#ifndef WT_SCRIPTRESOURCE_H
#define WT_SCRIPTRESOURCE_H

#include "wt/AResourceManager.h"
#include "wt/Singleton.h"

namespace wt
{

class ScriptResource : public AResource<ScriptResource>{
public:
	ScriptResource(AResourceManager<ScriptResource>* manager=NULL, ResourceHandle handle=0, const String& name="");

	~ScriptResource();

	const String& getSource() const;

	void setSource(const String& source);

private:
	String mSource;
}; // </ScriptResource>

class ScriptLoader : public AResourceLoader<ScriptResource>, public Singleton<ScriptLoader>{
public:
	void load(AIOStream* stream, ScriptResource* dst);

	void save(AIOStream* stream, ScriptResource* src);
}; // </ScriptLoader>

} // </wt>

#endif // </WT_SCRIPTRESOURCE_H>

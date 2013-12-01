#ifndef WT_MODELMANAGER_H
#define WT_MODELMANAGER_H


#include "wt/stdafx.h"

#include "wt/Model.h"
#include "wt/Singleton.h"

namespace wt{

class ModelManager : public AResourceManager<Model>{
public:
	ModelManager(AResourceSystem* assets) : AResourceManager(assets){
	}

	Model* create(const String& name){
		Model* res = AResourceManager<Model>::create(name);

		return res;
	}

}; // </ModelManager>

}; // </wt>

#endif

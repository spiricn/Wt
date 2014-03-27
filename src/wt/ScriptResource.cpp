#include "wt/stdafx.h"
#include "wt/ScriptResource.h"

#define TD_TRACE_TAG "ScriptResource"

namespace wt
{

ScriptResource::ScriptResource(AResourceManager<ScriptResource>* manager, ResourceHandle handle, const String& name) : AResource(manager, handle, name){
}

ScriptResource::~ScriptResource(){
}

const String& ScriptResource::getSource() const{
	return mSource;
}

void ScriptResource::setSource(const String& source){
	mSource = source;
}

void ScriptLoader::load(AIOStream* stream, ScriptResource* dst){
	int64_t size = stream->getSize();

	char* bfr = new char[size + 1];
	memset(bfr, 0x00, size + 1);

	stream->read(bfr, size);

	dst->setSource(bfr);

	delete[] bfr;
}

void ScriptLoader::save(AIOStream* stream, ScriptResource* src){
	stream->write(src->getSource().c_str(), src->getSource().size());
}

	
} // </wt>


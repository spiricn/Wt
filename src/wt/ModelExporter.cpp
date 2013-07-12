#include "Wt/stdafx.h"
#include "Wt/Assets.h"
#include "Wt/Defines.h"

#ifdef EXPORTER_MODE

using namespace std;
using namespace Wt;

void convert(const String& src, const String& name){
	GeoModel model;
	Animation animation;

	AssimpModelLoader::getSingleton().load(src, &model, &animation);

	String dstMdl=name;
	dstMdl.append(".wtm");

	String dstAni=name;
	dstAni.append(".wta");

	ModelLoader::getSingleton().save(dstMdl, &model);
	AnimationLoader::getSingleton().save(dstAni, &animation);
}

int main(int argc, char** argv){
	if(argc!=3){
		cout << "Usage: <input_file_name> <output_name>\n";
		exit(0);
	}

	try{
		convert(argv[1], argv[2]);
	}catch(std::exception& e){
		e.what();
		exit(1);
	}

	cout << "Success\n";

	return 0;
}

#endif
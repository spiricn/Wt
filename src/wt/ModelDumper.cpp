#include "Wt/stdafx.h"

#include "Wt/Defines.h"
#include "Wt/Assets.h"

#ifdef DUMPER_MODE

std::ostream& out = std::cout;

using namespace Wt;
using namespace std;

void printNode(GeoNode* node){
}

int main(int argc, char** argv){
	/*if(argc==1){
		return 0;
	}
	*/
	Wt::Utils::setHomeDir("d:\\Documents\\Programiranje\\C++\\Projekti\\Wt");
	GeoModel model;
	try{
		ModelLoader::getSingleton().load("assets/models/elf/elf.wtm", &model);
	}catch(std::exception&e){
		exit(1);
	}

	GeoModel::GeoMap& map = model.getMeshMap();

	out << "Model dump:\n"
		<< "Number of meshes: " << map.size() << "\n";

	int c=0;
	for(GeoModel::GeoMap::iterator i=map.begin(); i!=map.end(); i++){
		out << "[\"" <<  i->second->getName() << "\"] = "",\n";
	}
	

	return 0;
}

#endif
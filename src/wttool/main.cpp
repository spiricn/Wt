#include "wttool/stdafx.h"

#include <string>

#include <wt/ModelLoader.h>
#include <wt/AssimpModelLoader.h>
#include <wt/AnimationLoader.h>

#define WT_CONV_MODE 1
#define WT_DUMP_MODE 0

#if WT_CONV_MODE

using namespace wt;

static const char* TAG = "WtConv";
int main(int argc, char** argv){

	if(argc < 3){
		goto usage;
	}

	const char* input = argv[1];
	const char* output = argv[2];

	if(argc==5){
		const char* flag = argv[3];
		
		if(strcmp(flag, "-r")==0){
			const char* geoName = argv[4];
			wt::Model model(0, "tmp");

			try{
				LOGD(TAG, "Loading model from \"%s\"...", input);
				ModelLoader::getSingleton().load(input, &model);

				LOGD(TAG, "Removing geometry batch \"%s\"...", geoName);
				model.removeGeometry(geoName);

				LOGD(TAG, "Saving to \"%s\"...", output);
				ModelLoader::getSingleton().save(output, &model);
			}
			catch(wt::Exception&){
				goto error;
			}
		}
		else{
			goto usage;
		}
	}
	else{
		wt::Model model(0, "tmp");
		wt::Animation animation;

		LOGD(TAG, "Loading model from \"%s\"...", input);

		try{
			AssimpModelLoader::getSingleton().load(input, model, &animation);

			LOGD(TAG, "Converting to wtm...");

			char tmp[1024];
			*tmp = 0;

			sprintf(tmp, "%s.wtm", output);
			LOGD(TAG, "Saving geo model to \"%s\"...", tmp);
			wt::ModelLoader::getSingleton().save(tmp, &model);

			sprintf(tmp, "%s.wta", output);
			LOGD(TAG, "Saving animation to \"%s\"...", tmp);
			AnimationLoader::getSingleton().save(tmp, &animation);
		}
		catch(wt::Exception&){
			goto error;
		}
	}

	LOGI(TAG, "Done!");

	return 0;

error:
	LOGE(TAG, "Error converting model");
	return 1;

usage:
	LOGW(TAG, "Program usage: wtconv [<input_file> <out_name>] [<input_file> <out_file> -r <geo_name>");
	return 0;
}
#endif

#if WT_DUMP_MODE
void printGeoNode(std::ostream& out, SkeletonBone* bone, wt::Uint32 depth){
	out << "\n" << Utils::Indent(depth) << "\"" << bone->getName() << "\" : {\n";
	
	glm::vec3 pos,scale;
	glm::vec4 rot;
	glm::quat quat;

	math::decomposeMatrix(bone->getTransform(), quat, pos, scale);
	math::quatToAxisAngle(quat, rot);

	out <<	Utils::Indent(depth+1) << "\"numChildren\" : " << bone->getNumChildren(false) << ",\n" <<
			Utils::Indent(depth+1) << "\"translation\" : [" << pos.x << ", " << pos.y << ", " << pos.z << "],\n" <<
			Utils::Indent(depth+1) << "\"scale\" : [" << scale.x << ", " << scale.y << ", " << scale.z << "],\n" <<
			Utils::Indent(depth+1) << "\"children\" : {\n";
	for(SkeletonBone::Iterator i=bone->getBeg(); i!=bone->getEnd(); i++){
		printGeoNode(out, *i, depth+2);

		out << Utils::Indent(depth+2) << ",\n";
	}
	out << Utils::Indent(depth+1) << "}\n";

	out << Utils::Indent(depth) << "}\n\n";
}

void printModel(std::ostream& out, wt::GeoModel& model){
	out << "numGeometry = " << model.getGeometry().size() << "\n\n";

	out << "geometry = [\n";

	for(GeoModel::GeoList::iterator i=model.getGeometry().begin(); i!=model.getGeometry().end(); i++){
		out << "\"" << (*i)->getName().c_str() << "\", numVertices=" << (*i)->getVertices().getCapacity() << "\n";
	}

	out << "]\n\n";

	out << "skeleton = {\n";
	if(model.getRootBone()!=NULL){
		printGeoNode(out, model.getRootBone(), 0);
	}
	
	out << "}";
}

void printAnimation(std::ostream& out, wt::Animation& animation){
	out << "duration = " << animation.getDuration() << '\n'
		<< "numAnimationChannels = " << animation.mNodeAnimations.size() << '\n';

	out << "channels = {\n\n";

	for(wt::Uint32 i=0; i<animation.mNodeAnimations.size(); i++){
		NodeAnimation& ani = *animation.mNodeAnimations[i];

		out << Utils::Indent(1) << "\"" << ani.getTargetNode() << "\" :{\n";

		// position
		out << Utils::Indent(2) << "\"position keys\" : [\n";

		for(wt::Uint32 j=0; j<ani.getPositionKeys().size(); j++){
			NodeAnimation::PositionKey& key = ani.getPositionKeys()[j];

			out << Utils::Indent(3) << "[ " << key.time << ", ["
				<< key.position.x << ", " << key.position.y << ", " << key.position.z << " ] ],\n";
		}
		out << Utils::Indent(2) << "], # end \"position keys\"\n"; 

		// rotation
		out << Utils::Indent(2) << "\"rotation keys\" : [\n";

		for(wt::Uint32 j=0; j<ani.getRotationKeys().size(); j++){
			NodeAnimation::RotationKey& key = ani.getRotationKeys()[j];
			glm::vec4 rot;
			math::quatToAxisAngle(key.rotation, rot);

			out << Utils::Indent(3) << "[ " << key.time << ", ["
				<< rot.x << ", " << rot.y << ", " << rot.z << ", " << rot.w << " ] ],\n";
		}
		out << Utils::Indent(2) << "], # end \"rotation keys\"\n";

		// scale
		out << Utils::Indent(2) << "\"scale keys\" : [\n";

		for(wt::Uint32 j=0; j<ani.getScaleKeys().size(); j++){
			NodeAnimation::ScaleKey& key = ani.getScaleKeys()[j];

			out << Utils::Indent(3) << "[ " << key.time << ", ["
				<< key.scale.x << ", " << key.scale.y << ", " << key.scale.z << " ] ],\n";
		}
		out << Utils::Indent(2) << "], # end \"scale keys\"\n"
			<< Utils::Indent(1) << "}, # end \"" << ani.getTargetNode() << "\"\n\n";

	}

	out << "} # end channels\n";

}

static const char* TAG = "WtDump";
int main(int argc, char** argv){

	if(argc != 3){
		return 0;
	}

	/*const wt::String input = "D:/documents/prog/c++/workspace/wt/assets/models/sylvana/sylvana.wtm";
	const char* output = "C:/users/nikola/desktop/dump";*/
	const wt::String input = argv[1];
	const char* output = argv[2];

	wt::GeoModel model(0, "");
	wt::Animation animation;

	std::ofstream outFile(output);
	if(!outFile.is_open()){
		LOGE(TAG, "Error openning output file \"%s\"", output);
		goto error;
	}

	try{
		AssimpModelLoader::TextureMap texMap;

		bool isModel=true;

		if(Utils::endsWith(input, ".wtm")){
			LOGD(TAG, "Loading model from \"%s\" using WTM loader...", input.c_str());
			ModelLoader::getSingleton().load(input, &model);
		}
		else if(Utils::endsWith(input, ".wta")){
			LOGD(TAG, "Loading animation from \"%s\" using WTA loader...", input.c_str());
			AnimationLoader::getSingleton().load(input, &animation);
			isModel=false;
		}
		else{
			LOGD(TAG, "Loading model from \"%s\" using ASSIMP loader...", input.c_str());
			AssimpModelLoader::getSingleton().load(input, model, &animation, &texMap);
		}


		if(isModel){
			LOGD(TAG, "Dumping model data..");
			printModel(outFile, model);

			if(!texMap.empty()){
				outFile << "\ntexture_map = {\n";
				for(AssimpModelLoader::TextureMap::iterator i=texMap.begin(); i!=texMap.end(); i++){
					outFile << "\t\"" << i->first << "\" :  \"" << i->second << "\",\n";
				}
				outFile << "}";
			}
		}
		else{
			LOGD(TAG, "Dumping animation data..");

			printAnimation(outFile, animation);
		}

		LOGI(TAG, "Dump saved to \"%s\"", output);
	}
	catch(wt::Exception&){
		outFile.close();
		goto error;
	}

	outFile.close();

	return 0;

error:
	LOGE(TAG, "Error converting model");
	return 1;

usage:
	LOGW(TAG, "Program usage: wtdump <input_file> <out_name>");
	return 0;
}

#endif
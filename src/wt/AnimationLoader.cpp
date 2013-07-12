#include "wt/stdafx.h"
#include "wt/AnimationLoader.h"

#define TD_TRACE_TAG "AnimationLoader"

namespace wt{

const char* AnimationLoader::FORMAT_ID = "#WTANI";

void AnimationLoader::load(const String& path, Animation* ani){
	std::ifstream file(path.c_str(), std::ios::binary);

	if(!file.is_open()){
		WT_THROW("Unable to open file: \"%s\"", path.c_str());
	}

	// format_identifier
	char fmtId[7];
	fmtId[6]=0;
	file.read(fmtId, 6);

	if(strcmp(fmtId, FORMAT_ID) != 0){
		WT_THROW("Invalid wt animation file \"%s\"", path.c_str());
	}

	// duration
	float dur;
	file.read((char*)&dur, 4);
	ani->setDuration(dur);
	//LOGV("Duration: %f", dur);

	// num_node_anims
	Uint32 numNodes;
	file.read((char*)&numNodes, 4);
	//LOGV("Num nodes: %d", numNodes);

	// nodes
	for(Uint32 i=0; i<numNodes; i++){
		NodeAnimation* node = ani->addNodeAnimation();

		// node_name_id
		String name;
		std::getline(file, name, '\0');
		node->setTargetNode(name);

		// num_pos_keys
		Uint32 numPosKeys;
		file.read((char*)&numPosKeys, 4);

		// pos_keys
		for(Uint32 j=0; j<numPosKeys; j++){
			NodeAnimation::PositionKey key;

			file.read((char*)&key, sizeof(NodeAnimation::PositionKey));

			node->getPositionKeys().push_back(key);
		}

		// num_rot_keys
		Uint32 numRotKeys;
		file.read((char*)&numRotKeys, 4);

		// rot_keys
		for(Uint32 j=0; j<numRotKeys; j++){
			NodeAnimation::RotationKey key;

			// rotation
			float rot[4];
			file.read((char*)rot, 4*sizeof(float));
			key.rotation.x = rot[0];
			key.rotation.y = rot[1];
			key.rotation.z = rot[2];
			key.rotation.w = rot[3];

			// time
			file.read((char*)&key.time, sizeof(float));

			node->getRotationKeys().push_back(key);
		}

		// num_scale_keys
		Uint32 numScaleKeys;
		file.read((char*)&numScaleKeys, 4);
		//LOGV("Num scale keys: %d", numScaleKeys);

		// pos_keys
		for(Uint32 j=0; j<numScaleKeys; j++){
			NodeAnimation::ScaleKey key;

			// position
			file.read((char*)glm::value_ptr(key.scale), 3*sizeof(float));

			// time
			file.read((char*)&key.time, sizeof(float));

			node->getScaleKeys().push_back(key);
		}
	}

#if 0
	glm::mat4 rot = glm::mat4_cast(glm::angleAxis(90.0f, glm::vec3(0, 1, 0)));
	postProcess(ani, rot);
#endif
	file.close();
}

void AnimationLoader::postProcess(Animation* ani, const glm::mat4& transform){
	// TODO this is probably not the way of doing this, but 
	// until the bones are propely post processed (check out post processing in ModelLoader.cpp) we can't know for sure..
#if 0
	glm::quat rot;
	glm::vec3 trans, scale;
	math::decomposeMatrix(transform, rot, trans, scale);
	
	for(Uint32 i=0; i<ani->mNodeAnimations.size(); i++){
		NodeAnimation* node = ani->mNodeAnimations[i];

		for(Uint32 j=0; j<node->getPositionKeys().size(); j++){
			NodeAnimation::PositionKey& key = node->getPositionKeys()[j];
			key.position += trans;
		}

		for(Uint32 j=0; j<node->getRotationKeys().size(); j++){
			NodeAnimation::RotationKey& key = node->getRotationKeys()[j];
			key.rotation = rot * key.rotation;
		}
	}
#endif
}

void AnimationLoader::save(const String& path, Animation* ani){
	std::ofstream file(path.c_str(), std::ios::binary);

	if(!file.is_open()){
		WT_THROW("Unable to open file: \"%s\"", path.c_str());
	}

	// format_identifier
	file.write(FORMAT_ID, strlen(FORMAT_ID));

	// duration
	float dur = ani->getDuration();
	file.write((char*)&dur, 4);

	// num_node_anims
	Uint32 numNodes = ani->mNodeAnimations.size();
	file.write((char*)&numNodes, 4);

	// node_anims
	for(Animation::NodeAnimationList::iterator i=ani->mNodeAnimations.begin(); i!=ani->mNodeAnimations.end(); i++){
		NodeAnimation* node = *i;

		// node_name_id
		file.write(node->getTargetNode().c_str(), node->getTargetNode().size()+1);

		// num_pos_keys
		Uint32 numPosKeys = node->getPositionKeys().size();
		file.write((char*)&numPosKeys, 4);

		// pos_keys
		for(Uint32 j=0; j<numPosKeys; j++){
			// position
			file.write((char*)glm::value_ptr(node->getPositionKeys()[j].position), 3*sizeof(float));

			// time
			file.write((char*)&node->getPositionKeys()[j].time, sizeof(float));
		}

		// num_rot_keys
		Uint32 numRotKeys = node->getRotationKeys().size();
		file.write((char*)&numRotKeys, 4);

		// rot_keys
		for(NodeAnimation::RotKeyList::iterator i=node->getRotationKeys().begin(); i!=node->getRotationKeys().end(); i++){
			// rotation
			float rot[4] = {i->rotation.x,
				i->rotation.y,
				i->rotation.z,
				i->rotation.w};

			file.write((char*)rot, 4*sizeof(float));

			// time
			file.write((char*)&i->time, sizeof(float));
		}

		// num_scale_keys
		Uint32 numScaleKeys = node->getScaleKeys().size();
		file.write((char*)&numScaleKeys, 4);

		// scale_keys
		for(NodeAnimation::ScaleKeyList::iterator i=node->getScaleKeys().begin(); i!=node->getScaleKeys().end(); i++){
			// scale
			file.write((char*)glm::value_ptr(i->scale), 3*sizeof(float));

			// time
			file.write((char*)&i->time, sizeof(float));
		}
	}

	file.close();
}

}; // </wt>

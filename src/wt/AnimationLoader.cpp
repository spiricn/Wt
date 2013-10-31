#include "wt/stdafx.h"
#include "wt/AnimationLoader.h"

#define TD_TRACE_TAG "AnimationLoader"

namespace wt{

const char* AnimationLoader::FORMAT_ID = "#WTANI";

void AnimationLoader::load(AIOStream* stream, Animation* ani){
	if(!stream->isReadable()){
		WT_THROW("Unable load animation - stream not readable");
	}

	// format_identifier
	char fmtId[7];
	fmtId[6]=0;
	stream->read(fmtId, 6);

	if(strcmp(fmtId, FORMAT_ID) != 0){
		WT_THROW("Invalid wt animation data (header missing)");
	}

	// duration
	float dur;
	stream->read((char*)&dur, 4);
	ani->setDuration(dur);
	//LOGV("Duration: %f", dur);

	// num_node_anims
	uint32_t numNodes;
	stream->read((char*)&numNodes, 4);
	//LOGV("Num nodes: %d", numNodes);

	// nodes
	for(uint32_t i=0; i<numNodes; i++){
		NodeAnimation* node = ani->addNodeAnimation();

		// node_name_id
		String name;
		WT_ASSERT(stream->getLine(name, '\0') > 0, "Error reading animation data");

		node->setTargetNode(name);

		// num_pos_keys
		uint32_t numPosKeys;
		stream->read((char*)&numPosKeys, 4);

		// pos_keys
		for(uint32_t j=0; j<numPosKeys; j++){
			NodeAnimation::PositionKey key;

			stream->read((char*)&key, sizeof(NodeAnimation::PositionKey));

			node->getPositionKeys().push_back(key);
		}

		// num_rot_keys
		uint32_t numRotKeys;
		stream->read((char*)&numRotKeys, 4);

		// rot_keys
		for(uint32_t j=0; j<numRotKeys; j++){
			NodeAnimation::RotationKey key;

			// rotation
			float rot[4];
			stream->read((char*)rot, 4*sizeof(float));
			key.rotation.x = rot[0];
			key.rotation.y = rot[1];
			key.rotation.z = rot[2];
			key.rotation.w = rot[3];

			// time
			stream->read((char*)&key.time, sizeof(float));

			node->getRotationKeys().push_back(key);
		}

		// num_scale_keys
		uint32_t numScaleKeys;
		stream->read((char*)&numScaleKeys, 4);
		//LOGV("Num scale keys: %d", numScaleKeys);

		// pos_keys
		for(uint32_t j=0; j<numScaleKeys; j++){
			NodeAnimation::ScaleKey key;

			// position
			stream->read((char*)glm::value_ptr(key.scale), 3*sizeof(float));

			// time
			stream->read((char*)&key.time, sizeof(float));

			node->getScaleKeys().push_back(key);
		}
	}

#if 0
	glm::mat4 rot = glm::mat4_cast(glm::angleAxis(90.0f, glm::vec3(0, 1, 0)));
	postProcess(ani, rot);
#endif
}

void AnimationLoader::postProcess(Animation* ani, const glm::mat4& transform){
	// TODO this is probably not the way of doing this, but 
	// until the bones are propely post processed (check out post processing in ModelLoader.cpp) we can't know for sure..
#if 0
	glm::quat rot;
	glm::vec3 trans, scale;
	math::decomposeMatrix(transform, rot, trans, scale);
	
	for(uint32_t i=0; i<ani->mNodeAnimations.size(); i++){
		NodeAnimation* node = ani->mNodeAnimations[i];

		for(uint32_t j=0; j<node->getPositionKeys().size(); j++){
			NodeAnimation::PositionKey& key = node->getPositionKeys()[j];
			key.position += trans;
		}

		for(uint32_t j=0; j<node->getRotationKeys().size(); j++){
			NodeAnimation::RotationKey& key = node->getRotationKeys()[j];
			key.rotation = rot * key.rotation;
		}
	}
#endif
}

void AnimationLoader::save(AIOStream* stream, Animation* ani){
	WT_ASSERT(stream->isWritable(), "Unable to save animation - stream not writtable");

	// format_identifier
	stream->write(FORMAT_ID, strlen(FORMAT_ID));

	// duration
	float dur = ani->getDuration();
	stream->write((char*)&dur, 4);

	// num_node_anims
	uint32_t numNodes = ani->getNodeAnimationList().size();
	stream->write((char*)&numNodes, 4);

	// node_anims
	for(Animation::NodeAnimationList::iterator i=ani->getNodeAnimationList().begin(); i!=ani->getNodeAnimationList().end(); i++){
		NodeAnimation* node = *i;

		// node_name_id
		stream->write(node->getTargetNode().c_str(), node->getTargetNode().size()+1);

		// num_pos_keys
		uint32_t numPosKeys = node->getPositionKeys().size();
		stream->write((char*)&numPosKeys, 4);

		// pos_keys
		for(uint32_t j=0; j<numPosKeys; j++){
			// position
			stream->write((char*)glm::value_ptr(node->getPositionKeys()[j].position), 3*sizeof(float));

			// time
			stream->write((char*)&node->getPositionKeys()[j].time, sizeof(float));
		}

		// num_rot_keys
		uint32_t numRotKeys = node->getRotationKeys().size();
		stream->write((char*)&numRotKeys, 4);

		// rot_keys
		for(NodeAnimation::RotKeyList::iterator i=node->getRotationKeys().begin(); i!=node->getRotationKeys().end(); i++){
			// rotation
			float rot[4] = {i->rotation.x,
				i->rotation.y,
				i->rotation.z,
				i->rotation.w};

			stream->write((char*)rot, 4*sizeof(float));

			// time
			stream->write((char*)&i->time, sizeof(float));
		}

		// num_scale_keys
		uint32_t numScaleKeys = node->getScaleKeys().size();
		stream->write((char*)&numScaleKeys, 4);

		// scale_keys
		for(NodeAnimation::ScaleKeyList::iterator i=node->getScaleKeys().begin(); i!=node->getScaleKeys().end(); i++){
			// scale
			stream->write((char*)glm::value_ptr(i->scale), 3*sizeof(float));

			// time
			stream->write((char*)&i->time, sizeof(float));
		}
	}
}

}; // </wt>

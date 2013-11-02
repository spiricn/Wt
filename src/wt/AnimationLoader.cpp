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
		// node_name_id
		String name;
		WT_ASSERT(stream->getLine(name, '\0') > 0, "Error reading animation data");

		NodeAnimation* node = ani->addNodeAnimation(name);

		// num_pos_keys
		uint32_t numPosKeys;
		stream->read((char*)&numPosKeys, 4);

		// pos_keys
		for(uint32_t j=0; j<numPosKeys; j++){
			NodeAnimation::PositionKey* key = node->addPositionKey();

			// position
			stream->read((char*)glm::value_ptr(key->value), 3*sizeof(float));

			// time
			stream->read((char*)&key->time, sizeof(float));
		}

		// num_rot_keys
		uint32_t numRotKeys;
		stream->read((char*)&numRotKeys, 4);

		// rot_keys
		for(uint32_t j=0; j<numRotKeys; j++){
			NodeAnimation::RotationKey* key = node->addRotationKey();

			// rotation
			float rot[4];
			stream->read((char*)rot, 4*sizeof(float));
			key->value.x = rot[0];
			key->value.y = rot[1];
			key->value.z = rot[2];
			key->value.w = rot[3];

			// time
			stream->read((char*)&key->time, sizeof(float));
		}

		// num_scale_keys
		uint32_t numScaleKeys;
		stream->read((char*)&numScaleKeys, 4);
		//LOGV("Num scale keys: %d", numScaleKeys);

		// pos_keys
		for(uint32_t j=0; j<numScaleKeys; j++){
			NodeAnimation::ScaleKey* key = node->addScaleKey();

			// scale
			stream->read((char*)glm::value_ptr(key->value), 3*sizeof(float));

			// time
			stream->read((char*)&key->time, sizeof(float));
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
	const float dur = ani->getDuration();
	stream->write((char*)&dur, 4);

	// num_node_anims
	const uint32_t numNodes = ani->getNodeAnimationList().size();
	stream->write((char*)&numNodes, 4);

	// node_anims
	for(Animation::NodeAnimationList::iterator i=ani->getNodeAnimationList().begin(); i!=ani->getNodeAnimationList().end(); i++){
		NodeAnimation* node = *i;

		// node_name_id
		stream->write(node->getName().c_str(), node->getName().size()+1);

		// num_pos_keys
		uint32_t numPosKeys = node->getNumPosKeys();
		stream->write((char*)&numPosKeys, 4);

		// pos_keys
		for(NodeAnimation::PosKeyIter iter=node->getPosKeysBegin(); iter!=node->getPosKeysEnd(); iter++){
			// position
			stream->write((char*)glm::value_ptr((*iter)->value), 3*sizeof(float));

			// time
			stream->write((char*)&(*iter)->time, sizeof(float));
		}

		// num_rot_keys
		uint32_t numRotKeys = node->getNumRotKeys();
		stream->write((char*)&numRotKeys, 4);

		// rot_keys
		for(NodeAnimation::RotKeyIter iter=node->getRotKeysBegin(); iter!=node->getRotKeysEnd(); iter++){
			// rotation
			const float rot[4] = {
				(*iter)->value.x,
				(*iter)->value.y,
				(*iter)->value.z,
				(*iter)->value.w
			};

			stream->write((char*)rot, 4*sizeof(float));

			// time
			stream->write((char*)&(*iter)->time, sizeof(float));
		}

		// num_scale_keys
		const uint32_t numScaleKeys = node->getNumScaleKeys();
		stream->write((char*)&numScaleKeys, 4);

		// scale_keys
		for(NodeAnimation::ScaleKeyIter iter=node->getScaleKeysBegin(); iter!=node->getScaleKeysEnd(); iter++){
			// scale
			stream->write((char*)glm::value_ptr((*iter)->value), 3*sizeof(float));

			// time
			stream->write((char*)&(*iter)->time, sizeof(float));
		}
	}
}

}; // </wt>

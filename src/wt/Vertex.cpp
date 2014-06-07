/**
 * @file Vertex.cpp
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */

#include "wt/stdafx.h"
#include "wt/Vertex.h"
#include "wt/Exception.h"

#define TD_TRACE_TAG "Vertex"

namespace wt
{

Vertex::Vertex() : x(0.0f), y(0.0f), z(0.0f), s(0.0f), t(0.0f),
	nx(0.0f), ny(0.0f), nz(0.0f), tx(0.0f), ty(0.0f), tz(0.0f){

	for(uint32_t i=0; i<4; i++){
		weights[i] = 0.0f;

		// Bone index of value kINVALID_BONE_INDEX indicates that the slot is not taken
		bones[i] = kINVALID_BONE_INDEX; 
	}
}

void Vertex::addBone(int32_t index, float weight){
	// find the first bone slot that's not taken
	for(uint32_t i=0; i<4; i++){
		if(bones[i] == kINVALID_BONE_INDEX){
			// assign the index and weight
			bones[i]=index;
			weights[i]=weight;
			return;
		}
	}

	WT_THROW("Vertex supports max 4 bones (attempted to add 5th)");
}


} // </wt>


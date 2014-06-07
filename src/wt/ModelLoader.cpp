#include "wt/stdafx.h"
#include "wt/ModelLoader.h"
#include "wt/Geometry.h"

#define TD_TRACE_TAG "ModelLoader"

namespace wt{

const char* ModelLoader::TAG = "ModelLoader";
const char* ModelLoader::FORMAT_ID = "#WTMDL";

void ModelLoader::create(Model* model){
	model->create();
}

void ModelLoader::writeNode(AIOStream* stream, SkeletonBone* bone){
	// name_id
	stream->write(bone->getName().c_str(), bone->getName().size()+1);

	// transform
	stream->write((const char*)glm::value_ptr(bone->getTransform()), 16*sizeof(float));

	// offset
	stream->write((const char*)glm::value_ptr(bone->getOffset()), 16*sizeof(float));

	// index
	uint32_t idx=bone->getIndex();
	stream->write((const char*)&idx, sizeof(uint32_t));

	// num_children
	uint32_t num_children = bone->getNumChildren(false);
	stream->write((const char*)&num_children, 4);

	// children
	for(SkeletonBone::Iterator i=bone->getBeg(); i!=bone->getEnd(); i++){
		writeNode(stream, *i);
	}
}

void ModelLoader::readNode(AIOStream* stream, Model* model, SkeletonBone* parent){
	// name_id
	String name;
	WT_ASSERT(stream->getLine(name, '\0') > 0, "Error reading model data");
	//LOGV("Node name: %s", name.c_str());

	// transform
	glm::mat4 transform;
	stream->read((char*)glm::value_ptr(transform), 16*sizeof(float));

	// offset
	glm::mat4 offset;
	stream->read((char*)glm::value_ptr(offset), 16*sizeof(float));

#if 0
	glm::mat4 rot = glm::mat4_cast(glm::angleAxis(90.0f, glm::vec3(0, 1, 0)));
	offset =  offset * glm::inverse(rot);
#endif

	// index
	uint32_t idx;
	stream->read((char*)&idx, sizeof(uint32_t));

	// create node
	SkeletonBone* self;
	if(parent==NULL){
		self = model->createRootBone(name);
	}
	else{
		self = parent->addChild(name, idx);
	}

	self->setTransform(transform);
	self->getOffset() = offset;

	// num_children
	uint32_t numChildren;
	stream->read((char*)&numChildren, 4);
	//LOGV("Num children: %d", numChildren);

	// read children
	for(uint32_t i=0; i<numChildren; i++){
		readNode(stream, model, self);
	}
}

void ModelLoader::load(AIOStream* stream, Model* model){
	if(!stream->isReadable()){
		WT_THROW("Error loading model, stream not readable");
	}

	// format_identifier
	char fmtId[7];
	fmtId[6] = 0; // null terminator
	stream->read(fmtId, 6);

	if(strcmp(fmtId, FORMAT_ID) != 0){
		WT_THROW("Stream does not contain valid wt model data (header missing)");
	}

	// num_geometry
	uint32_t numGeometry;
	stream->read((char*)&numGeometry, 4);

	/* start of geometry data */
	uint64_t geoOffset = stream->tell();


	uint32_t numVertices=0;
	uint32_t numIndices=0;
	/* calculate the total number of vertices/indices */
	for(uint32_t i=0; i<numGeometry; i++){
		// name_id
		String name;
		WT_ASSERT(stream->getLine(name, '\0') > 0, "Error reading model data");

		// NoV
		uint32_t nov;
		stream->read((char*)&nov, 4);
		numVertices += nov;

		// NoI
		uint32_t noi;
		stream->read((char*)&noi, 4);
		numIndices += noi;

		if(i < numGeometry-1){
			/* skip the vertex/index data since we're only interested in the number
			of vertices/indices*/
			stream->seek(AIOStream::eSEEK_CURRENT, nov*sizeof(Vertex) + noi*sizeof(uint32_t));
		}

	}

	model->setSize(numVertices, numIndices);

	/* seek back to the beggining of geometry data */
	stream->seek(geoOffset);

	// geometry
	for(uint32_t i=0; i<numGeometry; i++){
		// name_id
		String name;
		WT_ASSERT(stream->getLine(name, '\0') > 0, "Error reading model data");
		//LOGV("geo name_id: %s", name.c_str());

		// create structure
		//Geometry* geo = model->createGeometry(name);

		// NoV
		uint32_t nov;
		stream->read((char*)&nov, 4);

		// NoI
		uint32_t noi;
		stream->read((char*)&noi, 4);
		//LOGV("noi: %d", noi);

		// vertex_data
		Buffer<Vertex> vertices(nov);
		stream->read((char*)vertices.getData(), nov*sizeof(Vertex));

		// Verify vertices
		for(uint32_t i=0; i<nov; i++){
#define VALID_FLOAT( val ) ( !(glm::isnan( (val) ) || glm::isinf( (val) )) )
			
			WT_ASSERT(VALID_FLOAT(vertices[i].x), "Corrupted vertex data");
			WT_ASSERT(VALID_FLOAT(vertices[i].y), "Corrupted vertex data");
			WT_ASSERT(VALID_FLOAT(vertices[i].z), "Corrupted vertex data");
		}

		// post processing
#if 0
		// NOTE this part works fine
		glm::mat4 rot = glm::mat4_cast(glm::angleAxis(90.0f, glm::vec3(0, 1, 0)));

		for(uint32_t j=0; j<vertices.getCapacity(); j++){
			glm::vec4 pos(vertices[j].x, vertices[j].y, vertices[j].z, 0.0f);
			glm::vec4 normal(vertices[j].nx, vertices[j].ny, vertices[j].nz, 0.0f);

			pos = rot*pos;
			normal = rot*normal;

			vertices[j].x = pos.x;
			vertices[j].y = pos.y;
			vertices[j].z = pos.z;

			vertices[j].nx = normal.x;
			vertices[j].ny = normal.y;
			vertices[j].nz = normal.z;
		}
#endif
		// index_data
		Buffer<GLuint> indices(noi);
		stream->read((char*)indices.getData(), noi*sizeof(uint32_t));

		// Verfify indices
		for(uint32_t i=0; i<noi; i++){
			WT_ASSERT(indices[i] < nov, "Invalid geometry index, data corrupted (index=%u num_vertices=%u)", indices[i], nov);
		}

		model->addGeometry(name, vertices, indices);
		//// setup structure
		//geo->create(vertices, indices);
	}

	// has_skeleton
	bool hasSkeleton = stream->get()==1?true:false;

	if(hasSkeleton){
		readNode(stream, model, NULL);
	}
}

void ModelLoader::postProcess(SkeletonBone* bone, const glm::mat4& transform){
	// TODO bones have to be processed as well, their offset matrices and local matrices
#if 0
	for(uint32_t i=0; i<bone->getNumChildren(false); i++){
 		postProcess(bone->getChildAt(i), transform);
	}
#endif
}

void ModelLoader::postProcess(Model* model, const glm::mat4& transform){
	for(Model::GeoList::iterator i = model->getGeometry().begin(); i!=model->getGeometry().end(); i++){

		Buffer<Vertex>& vertices = (*i)->getVertices();

		for(uint32_t j=0; j<vertices.getCapacity(); j++){
			Vertex& vertex = vertices[j];

			glm::vec4 position = transform * glm::vec4(vertex.x, vertex.y, vertex.z, 0.0f);
			glm::vec4 normal = transform * glm::vec4(vertex.nx, vertex.ny, vertex.nz, 0.0f);
			glm::vec4 tangent = transform * glm::vec4(vertex.tx, vertex.ty, vertex.tz, 0.0f);

			memcpy(&vertex.x, glm::value_ptr(position), 3*sizeof(float));
			memcpy(&vertex.nx, glm::value_ptr(normal), 3*sizeof(float));
			memcpy(&vertex.tx, glm::value_ptr(tangent), 3*sizeof(float));
		}
	}


#if 0
	// TODO
	if(model->getRootBone() != NULL){
		glm::mat4 boneTransform = transform;
		postProcess(model->getRootBone(), boneTransform);
	}
#endif
}

void ModelLoader::save(AIOStream* stream, Model* model){
	if(!stream->isWritable()){
		WT_THROW("Unable to save model - stream not writable");
	}

	// format_identifier
	stream->write(FORMAT_ID, strlen(FORMAT_ID));

	// num_geometry
	uint32_t numGeometry = model->getGeometry().size();
	stream->write((const char*)&numGeometry, 4);

	// geometry
	for(Model::GeoList::iterator i=model->getGeometry().begin(); i!=model->getGeometry().end(); i++){
		Geometry* geo = *i;

		// name_id
		stream->write(geo->getName().c_str(), geo->getName().size()+1);

		// NoV
		uint32_t nov = geo->getVertices().getCapacity();
		stream->write((const char*)&nov, 4); 

		// NoI
		uint32_t noi = geo->getIndices().getCapacity();
		stream->write((const char*)&noi, 4);

		// vertex_data
		void* vertices = (void*)geo->getVertices().getData();
		stream->write((const char*)vertices, nov*sizeof(Vertex));
		
		// index_data
		void* indices = (void*)geo->getIndices().getData();
		stream->write((const char*)indices, noi*4);
	}

	// has_skeleton 
	char hasSkeleton = model->getRootBone()==NULL?0:1;
	stream->write(&hasSkeleton, 1);

	if(hasSkeleton){
		// root_node
		writeNode(stream, model->getRootBone());
	}
}

}; // </wt>

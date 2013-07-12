#include "wt/stdafx.h"
#include "wt/ModelLoader.h"

namespace wt{

const char* ModelLoader::TAG = "ModelLoader";
const char* ModelLoader::FORMAT_ID = "#WTMDL";

void ModelLoader::create(Model* model){
	model->create();
}

void ModelLoader::writeNode(std::ofstream& file, SkeletonBone* bone){
	// name_id
	file.write(bone->getName().c_str(), bone->getName().size()+1);

	// transform
	file.write((const char*)glm::value_ptr(bone->getTransform()), 16*sizeof(float));

	// offset
	file.write((const char*)glm::value_ptr(bone->getOffset()), 16*sizeof(float));

	// index
	Uint32 idx=bone->getIndex();
	file.write((const char*)&idx, sizeof(Uint32));

	// num_children
	Uint32 num_children = bone->getNumChildren(false);
	file.write((const char*)&num_children, 4);

	// children
	for(SkeletonBone::Iterator i=bone->getBeg(); i!=bone->getEnd(); i++){
		writeNode(file, *i);
	}
}

void ModelLoader::readNode(std::ifstream& file, Model* model, SkeletonBone* parent){
	// name_id
	std::string name;
	std::getline(file, name, '\0');
	//LOGV(TAG, "Node name: %s", name.c_str());

	// transform
	glm::mat4 transform;
	file.read((char*)glm::value_ptr(transform), 16*sizeof(float));

	// offset
	glm::mat4 offset;
	file.read((char*)glm::value_ptr(offset), 16*sizeof(float));

#if 0
	glm::mat4 rot = glm::mat4_cast(glm::angleAxis(90.0f, glm::vec3(0, 1, 0)));
	offset =  offset * glm::inverse(rot);
#endif

	// index
	Uint32 idx;
	file.read((char*)&idx, sizeof(Uint32));

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
	Uint32 numChildren;
	file.read((char*)&numChildren, 4);
	//LOGV(TAG, "Num children: %d", numChildren);

	// read children
	for(Uint32 i=0; i<numChildren; i++){
		readNode(file, model, self);
	}
}

void ModelLoader::load(const String& path, Model* model){
	std::ifstream file(path.c_str(), std::ios::binary);

	if(!file.is_open()){
		WT_QEXCEPT("Unable to open file: \"%s\"", path.c_str());
	}

	// format_identifier
	char fmtId[7];
	fmtId[6] = 0; // null terminator
	file.read(fmtId, 6);

	if(strcmp(fmtId, FORMAT_ID) != 0){
		WT_QEXCEPT("Invalid wt model file \"%s\"", path.c_str());
	}

	// num_geometry
	Uint32 numGeometry;
	file.read((char*)&numGeometry, 4);

	/* start of geometry data */
	Uint32 geoOffset = file.tellg();


	Uint32 numVertices=0;
	Uint32 numIndices=0;
	/* calculate the total number of vertices/indices */
	for(Uint32 i=0; i<numGeometry; i++){
		// name_id
		std::string name;
		std::getline(file, name, '\0');

		// NoV
		Uint32 nov;
		file.read((char*)&nov, 4);
		numVertices += nov;

		// NoI
		Uint32 noi;
		file.read((char*)&noi, 4);
		numIndices += noi;

		if(i < numGeometry-1){
			/* skip the vertex/index data since we're only interested in the number
			of vertices/indices*/
			file.seekg(nov*sizeof(Geometry::Vertex) + noi*sizeof(Uint32), std::ios::cur);
		}

	}

	model->setSize(numVertices, numIndices);

	/* seek back to the beggining of geometry data */
	file.seekg(geoOffset);

	// geometry
	for(Uint32 i=0; i<numGeometry; i++){
		// name_id
		std::string name;
		std::getline(file, name, '\0');
		//LOGV(TAG, "geo name_id: %s", name.c_str());

		// create structure
		//Geometry* geo = model->createGeometry(name);

		// NoV
		Uint32 nov;
		file.read((char*)&nov, 4);

		// NoI
		Uint32 noi;
		file.read((char*)&noi, 4);
		//LOGV(TAG, "noi: %d", noi);

		// vertex_data
		Buffer<Geometry::Vertex> vertices(nov);
		file.read((char*)vertices.getData(), nov*sizeof(Geometry::Vertex));

		// post processing
#if 0
		// NOTE this part works fine
		glm::mat4 rot = glm::mat4_cast(glm::angleAxis(90.0f, glm::vec3(0, 1, 0)));

		for(Uint32 j=0; j<vertices.getCapacity(); j++){
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
		file.read((char*)indices.getData(), noi*sizeof(Uint32));

		model->addGeometry(name, vertices, indices);
		// setup structure
		//geo->create(vertices, indices);
	}

	// has_skeleton
	bool hasSkeleton = file.get()==1?true:false;

	if(hasSkeleton){
		readNode(file, model, NULL);
	}

	file.close();
}

void ModelLoader::postProcess(SkeletonBone* bone, const glm::mat4& transform){
	// TODO bones have to be processed as well, their offset matrices and local matrices
#if 0
	for(Uint32 i=0; i<bone->getNumChildren(false); i++){
 		postProcess(bone->getChildAt(i), transform);
	}
#endif
}

void ModelLoader::postProcess(Model* model, const glm::mat4& transform){
	for(Model::GeoList::iterator i = model->getGeometry().begin(); i!=model->getGeometry().end(); i++){

		Geometry::VertexBuffer& vertices = (*i)->getVertices();

		for(Uint32 j=0; j<vertices.getCapacity(); j++){
			Geometry::Vertex& vertex = vertices[j];

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

void ModelLoader::save(const String& path, Model* model){
	std::ofstream file(path.c_str(), std::ios::binary);

	if(!file.is_open()){
		WT_QEXCEPT("Unable to open file: \"%s\"", path.c_str());
	}

	// format_identifier
	file.write(FORMAT_ID, strlen(FORMAT_ID));

	// num_geometry
	Uint32 numGeometry = model->getGeometry().size();
	file.write((const char*)&numGeometry, 4);

	// geometry
	for(Model::GeoList::iterator i=model->getGeometry().begin(); i!=model->getGeometry().end(); i++){
		Geometry* geo = *i;

		// name_id
		file.write(geo->getName().c_str(), geo->getName().size()+1);

		// NoV
		Uint32 nov = geo->getVertices().getCapacity();
		file.write((const char*)&nov, 4); 

		// NoI
		Uint32 noi = geo->getIndices().getCapacity();
		file.write((const char*)&noi, 4);

		// vertex_data
		void* vertices = (void*)geo->getVertices().getData();
		file.write((const char*)vertices, nov*sizeof(Geometry::Vertex));
		
		// index_data
		void* indices = (void*)geo->getIndices().getData();
		file.write((const char*)indices, noi*4);
	}

	// has_skeleton 
	char hasSkeleton = model->getRootBone()==NULL?0:1;
	file.write(&hasSkeleton, 1);

	if(hasSkeleton){
		// root_node
		writeNode(file, model->getRootBone());
	}

	file.close();
}

}; // </wt>

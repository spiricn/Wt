#ifndef WT_MODELSKIN_H
#define WT_MODELSKIN_H


#include "wt/AResourceManager.h"
#include "wt/Animation.h"
#include "wt/Geometry.h"
#include "wt/SkeletonBone.h"
#include "wt/Texture2D.h"
#include "wt/Sp.h"
#include "wt/SkeletalAnimation.h"
#include "wt/Material.h"

namespace wt
{

class Model;

class ModelSkin{
public:
	struct Mesh{
		Geometry* geometry;
		Texture2D* texture;
		Texture2D* normalMap;
		Material material;
		String geometryName;

		Mesh(Geometry* geometry) : geometry(geometry), texture(NULL), normalMap(NULL), geometryName(""){
		}
	}; // </GeometryTexture>

	typedef std::vector<Mesh*> MeshList;

	ModelSkin();

	virtual ~ModelSkin();

	Mesh* findMeshByGeometry(Geometry* geometry);

	Mesh* findMeshByName(const String& name);

	const String& getName() const;

	Model* getModel();

	MeshList::iterator getMeshListBeg();

	MeshList::iterator getMeshListEnd();

	MeshList::const_iterator getMeshListCBeg() const;

	MeshList::const_iterator getMeshListCEnd() const;

private:
	void deleteMesh(Mesh* mesh);

	Mesh* createMesh(Geometry* geometry);

	ModelSkin(Model* model, const String& name);

	String mName;
	MeshList mMeshes;
	Model* mModel;

	friend class Model;
}; // </ModelSkin>

} // </wt>

#endif // </WT_MODELSKIN_H>

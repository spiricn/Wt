#ifndef WTE_WORLDOBJECT_H
#define WTE_WORLDOBJECT_H

class WorldObject{
public:
	enum Type{
		eTYPE_INVALID = -1,
		eTYPE_ACTOR = 0,
		eTYPE_POINT_LIGHT = 1
	};

	Type type;
	void* ptr;

	WorldObject(Type type, void* ptr) : type(type), ptr(ptr){
	}

}; // </WorldObject>

#endif // </WTE_WORLDOBJECT_H

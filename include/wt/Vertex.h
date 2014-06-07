/**
 * @file Vertex.h
 * @author Nikola Spiric <nikola.spiric.ns@gmail.com>
 */
 
#ifndef WT_VERTEX_H
#define WT_VERTEX_H

namespace wt
{

/* shouldn't do anything since struct size is already 4 byte aligned */
#pragma pack(push)
#pragma pack(1)

struct Vertex{
public:

	static const uint8_t kINVALID_BONE_INDEX = 0xFF;

public:
	Vertex();

	void addBone(int32_t index, float weight);

public:
	/** Position */
	float x;
	float y;
	float z;

	/** Texture coordinates */
	float s;
	float t;

	/** Normal */
	float nx;
	float ny;
	float nz;

	/** Bone IDs this vertex is affected by */
	uint8_t bones[4];

	/** Bone weights of said bones */
	float weights[4];

	/** Tangent vector (relevant if normal map is present) */
	float tx;
	float ty;
	float tz;
}; // </Vertex>

#pragma pack(pop)

} // </wt>

#endif // </WT_VERTEX_H>

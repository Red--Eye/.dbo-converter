// *************************************************************************************************
//
// dbo-converter
// [custom loading of .dbo objects, a little start on a converter] 
// --------------------------------------
// Author: Red Eye @TheGameCreators > http://forum.thegamecreators.com/?m=view_profile&i=9288d39d06d900fb85ae58b5de979ca1
//
// This software is distributed under the terms of The MIT License (MIT).
// A copy of the license may be obtained at: http://choosealicense.com/licenses/mit/
//
// *************************************************************************************************

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

typedef unsigned long DWORD;
typedef unsigned short WORD;

namespace dbObject {
	struct sHeader {
		std::string pszString;
		DWORD dwVersion;
		DWORD dwReserved1;
		DWORD dwReserved2;
	};

	struct sAnimationPos {
		DWORD time;
		float position[3];
		float interpolation[3];
	};

	struct sAnimationRot {
		DWORD time;
		float rotation[4];
	};

	struct sAnimationScale {
		DWORD time;
		float scale[3];
		float interpolation[3];
	};

	struct sAnimationMatrix {
		DWORD time;
		float matrix[4][4];
		float interpolation[4][4];
	};

	struct sAnimationData {
		std::string name;
		DWORD positionkeys;
		std::vector<sAnimationPos> kPos;
		DWORD rotationkeys;
		std::vector<sAnimationRot> kRot;
		DWORD scalekeys;
		std::vector<sAnimationScale> kScale;
		DWORD matrixkeys;
		std::vector<sAnimationMatrix> kMatrix;
	};

	struct sAnimation { 
		std::string name;
		DWORD length;
		std::vector<sAnimationData> data;
	};

	struct sVertexData {
		float x,y,z,nx,ny,nz,tu,tv;
	};

	struct sMaterial {
		float diffuse[4];
		float ambient[4];
		float specular[4];
		float emissive[4];
		float power;
	};

	struct sMultipleMaterial {
		std::string name;
		sMaterial mat;
		DWORD start;
		DWORD count;
		DWORD polygon;
	};

	struct sTexture {
		unsigned int _tSize;
		std::string name;
		DWORD stage;
		DWORD blendmode;
		DWORD argument1;
		DWORD argument2;
		DWORD AddressU;
		DWORD AddressV;
		DWORD mag;
		DWORD min;
		DWORD mip;
		DWORD TCMode;
		DWORD PrimitiveStart;
		DWORD PrimitiveCount;
	};

	struct sBoneData {
		unsigned int _tSize;
		std::string name;
		DWORD NumInfluences;
		std::vector<DWORD> VertexList;
		std::vector<float> WeightList;
		float tMatrix[4][4];
	};

	struct sMeshData {
		DWORD FVF;
		/*
		FVF_XYZ			0x002	float X, Y, Z positions
		FVF_XYZRHW		0x004	float X, Y, Z transformed
		FVF_NORMAL		0x010	float X, Y, Z normal vector
		FVF_PSIZE		0x020	float point size for sprites
		FVF_DIFFUSE		0x040	DWORD diffuse colour
		FVF_SPECULAR	0x080	DWORD specular colour
		FVF_TEX0		0x000	texture coordinates 0
		FVF_TEX1		0x100	texture coordinates 1
		FVF_TEX2		0x200	texture coordinates 2
		FVF_TEX3		0x300	texture coordinates 3
		FVF_TEX4		0x400	texture coordinates 4
		FVF_TEX5		0x500	texture coordinates 5
		FVF_TEX6		0x600	texture coordinates 6
		FVF_TEX7		0x700	texture coordinates 7
		FVF_TEX8		0x800	texture coordinates 8
		*/
		DWORD FVFSize;
		DWORD VertexCount;
		DWORD IndexCount;
		
		std::vector<sVertexData> VertexData;
		std::vector<int> IndexData;

		DWORD pType;
		/*
		Type			Value	Information
		POINTLIST		1		Point list � particles
		LINELIST		2		Line list
		LINESTRIP		3		Line strip
		TRIANGLELIST	4		Triangle list
		TRIANGLESTRIP	5		Triangle strip
		TRIANGLEFAN		6		Triangle fan
		*/
		DWORD DrawVertexCount;
		DWORD DrawPrimitiveCount;

		//Mesh Vertex Declaration

		DWORD BoneCount;
		std::vector<sBoneData> bone;
		
		bool bUseMaterial;
		sMaterial mat;

		DWORD TextureCount;
		std::vector<sTexture> texture;
		
		bool bWireframe;
		
		bool bLight;

		bool bCull;
		bool bFog;
		bool bAmbient;
		bool bTransparency;
		bool bGhost;
		int GhostMode;
		//Mesh Linked ( internal � skip this block )
		//Mesh Sub Frames ( internal � skip this block )

		std::string effectname;
		DWORD AbitaryValue;
		bool ZBiasFlag;
		DWORD ZBiasSlope;
		DWORD ZBiasDepth;
		bool ZRead;
		bool ZWrite;
		DWORD AlphaTestValue;

		bool bUseMultipleMaterials;
		DWORD MaterialCount;
		std::vector<sMultipleMaterial> mmaterial;

		bool bVisible;
	};

	struct sFrame {
		std::string name;
		float matrix[4][4];
		sMeshData *m;
		sFrame *child;
		sFrame *sibling;
		float offset[3]; //size = 3*4 = 12
		float rot[3];
		float scale[3];
	};
	
	struct object {
		unsigned int _tAnims;
		sHeader head;
		sFrame *root;
		std::vector<sAnimation> anim;

		object() {
			this->_tAnims = 0;
		}
	};
}

class CDBOConv {
public:
	CDBOConv();
	CDBOConv(std::string path);
	~CDBOConv();
	bool open(std::string path);
	bool good();

	void create();
	void clean();
	void saveto(std::string path);
private:
	std::ifstream file;
	dbObject::object *obj;
	
	void getRootFrame(); // pretty much the same as geting a frame > starting point
	void getAnimation();
	void getAnimationData();
	void getMesh(dbObject::sMeshData *m);
	void getFrame(dbObject::sFrame *frame);
	dbObject::sBoneData getBoneData();
	dbObject::sTexture getTextureData();
	dbObject::sMaterial getMaterial();
	dbObject::sMultipleMaterial getMMaterial();

	void skipData(unsigned int);
	std::string getString(unsigned int);
	float getFloat();
	DWORD getDWORD();
	WORD getWORD();
	int getInteger();
	bool getBOOL();
};

#endif
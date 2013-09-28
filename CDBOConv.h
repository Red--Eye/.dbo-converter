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

#ifndef CDBOCONV_H
#define CDBOCONV_H

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
		sMeshData() {
			IndexCount = 0;
			VertexCount = 0;
		}

		DWORD FVF;
		DWORD FVFSize;
		DWORD VertexCount;
		DWORD IndexCount;
		
		std::vector<sVertexData> VertexData;
		std::vector<int> IndexData;

		DWORD pType;
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
		//Mesh Linked ( internal – skip this block )
		//Mesh Sub Frames ( internal – skip this block )

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
		float offset[3];
		float rot[3];
		float scale[3];
		bool bGood;

		bool good() {
			if(bGood == false || (m->IndexCount == 0 && m->VertexCount == 0) ) {
				return false;
			}
			return true;
		}

		sFrame() {
			bGood = false;
			m  = NULL;
			child = sibling = NULL;
		}
	};
	
	struct object {
		unsigned int _cAnims, _cFrames;
		sHeader head;
		sFrame *root;
		sFrame *oFrame;
		std::vector<sAnimation> anim;

		object() {
			_cAnims = 0, _cFrames = 0;
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
	dbObject::object *getObj();
private:
	std::ifstream file;
	dbObject::object *obj;

	void getAnimation();
	void getAnimationData();
	void getMesh(dbObject::sMeshData *m);
	void getFrame();
	dbObject::sBoneData getBoneData();
	dbObject::sTexture getTextureData();
	dbObject::sMaterial getMaterial();
	dbObject::sMultipleMaterial getMMaterial();

	void			readDATA(unsigned int);
	std::string		readSTRING(unsigned int);
	float			readFLOAT();
	DWORD			readDWORD();
	WORD			readWORD();
	int				readINTEGER();
	bool			readBOOL();
};

//.irrmesh Output Class Declaration
namespace nsIrrMesh {
	class CIrrMesh {
	public:
		CIrrMesh(CDBOConv*, std::string);
		void addBuffer(dbObject::sFrame*);
		void setMeshVersion(std::string, std::string);
		void addEnd(std::string);
		void setBoundingBox(float[],float[]);
		bool ready();
		void init();
		unsigned int getBufferCount();
	private:
		CDBOConv* src;
		std::ofstream ofile;
	};
}

#endif
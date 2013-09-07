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

#include "CDBOConv.h"

CDBOConv::CDBOConv() {
	
}

CDBOConv::~CDBOConv() {

}

CDBOConv::CDBOConv(string path) {
	std::cout << "* opening " << path << " *" << std::endl;
	open(path);
}

void CDBOConv::create() {
	std::cout << "* loading object *" << std::endl;
	obj = new dbObject::object;

	//Header Information
	obj->head.pszString = this->getString(this->getDWORD());
	obj->head.dwVersion = this->getDWORD();
	obj->head.dwReserved1 = this->getDWORD();
	obj->head.dwReserved2 = this->getDWORD();

	std::cout << "> header info, " << "version = " << obj->head.dwVersion << std::endl;

	//Data Blocks
	std::cout << "> reading data blocks" << std::endl;
	DWORD dwCode = this->getDWORD();
	int dwCodeSize = this->getDWORD();
	while(dwCode > 0) {
		switch(dwCode) {
		case 1: {
			this->getRootFrame();
				}break;
		case 2: {
			obj->_tAnims++; obj->anim.resize(obj->_tAnims);
			this->getAnimation();
				}break;
		case 406: { // custom data (currently skipping it)
			this->skipData(dwCodeSize);
				  }break;
		case 0: {
			std::cout << "* unkown error occured, file corrupted *" << endl;
			this->skipData(dwCodeSize);
				}break;
		default: {
			std::cout << "* unkown data, skipping data *" << endl;
			this->skipData(dwCodeSize);
				 }break;
		}
		dwCode = this->getDWORD();
		dwCodeSize = this->getDWORD();
	}
}

void CDBOConv::getAnimationData() {
	std::cout << "  > reading one animation data" << endl;
	DWORD dwCode = this->getDWORD();
	int dwCodeSize = this->getDWORD();
	if(dwCode == 0) { this->skipData(dwCodeSize); }
	dbObject::sAnimationData _data;
	while(dwCode > 0) {
		switch(dwCode) {
		case 211: {
			_data.name = this->getString(this->getDWORD());
				  }break;
		case 212: {
			_data.positionkeys = this->getDWORD();
				  }break;
		case 213: {
			for(unsigned int p=0; p<_data.positionkeys; p++) {
				dbObject::sAnimationPos pos;
				pos.time = this->getDWORD();
				pos.position[0] = this->getFloat();
				pos.position[1] = this->getFloat();
				pos.position[2] = this->getFloat();
				pos.interpolation[0] = this->getFloat();
				pos.interpolation[1] = this->getFloat();
				pos.interpolation[2] = this->getFloat();
				_data.kPos.push_back(pos);
			}
				  }break;
		case 214: {
			_data.rotationkeys = this->getDWORD();
				  }break;
		case 215: {
			for(unsigned int r=0; r<_data.rotationkeys; r++) {
				dbObject::sAnimationRot rot;
				rot.time = this->getDWORD();
				rot.rotation[0] = this->getFloat();
				rot.rotation[1] = this->getFloat();
				rot.rotation[2] = this->getFloat();
				_data.kRot.push_back(rot);
			}
				  }break;
		case 216:{
			_data.scalekeys = this->getDWORD();
				 }break;
		case 217: {
			for(unsigned int s=0; s<_data.scalekeys; s++) {
				dbObject::sAnimationScale scale;
				scale.time = this->getDWORD();
				scale.scale[0] = this->getFloat();
				scale.scale[1] = this->getFloat();
				scale.scale[2] = this->getFloat();
				_data.kScale.push_back(scale);
			}
				  }break;
		case 218: {
			_data.matrixkeys = this->getDWORD();
				  }break;
		case 219: {
			for(unsigned int m=0; m<_data.matrixkeys; m++) {
				dbObject::sAnimationMatrix matrix;
				matrix.time = this->getDWORD();
				for(int x=0; x<4; x++) {
					for(int y=0; y<4; y++) {
						matrix.matrix[x][y] = this->getFloat();
					}
				}
				for(int x=0; x<4; x++) {
					for(int y=0; y<4; y++) {
						matrix.interpolation[x][y] = this->getFloat();
					}
				}
				_data.kMatrix.push_back(matrix);
			}
				  }break;
		case 220: {
			obj->anim[obj->_tAnims-1].data.push_back(_data);
			this->getAnimationData();
				  }break;
		default: {
			this->skipData(dwCodeSize);
				 }break;
		}
		dwCode = this->getDWORD();
		dwCodeSize = this->getDWORD();
	}
}

void CDBOConv::getAnimation() {
	std::cout << " > reading animation " << endl;
	DWORD dwCode = this->getDWORD();
	int dwCodeSize = this->getDWORD();
	if(dwCode == 0) { this->skipData(dwCodeSize); }

	while(dwCode > 0) {
		switch(dwCode) {
		case 201: {
			obj->anim[obj->_tAnims-1].name = this->getString(this->getDWORD());
				  }break;
		case 202: {
			obj->anim[obj->_tAnims-1].length = this->getDWORD();
				  }break;
		case 203: {
			this->getAnimationData();
				  }break;
		}
		dwCode = this->getDWORD();
		dwCodeSize = this->getDWORD();
	}
}

dbObject::sBoneData CDBOConv::getBoneData() {
	std::cout << "   > reading a bone" <<  endl;
	dbObject::sBoneData bone; bone._tSize = 8;
	DWORD dwCode = this->getDWORD();
	int dwCodeSize = this->getDWORD();
	while(dwCode > 0) {
		switch(dwCode) {
		case 301:{
			bone.name = this->getString(this->getDWORD());
				 }break;
		case 302:{
			bone.NumInfluences = this->getDWORD();
				 }break;
		case 303:{
			for(unsigned int n=0; n<bone.NumInfluences; n++) {
				bone.VertexList.push_back(this->getDWORD());
			}
				 }break;
		case 304: {
			for(unsigned int n=0; n<bone.NumInfluences; n++) {
				bone.WeightList.push_back(this->getFloat());
			}
				  }break;
		case 305: {
			for(int x=0; x<4; x++) {
				for(int y=0; y<4; y++) {
					bone.tMatrix[x][y] = (float)this->getDWORD();
				}
			}
				  }break;
		}
		bone._tSize += dwCodeSize + 8;
		dwCode = this->getDWORD();
		dwCodeSize = this->getDWORD();
	}
	return bone;
}

dbObject::sTexture CDBOConv::getTextureData() {
	std::cout << "   > reading a texture" <<  endl;
	dbObject::sTexture tex; tex._tSize = 8;
	DWORD dwCode = this->getDWORD();
	int dwCodeSize = this->getDWORD();
	std::cout << "     {" << dwCode << ",";
	while(dwCode > 0) {
		switch(dwCode) {
			case 141: {	tex.name = this->getString(this->getDWORD());  }break;
			case 142: {	tex.stage = this->getDWORD();  }break;
			case 143: {	tex.blendmode = this->getDWORD();  }break;
			case 144: {	tex.argument1 = this->getDWORD();  }break;
			case 145: {	tex.argument2 = this->getDWORD();  }break;
			case 146: {	tex.AddressU = this->getDWORD();  }break;
			case 147: {	tex.AddressV = this->getDWORD();  }break;
			case 148: {	tex.mag = this->getDWORD();  }break;
			case 149: {	tex.min = this->getDWORD();  }break;
			case 150: {	tex.mip = this->getDWORD();  }break;
			case 151: {	tex.TCMode = this->getDWORD();  }break;
			case 152: {	tex.PrimitiveStart = this->getDWORD();  }break;
			case 153: {	tex.PrimitiveCount = this->getDWORD();  }break;
			default: { 
				//skip data (dwCodeSize)
				this->skipData(dwCodeSize);
					 }break;
		}
		std::cout << dwCode << ",";
		tex._tSize = tex._tSize + dwCodeSize + 8;
		dwCode = this->getDWORD();
		dwCodeSize = this->getDWORD();
	}
	std::cout << "_tSize:" << tex._tSize << "}" << std::endl;
	return tex;
}

dbObject::sMaterial CDBOConv::getMaterial() {
	std::cout << "   > reading a material" <<  endl;
	dbObject::sMaterial mat;
	for(int d=0; d<4; d++) {mat.diffuse[d] = this->getFloat();}
	for(int a=0; a<4; a++) {mat.ambient[a] = this->getFloat();}
	for(int s=0; s<4; s++) {mat.specular[s] = this->getFloat();}
	for(int e=0; e<4; e++) {mat.emissive[e] = this->getFloat();}
	return mat;
}

dbObject::sMultipleMaterial CDBOConv::getMMaterial() {
	std::cout << "  > reading a multiple material" <<  endl;
	dbObject::sMultipleMaterial mat;
	DWORD dwCode = this->getDWORD();
	int dwCodeSize = this->getDWORD();
	while(dwCode > 0) {
		switch(dwCode) {
		case 161: {
			mat.name = this->getString(this->getDWORD());
				  };
		case 162: {
			mat.mat = this->getMaterial();
			this->skipData(dwCodeSize-64);
				  };
		case 163: {
			mat.start = this->getDWORD();
				  };
		case 164: {
			mat.count = this->getDWORD();
				  };
		case 165: {
			mat.polygon = this->getDWORD();
				  };
		default: {
			this->skipData(dwCodeSize);
				 }break;
		}
		dwCode = this->getDWORD();
		dwCodeSize = this->getDWORD();
	}
	return mat;
}

void CDBOConv::getMesh(dbObject::sMeshData *m) {
	std::cout << "  > reading a mesh" <<  endl;
	DWORD dwCode = this->getDWORD();
	int dwCodeSize = this->getDWORD();
	while(dwCode > 0) {
		switch(dwCode) {
			case 111: { // mesh fvf
				m->FVF = this->getDWORD();
					  }break;
			case 112: {
				m->FVFSize = this->getDWORD();
					  }break;
			case 113: {
				m->VertexCount = this->getDWORD();
					  }break;
			case 114: {
				m->IndexCount = this->getDWORD();
					  }break;
			case 115: {
				for(unsigned int i=0; i<m->VertexCount; i++) {
					dbObject::sVertexData data;
					data.x = this->getFloat();
					data.y = this->getFloat();
					data.z = this->getFloat();
					
					data.nx = this->getFloat();
					data.ny = this->getFloat();
					data.nz = this->getFloat();
					
					data.tu = this->getFloat();
					data.tv = this->getFloat();
					m->VertexData.push_back(data);
				}
				this->skipData(dwCodeSize - m->VertexCount*32);
					  }break;
			case 116: {
				for(unsigned int i=0; i<m->IndexCount; i++) {
					m->IndexData.push_back(this->getWORD());
				}
					  }break;
			case 117: {
				m->pType = this->getDWORD();
					  }break;
			case 118: {
				m->DrawVertexCount = this->getDWORD();
					  }break;
			case 119: {
				m->DrawPrimitiveCount = this->getDWORD();
					  }break;
			case 120: {
				this->skipData(dwCodeSize);
					  }break;
			case 121: {
				m->BoneCount = this->getDWORD();
					  }break;
			case 122: { // gotta keep track of size read in to delete error
				unsigned int _tSize = 0;
				for(unsigned int b=0; b<m->BoneCount; b++) {
					dbObject::sBoneData bone = this->getBoneData();
					m->bone.push_back(bone);
					_tSize += bone._tSize;
				}

				if(m->BoneCount == 0) {
					this->skipData(dwCodeSize);
				} else {
					this->skipData(dwCodeSize-_tSize);
				}
					  }break;
			case 125: {
				m->bUseMaterial = this->getBOOL();
					  }break;
			case 126: {
				if(m->bUseMaterial) {
					m->mat = this->getMaterial();
					this->skipData(dwCodeSize-(4*4*4));
				} else {
					this->skipData(dwCodeSize);
				}
					  }break; 
			case 127: {
				m->TextureCount = this->getDWORD();
					  }break;
			case 128: {
				unsigned int _tSize = 0;
				for(unsigned int t=0; t<m->TextureCount; t++) {
					dbObject::sTexture tex = this->getTextureData();
					m->texture.push_back(tex);
					_tSize += tex._tSize;
				}
				if(m->TextureCount == 0) { this->skipData(dwCodeSize); } else { 
					this->skipData(dwCodeSize-_tSize);
				}
					  }break;
			case 140: {
				m->bVisible = this->getBOOL();
					  }break;
			case 129: {
				m->bWireframe = this->getBOOL();
					  }break;
			case 130: {
				m->bLight = this->getBOOL();
					  }break;
			case 131: {
				m->bCull = this->getBOOL();
					  }break;
			case 132: {
				m->bFog = this->getBOOL();
					  }break;
			case 133: {
				m->bAmbient = this->getBOOL();
					  }break;
			case 134: {
				m->bTransparency = this->getBOOL();
					  }break;
			case 135: {
				m->bGhost = this->getBOOL();
					  }break;
			case 136: {
				m->GhostMode = this->getDWORD();
					  }break;
			case 123: {
				m->bUseMultipleMaterials = this->getBOOL();
					  }break;
			case 124: {
				m->MaterialCount = this->getDWORD();
					  }break;
			case 139: {
				for(unsigned int _m=0; _m<m->MaterialCount; _m++) {
					m->mmaterial.push_back(this->getMMaterial());
				}
				if(m->MaterialCount == 0) {
					this->skipData(dwCodeSize);
				}
					  }break;
			case 154: {
				m->effectname = this->getString(this->getDWORD());
					  }break;
			case 155: {
				this->skipData(dwCodeSize);
					  }break;
			case 156: {
				this->skipData(dwCodeSize);
					  }break;
			case 157: {
				this->skipData(dwCodeSize);
					  }break;
			case 158: {
				this->skipData(dwCodeSize);
					  }break;
			case 159: {
				this->skipData(dwCodeSize);
					  }break;
			case 160: {
				this->skipData(dwCodeSize);
					  }break;
			case 166: {
				this->skipData(dwCodeSize);
					  }break;
			default: { 
				//skip data (dwCodeSize)
				this->skipData(dwCodeSize);
					 }break;
		}
		dwCode = this->getDWORD();
		dwCodeSize = this->getDWORD();
	}
}

void CDBOConv::getFrame(dbObject::sFrame *frame) {
	std::cout << "  > reading a frame" <<  endl;
	int dwCode = this->getDWORD();
	int dwCodeSize = this->getDWORD();
	while( dwCode > 0 ) {
		switch(dwCode) {
		case 101: { //frame name
			frame->name = this->getString(this->getDWORD());
				  }break;
		case 102: { //frame matrix
			for(int x=0; x<4; x++) {
				for(int y=0; y<4; y++) {
					frame->matrix[x][y] = (float)this->getDWORD();
				}
			}
				  }break;
		case 103: {
			frame->m = new dbObject::sMeshData();
			this->getMesh(frame->m);
				  }break;
		case 104: {
			std::cout << " ";
			frame->child = new dbObject::sFrame();
			this->getFrame(frame->child);
				  }break;
		case 105: {
			std::cout << " ";
			frame->sibling= new dbObject::sFrame();
			this->getFrame(frame->sibling);
				  }break;
		case 106: {
			frame->offset[0] = this->getFloat();
			frame->offset[1] = this->getFloat();
			frame->offset[2] = this->getFloat();
				  }break;
		case 107: {
			frame->rot[0] = this->getFloat();
			frame->rot[1] = this->getFloat();
			frame->rot[2] = this->getFloat();
				  }break;
		case 108: {
			frame->scale[0] = this->getFloat();
			frame->scale[1] = this->getFloat();
			frame->scale[2] = this->getFloat();
				  }break;
		default: {
			//skip data
			this->skipData(dwCodeSize);
				 }break;
		}

		dwCode = this->getDWORD();
		dwCodeSize = this->getDWORD();
	}
}

void CDBOConv::getRootFrame() {
	std::cout << " > reading root " << endl;
	int dwCode = this->getDWORD();
	int dwCodeSize = this->getDWORD();
	obj->root = new dbObject::sFrame();
	while(dwCode > 0) {
		switch(dwCode) {
		case 101: { //frame name
			obj->root->name = this->getString(this->getDWORD());
				  }break;
		case 102: { //frame matrix
			for(int x=0; x<4; x++) {
				for(int y=0; y<4; y++) {
					obj->root->matrix[x][y] = (float)this->getDWORD();
				}
			}
				  }break;
		case 103: {
			obj->root->m = new dbObject::sMeshData();
			this->getMesh(obj->root->m);
				  }break;
		case 104: {
			obj->root->child = new dbObject::sFrame();
			this->getFrame(obj->root->child);
				  }break;
		case 105: {
			obj->root->sibling= new dbObject::sFrame();
			this->getFrame(obj->root->sibling);
				  }break;
		case 106: {
			obj->root->offset[0] = this->getFloat();
			obj->root->offset[1] = this->getFloat();
			obj->root->offset[2] = this->getFloat();
				  }break;
		case 107: {
			obj->root->rot[0] = this->getFloat();
			obj->root->rot[1] = this->getFloat();
			obj->root->rot[2] = this->getFloat();
				  }break;
		case 108: {
			obj->root->scale[0] = this->getFloat();
			obj->root->scale[1] = this->getFloat();
			obj->root->scale[2] = this->getFloat();
				  }break;
		default: {
			//skip data
			this->skipData(dwCodeSize);
				 }break;
		}

		dwCode = this->getDWORD();
		dwCodeSize = this->getDWORD();
	}
}

void CDBOConv::clean() {
	/* this ain't cleaning, can't be bothered */
	delete obj->root;
	delete obj;
	file.close();
	std::cout << "> memory cleared" << std::endl << "  > file closed " << std::endl << "* the end *" << std::endl; 
}

bool CDBOConv::open(std::string path) {
	file.open((char*)path.c_str(), ios::in|ios::binary);

	if( file.is_open() == true ) {
		return true;
	} else { return false; }
}

bool CDBOConv::good() {
	return file.is_open();
}

string CDBOConv::getString(unsigned int size) {
	string buffer;
	for(unsigned int n = 0; n < size; n++) {
		char c[128];
		file.read(c, 1);
		if(string(c).size() > 0) {
			buffer.push_back(string(c).substr(0,1).at(0));
		} else {
			buffer.push_back((char)" ");
		}
	}
	return string(buffer).substr(0,size);
}

void CDBOConv::skipData(unsigned int size) {
	for(unsigned int n = 0; n < size; n++) {
		char c[128];
		file.read(c, 1);
	}
}

float CDBOConv::getFloat() {
	float temp = 0.0f;
	file.read(reinterpret_cast<char*>(&temp),sizeof(temp));
	return temp;
}
 
int CDBOConv::getInteger() {
	int temp = 0;
	file.read(reinterpret_cast<char*>(&temp),sizeof(temp));
	return temp;
}

DWORD CDBOConv::getDWORD() {
	DWORD temp = NULL;
	file.read(reinterpret_cast<char*>(&temp),sizeof(DWORD));
	return temp;
}

WORD CDBOConv::getWORD() {
	WORD temp = NULL;
	file.read(reinterpret_cast<char*>(&temp),sizeof(WORD));
	return temp;
}

bool CDBOConv::getBOOL() {
	bool temp = false;
	file.read(reinterpret_cast<char*>(&temp),sizeof(bool));
	return temp;
}
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

CDBOConv::~CDBOConv() {

}

CDBOConv::CDBOConv(std::string path) {
	std::cout << "* opening " << path << " *" << std::endl;
	open(path);
}

void CDBOConv::create() {
	std::cout << "* loading object *" << std::endl;

	this->obj = new dbObject::object();

	//Header Information
	this->obj->head.pszString = this->readSTRING(this->readDWORD());
	this->obj->head.dwVersion = this->readDWORD();
	this->obj->head.dwReserved1 = this->readDWORD();
	this->obj->head.dwReserved2 = this->readDWORD();

	std::cout << "> header info, " << "version = " << this->obj->head.dwVersion << std::endl;

	//Data Blocks
	std::cout << "> reading data blocks" << std::endl;
	DWORD dwCode = this->readDWORD();
	int dwCodeSize = this->readDWORD();
	while(dwCode > 0) {
		switch(dwCode) {
		case 1: { //root
			this->obj->root = new dbObject::sFrame();
			this->obj->oFrame = this->obj->root;
			this->getFrame();
				}break;
		case 2: {
			this->obj->_cAnims++; this->obj->anim.resize(this->obj->_cAnims);
			this->getAnimation();
				}break;
		case 406: {
			this->readDATA(dwCodeSize);
				  }break;
		case 0: {
			std::cout << "* unkown error occured, file corrupted *" << std::endl;
			this->readDATA(dwCodeSize);
				}break;
		default: {
			std::cout << "* unkown data, skipping data *" << std::endl;
			this->readDATA(dwCodeSize);
				 }break;
		}
		dwCode = this->readDWORD();
		dwCodeSize = this->readDWORD();
	}

	std::cout << " > extra info: anim count: " << this->obj->anim.size() << " frame count: " << this->obj->_cFrames << std::endl;
}

void CDBOConv::getAnimationData() {
	std::cout << "  > reading one animation data" << std::endl;
	DWORD dwCode = this->readDWORD();
	int dwCodeSize = this->readDWORD();
	if(dwCode == 0) { this->readDATA(dwCodeSize); }
	dbObject::sAnimationData _data;
	while(dwCode > 0) {
		switch(dwCode) {
		case 211: {
			_data.name = this->readSTRING(this->readDWORD());
				  }break;
		case 212: {
			_data.positionkeys = this->readDWORD();
				  }break;
		case 213: {
			for(unsigned int p=0; p<_data.positionkeys; p++) {
				dbObject::sAnimationPos pos;
				pos.time = this->readDWORD();
				pos.position[0] = this->readFLOAT();
				pos.position[1] = this->readFLOAT();
				pos.position[2] = this->readFLOAT();
				pos.interpolation[0] = this->readFLOAT();
				pos.interpolation[1] = this->readFLOAT();
				pos.interpolation[2] = this->readFLOAT();
				_data.kPos.push_back(pos);
			}
				  }break;
		case 214: {
			_data.rotationkeys = this->readDWORD();
				  }break;
		case 215: {
			for(unsigned int r=0; r<_data.rotationkeys; r++) {
				dbObject::sAnimationRot rot;
				rot.time = this->readDWORD();
				rot.rotation[0] = this->readFLOAT();
				rot.rotation[1] = this->readFLOAT();
				rot.rotation[2] = this->readFLOAT();
				_data.kRot.push_back(rot);
			}
				  }break;
		case 216:{
			_data.scalekeys = this->readDWORD();
				 }break;
		case 217: {
			for(unsigned int s=0; s<_data.scalekeys; s++) {
				dbObject::sAnimationScale scale;
				scale.time = this->readDWORD();
				scale.scale[0] = this->readFLOAT();
				scale.scale[1] = this->readFLOAT();
				scale.scale[2] = this->readFLOAT();
				_data.kScale.push_back(scale);
			}
				  }break;
		case 218: {
			_data.matrixkeys = this->readDWORD();
				  }break;
		case 219: {
			for(unsigned int m=0; m<_data.matrixkeys; m++) {
				dbObject::sAnimationMatrix matrix;
				matrix.time = this->readDWORD();
				for(int x=0; x<4; x++) {
					for(int y=0; y<4; y++) {
						matrix.matrix[x][y] = this->readFLOAT();
					}
				}
				for(int x=0; x<4; x++) {
					for(int y=0; y<4; y++) {
						matrix.interpolation[x][y] = this->readFLOAT();
					}
				}
				_data.kMatrix.push_back(matrix);
			}
				  }break;
		case 220: {
			this->obj->anim[this->obj->_cAnims-1].data.push_back(_data);
			this->getAnimationData();
				  }break;
		default: {
			this->readDATA(dwCodeSize);
				 }break;
		}
		dwCode = this->readDWORD();
		dwCodeSize = this->readDWORD();
	}
}

void CDBOConv::getAnimation() {
	std::cout << " > reading animation " << std::endl;
	DWORD dwCode = this->readDWORD();
	int dwCodeSize = this->readDWORD();
	if(dwCode == 0) { this->readDATA(dwCodeSize); }

	while(dwCode > 0) {
		switch(dwCode) {
		case 201: {
			this->obj->anim[this->obj->_cAnims-1].name = this->readSTRING(this->readDWORD());
				  }break;
		case 202: {
			this->obj->anim[this->obj->_cAnims-1].length = this->readDWORD();
				  }break;
		case 203: {
			this->getAnimationData();
				  }break;
		}
		dwCode = this->readDWORD();
		dwCodeSize = this->readDWORD();
	}
}

dbObject::sBoneData CDBOConv::getBoneData() {
	std::cout << "   > reading a bone" <<  std::endl;
	dbObject::sBoneData bone; bone._tSize = 8;
	DWORD dwCode = this->readDWORD();
	int dwCodeSize = this->readDWORD();
	while(dwCode > 0) {
		switch(dwCode) {
		case 301:{
			bone.name = this->readSTRING(this->readDWORD());
				 }break;
		case 302:{
			bone.NumInfluences = this->readDWORD();
				 }break;
		case 303:{
			for(unsigned int n=0; n<bone.NumInfluences; n++) {
				bone.VertexList.push_back(this->readDWORD());
			}
				 }break;
		case 304: {
			for(unsigned int n=0; n<bone.NumInfluences; n++) {
				bone.WeightList.push_back(this->readFLOAT());
			}
				  }break;
		case 305: {
			for(int x=0; x<4; x++) {
				for(int y=0; y<4; y++) {
					bone.tMatrix[x][y] = (float)this->readDWORD();
				}
			}
				  }break;
		}
		bone._tSize += dwCodeSize + 8;
		dwCode = this->readDWORD();
		dwCodeSize = this->readDWORD();
	}
	return bone;
}

dbObject::sTexture CDBOConv::getTextureData() {
	std::cout << "   > reading a texture" <<  std::endl;
	dbObject::sTexture tex; tex._tSize = 8;
	DWORD dwCode = this->readDWORD();
	int dwCodeSize = this->readDWORD();
	std::cout << "     {" << dwCode << ",";
	while(dwCode > 0) {
		switch(dwCode) {
			case 141: {	tex.name = this->readSTRING(this->readDWORD());  }break;
			case 142: {	tex.stage = this->readDWORD();  }break;
			case 143: {	tex.blendmode = this->readDWORD();  }break;
			case 144: {	tex.argument1 = this->readDWORD();  }break;
			case 145: {	tex.argument2 = this->readDWORD();  }break;
			case 146: {	tex.AddressU = this->readDWORD();  }break;
			case 147: {	tex.AddressV = this->readDWORD();  }break;
			case 148: {	tex.mag = this->readDWORD();  }break;
			case 149: {	tex.min = this->readDWORD();  }break;
			case 150: {	tex.mip = this->readDWORD();  }break;
			case 151: {	tex.TCMode = this->readDWORD();  }break;
			case 152: {	tex.PrimitiveStart = this->readDWORD();  }break;
			case 153: {	tex.PrimitiveCount = this->readDWORD();  }break;
			default: {
				this->readDATA(dwCodeSize);
					 }break;
		}
		std::cout << dwCode << ",";
		tex._tSize = tex._tSize + dwCodeSize + 8;
		dwCode = this->readDWORD();
		dwCodeSize = this->readDWORD();
	}
	std::cout << "_tSize:" << tex._tSize << "}" << std::endl;
	return tex;
}

dbObject::sMaterial CDBOConv::getMaterial() {
	dbObject::sMaterial mat;
	std::cout << "   > reading a material" <<  std::endl;
	for(int d=0; d<4; d++) { mat.diffuse[d] = this->readFLOAT(); }
	for(int a=0; a<4; a++) { mat.ambient[a] = this->readFLOAT(); }
	for(int s=0; s<4; s++) { mat.specular[s] = this->readFLOAT(); }
	for(int e=0; e<4; e++) { mat.emissive[e] = this->readFLOAT(); }
	return mat;
}

dbObject::sMultipleMaterial CDBOConv::getMMaterial() {
	std::cout << "  > reading a multiple material" <<  std::endl;
	dbObject::sMultipleMaterial mat;
	DWORD dwCode = this->readDWORD();
	int dwCodeSize = this->readDWORD();
	while(dwCode > 0) {
		switch(dwCode) {
		case 161: {
			mat.name = this->readSTRING(this->readDWORD());
				  };
		case 162: {
			mat.mat = this->getMaterial();
			this->readDATA(dwCodeSize-64);
				  };
		case 163: {
			mat.start = this->readDWORD();
				  };
		case 164: {
			mat.count = this->readDWORD();
				  };
		case 165: {
			mat.polygon = this->readDWORD();
				  };
		default: {
			this->readDATA(dwCodeSize);
				 }break;
		}
		dwCode = this->readDWORD();
		dwCodeSize = this->readDWORD();
	}
	return mat;
}

void CDBOConv::getMesh(dbObject::sMeshData *m) {
	std::cout << "  > reading a mesh" <<  std::endl;
	DWORD dwCode = this->readDWORD();
	int dwCodeSize = this->readDWORD();
	while(dwCode > 0) {
		switch(dwCode) {
			case 111: {
				m->FVF = this->readDWORD();
					  }break;
			case 112: {
				m->FVFSize = this->readDWORD();
					  }break;
			case 113: {
				m->VertexCount = this->readDWORD();
					  }break;
			case 114: {
				m->IndexCount = this->readDWORD();
					  }break;
			case 115: {
				for(unsigned int i=0; i<m->VertexCount; i++) {
					dbObject::sVertexData data;
					data.x = this->readFLOAT();
					data.y = this->readFLOAT();
					data.z = this->readFLOAT();
					
					data.nx = this->readFLOAT();
					data.ny = this->readFLOAT();
					data.nz = this->readFLOAT();
					
					data.tu = this->readFLOAT();
					data.tv = this->readFLOAT();
					m->VertexData.push_back(data);
				}
				this->readDATA(dwCodeSize - m->VertexCount*32);
					  }break;
			case 116: {
				for(unsigned int i=0; i<m->IndexCount; i++) {
					m->IndexData.push_back(this->readWORD());
				}
					  }break;
			case 117: {
				m->pType = this->readDWORD();
					  }break;
			case 118: {
				m->DrawVertexCount = this->readDWORD();
					  }break;
			case 119: {
				m->DrawPrimitiveCount = this->readDWORD();
					  }break;
			case 120: {
				this->readDATA(dwCodeSize);
					  }break;
			case 121: {
				m->BoneCount = this->readDWORD();
					  }break;
			case 122: {
				unsigned int _tSize = 0;
				for(unsigned int b=0; b<m->BoneCount; b++) {
					dbObject::sBoneData bone = this->getBoneData();
					m->bone.push_back(bone);
					_tSize += bone._tSize;
				}

				if(m->BoneCount == 0) {
					this->readDATA(dwCodeSize);
				} else {
					this->readDATA(dwCodeSize-_tSize);
				}
					  }break;
			case 125: {
				m->bUseMaterial = this->readBOOL();
					  }break;
			case 126: {
				if(m->bUseMaterial) {
					m->mat = this->getMaterial();
					this->readDATA(dwCodeSize-(4*4*4));
				} else {
					this->readDATA(dwCodeSize);
				}
					  }break; 
			case 127: {
				m->TextureCount = this->readDWORD();
					  }break;
			case 128: {
				unsigned int _tSize = 0;
				for(unsigned int t=0; t<m->TextureCount; t++) {
					dbObject::sTexture tex = this->getTextureData();
					m->texture.push_back(tex);
					_tSize += tex._tSize;
				}
				if(m->TextureCount == 0) { this->readDATA(dwCodeSize); } else { 
					this->readDATA(dwCodeSize-_tSize);
				}
					  }break;
			case 140: {
				m->bVisible = this->readBOOL();
					  }break;
			case 129: {
				m->bWireframe = this->readBOOL();
					  }break;
			case 130: {
				m->bLight = this->readBOOL();
					  }break;
			case 131: {
				m->bCull = this->readBOOL();
					  }break;
			case 132: {
				m->bFog = this->readBOOL();
					  }break;
			case 133: {
				m->bAmbient = this->readBOOL();
					  }break;
			case 134: {
				m->bTransparency = this->readBOOL();
					  }break;
			case 135: {
				m->bGhost = this->readBOOL();
					  }break;
			case 136: {
				m->GhostMode = this->readDWORD();
					  }break;
			case 123: {
				m->bUseMultipleMaterials = this->readBOOL();
					  }break;
			case 124: {
				m->MaterialCount = this->readDWORD();
					  }break;
			case 139: {
				for(unsigned int _m=0; _m<m->MaterialCount; _m++) {
					m->mmaterial.push_back(this->getMMaterial());
				}
				if(m->MaterialCount == 0) {
					this->readDATA(dwCodeSize);
				}
					  }break;
			case 154: {
				m->effectname = this->readSTRING(this->readDWORD());
					  }break;
			case 155: {
				this->readDATA(dwCodeSize);
					  }break;
			case 156: {
				this->readDATA(dwCodeSize);
					  }break;
			case 157: {
				this->readDATA(dwCodeSize);
					  }break;
			case 158: {
				this->readDATA(dwCodeSize);
					  }break;
			case 159: {
				this->readDATA(dwCodeSize);
					  }break;
			case 160: {
				this->readDATA(dwCodeSize);
					  }break;
			case 166: {
				this->readDATA(dwCodeSize);
					  }break;
			default: {
				this->readDATA(dwCodeSize);
					 }break;
		}
		dwCode = this->readDWORD();
		dwCodeSize = this->readDWORD();
	}
}

void CDBOConv::getFrame() {
	this->obj->_cFrames++;
	std::cout << "  > reading a frame" <<  std::endl;
	int dwCode = this->readDWORD();
	int dwCodeSize = this->readDWORD();
	while( dwCode > 0 ) {
		switch(dwCode) {
		case 101: {
			obj->oFrame->bGood = true;
			obj->oFrame->name = this->readSTRING(this->readDWORD());
				  }break;
		case 102: {
			for(int x=0; x<4; x++) {
				for(int y=0; y<4; y++) {
					obj->oFrame->matrix[x][y] = (float)this->readDWORD();
				}
			}
				  }break;
		case 103: {
			obj->oFrame->m = new dbObject::sMeshData();
			this->getMesh(obj->oFrame->m);
				  }break;
		case 104: {
			obj->oFrame->child = new dbObject::sFrame();
			obj->oFrame = obj->oFrame->child;
			this->getFrame();
				  }break;
		case 105: {
			obj->oFrame->sibling= new dbObject::sFrame();
			obj->oFrame = obj->oFrame->sibling;
			this->getFrame();
				  }break;
		case 106: {
			obj->oFrame->offset[0] = this->readFLOAT();
			obj->oFrame->offset[1] = this->readFLOAT();
			obj->oFrame->offset[2] = this->readFLOAT();
				  }break;
		case 107: {
			obj->oFrame->rot[0] = this->readFLOAT();
			obj->oFrame->rot[1] = this->readFLOAT();
			obj->oFrame->rot[2] = this->readFLOAT();
				  }break;
		case 108: {
			obj->oFrame->scale[0] = this->readFLOAT();
			obj->oFrame->scale[1] = this->readFLOAT();
			obj->oFrame->scale[2] = this->readFLOAT();
				  }break;
		default: {
			this->readDATA(dwCodeSize);
				 }break;
		}

		dwCode = this->readDWORD();
		dwCodeSize = this->readDWORD();
	}
}

void CDBOConv::clean() {
	delete obj;
	file.close();
	std::cout << "> memory cleared" << std::endl << "  > file closed " << std::endl << "* the end *" << std::endl; 
}

bool CDBOConv::open(std::string path) {
	file.open((char*)path.c_str(), std::ios::in|std::ios::binary);
	return file.is_open();
}

bool CDBOConv::good() {
	return file.is_open();
}

void CDBOConv::saveto(std::string path) {
	std::string oformat = path.substr(path.find_last_of("."),path.size()-path.find_last_of("."));
	bool _known = false;
	
	if(oformat == ".irrmesh") {
		_known = true;

		//Create & Convert
		nsIrrMesh::CIrrMesh *irrmesh = new nsIrrMesh::CIrrMesh(this, path);
		irrmesh->init();
		std::cout << " > saving to output format: " << oformat << std::endl;
		if(irrmesh->ready()) {
			irrmesh->setMeshVersion("http://irrlicht.sourceforge.net/IRRMESH_09_2007","1.0");
			
			std::cout << "  > found " << irrmesh->getBufferCount() << " buffers" << std::endl;
			irrmesh->addBuffer(this->obj->root);

			irrmesh->addEnd("mesh");
		} else {
			std::cout << " > could not open <" << path << "> to write " << std::endl;
		}
	}

	if(_known == false) {
		std::cout << " > saving: " << oformat << " is unknown" << std::endl;
	} else {
		std::cout << " > saving to <" << path << "> is done" << std::endl;
	}
}

dbObject::object *CDBOConv::getObj() {
	return obj;
}

std::string CDBOConv::readSTRING(unsigned int size) {
	std::string buffer;
	for(unsigned int n = 0; n < size; n++) {
		char c[1];
		file.read(c, 1);
		if(std::string(c).size() > 0) {
			buffer.push_back(std::string(c).substr(0,1).at(0));
		} else {
			buffer.push_back((char)" ");
		}
	}
	return std::string(buffer).substr(0,size);
}

void CDBOConv::readDATA(unsigned int size) {
	for(unsigned int n = 0; n < size; n++) {
		char c[1];
		file.read(c, 1);
	}
}

float CDBOConv::readFLOAT() {
	float temp = 0.0f;
	file.read(reinterpret_cast<char*>(&temp),sizeof(temp));
	return temp;
}
 
int CDBOConv::readINTEGER() {
	int temp = 0;
	file.read(reinterpret_cast<char*>(&temp),sizeof(temp));
	return temp;
}

DWORD CDBOConv::readDWORD() {
	DWORD temp = NULL;
	file.read(reinterpret_cast<char*>(&temp),sizeof(DWORD));
	return temp;
}

WORD CDBOConv::readWORD() {
	WORD temp = NULL;
	file.read(reinterpret_cast<char*>(&temp),sizeof(WORD));
	return temp;
}

bool CDBOConv::readBOOL() {
	bool temp = false;
	file.read(reinterpret_cast<char*>(&temp),sizeof(bool));
	return temp;
}

//.irrmesh Output Class Definition
nsIrrMesh::CIrrMesh::CIrrMesh(CDBOConv *that, std::string path) {
	src = that;
	ofile.open(path);
}

bool nsIrrMesh::CIrrMesh::ready() {
	return (ofile.good() && src != NULL);
}

unsigned int nsIrrMesh::CIrrMesh::getBufferCount() {
	return this->src->getObj()->_cFrames;
}

void nsIrrMesh::CIrrMesh::setMeshVersion(std::string _namespace, std::string version) {
	ofile << "<mesh xmlns=\"" << _namespace << "\" version=\"" << version << "\">" << std::endl;
}

void nsIrrMesh::CIrrMesh::setBoundingBox(float min[3],float max[3]) {
	ofile << "<boundingBox minEdge=\"" << min[0] << min[1] << min[2] << "\" maxEdge=\"" << max[0] << max[1] << max[2] << "/>" << std::endl;
}

void nsIrrMesh::CIrrMesh::init() {
	ofile.precision(5); 
	ofile << std::fixed;
	ofile << "<?xml version=\"1.0\"?>" << std::endl;
}

void nsIrrMesh::CIrrMesh::addEnd(std::string tag) {
	ofile << "</" << tag << ">" << std::endl;
}

void nsIrrMesh::CIrrMesh::addBuffer(dbObject::sFrame *next) { //left right tree search check and write
	dbObject::sFrame *temp = next;
	while(temp != NULL) {		
		if(temp->good()) {
			ofile << "<buffer>" << std::endl;
				ofile << "<boundingBox minEdge=\"-100.000000 -100.000000 -100.000000\" maxEdge=\"100.000000 100.000000 100.000000\" />" << std::endl;
				ofile << "<material>" << std::endl;
					ofile << "<enum name=\"Type\" value=\"solid\" />" << std::endl;
					ofile << "<color name=\"Ambient\" value=\"ffffffff\" />" << std::endl;
					ofile << "<color name=\"Diffuse\" value=\"ffffffff\" />" << std::endl;
					ofile << "<color name=\"Emissive\" value=\"00000000\" />" << std::endl;
					ofile << "<color name=\"Specular\" value=\"ffffffff\" />" << std::endl;
					ofile << "<float name=\"Shininess\" value=\"0.000000\" />" << std::endl;
					ofile << "<float name=\"Param1\" value=\"0.000000\" />" << std::endl;
					ofile << "<float name=\"Param2\" value=\"0.000000\" />" << std::endl;
					for(unsigned int tc=0; tc<(unsigned int)temp->m->TextureCount; tc++) {
						ofile << "<texture name=\"Texture"<< tc+1 <<"\" value=\"" << temp->m->texture[tc].name << "\" />" << std::endl;
					}
					ofile << "<bool name=\"Wireframe\" value=\"" << "false" << "\" />" << std::endl;
					ofile << "<bool name=\"GouraudShading\" value=\"true\" />" << std::endl;
					ofile << "<bool name=\"Lighting\" value=\"true\" />" << std::endl;
					ofile << "<bool name=\"ZWriteEnable\" value=\"true\" />" << std::endl;
					ofile << "<int name=\"ZBuffer\" value=\"1\" />" << std::endl;
					ofile << "<bool name=\"BackfaceCulling\" value=\"true\" />" << std::endl;
					ofile << "<bool name=\"FrontfaceCulling\" value=\"false\" />" << std::endl;
					ofile << "<bool name=\"FogEnable\" value=\"false\" />" << std::endl;
					ofile << "<bool name=\"NormalizeNormals\" value=\"false\" />" << std::endl;
					ofile << "<int name=\"AntiAliasing\" value=\"5\" />" << std::endl;
					ofile << "<int name=\"ColorMask\" value=\"15\" />" << std::endl;
					ofile << "<bool name=\"BilinearFilter1\" value=\"true\" />" << std::endl;
					ofile << "<bool name=\"BilinearFilter2\" value=\"true\" />" << std::endl;
					ofile << "<bool name=\"BilinearFilter3\" value=\"true\" />" << std::endl;
					ofile << "<bool name=\"BilinearFilter4\" value=\"true\" />" << std::endl;
					ofile << "<bool name=\"TrilinearFilter1\" value=\"false\" />" << std::endl;
					ofile << "<bool name=\"TrilinearFilter2\" value=\"false\" />" << std::endl;
					ofile << "<bool name=\"TrilinearFilter3\" value=\"false\" />" << std::endl;
					ofile << "<bool name=\"TrilinearFilter4\" value=\"false\" />" << std::endl;
					ofile << "<int name=\"AnisotropicFilter1\" value=\"0\" />" << std::endl;
					ofile << "<int name=\"AnisotropicFilter2\" value=\"0\" />" << std::endl;
					ofile << "<int name=\"AnisotropicFilter3\" value=\"0\" />" << std::endl;
					ofile << "<int name=\"AnisotropicFilter4\" value=\"0\" />" << std::endl;
					ofile << "<enum name=\"TextureWrap1\" value=\"texture_clamp_repeat\" />" << std::endl;
					ofile << "<enum name=\"TextureWrap2\" value=\"texture_clamp_repeat\" />" << std::endl;
					ofile << "<enum name=\"TextureWrap3\" value=\"texture_clamp_repeat\" />" << std::endl;
					ofile << "<enum name=\"TextureWrap4\" value=\"texture_clamp_repeat\" />" << std::endl;
					ofile << "<int name=\"LODBias1\" value=\"0\" />" << std::endl;
					ofile << "<int name=\"LODBias2\" value=\"0\" />" << std::endl;
					ofile << "<int name=\"LODBias3\" value=\"0\" />" << std::endl;
					ofile << "<int name=\"LODBias4\" value=\"0\" />" << std::endl;
				ofile << "</material>" << std::endl;
				ofile << "<vertices type=\"standard\" vertexCount=\"" << temp->m->VertexCount << "\">" << std::endl;
				for(unsigned int v=0; v<temp->m->VertexCount; v++) {
					ofile << temp->m->VertexData[v].x << " " << temp->m->VertexData[v].y << " " << temp->m->VertexData[v].z << " " << temp->m->VertexData[v].nx << " " << temp->m->VertexData[v].ny << " " << temp->m->VertexData[v].nz << " " << "64ffffff" << " " << temp->m->VertexData[v].tu << " " << temp->m->VertexData[v].tv << std::endl;
				}
				ofile << std::endl << "</vertices>" << std::endl;
				
				if(temp->m->IndexCount == 0) {
					ofile << "<indices indexCount=\"" << temp->m->VertexCount << "\">" << std::endl;
					for(unsigned int i=0; i<temp->m->VertexCount; i++) {
						ofile << i << " ";
					}
				} else {
					ofile << "<indices indexCount=\"" << temp->m->IndexCount << "\">" << std::endl;
					for(unsigned int i=0; i<temp->m->IndexCount; i++) {
						ofile << temp->m->IndexData[i] << " ";
					} 
				}
				ofile << std::endl << "</indices>" << std::endl;
			ofile << "</buffer>" << std::endl;
		}
		
		addBuffer(temp->child);
		addBuffer(temp->sibling);
		//The idea was that it would automatically also clean walked branches, Maybe do this at the end? Debatable.
		delete temp;
		temp = NULL;
	}
}
// *************************************************************************************************
//
// dbo-converter
// [custom loading of .dbo objects, a little start on a converter] 
// Current output formats supported: .irrmesh [still wip]
// --------------------------------------
// Author: Red Eye @TheGameCreators > http://forum.thegamecreators.com/?m=view_profile&i=9288d39d06d900fb85ae58b5de979ca1
//
// This software is distributed under the terms of The MIT License (MIT).
// A copy of the license may be obtained at: http://choosealicense.com/licenses/mit/
//
// *************************************************************************************************

#include <stdio.h>
#include <iostream>

#include "CDBOConv.h"

int main(int argc,char *args[]) {
	if(argc < 2) {
		std::cout << "> missing arguments: source | destination" << std::endl;
	} else {
		CDBOConv *dboFile = new CDBOConv(std::string(args[1]));
		if(!dboFile->good()) {
			std::cout << "> loading: " << args[1] << " could not be found" << std::endl;
		} else {
			dboFile->create();
			dboFile->saveto(std::string(args[2])); //Warning! Deletes file loaded in memory!
		}

		dboFile->clean();
	}
	std::cin.get();
	
	return 0;
}
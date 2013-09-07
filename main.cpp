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

#include <stdio.h>
#include <iostream>
#include <vector>

#include "CDBOConv.h"

int main(void) {
	
	CDBOConv *dboFile = new CDBOConv("object.dbo");
	if(!dboFile->good()) {
		std::cout << "Error loading dboFile." << std::endl;
	} else {
		dboFile->create();
	}

	dboFile->clean();
	std::cin.get();
	
	return 0;
}
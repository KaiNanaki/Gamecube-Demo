#ifndef __Object__
#define __Object__

#include "CTMaterial.h"
#include "CTModel.h"
#include "CTGraphicsSlave.h"

class Object
{
private:
	CTMaterial		*myMat;
	CTModel			*myModel;
	CTGraphicsSlave	*myGSlave;
	
public:
	void objSetup(CTMaterial* _mat,	CTModel* _model, CTGraphicsSlave* _gSlave)
	{

		myMat		= _mat;
		myModel		= _model;
		myGSlave	= _gSlave;

	}
	
	void drawObj()
	{
		//apply texture and draw the object
		myGSlave->applyMaterial(myMat);
		myModel->draw();

	}
	void setMaterial(CTMaterial* _mat){myMat = _mat;}
	void setModel(CTModel* _model) {myModel = _model;}

};

#endif
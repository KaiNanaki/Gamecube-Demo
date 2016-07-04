#ifndef __Obstacles__
#define __Obstacles__

#include <math.h>
#include "CTGraphicsSlave.h"
#include "CTMaterialSlave.h"
#include "CTAssetSlave.h"

#include "Object.h"


enum face {LEFT, RIGHT, FRONT, BACK, TOP, BOTTOM};

class CubeObstacle
{
private:

	CTGraphicsSlave	*myGSlave;
	CTMaterialSlave	*myMSlave;
	CTAssetSlave	*myASlave;
	Object			myObject;

	bool			active;
	int				ObstIndex;
	Point3d			ObsLoc;
	Point3d			ObsBoundingBox;
	

public:
	void setup(CTMaterial* _mat, CTModel* _model, CTGraphicsSlave* _gSlave)
	{
		myGSlave	= _gSlave;
		active		= false;
		ObstIndex	= -1;
		
		myObject.objSetup(_mat, _model, _gSlave);

	}

	void draw()
	{
		if (active)
		{
			myGSlave->origin();
			myGSlave->move(ObsLoc.x, ObsLoc.y, ObsLoc.z);
			myObject.drawObj();
		}

	}
	void setObstValues(int _index, int _xpos, int _zpos, int _ypos, 
		int _width, int _length, int _height)
	{
		ObstIndex	= _index;
		ObsLoc.x	= _xpos;	//x plane (floor)
		ObsLoc.z	= _zpos;	//z plane (floor)
		ObsLoc.y	= _ypos;	//y plane (height)

		ObsBoundingBox.x = _width;
		ObsBoundingBox.z = _length;
		ObsBoundingBox.y = _height;
	}

	void setActive()
	{
		active = true;
	}
	Point3d getObsLoc()
	{
		return ObsLoc;
	}
	Point3d getObsBB()
	{
		return ObsBoundingBox;
	}
	int getObstIndex()
	{
		return ObstIndex;
	}
	bool getActive()
	{
		return active;
	}
};



class ObstacleManager
{
private:
	CubeObstacle	cubes[OBST_COUNT];
	face			ObstFace;

public:
	void setup(char* filename, CTMaterialSlave* _mSlave, CTAssetSlave* _aSlave, CTGraphicsSlave* _gSlave)
	{
		loadObstacle(filename, _mSlave, _aSlave, _gSlave);
		
	}//end setup()

	void loadObstacle(char* filename, CTMaterialSlave* _mSlave, CTAssetSlave* _aSlave, CTGraphicsSlave* _gSlave)
	{
		//myGSlave	= _gSlave;
		char		tempchar[40];
		char		instruction	= ' ';
		int			index		= -1;
		int			readint1, readint2, readint3;

		danFileParser readfile(filename);
		while (readfile.getLine(tempchar))
		{// second pass to get inctruction and values
			danLineParser readline(tempchar);
			if (readline.getChar(instruction))
			{
				switch (instruction)
				{
				case 'O':
					if (readline.getInt(index) && readline.getInt(readint1) && readline.getInt(readint2) && readline.getInt(readint3))
					{
						cubes[index].setup(_mSlave->getMaterial(MAT_BLUEPLASTIC), _aSlave->getModel(MODEL_CUBE), _gSlave);
						cubes[index].setObstValues(index, readint1, readint2, readint3, CUBE_WIDTH, CUBE_LENGTH, CUBE_HEIGHT);
						cubes[index].setActive();
					}
					break;
				//case 'p':
				//	if (readline.getInt(index) && readline.getInt(readint1) && readline.getInt(readint2) && readline.getInt(readint3))
				//	{
				//		cubes[index].setup(_mSlave->getMaterial(MAT_BLUEPLASTIC), _aSlave->getModel(MODEL_CUBE2), _gSlave);
				//		cubes[index].setObstValues(index, readint1, readint2, readint3, CUBE_WIDTH, CUBE_LENGTH, CUBE_HEIGHT);
				//		cubes[index].setActive();
				//	}
				//	break;
				default:
					break;
				}//end switch
			}//end if
		}//end while

	}//end loadObstacle

	//void collisionCheckAll(Player* _player)
	//{
	//	for (int i = 0; i < OBST_COUNT; i++)
	//	{
	//		cubes[i].collisionCheck(_player);
	//	}
	//	//cubes[1].collisionCheck(_player);

	//}

	void draw()
	{
		for (int i = 0; i < OBST_COUNT; i++)
		{
			cubes[i].draw();
		}

	}//end draw


	CubeObstacle getCubeArray(int _i)
	{
		return cubes[_i];
	}

};

#endif
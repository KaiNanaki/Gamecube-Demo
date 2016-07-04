#ifndef __Player__
#define __Player__

#include "CTGraphicsSlave.h"
#include "CTMaterialSlave.h"
#include "CTAssetSlave.h"
#include "Object.h"

#include "Obstacles.h"

class Player
{
private:
	CTGraphicsSlave	*myGSlave;
	CTMaterialSlave	*myMSlave;
	CTAssetSlave	*myASlave;
	Object			myPlayer;
	Object			myShadow;

	ObstacleManager	*myObstMan;

	Mtx		matrix;			//holds world transformations for player 
	Mtx		pview;			//holds pseudo-view matrix
	Mtx		trans;			//holds additional transformations
	
	Point3d	loc;			//player position
	Vec		dir;			//player direction
	Vec		up;				//player orientation
	Point3d boundingBox;	//holds player width, length and height
	
	Point3d	look;			//pseudo-view / look-at point
	Vec		temp;			//extra vec for calculations
	Vec		temp1;			//extra vec for calculations

	Point3d	shadowLoc;		//holds shadow location

	//may not neeed these...
	Vec		temp2;			//extra vec for calculations
	Point3d	camMove;		//holds camera transformations


	f32		tempGravity;
	f32		jumpVelocity;
	bool	jumped;
	bool	alive;
	bool	win;
	bool	cheatMode;
	bool	onObstacle[OBST_COUNT];

public:
	void setup(CTMaterialSlave *_mSlave, CTAssetSlave *_aSlave, CTGraphicsSlave *_gSlave)
	{
		myMSlave	= _mSlave;
		myGSlave	= _gSlave;
		myASlave	= _aSlave;

		//Set up player object and shadow
		myPlayer.objSetup(myMSlave->getMaterial(MAT_METAL), myASlave->getModel(MODEL_PLAYER), myGSlave);
		myShadow.objSetup(myMSlave->getMaterial(MAT_SHADOW), myASlave->getModel(MODEL_SHADOW), myGSlave);

		//Set player default values
		loc.x=0;	loc.y=0;	loc.z=0;
		dir.x=0;	dir.y=0;	dir.z=-1;
		up.x=0;		up.y=1;		up.z=0;

		boundingBox.x	= 0.3;
		boundingBox.z	= 0.3;
		boundingBox.y	= 1.1;

		shadowLoc		= loc;
		shadowLoc.y		= 0.0;

		tempGravity		= 0;
		jumpVelocity	= 0.0;
		jumped			= false;
		alive			= true;
		win				= false;
		cheatMode		= false;
		
		for (int i = 0; i < OBST_COUNT; i++)
		{
			onObstacle[i] = false;
		}

		refreshmatrix();

	}

	//Reset to default values
	void reset()
	{
		loc.x=0;	loc.y=0;	loc.z=0;
		dir.x=0;	dir.y=0;	dir.z=-1;
		up.x=0;		up.y=1;		up.z=0;

		tempGravity		= 0;
		jumpVelocity	= 0.0;
		jumped			= false;
		alive			= true;
		win				= false;
		cheatMode		= false;

		for (int i = 0; i < OBST_COUNT; i++)
		{
			onObstacle[i] = false;
		}
	}


	//Draw player and player shadow
	void draw()
	{
		myGSlave->origin();			//retrieve origin
		myGSlave->applyMtx(matrix);	//applyworld transformations to player
		myGSlave->campull(loc);		//move camera to players position
		myPlayer.drawObj();			//draw player

		myGSlave->origin();					//retrieve origin
		myGSlave->move(loc.x, -0.02, loc.z);//move shadow/keep it below the player
		myShadow.drawObj();					//draw shadow
		
	}


	void refreshmatrix()
	{
		MTXIdentity(matrix);				//origin
		VECAdd(&loc, &dir, &look);			//generate look-at point
		MTXLookAt(pview, &loc, &up, &look); //create pseudo-view
		MTXInverse(pview, trans);			//Inverse
		MTXConcat(matrix, trans, matrix);	//apply

	}

	//******************************
	//Vector transformations
	//******************************
	void move(f32 x, f32 y, f32 z)
	{
		MTXTrans(trans, x, y, z);
		MTXMultVec(trans, &loc, &loc);
		refreshmatrix();
	}

	void rotx(f32 deg)
	{
		MTXRotDeg(trans, 'x', deg);
		MTXMultVec(trans, &dir, &dir);
		MTXMultVec(trans, &up, &up);
		refreshmatrix();
	}
	void roty(f32 deg)
	{
		MTXRotDeg(trans, 'y', deg);
		MTXMultVec(trans, &dir, &dir);
		MTXMultVec(trans, &up, &up);
		refreshmatrix();
	}
	void rotz(f32 deg)
	{
		MTXRotDeg(trans, 'z', deg);
		MTXMultVec(trans, &dir, &dir);
		MTXMultVec(trans, &up, &up);
		refreshmatrix();
	}
	void trackio(f32 scale)
	{//in-out relative tracking
		VECNormalize(&dir, &temp);
		VECScale(&temp, &temp, scale);
		VECAdd(&loc, &temp, &loc);
		refreshmatrix();
	}
	void trackud(f32 scale) 
	{//up-down relative tracking
		VECNormalize(&dir, &temp);
		VECScale(&up, &temp, scale);
		VECAdd(&loc, &temp, &loc);
		refreshmatrix();
	}
	void tracklr(f32 scale)
	{//side-to-side relative tracking
		VECCrossProduct(&dir, &up, &temp);
		VECScale(&temp, &temp, scale);
		VECAdd(&loc, &temp, &loc);
		refreshmatrix();
	}
	void tracklrY(f32 scale)
	{//side-to-side relative tracking
		
		Vec trackUp;
		trackUp.y = 1.0;

		VECCrossProduct(&dir, &trackUp, &temp);
		VECScale(&temp, &temp, scale);
		VECAdd(&loc, &temp, &loc);
		refreshmatrix();
	}
	void pan(f32 deg)
	{//rotation around axis 'up'
		MTXRotAxisDeg(trans, &up, deg);
		MTXMultVec(trans, &dir, &dir);
		MTXMultVec(trans, &up, &up);
		refreshmatrix();
	}
	void roll(f32 deg)
	{//rotation around axis 'CtoO'
		MTXRotAxisDeg(trans, &dir, deg);
		MTXMultVec(trans, &dir, &dir);
		MTXMultVec(trans, &up, &up);
		refreshmatrix();
	}
	//******************************
	//End vector transformations
	//******************************

	
	void jump()
	{
		if (cheatMode)
		{
			jumpVelocity = 0.6;
			jumped = true;
		}
		else
		{
			jumpVelocity = 0.18;
			jumped = true;
			//printf("\nJUMPED!!! Velocity: %.1f", jumpVelocity);
		}
	}

	//Check win condition and player jump status
	void updatePlayer(ObstacleManager* _ObstMan)
	{
		if (loc.y >= 10.0)
		{//win condition
			win = true;
		}


		if (getOnObstacle())
		{
			if (jumped)
			{
				//printf("\nJumped from obstacle");
				printf("\nUpdating, Velocity: %.1f", jumpVelocity);
				jumpVelocity -= 0.01;
				loc.y += jumpVelocity;
				//printf("\nPlayer Location.y: %.1f", loc.y);
			}//end if
			
		}
		else if (!getOnObstacle())
		{
			//jump code
			if (jumped)
			{
				if (loc.y >= 0.0 )
				{
					jumpVelocity -= 0.01;
					loc.y += jumpVelocity;
				}
			}//end if
			else if (loc.y > 0.0 )
			{//apply gravity
				loc.y -= 0.1;
			}

			if (loc.y < 0.0 )
			{//stop playing going through the ground
				loc.y = 0.0;
				jumped = false;
				jumpVelocity = 0.0;
			}
	

		}//end if

	}//end updatePlayer()


	//loop through all game obstacles and check for collisions
	void collisionCheckAll(ObstacleManager* _ObstMan)
	{
		for (int i = 0; i < OBST_COUNT; i++)
		{
			collisionCheck(_ObstMan, i);
		}
		updatePlayer(_ObstMan);
	}//end collisionCheckAll()


	//check for collision with current obstacle
	void collisionCheck(ObstacleManager* _ObstMan, int _i)
	{
		
		if (_ObstMan->getCubeArray(_i).getActive())
		{//if the obstacle is active, check if it is within its x and z boundaries

			//printf("\nChecking for collision with obstacle: %d", _i);
			if(	   ((loc.x	+ boundingBox.x)	>= (_ObstMan->getCubeArray(_i).getObsLoc().x - (_ObstMan->getCubeArray(_i).getObsBB().x/2)))
				&& ((loc.x	- boundingBox.x)	<= (_ObstMan->getCubeArray(_i).getObsLoc().x + (_ObstMan->getCubeArray(_i).getObsBB().x/2)) )//<- this ")" = end of total x check

				&& ((loc.z	+ boundingBox.z)	>= (_ObstMan->getCubeArray(_i).getObsLoc().z - (_ObstMan->getCubeArray(_i).getObsBB().z/2)))
				&& ((loc.z	- boundingBox.z)	<= (_ObstMan->getCubeArray(_i).getObsLoc().z + (_ObstMan->getCubeArray(_i).getObsBB().z/2)) )//<- this ")" = end of total z check

				)//end if
			{
				//printf("\nIn an obstacle: %d", _i);
				if ((loc.y	>= _ObstMan->getCubeArray(_i).getObsLoc().y) || (loc.y	<= (_ObstMan->getCubeArray(_i).getObsLoc().y - _ObstMan->getCubeArray(_i).getObsBB().y)))
				{//if player is above or below the obstacle, check if they can land on it
					checkOnPlatform(_ObstMan, _i);

				}
				else
				{//if the player is not above or below, pushit out
				pushOutFromFace(findCollisionFace(_player), _player);
				}
			}//end if
			else
			{
				onObstacle[_i] = false;
				//printf("\nNot on obstacle: %d", ObstIndex);
				
			}
		}//end if
	}

	//check if player is on an obstacle
	void checkOnPlatform(ObstacleManager* _ObstMan, int _i)
	{
		//printf("\nChecking if on object: %d", _i);
		if (onObstacle[_i] == false && jumped == true)
		{
			if((loc.y >= (_ObstMan->getCubeArray(_i).getObsLoc().y-0.2f)) && (loc.y <= (_ObstMan->getCubeArray(_i).getObsLoc().y + 0.2f)))
			{
				//printf("\nstood on object: %d", _i);	
				loc.y = (_ObstMan->getCubeArray(_i).getObsLoc().y);
				onObstacle[_i] = true;
				jumped = false;
			}
		}
		//used for testing
		else if (onObstacle[_i] == true && jumped == false)
		{
			//printf("\nStill stood on object: %d", _i);
			//updatePlayer(_ObstMan, _i);
			//_player->updatePlayerGravity();
		}
	}//end checkOnPlatform()


	//find which face of the obstacle the player is colliding with
	face findCollisionFace(ObstacleManager* _ObstMan)
	{
		f32		leftFaceValue;
		f32		rightFaceValue;
		f32		frontFaceValue;
		f32		backFaceValue;
		//f32		topFaceValue;
		//f32		bottomFaceValue;

		leftFaceValue	= fabs((_player->getLocation().x + _player->getBoundingBox().x) - (ObsLoc.x - (ObsBoundingBox.x/2)));
		//printf("\nLeft Value: %.3f", leftFaceValue);
		rightFaceValue	= fabs((_player->getLocation().x - _player->getBoundingBox().x) - (ObsLoc.x + (ObsBoundingBox.x/2)));
		//printf("\nRight Value: %.3f", rightFaceValue);

		frontFaceValue	= fabs((_player->getLocation().z + _player->getBoundingBox().z) - (ObsLoc.z - (ObsBoundingBox.z/2)));
		//printf("\nFront Value: %.3f", frontFaceValue);
		backFaceValue	= fabs((_player->getLocation().z - _player->getBoundingBox().z) - (ObsLoc.z + (ObsBoundingBox.z/2)));
		//printf("\nBack Value: %.3f", backFaceValue);

		//topFaceValue	= fabs(_player->getLocation().y - (ObsLoc.y));
		//bottomFaceValue	= fabs((_player->getLocation().y + _player->getBoundingBox().y) - (ObsLoc.y - (ObsBoundingBox.y/2)));


		if (topFaceValue <= bottomFaceValue)
		{//check if the collision is on the top face
			//printf("\ncolliding with top face");
			printf("\ncolliding with top face");
			return TOP;
		}
		else if (bottomFaceValue <= topFaceValue)
		{
			printf("\ncolliding with bottom face");
			return BOTTOM;
		}


		if (leftFaceValue <= rightFaceValue)
		{//check if the collision is on the left face
			if (frontFaceValue <= backFaceValue)
			{//check if the collision on the left face is closer to the front face
				if(leftFaceValue <= frontFaceValue)
				{
					printf("\ncolliding with left face");
					return LEFT;
				}
				else
				{
					printf("\ncolliding with front face");
					return FRONT;
				}

			}//end if
			else
			{//else the collision on the left face is closer to the back face
				if(leftFaceValue <= backFaceValue)
				{
					printf("\ncolliding with left face");
					return LEFT;
				}
				else
				{
					printf("\ncolliding with back face");
					return BACK;
				}

			}//end if
		}//end if
		else
		{//else the collision is on the right face
			if (frontFaceValue <= backFaceValue)
			{//check if the collision on the right face is closer to the front face
				if(rightFaceValue <= frontFaceValue)
				{
					printf("\ncolliding with right face");
					return RIGHT;
				}
				else
				{
					printf("\ncolliding with front face");
					return FRONT;
				}

			}//end if
			else
			{//check if the collision on the right face is closer to the back face
				if(rightFaceValue <= backFaceValue)
				{
					printf("\ncolliding with right face");
					return RIGHT;
				}
				else
				{
					printf("\ncolliding with back face");
					return BACK;
				}

			}//end if
		}//end else

	}//end findCollisionFace()


	//push the player out from the face it is colliding with
	void pushOutFromFace(face _face, ObstacleManager* _ObstMan)
	{
		//push playerout of the wall in the direction of the face it is closest to
		switch(_face)
		{
		case LEFT:
			_player->setPlayerLocationX((ObsLoc.x - (ObsBoundingBox.x/2)-0.01f) - _player->getBoundingBox().x);
			break;
		case RIGHT:
			_player->setPlayerLocationX((ObsLoc.x + (ObsBoundingBox.x/2)+0.01f) + _player->getBoundingBox().x);
			break;
		case FRONT:
			_player->setPlayerLocationZ((ObsLoc.z - (ObsBoundingBox.z/2)-0.01f) - _player->getBoundingBox().z);
			break;
		case BACK:
			_player->setPlayerLocationZ((ObsLoc.z + (ObsBoundingBox.z/2)+0.01f) + _player->getBoundingBox().z);
			break;
		case TOP:
			if(_player->getJumped() == true)
			{
				if (_player->getLocation().y <= ObsLoc.y+0.01f)
				{
					_player->setPlayerLocationY(ObsLoc.y+0.01f);
				}
			}
			_player->setOnObstacleTrue();
			break;
		case BOTTOM:
			break;
		default:
			break;
		}//end switch
		
	}

	

	//******************************
	//set functions
	//******************************

	void setPlayerDirection(Vec _ctoo)
	{		
		dir = _ctoo;
	}
	void setPlayerLocationX(f32 _x)
	{		
		loc.x = _x;
	}
	void setPlayerLocationZ(f32 _z)
	{		
		loc.z = _z;
	}
	void setPlayerLocationY(f32 _y)
	{		
		loc.y = _y;
	}
	void setJumpedTrue()
	{
		jumped = true;
	}
	void setJumpedFalse()
	{
		jumped = false;
	}
	void setOnObstacleTrue(int _i)
	{
		onObstacle[_i] = true;
		printf("\nOn an obstacle ");
	}
	void setOnObstacleFalse(int _i)
	{
		onObstacle[_i] = false;
		//printf("\nNOT on an obstacle ");
	}
	void setAliveFalse()
	{//set function
		alive = false;
	}

	void setCheatModeTrue()
	{
		cheatMode = true;
	}
	void setCheatModeFalse()
	{
		cheatMode = false;
	}

	//******************************
	//get functions
	//******************************

	Point3d getLocation()
	{//accessor function
		return loc;
	}//end getlocation()
	Point3d getBoundingBox()
	{//accessor function
		return boundingBox;
	}//end getlocation()
	bool getJumped()
	{
		return jumped;
	}
	bool getOnObstacle()
	{
		for (int i = 0; i < OBST_COUNT; i++)
		{
			if (onObstacle[i])
			{
				return true;
			}
		}
		return false;
	}
	bool getAlive()
	{//accessor function
		return alive;
	}//end isalive()
	bool getWin()
	{//accessor function
		return win;
	}//end getWin()
	bool getCheatMode()
	{//accessor function
		return cheatMode;
	}//end getCheatMode()
};

#endif
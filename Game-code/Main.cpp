//Kris Stevenson - GameCube Demo "King of the hill"

#include "Settings.h"	//init settings

#include <dolphin/os.h>

#include "CTFont.h"
#include "CTMaterial.h"
#include "CTMatrixStack.h"

#include "CTAssetSlave.h"
#include "CTColourSlave.h"
#include "CTMaterialSlave.h"
#include "CTGraphicsSlave.h"

#include "Object.h"
#include "HUD.h"
#include "Player.h"
#include "Obstacles.h"



f32     left    = -0.050F;	//Orthographic data
f32     top     = 0.0375F;

Mtx		m, localhome;		//Model matrices.
Mtx		mv;					//Modelview matrix. 
Mtx		mvit;				//Modelview inverse-transpose matrix. (for "normals" lighting)
Mtx		r1;					//rotation matrix - x
Mtx		r2;					//rotation matrix - y
Mtx		r3;					//rotation matrix - z
Mtx		t1;					//transformation matrix


PADStatus pad[PAD_MAX_CONTROLLERS];
//function prototypes
void padControl();
void startup();
void shutdown();
void reset();


//global objects
CTGraphicsSlave	gSlave;		//instance of the graphics slave
CTMaterialSlave	mSlave;		//instance of the material slave
CTColourSlave	cSlave;		//instance of the Colour slave
CTAssetSlave	aSlave;		//instance of the asset slave

HUD				myHUD;		//global myHUD object


//Global Game variables - Controller
f32 oldroll	= 0;
int rumblecount;

Player			thePlayer;
Object			theFloor;
ObstacleManager	obstacles;


//bool	upPressed		= false;
//bool	downPressed	= false;
//bool	leftPressed	= false;
//bool	rightPressed	= false;
bool APressed = false;
bool ZPressed = false;

OSTime		startFrameTime;
OSTime		endFrameTime;
OSTime		differenceInTime;
double		frameTime;


//******************************************************************************
//******************************************************************************
int main(void)
{

	startup();	//initialise classes, textures and models 

	aSlave.playSound(SOUND_MUSIC, VOICE_0, 1.0);	//play the game's music track

	while (true)
	{//main loop - Drawing  
		startFrameTime = OSGetTime();
		padControl();		

		DEMOBeforeRender();
		//******************************

			//place and draw the floor
		gSlave.origin();
		gSlave.move(0.0, -0.1, 0.0);
		theFloor.drawObj();

			//update and draw obstacles
		obstacles.draw();

			//update and draw the player
		thePlayer.collisionCheckAll(&obstacles);
		thePlayer.draw();


		if (thePlayer.getWin())			myHUD.drawWin();		//draw "Won game" myHUD if player has won
		else if (thePlayer.getAlive())	myHUD.draw();			//draw "time and cheat" myHUD if player is alive
		else							myHUD.drawGameOver();	//draw "game over" myHUD if player is dead

		
		//******************************
		DEMODoneRender();

		endFrameTime		= OSGetTime();
		differenceInTime	= endFrameTime - startFrameTime;
		myHUD.setFrameTime(differenceInTime / (double)OS_TIMER_CLOCK);

		// Check for collosions and provide feedback via sound/ controller rumble
		if (thePlayer.collision())
		{ 
			aSlave.playSound(SOUND_THUD, VOICE_0, 0.3);
			PADStartMotor(0);
			rumblecount = 10;
		}//end if
		rumblecount--;
		if (rumblecount<0)
		{
			rumblecount = 0; 
			PADStopMotorHard(0);
		}//end if

	}//end while

	shutdown();	//cleanup classes, textures and models
	

}//end main


//******************************************************************************
//initialise pad, engine slaves and game objects
//******************************************************************************
void startup()
{
	pad[0].button	= 0;
	DEMOInit(NULL);
	cSlave.colourSetup("lists/colour.list");
	gSlave.setup(&cSlave);
	aSlave.setupLoader(&gSlave);	
	aSlave.assetSetup("lists/asset.list", &gSlave);
	mSlave.matSetup("lists/material.list", &aSlave, &cSlave);


		//game object setups
	thePlayer.setup(&mSlave, &aSlave, &gSlave);
	aSlave.advanceLoader();	//obj 1 setup

	theFloor.objSetup(mSlave.getMaterial(MAT_GRASS), aSlave.getModel(MODEL_FLOOR), &gSlave);
	aSlave.advanceLoader();	//obj 2 setup

	obstacles.setup("lists/cubeObstacle.list",&mSlave, &aSlave, &gSlave);
	aSlave.advanceLoader();	//obj 3 setup

	myHUD.setup(aSlave.getModel(MODEL_HUDTILE), aSlave.getFont(FONT_HUD), &thePlayer, &gSlave); 
	aSlave.advanceLoader();	//obj 4 setup

	gSlave.setBackground(cSlave.getColour(COLOUR_BLUE));

}//end startup()

//******************************************************************************
//free assets and exit the game
//******************************************************************************
void shutdown()
{
	aSlave.assetRelease();	//clean up textures/release memory
	gSlave.release();		//clean up textures/release memory
	
	OSHalt("quitting");		//exit program
}//end shutdown()


//******************************************************************************
//reset game objects
//******************************************************************************
void reset()
{
	aSlave.playSound(SOUND_BOOM, VOICE_1, 1.0);
	thePlayer.reset();		//reset player location
	myHUD.resetTime();		//reset timer
	gSlave.setupView();		//reset camera location
	aSlave.playSound(SOUND_MUSIC, VOICE_0, 1.0);	//play the game's music track
	

}//end reset()

//******************************************************************************
//reset game objects
//******************************************************************************
void padControl()
{
	PADRead(pad);
	PADClamp(pad);

		//********** controls **********
	if (thePlayer.getAlive() && !thePlayer.getWin())
	{

			//holds vector of the main stick
		Vec playerMove;
		playerMove.x = (f32)pad[0].stickX/950.0f;//+ (f32)pad[0].substickX/120.0f;
		playerMove.z = (f32)pad[0].stickY/500.0f;
		playerMove.y = 1.0;

			//holds position of substick for camera movement 
		Point3d cameraLoc;
		cameraLoc.x = -((f32)pad[0].substickX/25.0f);
		cameraLoc.y = (f32)pad[0].substickY/70.0f;

			//holds vector of the players direction
		Vec playerDir;
		playerDir.x = gSlave.getCToO().x;	//get x CtoO
		playerDir.z = gSlave.getCToO().z;	//get z CtoO
		playerDir.y = 0.0;					//keep y at 0 to prevent going through the ground

			//set player direction to playerDir (found from CToO - camera to object)
		thePlayer.setPlayerDirection(playerDir);


			//player in-out movement
		thePlayer.trackio(playerMove.z);//((f32)pad[0].stickY/120.0f);
		gSlave.camtrackio(playerMove.z);
		

			//player left-right movement
		thePlayer.tracklrY(playerMove.x/3.0);
		gSlave.camtracklr(playerTemp.x);
		thePlayer.roty(playerTemp.x);
		gSlave.campan(playerTemp.x);    

			//camera pan and pitch on substick
		gSlave.camobjpanY(thePlayer.moveCamera(cameraLoc).x);
		gSlave.camobjpitch(thePlayer.moveCamera(cameraLoc).y);

		//set the cameras lookatpoint
		gSlave.camerasetlookat(thePlayer.getlocation().x, thePlayer.getlocation().y, thePlayer.getlocation().z);

		//turn cheat mode on/off - allows player to jump much higher
		if (pad[0].button & PAD_TRIGGER_Z)
		{		
			if (ZPressed)
			{
				if (!thePlayer.getCheatMode())
				{
					thePlayer.setCheatModeTrue();
				}
				else if (thePlayer.getCheatMode())
				{
					thePlayer.setCheatModeFalse();
				}
				aSlave.playSound(SOUND_BOOM, VOICE_2, 2.0);
			}
			ZPressed = false;
		}
		else if (!(pad[0].button & PAD_TRIGGER_Z))
		{		
			ZPressed = true;
		}

		//check if the player can jump
		if (pad[0].button & PAD_BUTTON_A && ((thePlayer.getLocation().y == 0.0) || (thePlayer.getOnObstacle() == true)))
		{
			printf("\nJumped!!!");
			if (APressed)
			{
				thePlayer.jump();
				aSlave.playSound(SOUND_THUD, VOICE_3, 2.0);
			}
			APressed = false;
		}
		else if (!(pad[0].button & PAD_BUTTON_A))
		{		
			APressed = true;
		}

	}

	//used to test if the pad is connected/input is working
	if (pad[0].button & PAD_BUTTON_B) printf("Pad is working! ");

	//reset the game
	if ((!thePlayer.getAlive()) || (thePlayer.getWin()))
	{
		if (pad[0].button & PAD_BUTTON_START) reset();
	}


}//end padControl()
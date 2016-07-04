#ifndef __HUD__
#define __HUD__

#include "CTGraphicsSlave.h"
#include "CTModel.h"
#include "CTFont.h"
#include "Player.h"

class HUD
{
private:
	
	CTModel*			myModel;
	CTFont*				myFont;
	Player*				myPlayer;
	CTGraphicsSlave*	myGSlave;

	double				frameTime;
	double				time;
	int					flashcounter;

public:
	void setup(CTModel* _Model, CTFont* _font, Player* _player, CTGraphicsSlave* _gSlave)
	{
		
		myModel			= _Model;
		myFont			= _font;
		myPlayer		= _player;
		myGSlave		= _gSlave;
		time			= TIME_LIMIT;

	}//end setup()

	void draw()
	{
		myGSlave->origin();
		myGSlave->move(-0.019, -0.037, -0.1001);
		write("Time: ");
		drawTime();
		if (myPlayer->getCheatMode())
		{
			myGSlave->origin();
			myGSlave->move(-0.025, 0.035, -0.1001);
			write(":O CHEATER!");
		}
		
	}//end draw()
	void drawWin()
	{
		flashcounter++;
		if (flashcounter < 100)
		{
			myGSlave->origin();
			myGSlave->move(-0.021, 0.0, -0.1001);
			write("You Win!!!");
		}
		if (flashcounter > 100)
		{
			myGSlave->origin();
			myGSlave->move(-0.020, 0.003, -0.1001);
			write("Youre The");

			myGSlave->origin();
			myGSlave->move(-0.042, -0.003, -0.1001);
			write("King Of The Hill!!!");
		}
		if (flashcounter > 200) flashcounter = 0;
		
	}//end draw()

	void drawGameOver()
	{
		flashcounter++;
		if (flashcounter > 10)
		{
			myGSlave->origin();
			myGSlave->move(-0.014, 0.003, -0.1001);
			write("Time Up");
			myGSlave->origin();
			myGSlave->move(-0.019, -0.003, -0.1001);
			write("Game Over");
		}
		if (flashcounter > 50) flashcounter = 0;

	}//end drawDead()

	void drawTime()
	{
		flashcounter++;
		if (time > 0.0)
		{
			time -= frameTime;
		}
		else
		{
			time = 0.0;
			myPlayer->setAliveFalse();
		}

		//flash the time when low (becomes quicker the lower it gets)
		if (time < 11.0)
		{
			if (flashcounter > 5)
			{
				writeInt(time);
			}
			if (flashcounter > 10) flashcounter = 0;

		}
		else if (time < 21.0)
		{
			if (flashcounter > 10)
			{
				writeInt(time);
			}
			if (flashcounter > 20) flashcounter = 0;
		}
		else if (time < 31.0)
		{
			if (flashcounter > 15)
			{
				writeInt(time);
			}
			if (flashcounter > 30) flashcounter = 0;

		}
		else
		{
			writeInt(time);
		}
	}//end drawTime()

	void write(char* _text)
	{//takes in a text string and writes it on the screen, does not cast to u8

		u8 len = strlen(_text);
		for (int i = 0; i < len; i++)
		{
			GXLoadTexObj(myFont->getCharacter(_text[i]), GX_TEXMAP0);
			myModel->drawHUD();
			myGSlave->move(0.005, 0, 0);	//move along so next chracter can be drawn
		}
	}//end write

	void writeInt(int _number)
	{
		int nextup = 0;
		if (_number < 0) 
		{
			_number = _number * -1; 
			write("-");
		}
		if (_number>9)
		{
			nextup = _number / 10;
			writeInt(nextup);
			_number = _number - (nextup * 10);
		}
		GXLoadTexObj(myFont->getCharacter(_number+48), GX_TEXMAP0);
		myModel->drawHUD();
		myGSlave->move(0.005, 0, 0);
	}//end writeint

	//******************************
	//******************************

	void setFrameTime(double _frameTime)
	{
		frameTime = _frameTime;
	}
	void resetTime()
	{
		time = TIME_LIMIT;
	}
	double getTime()
	{//accessor function
		return time;
	}


};//end class CTHUD
#endif
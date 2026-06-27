#include "MyPG.h"
#include "MyGameMain.h"
#include <xaudio2.h>

using namespace DG;
using namespace ML;

namespace Title
{
	//BG
	Image::SP bgImg;
	int bgPosX, bgPosY;
	float scale;
	const float bgMaxScale = 10.0f;

	//Title
	Image::SP titleImg;
	int logoPosX, logoPosY;
	const int logoMaxPos = 200;
	
	//StartGameText
	Font::SP startGameText;
	int textAnimCnt;
	bool textVisitable;

	//ScreenShake
	int shakeX = 0;
	int shakeY = 0;
	const float shakeMaxPower = 20.0f;
	const float shakeMaxTime = 20.0f;
	int shakeTimer;

	//プロトタイプ
	void bgDraw();
	void titleDraw();
	void screenShake(int& sx_, int& sy_);
	void startGameTextDraw();
	void startTextAnim(Color& tc_);

	void Initialize()
	{
		ge->dgi->EffectState().param.bgColor = Color(1.0f, 0.5f, 0.5f, 0.5f);

		srand((unsigned int)time(NULL));

		mciSendString(TEXT("open \".\\data\\Music\\TitleBGM.mp3\" alias TitleBGM"), NULL, 0, NULL);
		mciSendString(TEXT("play TitleBGM repeat"), NULL, 0, NULL);

		bgImg = Image::Create("./data/Image/TitleBG.png");
		titleImg = Image::Create("./data/Image/TitleName.png");
		startGameText = Font::Create("POP", 27, 54);

		bgPosX = 640;
		bgPosY = 360;
		scale = 0.0f;

		logoPosX = 640;
		logoPosY = -460;

		shakeTimer = shakeMaxTime;

		textVisitable = false;
	}

	void Finalize()
	{
		mciSendString(TEXT("close TitleBGM"), NULL, 0, NULL);
		bgImg.reset();
		titleImg.reset();
	}

	TaskFlag UpDate()
	{
		auto inp = ge->in1->GetState();
		
		textAnimCnt++;
		scale += 0.2f;
		if (scale >= bgMaxScale)
		{
			scale = bgMaxScale;
			logoPosY += 10;
		}
		if (logoPosY >= logoMaxPos)
		{
			logoPosY = logoMaxPos;
			screenShake(shakeX, shakeY);
		}
		if (shakeTimer <= 0)
		{
			textVisitable = true;
		}

		TaskFlag rtv = TaskFlag::Title;

		if (inp.ST.down == true)
		{
			if (scale != bgMaxScale || logoPosY != logoMaxPos)
			{
				scale = bgMaxScale;
				logoPosY = logoMaxPos;
				shakeX = 0;
				shakeY = 0;
				shakeTimer = 0;
			}
			else
			{
				rtv = TaskFlag::Game;
			}
		}

		return rtv;
	}

	void Render()
	{
		bgDraw();
		titleDraw();
		if (textVisitable)
		{
			startGameTextDraw();
		}
	}

	void bgDraw()
	{
		Box2D bgDraw(-70 * scale + shakeX, -40 * scale + shakeY, 141 * scale, 79 * scale);
		bgDraw.Offset(bgPosX, bgPosY);
		Box2D bgSrc(0, 0, 1280, 720);
		bgImg->Draw(bgDraw, bgSrc);
	}

	void titleDraw()
	{
		Box2D titleDraw(-200, -200, 400, 400);
		titleDraw.Offset(logoPosX, logoPosY);
		Box2D titleSrc(0, 0, 400, 400);
		titleImg->Draw(titleDraw, titleSrc);
	}

	void startGameTextDraw()
	{
		Box2D startTextDraw(375, 570, 600, 60);
		string startText = "Tap S or l> to Start";
		Color textColor;
		startTextAnim(textColor);
		startGameText->Draw(startTextDraw, startText, textColor);
	}

	void startTextAnim(Color& tc_)
	{
		switch (textAnimCnt / 5 % 8)
		{
			case 0: tc_ = Color( 1.0f, 1.0f, 1.0f, 1.0f); break;
			case 1: tc_ = Color(0.75f, 1.0f, 1.0f, 1.0f); break;
			case 2: tc_ = Color( 0.5f, 1.0f, 1.0f, 1.0f); break;
			case 3: tc_ = Color(0.25f, 1.0f, 1.0f, 1.0f); break;
			case 4: tc_ = Color( 0.0f, 1.0f, 1.0f, 1.0f); break;
			case 5: tc_ = Color(0.25f, 1.0f, 1.0f, 1.0f); break;
			case 6: tc_ = Color( 0.5f, 1.0f, 1.0f, 1.0f); break; 
			case 7: tc_ = Color(0.75f, 1.0f, 1.0f, 1.0f); break;
		}
	}

	void screenShake(int& sx_,int& sy_) 
	{
		shakeTimer--;
		if (shakeTimer >= 0)
		{
			float shakeScale = float(shakeTimer) / shakeMaxTime;

			float shakePower = shakeMaxPower * shakeScale;

			sx_ = rand() % int(shakePower + 2) - shakePower;
			sy_ = rand() % int(shakePower + 2) - shakePower;
		}
		else
		{
			sx_ = 0;
			sy_ = 0;
		}
	}
}
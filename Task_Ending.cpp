#include "MyPG.h"
#include "MyGameMain.h"

using namespace DG;
using namespace ML;

namespace Ending
{
	Image::SP endingBGImg[4];
	Image::SP endingBtnHintImg;
	int endingBGImgCnt = sizeof(endingBGImg) / sizeof(endingBGImg[0]);
	int logoPosY;
	Font::SP gameMakerName;
	int delayTime = 90;
	int delayCurrentTime;
	bool isClick;

	void Initialize()
	{
		mciSendString(TEXT("open \".\\data\\Music\\EndingBGM.mp3\" alias EndingBGM"), NULL, 0, NULL);
		mciSendString(TEXT("play EndingBGM repeat"), NULL, 0, NULL);

		gameMakerName = Font::Create("POP", 27, 54);

		ge->dgi->EffectState().param.bgColor = Color(1.0f, 0.5f, 0.5f, 0.5f);
		endingBGImg[0] = Image::Create("./data/Image/Ending_Morning.png");
		endingBGImg[1] = Image::Create("./data/Image/Ending_Afternoon.png");
		endingBGImg[2] = Image::Create("./data/Image/Ending_Evening.png");
		endingBGImg[3] = Image::Create("./data/Image/Ending_Night.png");
		endingBtnHintImg = Image::Create("./data/Image/Ending_ButtonHint.png");
		
		logoPosY = 0;
	}

	void Finalize()
	{
		mciSendString(TEXT("close EndingBGM"), NULL, 0, NULL);
		PlaySound(NULL, NULL, 0);

		for (int i = 0; i < endingBGImgCnt; i++) 
		{
			endingBGImg[i];
		}
		endingBtnHintImg.reset();

		gameMakerName.reset();
	}

	TaskFlag UpDate()
	{
		auto inp = ge->in1->GetState();

		delayCurrentTime++;
		if (delayCurrentTime >= delayTime)
		{
			logoPosY -= 1;
			if (logoPosY <= -2880) 
			{
				logoPosY = 0;
			}
		}
		
		TaskFlag rtv = TaskFlag::Ending;

		if (inp.ST.down == true) 
		{
			if (isClick) 
			{
				rtv = TaskFlag::Title;
			}
			else 
			{
				isClick = true;
			}
		}

		return rtv;
	}

	void Render()
	{
		//BG
		Box2D draw[4]
		{
			Box2D(0,    0, 1280, 720),
			Box2D(0,  720, 1280, 720),
			Box2D(0, 1440, 1280, 720),
			Box2D(0, 2160, 1280, 720),
		};
		for (int i = 0; i < endingBGImgCnt; i++) 
		{
			draw[i].Offset(0, logoPosY);
		}
		Box2D bgSrc(0, 0, 2304, 1296);
		for (int i = 0; i < endingBGImgCnt; i++) 
		{
			endingBGImg[i]->Draw(draw[i], bgSrc);
		}
		
		Box2D draw1[4]
		{
			Box2D(0, 2880 +    0, 1280, 720),
			Box2D(0, 2880 +  720, 1280, 720),
			Box2D(0, 2880 + 1440, 1280, 720),
			Box2D(0, 2880 + 2160, 1280, 720),
		};
		for (int i = 0; i < endingBGImgCnt; i++) 
		{
			draw1[i].Offset(0, logoPosY);
		}
		for (int i = 0; i < endingBGImgCnt; i++)
		{
			endingBGImg[i]->Draw(draw1[i], bgSrc);
		}

		//Hint
		if (isClick)
		{
			Box2D hintDraw(960, 656, 320, 64);
			Box2D hintSrc(0, 0, 320, 64);
			endingBtnHintImg->Draw(hintDraw, hintSrc);
		}

		Box2D nameTextBox(820, 130 + 720, 300, 300);
		nameTextBox.Offset(0, logoPosY);
		string nameText = "Devoloper\n\n   Chen\n    Wei\n   Hong";
		gameMakerName->Draw(nameTextBox, nameText, ML::Color(1.0f, 0.0f, 0.0f, 0.0f));
	}
}
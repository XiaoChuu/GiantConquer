#include "MyPG.h"
#include "MyGameMain.h"

using namespace DG;
using namespace ML;
using namespace std;

namespace Game
{
	//グローバル変数
	//Common
	enum class State
	{
		Normal, Hit, Non, Attack, Defend, Stun
	};

	enum class FireBallState 
	{
		Normal, Hit, Non, Explode
	};

	enum class Angle
	{
		Left, Right, Up, Down
	};

	struct EnemyCreater
	{
		float x, y;
		Box2D hitBase;
		Angle angle;
		State state;
		Box2D drawBase;
		Box2D src;
		int animCnt;
	};

	struct Player 
	{
		float x, y;
		float vx;
		Box2D hitBase;
		Angle angle;
		State state;
		Box2D drawBase;
		Box2D src;
		int animCnt;
	};

	struct FireBall 
	{
		float x, y;
		float vx, vy;
		float scale;
		Box2D hitBase;
		FireBallState state;
		Box2D drawBase;
		Box2D src;
		int animCnt;
		bool isDropping;
	};

	struct Enemy
	{
		State state;
		float x, y;
		float vx, vy;
		Angle angle;
		Box2D hitBase;
		Box2D drawBase;
		Box2D src;
		int animCnt;
		float enemyWalkSpeedRecord;
		int enemyAttackTime, enemyDeadTime, enemyChoose;
	};

	struct castleData
	{
		int castle[11][28];
		Box2D castleTile[32];
		Box2D hitBase[11][28];
	};

	//Result
	Image::SP victoryImg, defeatImg, resultBGImg;
	bool victory, defeat;
	int victoryDelayTimer;

	//Map
	Image::SP backgroundImg, itemFrameImg, statusBGImg;
	constexpr int leftBorder = 0;
	constexpr int rightBorder = 1000;
	constexpr int upBorder = 0;
	constexpr int downBorder = 700;

	//Village
	Image::SP villageImg;

	//EnemyCastle
	Image::SP castleTileImg;
	castleData castleBuild;
	const int tileCnt = 32;
	const int castleSizeY = 11;
	const int castleSizeX = 28;

	//Player
	Image::SP playerImg[4], defendImg;
	int playerImgCnt = sizeof(playerImg) / sizeof(playerImg[0]);
	Player player;
	const int playerMoveSpeed = 8;
	int stunCnt = 0;
	int atkAnimCnt;
	constexpr float isNearlyThreshold = 0.0001f;

	//EnemyDoor
	Image::SP enemyDoorImg;
	EnemyCreater enemyDoor[2];
	int enemyDoorCnt = sizeof(enemyDoor) / sizeof(enemyDoor[0]);
	int openTimeCnt;
	int openTimeDis;
	const int doorPosX[2] = { 246,694 };

	//Enemy
	Image::SP enemyWalkImg[4], enemyAtkImg[4], enemyDeathImg[4];
	int enemyKindCnt = sizeof(enemyWalkImg) / sizeof(enemyWalkImg[0]);
	Enemy enemy[10];
	int enemyCnt = sizeof(enemy) / sizeof(enemy[0]);
	Box2D enemyHitBase[4]
	{
		Box2D(-24, -40, 48, 80), //wizard
		Box2D(-32, -24, 64, 48), //fireworm
		Box2D(-32, -32, 64, 64), //skeleton
		Box2D(-32, -24, 64, 48), //bat
	};

	//FireBallShooter
	Image::SP fbMonImg;
	EnemyCreater fbMon;
	const float fbmMoveSpeed = 3.0f;
	float fbmMoveDis;
	float fbmDir;
	float fbmMoved = 0;
	int shotTimeDis;
	int shotTimeCnt = 0;
	int shotDelay = 0;

	//FireBall
	Image::SP fireBallImg[2];
	const int fireBallImgCnt = sizeof(fireBallImg) / sizeof(fireBallImg[0]);
	FireBall fireBall[10];
	const int fireballCnt = sizeof(fireBall) / sizeof(fireBall[0]);
	const float fBDropSpeed = 6.0f;
	const float fireBallDrawW = 64.0f;
	const float fireBallDrawH = 96.0f;
	const float fireBallScaleMin = 0.5f;
	const float fireBallScaleDelta = -0.0075f;
	int fbHitSpeed = 10;
	int scrollTimer = 0;
	int explodeTime = 0;

	//HPBar
	Image::SP hpFrameImg, hpIconImg;
	Image::SP playerHPImg, enemyHPImg;
	Image::SP hpAlertImg;
	Font::SP showPlayerHP, showEnemyHP;
	const int playerHP = 10;
	const int enemyHP = 100;
	int playerCurrentHP, enemyCurrentHP;
	float playerHPScale, enemyHPScale;
	int playerHPAnim;

	//Sound
	bool isPlaying;

	//プロトタイプ
	//System
	void gameInitialize();
	void gameUpdate();
	void result_Initialize();
	void result_Update();
	void allDead();
	void result_Render();
	//BG
	void background_Render();
	//Village
	void village_Render();
	//Castle
	void castle_Initialize(castleData& cd_);
	void castle_Update(castleData& cd_);
	void castle_Destroy(castleData& cd_, int x_, int y_);
	bool castle_Load(castleData& cd_, int fn_);
	void castle_Render(castleData& cd_);
	//Player
	void player_Initialize(Player& p_, int x_, int y_);
	void player_Update(Player& p_);
	void player_UpdateAngle(Player& p_);
	void player_Render(Player& p_);
	void player_Anim(Player& p_);
	bool isNearly(float playerV, float v);
	//Enemy
	void enemy_Initialize(Enemy& e_, float x_, float y_);
	void enemy_Update(Enemy& e_);
	void enemy_UpdateAngle(Enemy& e_);
	void enemy_Appear(float x_, float y_);
	void enemy_Render(Enemy& e_);
	void enemy_Anim(Enemy& e_);
	//EnemyDoor
	void enemyDoor_Initialize(EnemyCreater& ed_, int x_, int y_);
	void enemyDoor_Update(EnemyCreater& ed_);
	void enemyDoor_Render(EnemyCreater& ed_);
	void enemyDoor_Anim(EnemyCreater& ed_);
	//FireBallShooter
	void fbMon_Initialize(EnemyCreater& fbm_, int x_, int y_);
	void fbMon_Update(EnemyCreater& fbm_);
	void fbMon_Render(EnemyCreater& fbm_);
	void fbMon_Anim(EnemyCreater& c_);
	//FireBall
	void fireBall_Initialize(FireBall& f_, float x_, float y_);
	void fireBall_Update(FireBall& f_);
	void fireBall_Render(FireBall& f_);
	void fireBall_Appear(float x_, float y_);
	void fireBall_Anim(FireBall& f_);
	void fireBall_UpdateDrawBase(FireBall& f_);
	//HPBar
	void hpBar_Initialize();
	void hpBar_Update();
	void hpBar_Render();
	void hpBar_Anim(Color& phc_);

	void Initialize()
	{
		ge->dgi->EffectState().param.bgColor = Color(1.0f, 0.5f, 0.5f, 0.5f);

		gameInitialize();
	}

	void Finalize()
	{
		mciSendString(TEXT("close Level1BGM"), NULL, 0, NULL);
		PlaySound(NULL, NULL, 0);

		victoryImg.reset();
		defeatImg.reset();
		resultBGImg.reset();
		backgroundImg.reset();
		itemFrameImg.reset();
		statusBGImg.reset();
		for (int i = 0; i < playerImgCnt; i++)
		{
			playerImg[i].reset();
		}
		defendImg.reset();
		for (int j = 0; j < fireBallImgCnt; j++)
		{
			fireBallImg[j].reset();
		}
		fbMonImg.reset();
		villageImg.reset();
		castleTileImg.reset();
		enemyDoorImg.reset();
		for (int i = 0; i < enemyKindCnt; i++)
		{
			enemyWalkImg[i].reset();
			enemyAtkImg[i].reset();
			enemyDeathImg[i].reset();
		}
		hpFrameImg.reset();
		hpIconImg.reset();
		playerHPImg.reset();
		enemyHPImg.reset();
		hpAlertImg.reset();

		showPlayerHP.reset();
		showEnemyHP.reset();
	}

	TaskFlag UpDate()
	{
		auto inp = ge->in1->GetState();

		result_Update();

		if (!victory && !defeat)
		{
			gameUpdate();
		}

		//Scene Change
		TaskFlag rtv = TaskFlag::Game;

		if (victory)
		{
			if (!isPlaying)
			{
				mciSendString(TEXT("close Level1BGM"), NULL, 0, NULL);
				PlaySound(TEXT("./data/Music/VictorySound.wav"), NULL, SND_ASYNC);
				isPlaying = true;
			}
			if (inp.ST.down == true)
			{
				rtv = TaskFlag::Ending;
			}
		}
		else if (defeat)
		{
			if (!isPlaying)
			{
				mciSendString(TEXT("close Level1BGM"), NULL, 0, NULL);
				PlaySound(TEXT("./data/Music/DefeatSound.wav"), NULL, SND_ASYNC);
				isPlaying = true;
			}
			if (inp.SE.down == true)
			{
				rtv = TaskFlag::Title;
			}
			if (inp.ST.down == true)
			{
				gameInitialize();
			}
		}

		return rtv;
	}

	void Render()
	{
		background_Render();
		castle_Render(castleBuild);
		for (int i = 0; i < enemyDoorCnt; i++)
		{
			enemyDoor_Render(enemyDoor[i]);
		}
		player_Render(player);
		village_Render();
		for (int i = 0; i < enemyCnt; i++)
		{
			enemy_Render(enemy[i]);
		}
		fbMon_Render(fbMon);
		for (int i = 0; i < fireballCnt; i++)
		{
			fireBall_Render(fireBall[i]);
		}
		hpBar_Render();
		result_Render();
	}

	void background_Render()
	{
		Box2D bgDraw(0, 0, 1000, 360);
		Box2D bgSrc(0, 0, 576, 275);
		backgroundImg->Draw(bgDraw, bgSrc);
		bgDraw.y = 360;
		bgSrc = Box2D(0, 246, 576, 78);
		backgroundImg->Draw(bgDraw, bgSrc);

		Box2D statusBGDraw(1000, 0, 280, 720);
		Box2D statusBGSrc(0, 0, 1340, 2080);
		statusBGImg->Draw(statusBGDraw, statusBGSrc);

		Box2D itemFrameDraw[4]
		{ 
			Box2D(1044, 600, 64, 64),
			Box2D(1108, 600, 64, 64),
			Box2D(1172, 600, 64, 64),
			Box2D(1108, 536, 64, 64)
		};
		int frameDrawCnt = sizeof(itemFrameDraw) / sizeof(itemFrameDraw[0]);
		Box2D itemFrameSrc(16, 16, 96, 96);
		for (int i = 0; i < frameDrawCnt; i++)
		{
			itemFrameImg->Draw(itemFrameDraw[i], itemFrameSrc, Color(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}

	void village_Render()
	{
		Box2D villageDraw(0, 0, 0, 0);
		Box2D villageSrc[]
		{
			Box2D(32, 32, 96, 128),
			Box2D(160, 32, 160, 128),
			Box2D(16, 176, 192, 128),
			Box2D(160, 32, 160, 128)
		};
		int buildingSize = sizeof(villageSrc) / sizeof(villageSrc[0]);
		for (int i = 0; i < buildingSize; i++)
		{
			villageDraw = Box2D(villageDraw.x, 624, villageSrc[i].w + 48, villageSrc[i].h - 32);
			villageImg->Draw(villageDraw, villageSrc[i]);
			villageDraw.x = villageDraw.x + villageSrc[i].w + 112;
		}
	}

	void castle_Initialize(castleData& cd_) 
	{
		for (int t = 0; t < tileCnt; t++)
		{
			int x = t % 8;
			int y = t / 8;
			cd_.castleTile[t] = Box2D(64 * x, 64 * y, 64, 64);
		}
		for (int y = 0; y < castleSizeY; y++)
		{
			for (int x = 0; x < castleSizeX; x++)
			{
				cd_.castle[y][x] = 0;
				cd_.hitBase[y][x] = Box2D(-16, -16, 32, 32);
			}
		}
	}

	void castle_Update(castleData& cd_)
	{
		for (int y = 0; y < castleSizeY; y++)
		{
			for (int x = 0; x < castleSizeX; x++)
			{
				castle_Destroy(cd_, x, y);
			}
		}
	}

	void castle_Destroy(castleData& cd_, int x_, int y_)
	{
		if (cd_.castle[y_][x_] != 0 && cd_.castle[y_][x_] != 1)
		{
			Box2D block = cd_.hitBase[y_][x_].OffsetCopy(54 + x_ * 32, 10 + y_ * 32);
			for (int f = 0; f < fireballCnt; f++)
			{
				if (fireBall[f].state == FireBallState::Hit && fireBall[f].isDropping)
				{
					Box2D fbHit = fireBall[f].hitBase.OffsetCopy(fireBall[f].x, fireBall[f].y);
					if (block.Hit(fbHit) == true)
					{
						if (cd_.castle[y_][x_] == 17)
						{
							fireBall[f].state = FireBallState::Explode;
						}
						else
						{
							if (cd_.castle[y_ + 1][x_] == 0 || cd_.castle[y_ + 1][x_] == 1)
							{
								cd_.castle[y_][x_] = 0;
							}
							else
							{
								cd_.castle[y_][x_] = 1;
							}
							
							if (y_ - 1 > -1)
							{
								cd_.castle[y_ - 1][x_] = 0;
							}
							fireBall[f].state = FireBallState::Explode;
							enemyCurrentHP--;
						}
					}
				}
			}
		}
	}

	bool castle_Load(castleData& cd_, int fn_)
	{
		string filePath = "./data/Stage/Castle" + to_string(fn_) + ".txt";

		ifstream fin(filePath);
		if (!fin)
		{
			return false;
		};

		for (int y = 0; y < castleSizeY; y++)
		{
			for (int x = 0; x < castleSizeX; x++)
			{
				fin >> cd_.castle[y][x];
			}
		}
		fin.close();
		return true;
	}

	void castle_Render(castleData& cd_)
	{
		for (int y = 0; y < castleSizeY; y++)
		{
			for (int x = 0; x < castleSizeX; x++)
			{
				Box2D castleDraw(54, 10, 32, 32);
				castleDraw.Offset(x * 32, y * 32);
				castleTileImg->Draw(castleDraw, cd_.castleTile[cd_.castle[y][x]]);
			}
		}
	}

	void enemyDoor_Initialize(EnemyCreater& ed_, int x_, int y_) //Randonじゃなくてドアを一つずつ分けてタイムカウントする
	{
		//Common
		ed_.state = State::Normal;
		ed_.x = x_;
		ed_.y = y_;
		ed_.drawBase = Box2D(0, 0, 0, 0);
		ed_.src = Box2D(0, 0, 0, 0);
		//EnemyCreate(Open)
		openTimeDis = rand() % 121 + 180;
	}

	void enemyDoor_Update(EnemyCreater& ed_)
	{
		if (ed_.state == State::Normal)
		{
			openTimeCnt++;
			if (openTimeCnt >= openTimeDis)
			{
				ed_.state = State::Attack;
				enemy_Appear(ed_.x, ed_.y + 10);
				openTimeCnt = 0;
			}
		}
		else if(ed_.state == State::Attack)
		{
			openTimeCnt++;
			if (openTimeCnt >= 32)
			{
				ed_.state = State::Normal;
				openTimeCnt = 0;
				openTimeDis = rand() % 121 + 180;
			}
		}

		ed_.animCnt++;
	}

	void enemyDoor_Render(EnemyCreater& ed_)
	{
		if (ed_.state == State::Non)
		{
			return;
		}
		enemyDoor_Anim(ed_);
		Box2D doorDraw = ed_.drawBase.OffsetCopy(int(ed_.x), int(ed_.y));
		enemyDoorImg->Draw(doorDraw, ed_.src);
	}

	void enemyDoor_Anim(EnemyCreater& ed_)
	{
		ed_.drawBase = Box2D(-32, -48, 64, 96);
		if (ed_.state == State::Normal)
		{
			ed_.src = Box2D(0, 128, 64, 64);
		}
		else if (ed_.state == State::Attack)
		{
			switch (ed_.animCnt / 4 % 5)
			{
				case 0:	ed_.src = Box2D(0  , 128, 64, 64); break;
				case 1:	ed_.src = Box2D(64 , 128, 64, 64); break;
				case 2:	ed_.src = Box2D(128, 128, 64, 64); break;
				case 3:	ed_.src = Box2D(192, 128, 64, 64); break;
				case 4:	ed_.src = Box2D(256, 128, 64, 64); break;
			}
		}
	}

	void enemy_Initialize(Enemy& e_, float x_, float y_)
	{
		e_.state = State::Normal;
		e_.x = x_;
		e_.y = y_;
		e_.enemyChoose = rand() % enemyKindCnt;
		float enemyWalkSpeed = rand() % 3 + 1.0f;
		float walkAngle = (rand() % 100 - 40) * (PI / 180.0f); // ラジアン
		e_.vx = sin(walkAngle) * enemyWalkSpeed;
		e_.vy = cos(walkAngle) * enemyWalkSpeed;
		e_.enemyWalkSpeedRecord = e_.vy;
		e_.hitBase = enemyHitBase[e_.enemyChoose];
		e_.drawBase = Box2D(0, 0, 0, 0);
		e_.src = Box2D(0, 0, 0, 0);
	}

	void enemy_Update(Enemy& e_)
	{
		//playerDamage
		if (e_.state == State::Normal) 
		{
			if (e_.y >= downBorder - (e_.hitBase.h / 2.0f)) 
			{
				e_.state = State::Attack;
				playerCurrentHP--;
			}
		}

		//Movement
		if (e_.state == State::Normal || e_.state == State::Hit) 
		{
			e_.x += e_.vx;
			e_.y += e_.vy;
		}

		// 反射処理.
		if (e_.x > rightBorder - (e_.hitBase.w / 2.0f) || e_.x < leftBorder + (e_.hitBase.w / 2.0f))
		{
			e_.vx = -e_.vx;
		}
		enemy_UpdateAngle(e_);

		//Attack
		if (e_.state == State::Attack)
		{
			e_.enemyAttackTime++;
			if (e_.enemyAttackTime >= 60)
			{
				e_.state = State::Non;
				e_.enemyAttackTime = 0;
			}
		}

		//hitByPlayer
		if (e_.state == State::Normal)
		{
			Box2D enemy = e_.hitBase.OffsetCopy(e_.x, e_.y);
			if (player.state == State::Attack)
			{
				Box2D playerHit = player.hitBase.OffsetCopy(player.x, player.y);
				if (enemy.Hit(playerHit) == true)
				{
					e_.state = State::Hit;
				}
			}
			else if (player.state == State::Normal || player.state == State::Defend)
			{
				Box2D playerHit = player.hitBase.OffsetCopy(player.x, player.y);
				if (enemy.Hit(playerHit) == true)
				{
					e_.vy = 0.0f;
				}
				else
				{
					e_.vy = e_.enemyWalkSpeedRecord;
				}
			}
		}

		//Dead
		if (e_.state == State::Hit)
		{
			e_.vy -= 50.0f;
			e_.vx = 0.0f;
			if (e_.y <= 200.0f)
			{
				e_.y = 200.0f;
				e_.enemyDeadTime++;
				if (e_.enemyDeadTime >= 60)
				{
					e_.state = State::Non;
					e_.enemyDeadTime = 0;
				}
			}
		}

		e_.animCnt++;
	}

	void enemy_UpdateAngle(Enemy& e_)
	{
		if (e_.vx > 0.0f)
		{
			e_.angle = Angle::Right;
		}
		else
		{
			e_.angle = Angle::Left;
		}
	}

	void enemy_Appear(float x_,float y_) 
	{
		for (int i = 0; i < enemyCnt; i++) 
		{
			if (enemy[i].state == State::Non)
			{	
				enemy_Initialize(enemy[i], x_, y_);
				break;
			}
		}
	}

	void enemy_Render(Enemy& e_)
	{
		if (e_.state == State::Non)
		{
			return;
		}
		enemy_Anim(e_);
		Box2D enemyDraw = e_.drawBase.OffsetCopy(int(e_.x), int(e_.y));
		switch (e_.state)
		{
			case State::Normal: enemyWalkImg[e_.enemyChoose]->Draw(enemyDraw, e_.src); break;
			case State::Attack: enemyAtkImg[e_.enemyChoose]->Draw(enemyDraw, e_.src); break;
			case State::Hit: enemyDeathImg[e_.enemyChoose]->Draw(enemyDraw, e_.src); break;
		}
	}

	void enemy_Anim(Enemy& e_)
	{
		e_.drawBase = Box2D(-64, -64, 128, 128);
		if (e_.enemyChoose == 0) //wizard
		{
			if (e_.angle == Angle::Right && e_.drawBase.w >= 0)
			{
				e_.drawBase.x = -e_.drawBase.x;
				e_.drawBase.w = -e_.drawBase.w;
			}
			switch (e_.state)
			{
				case State::Normal:
					switch (e_.animCnt / 8 % 10)
					{
						case 0: e_.src = Box2D(0, 0, 80, 80); break;
						case 1: e_.src = Box2D(80, 0, 80, 80); break;
						case 2: e_.src = Box2D(160, 0, 80, 80); break;
						case 3: e_.src = Box2D(240, 0, 80, 80); break;
						case 4: e_.src = Box2D(320, 0, 80, 80); break;
						case 5: e_.src = Box2D(400, 0, 80, 80); break;
						case 6: e_.src = Box2D(480, 0, 80, 80); break;
						case 7: e_.src = Box2D(560, 0, 80, 80); break;
						case 8: e_.src = Box2D(640, 0, 80, 80); break;
						case 9: e_.src = Box2D(720, 0, 80, 80); break;
					}
					break;
				case State::Attack:
					switch (e_.animCnt / 8 % 6)
					{
						case 0: e_.src = Box2D(0, 0, 80, 80); break;
						case 1: e_.src = Box2D(80, 0, 80, 80); break;
						case 2: e_.src = Box2D(160, 0, 80, 80); break;
						case 3: e_.src = Box2D(240, 0, 80, 80); break;
						case 4: e_.src = Box2D(320, 0, 80, 80); break;
						case 5: e_.src = Box2D(400, 0, 80, 80); break;
					}
					break;
				case State::Hit:
					switch (e_.animCnt / 8 % 10)
					{
						case 0: e_.src = Box2D(0, 0, 80, 80); break;
						case 1: e_.src = Box2D(80, 0, 80, 80); break;
						case 2: e_.src = Box2D(160, 0, 80, 80); break;
						case 3: e_.src = Box2D(240, 0, 80, 80); break;
						case 4: e_.src = Box2D(320, 0, 80, 80); break;
						case 5: e_.src = Box2D(400, 0, 80, 80); break;
						case 6: e_.src = Box2D(480, 0, 80, 80); break;
						case 7: e_.src = Box2D(560, 0, 80, 80); break;
						case 8: e_.src = Box2D(640, 0, 80, 80); break;
						case 9: e_.src = Box2D(720, 0, 80, 80); break;
					}
					break;
			}
		}
		else if (e_.enemyChoose == 1) //fireworm
		{
			if (e_.angle == Angle::Left && e_.drawBase.w >= 0)
			{
				e_.drawBase.x = -e_.drawBase.x;
				e_.drawBase.w = -e_.drawBase.w;
			}
			switch (e_.state)
			{
				case State::Normal:
					switch (e_.animCnt / 8 % 9)
					{
						case 0: e_.src = Box2D(  0, 16, 90, 48); break;
						case 1: e_.src = Box2D( 90, 16, 90, 48); break;
						case 2: e_.src = Box2D(180, 16, 90, 48); break;
						case 3: e_.src = Box2D(270, 16, 90, 48); break;
						case 4: e_.src = Box2D(360, 16, 90, 48); break;
						case 5: e_.src = Box2D(450, 16, 90, 48); break;
						case 6: e_.src = Box2D(540, 16, 90, 48); break;
						case 7: e_.src = Box2D(630, 16, 90, 48); break;
						case 8: e_.src = Box2D(720, 16, 90, 48); break;
					}
					break;
				case State::Attack:
					switch (e_.animCnt / 6 % 16)
					{
						case 0: e_.src = Box2D(  0, 16, 90, 48); break;
						case 1: e_.src = Box2D( 90, 16, 90, 48); break;
						case 2: e_.src = Box2D(180, 16, 90, 48); break;
						case 3: e_.src = Box2D(270, 16, 90, 48); break;
						case 4: e_.src = Box2D(360, 16, 90, 48); break;
						case 5: e_.src = Box2D(450, 16, 90, 48); break;
						case 6: e_.src = Box2D(540, 16, 90, 48); break;
						case 7: e_.src = Box2D(630, 16, 90, 48); break;
						case 8: e_.src = Box2D(720, 16, 90, 48); break;
						case 9: e_.src = Box2D(810, 16, 90, 48); break;
						case 10: e_.src = Box2D(900, 16, 90, 48); break;
						case 11: e_.src = Box2D(990, 16, 90, 48); break;
						case 12: e_.src = Box2D(1080, 16, 90, 48); break;
						case 13: e_.src = Box2D(1170, 16, 90, 48); break;
						case 14: e_.src = Box2D(1260, 16, 90, 48); break;
						case 15: e_.src = Box2D(1350, 16, 90, 48); break;
					}
					break;
				case State::Hit:
					switch (e_.animCnt / 8 % 8)
					{
						case 0: e_.src = Box2D(  0, 16, 90, 48); break;
						case 1: e_.src = Box2D( 90, 16, 90, 48); break;
						case 2: e_.src = Box2D(180, 16, 90, 48); break;
						case 3: e_.src = Box2D(270, 16, 90, 48); break;
						case 4: e_.src = Box2D(360, 16, 90, 48); break;
						case 5: e_.src = Box2D(450, 16, 90, 48); break;
						case 6: e_.src = Box2D(540, 16, 90, 48); break;
						case 7: e_.src = Box2D(630, 16, 90, 48); break;
					}
					break;
			}
		}
		else if (e_.enemyChoose == 2) //skeleton
		{
			if (e_.angle == Angle::Left && e_.drawBase.w >= 0)
			{
				e_.drawBase.x = -e_.drawBase.x;
				e_.drawBase.w = -e_.drawBase.w;
			}
			switch (e_.state)
			{
				case State::Normal:
					switch (e_.animCnt / 8 % 4)
					{
						case 0: e_.src = Box2D( 48, 48, 64, 64); break;
						case 1: e_.src = Box2D(192, 48, 64, 64); break;
						case 2: e_.src = Box2D(352, 48, 64, 64); break;
						case 3: e_.src = Box2D(496, 48, 64, 64); break;
					}
					break;
				case State::Attack:
					switch (e_.animCnt / 8 % 8)
					{
						case 0: e_.src = Box2D( 48, 32, 96, 80); break;
						case 1: e_.src = Box2D(208, 32, 96, 80); break;
						case 2: e_.src = Box2D(352, 32, 96, 80); break;
						case 3: e_.src = Box2D(496, 32, 96, 80); break;
						case 4: e_.src = Box2D(640, 32, 96, 80); break;
						case 5: e_.src = Box2D(800, 32, 96, 80); break;
						case 6: e_.src = Box2D(960, 32, 96, 80); break;
						case 7: e_.src = Box2D(1104, 32, 96, 80); break;
					}
					break;
				case State::Hit:
					switch (e_.animCnt / 8 % 4)
					{
						case 0: e_.src = Box2D( 48, 48, 64, 64); break;
						case 1: e_.src = Box2D(192, 48, 64, 64); break;
						case 2: e_.src = Box2D(352, 48, 64, 64); break;
						case 3: e_.src = Box2D(496, 48, 64, 64); break;
					}
					break;
			}
		}
		else if (e_.enemyChoose == 3) //bat
		{
			if (e_.angle == Angle::Left && e_.drawBase.w >= 0)
			{
				e_.drawBase.x = -e_.drawBase.x;
				e_.drawBase.w = -e_.drawBase.w;
			}
			switch (e_.state)
			{
				case State::Normal:
					switch (e_.animCnt / 8 % 8)
					{
						case 0: e_.src = Box2D( 48, 48, 64, 48); break;
						case 1: e_.src = Box2D(192, 48, 64, 48); break;
						case 2: e_.src = Box2D(352, 48, 64, 48); break;
						case 3: e_.src = Box2D(496, 48, 64, 48); break;
						case 4: e_.src = Box2D(656, 48, 64, 48); break;
						case 5: e_.src = Box2D(800, 48, 64, 48); break;
						case 6: e_.src = Box2D(944, 48, 64, 48); break;
						case 7: e_.src = Box2D(1088, 48, 64, 48); break;
					}
					break;
				case State::Attack:
					switch (e_.animCnt / 8 % 8)
					{
						case 0: e_.src = Box2D( 48, 48, 64, 48); break;
						case 1: e_.src = Box2D(192, 48, 64, 48); break;
						case 2: e_.src = Box2D(352, 48, 64, 48); break;
						case 3: e_.src = Box2D(496, 48, 64, 48); break;
						case 4: e_.src = Box2D(640, 48, 64, 48); break;
						case 5: e_.src = Box2D(800, 48, 64, 48); break;
						case 6: e_.src = Box2D(960, 48, 64, 48); break;
						case 7: e_.src = Box2D(1104, 48, 64, 48); break;
					}
					break;
				case State::Hit:
					switch (e_.animCnt / 8 % 4)
					{
						case 0: e_.src = Box2D( 48, 48, 64, 48); break;
						case 1: e_.src = Box2D(192, 64, 64, 48); break;
						case 2: e_.src = Box2D(352, 64, 64, 48); break;
						case 3: e_.src = Box2D(496, 64, 64, 48); break;
					}
					break;
				}
		}
	}

	void player_Initialize(Player& p_, int x_, int y_)
	{
		p_.x = x_;
		p_.y = y_;
		p_.hitBase = Box2D(-35, -55, 70, 110);
		p_.drawBase = Box2D(0, 0, 0, 0);
		p_.src = Box2D(0, 0, 0, 0);
		p_.state = State::Normal;
		p_.angle = Angle::Up;
	}

	void player_Update(Player& p_)
	{
		auto inp = ge->in1->GetState();
		
		p_.animCnt++;

		//Movement
		if (p_.state == State::Stun) 
		{
			p_.vx = 0.0f;
		}
		else
		{
			if (inp.LStick.BL.on)
			{
				if (p_.state == State::Defend)
				{
					p_.vx = -(playerMoveSpeed - 2);
				}
				else
				{
					p_.vx = -playerMoveSpeed;
				}
				
				if (p_.x <= leftBorder + 80)
				{
					p_.x = leftBorder + 80;
					p_.vx = 0.0f;
				}
			}
			else if (inp.LStick.BR.on)
			{
				if (p_.state == State::Defend)
				{
					p_.vx = playerMoveSpeed - 2;
				}
				else
				{
					p_.vx = playerMoveSpeed;
				}

				if (p_.x >= rightBorder - 80)
				{
					p_.x = rightBorder - 80;
					p_.vx = 0.0f;
				}
			} 
			else
			{
				p_.vx = 0.0f;
			}
		}
		p_.x += p_.vx;

		player_UpdateAngle(p_);

		if (p_.state != State::Attack)
		{
			p_.hitBase = Box2D(-35, -55, 70, 110);
		}

		//Attack
		if (p_.state != State::Stun && p_.state != State::Defend)
		{
			if (inp.B1.down)
			{
				p_.state = State::Attack;
				p_.hitBase = Box2D(-93, -93, 172, 172);
			}
			if (p_.state == State::Attack)
			{
				atkAnimCnt++;
				if (atkAnimCnt >= 30)
				{
					p_.state = State::Normal;
					atkAnimCnt = 0;
				}
			}		
		}
		
		if (p_.state == State::Attack)
		{
			Box2D playerAtk = p_.hitBase.OffsetCopy(p_.x, p_.y);
			for (int f = 0; f < fireballCnt; f++)
			{
				if (fireBall[f].state == FireBallState::Normal)
				{
					Box2D fbDrop = fireBall[f].hitBase.OffsetCopy(fireBall[f].x, fireBall[f].y);
					if (fbDrop.Hit(playerAtk) == true)
					{
						fireBall[f].state = FireBallState::Hit;
						fireBall[f].vx = 0.0f;
						fireBall[f].vy = -fBDropSpeed;
					}
				}
			}
		}

		//Defend
		if (p_.state != State::Stun && p_.state != State::Attack)
		{
			if (inp.B2.on)
			{
				p_.state = State::Defend;
			}
			else
			{
				p_.state = State::Normal;
			}
		}
		
		if (p_.state == State::Defend)
		{
			Box2D playerDef = p_.hitBase.OffsetCopy(p_.x, p_.y); 
			for (int f = 0; f < fireballCnt; f++)
			{
				if (fireBall[f].state == FireBallState::Normal)
				{
					Box2D fbDrop = fireBall[f].hitBase.OffsetCopy(fireBall[f].x, fireBall[f].y);
					if (fbDrop.Hit(playerDef) == true)
					{
						fireBall[f].state = FireBallState::Hit;
						
						if (fireBall[f].vx <= 0)
						{
							if (p_.vx > 0)
							{
								fireBall[f].vx = p_.vx * 0.5f + fireBall[f].vx;
							}
							else if (p_.vx < 0)
							{
								fireBall[f].vx = -(p_.vx * 0.5f) + fireBall[f].vx;
							}
						} 
						else if (fireBall[f].vx > 0)
						{
							if (p_.vx > 0)
							{
								fireBall[f].vx = -(p_.vx * 0.5f) + fireBall[f].vx;
							}
							else if (p_.vx < 0)
							{
								fireBall[f].vx = p_.vx * 0.5f + fireBall[f].vx;
							}
						}
						fireBall[f].vy = -fBDropSpeed;
					}
				}
			}
		}

		//Stun
		if (p_.state == State::Stun)
		{
			stunCnt++;
			if (stunCnt >= 60)
			{
				p_.state = State::Normal;
				stunCnt = 0;
			}
		}

		if (p_.state == State::Normal)
		{
			Box2D playerN = p_.hitBase.OffsetCopy(p_.x, p_.y);
			for (int f = 0; f < fireballCnt; f++)
			{
				if (fireBall[f].state == FireBallState::Normal)
				{
					Box2D fbDrop = fireBall[f].hitBase.OffsetCopy(fireBall[f].x, fireBall[f].y);
					if (fbDrop.Hit(playerN) == true)
					{
						fireBall[f].state = FireBallState::Explode;
						p_.state = State::Stun;
					}
				}		
			}
		}
	}

	void player_UpdateAngle(Player& p_) 
	{
		if (p_.vx > 0.0f) 
		{
			p_.angle = Angle::Right;
		}
		else if (p_.vx < 0.0f) 
		{
			p_.angle = Angle::Left;
		} 
		else 
		{
			p_.angle = Angle::Up;
		}
	}

	bool isNearly(float playerV, float v) {
		return abs(playerV - v) < isNearlyThreshold;
	}

	void player_Render(Player& p_)
	{
		switch (p_.state)
		{
			case State::Normal:
			{
				if (!isNearly(p_.vx,0.0f))
				{
					if (p_.state == State::Non)
					{
						return;
					}
					player_Anim(p_);
					Box2D playerDraw = p_.drawBase.OffsetCopy(int(p_.x), int(p_.y));
					playerImg[1]->Draw(playerDraw, p_.src);
				}
				else
				{
					Box2D playerDraw(-80, -80, 160, 160);
					playerDraw.Offset(p_.x, p_.y);
					Box2D playerSrc(16, 208, 32, 32);
					playerImg[0]->Draw(playerDraw, playerSrc);
				}
			}
			break;
			case State::Attack:
			{
				if (p_.state == State::Non)
				{
					return;
				}
				player_Anim(p_);
				Box2D playerDraw = p_.drawBase.OffsetCopy(int(p_.x), int(p_.y));
				playerImg[2]->Draw(playerDraw, p_.src);
			}
			break;
			case State::Defend:
			{
				Box2D playerDraw(-80, -80, 160, 160);
				playerDraw.Offset(p_.x, p_.y);
				Box2D playerSrc(16, 208, 32, 32);
				playerImg[0]->Draw(playerDraw, playerSrc);
				if (p_.state == State::Non)
				{
					return;
				}
				player_Anim(p_);
				Box2D defendDraw(-80, -80, 160, 160);
				defendDraw = p_.drawBase.OffsetCopy(int(p_.x), int(p_.y));
				defendImg->Draw(defendDraw, p_.src);
			}
			break;
			case State::Stun:
			{
				if (p_.state == State::Non)
				{
					return;
				}
				player_Anim(p_);
				Box2D playerDraw = p_.drawBase.OffsetCopy(int(p_.x), int(p_.y));
				playerImg[3]->Draw(playerDraw, p_.src);
			}
			break;
		}
	}

	void player_Anim(Player& p_)
	{
		switch (p_.state)
		{
			case State::Normal:
				if (!isNearly(p_.vx, 0.0f))
				{
					p_.drawBase = Box2D(-80, -80, 160, 160);
					switch (p_.animCnt / 8 % 8)
					{
						case 0: p_.src = Box2D(16, 144, 32, 32); break;
						case 1: p_.src = Box2D(80, 144, 32, 32); break;
						case 2: p_.src = Box2D(144, 144, 32, 32); break;
						case 3: p_.src = Box2D(208, 144, 32, 32); break;
						case 4: p_.src = Box2D(272, 144, 32, 32); break;
						case 5: p_.src = Box2D(336, 144, 32, 32); break;
						case 6: p_.src = Box2D(400, 144, 32, 32); break;
						case 7: p_.src = Box2D(464, 144, 32, 32); break;
					}
				}

				if (p_.angle == Angle::Left && p_.drawBase.w >= 0)
				{
					p_.drawBase.x = -p_.drawBase.x;
					p_.drawBase.w = -p_.drawBase.w;
				}
				break;
			case State::Attack:
				p_.drawBase = Box2D(-93, -93, 172, 172);
				switch (atkAnimCnt / 4 % 8)
				{
					case 0: p_.src = Box2D(16  - 3, 208 - 3, 32 + 3, 32 + 3); break;
					case 1: p_.src = Box2D(80  - 3, 208 - 3, 32 + 3, 32 + 3); break;
					case 2: p_.src = Box2D(144 - 3, 208 - 3, 32 + 3, 32 + 3); break;
					case 3: p_.src = Box2D(208 - 3, 208 - 3, 32 + 3, 32 + 3); break;
					case 4: p_.src = Box2D(272 - 3, 208 - 3, 32 + 3, 32 + 3); break;
					case 5: p_.src = Box2D(336 - 3, 208 - 3, 32 + 3, 32 + 3); break;
					case 6: p_.src = Box2D(400 - 3, 208 - 3, 32 + 3, 32 + 3); break;
					case 7: p_.src = Box2D(464 - 3, 208 - 3, 32 + 3, 32 + 3); break;
				}
				break;
			case State::Defend:
				p_.drawBase = Box2D(-80, -80, 160, 160);
				switch (p_.animCnt / 2 % 8)
				{
					case 0: p_.src = Box2D(0,  256, 64, 64); break;
					case 1: p_.src = Box2D(64, 256, 64, 64); break;
					case 2: p_.src = Box2D(128, 256, 64, 64); break;
					case 3: p_.src = Box2D(192, 256, 64, 64); break;
					case 4: p_.src = Box2D(128, 256, 64, 64); break;
					case 5: p_.src = Box2D(64, 256, 64, 64); break;
					case 6: p_.src = Box2D(0, 256, 64, 64); break;
				}
				break;
			case State::Stun:
				p_.drawBase = Box2D(-80, -80, 160, 160);
				switch (p_.animCnt / 8 % 8)
				{
					case 0: p_.src = Box2D(16, 208, 32, 32); break;
					case 1: p_.src = Box2D(80, 208, 32, 32); break;
					case 2: p_.src = Box2D(144, 208, 32, 32); break;
					case 3: p_.src = Box2D(208, 208, 32, 32); break;
					case 4: p_.src = Box2D(272, 208, 32, 32); break;
					case 5: p_.src = Box2D(336, 208, 32, 32); break;
					case 6: p_.src = Box2D(400, 208, 32, 32); break;
					case 7: p_.src = Box2D(464, 208, 32, 32); break;
				}
				break;
		}
	}

	void fbMon_Initialize(EnemyCreater& fbm_, int x_, int y_) 
	{
		//Common
		fbm_.x = x_;
		fbm_.y = y_;
		fbm_.state = State::Normal;
		fbm_.angle = Angle::Right;
		fbm_.drawBase = Box2D(0, 0, 0, 0);
		fbm_.src = Box2D(0, 0, 0, 0);
		//Movement
		fbmDir = ((rand() % 2 == 0) ? -1.0f : 1.0f);
		fbmMoveDis =  fbmDir * (rand() % 301 + 200);
		//shot
		shotTimeDis = rand() % 121 + 120;
	}

	void fbMon_Update(EnemyCreater& fbm_)
	{
		//FireBallMonsterMovement
		if (fbm_.state == State::Normal)
		{
			fbm_.x += fbmMoveSpeed * fbmDir;
			fbmMoved += abs(fbmMoveSpeed);

			if (fbmMoved >= abs(fbmMoveDis))
			{
				fbmDir = ((rand() % 2 == 0) ? -1.0f: 1.0f);
				fbmMoveDis = fbmDir * (rand() % 301 + 200);
				fbmMoved = 0;
			}

			if (fbm_.x > rightBorder - 96 && fbmDir > 0.0f) 
			{
				fbmDir = -1.0f;
			}
			else if (fbm_.x < leftBorder + 96 && fbmDir < 0.0f)
			{
				fbmDir = 1.0f;
			}

			if (fbmDir < 0)
			{
				fbm_.angle = Angle::Left;
			}
			else
			{
				fbm_.angle = Angle::Right;
			}
		}

		fbm_.animCnt++;
		
		//Shot FireBall
		if (fbm_.state == State::Normal)
		{
			shotTimeCnt++;
			if (shotTimeCnt >= shotTimeDis)
			{
				fbm_.state = State::Attack;
				switch (fbm_.angle)
				{
					case Angle::Left:  fireBall_Appear(fbm_.x - 45, fbm_.y); break;
					case Angle::Right: fireBall_Appear(fbm_.x + 45, fbm_.y); break;
				}
				shotTimeCnt = 0;
			}
		}
		else if (fbm_.state == State::Attack)
		{
			shotTimeCnt++;
			if (shotTimeCnt >= 30)
			{
				fbm_.state = State::Normal;
				shotTimeCnt = 0;
				shotTimeDis = rand() % 121 + 120;
			}
		}
	}

	void fbMon_Render(EnemyCreater& fbm_)
	{
		if (fbm_.state == State::Non)
		{
			return;
		}
		fbMon_Anim(fbm_);
		Box2D fbMonDraw = fbm_.drawBase.OffsetCopy(fbm_.x, fbm_.y);
		fbMonImg->Draw(fbMonDraw, fbm_.src);
	}

	void fbMon_Anim(EnemyCreater& c_)
	{
		switch (c_.state)
		{
			case State::Normal:
				c_.drawBase = Box2D(-96, -64, 192, 128);
				switch ((c_.animCnt / 8) % 6)
				{
					case 0: c_.src = Box2D(0, 16, 64, 48); break;
					case 1: c_.src = Box2D(64, 16, 64, 48); break;
					case 2: c_.src = Box2D(128, 16, 64, 48); break;
					case 3: c_.src = Box2D(192, 16, 64, 48); break;
					case 4: c_.src = Box2D(256, 16, 64, 48); break;
					case 5: c_.src = Box2D(320, 16, 64, 48); break;
					case 6: c_.src = Box2D(384, 16, 64, 48); break;
				}
				break;
			case State::Attack:
				c_.drawBase = Box2D(-96, -64, 192, 128);
				switch ((c_.animCnt / 8) % 6)
				{
					case 0: c_.src = Box2D(0, 80, 64, 48); break;
					case 1: c_.src = Box2D(64, 80, 64, 48); break;
					case 2: c_.src = Box2D(128, 80, 64, 48); break;
					case 3: c_.src = Box2D(192, 80, 64, 48); break;
					case 4: c_.src = Box2D(256, 80, 64, 48); break;
					case 5: c_.src = Box2D(320, 80, 64, 48); break;
					case 6: c_.src = Box2D(384, 16, 64, 48); break;
				}
				break;
		}

		if (c_.angle == Angle::Left && c_.drawBase.w >= 0)
		{
			c_.drawBase.x = -c_.drawBase.x;
			c_.drawBase.w = -c_.drawBase.w;
		}
	}

	void fireBall_Initialize(FireBall& f_, float x_, float y_)
	{
		f_.state = FireBallState::Normal;
		f_.x = x_;
		f_.y = y_;
		float shootAngle = (rand() % 90 - 45) * (PI / 180.0f); // ラジアン.
		f_.vx = sinf(shootAngle) * fBDropSpeed;
		f_.vy = cosf(shootAngle) * fBDropSpeed;
		f_.scale = 1.0f;
		f_.hitBase = Box2D(-32, -32, 64, 64);
		f_.isDropping = false;
	}

	void fireBall_Update(FireBall& f_)
	{
		//playerDamage
		if (f_.state == FireBallState::Normal)
		{
			if (f_.y >= downBorder - 32)
			{
				f_.state = FireBallState::Explode;
				playerCurrentHP--;
			}
		}

		//Movement
		if (f_.state == FireBallState::Normal || f_.state == FireBallState::Hit)
		{
			f_.x += f_.vx;
			f_.y += f_.vy;
		}

		// 反射処理.
		if (f_.x > rightBorder - (f_.hitBase.w / 2.0f) || f_.x < leftBorder + (f_.hitBase.w / 2.0f))
		{
			f_.vx = -f_.vx;
		}

		if (f_.state == FireBallState::Hit)
		{
			if (!f_.isDropping)
			{
				if (f_.y <= upBorder + 32)
				{
					f_.isDropping = true;
					f_.vy = -f_.vy;
				}
			}

			// スケール処理.
			if (f_.scale > fireBallScaleMin)
			{
				f_.scale += fireBallScaleDelta;
			}
			f_.hitBase = Box2D(-32 * f_.scale, -32 * f_.scale, 64 * f_.scale, 64 * f_.scale);

			if (f_.isDropping)
			{
				if (f_.y >= downBorder / 2 - 32)
				{
					f_.isDropping = false;
					f_.state = FireBallState::Explode;
				}	
			}
		}

		if (f_.state == FireBallState::Explode)
		{
			explodeTime++;
			if (explodeTime >= 30)
			{
				f_.state = FireBallState::Non;
				explodeTime = 0;
			}
		}

		f_.animCnt++;
	}

	void fireBall_Render(FireBall& f_)
	{
		if (f_.state == FireBallState::Non)
		{
			return;
		}
		fireBall_UpdateDrawBase(f_);
		fireBall_Anim(f_);
		Box2D fireBallDraw = f_.drawBase.OffsetCopy(int(f_.x), int(f_.y));
		if (f_.state == FireBallState::Normal || f_.state == FireBallState::Hit)
		{
			float angle = atan2(f_.vx, f_.vy);
			if (f_.vy < 0.0f)
			{
				angle += PI;
			}
			fireBallImg[0]->Rotation(-angle, Vec2(fireBallDraw.w / 2.0f, fireBallDraw.h / 3.0f * 2.0f));
			fireBallImg[0]->Draw(fireBallDraw, f_.src);
		}
		else if (f_.state == FireBallState::Explode)
		{
			fireBallImg[1]->Draw(fireBallDraw, f_.src);
		}
	}

	void fireBall_Appear(float x_, float y_)
	{
		for (int i = 0; i < fireballCnt; i++)
		{
			if (fireBall[i].state == FireBallState::Non)
			{
				fireBall_Initialize(fireBall[i], x_, y_);
				break;
			}
		}
	}

	void fireBall_Anim(FireBall& f_)
	{
		switch (f_.state)
		{
			case FireBallState::Normal:
				switch (f_.animCnt / 2 % 8)
				{
					case 0: f_.src = Box2D(0, 0, 32, 48); break;
					case 1: f_.src = Box2D(32, 0, 32, 48); break;
					case 2: f_.src = Box2D(64, 0, 32, 48); break;
					case 3: f_.src = Box2D(96, 0, 32, 48); break;
					case 4: f_.src = Box2D(128, 0, 32, 48); break;
					case 5: f_.src = Box2D(160, 0, 32, 48); break;
					case 6: f_.src = Box2D(192, 0, 32, 48); break;
					case 7: f_.src = Box2D(224, 0, 32, 48); break;
				}
				break;
			case FireBallState::Hit:
				switch (f_.animCnt / 2 % 8)
				{
					case 0: f_.src = Box2D(0, 0, 32, 48); break;
					case 1: f_.src = Box2D(32, 0, 32, 48); break;
					case 2: f_.src = Box2D(64, 0, 32, 48); break;
					case 3: f_.src = Box2D(96, 0, 32, 48); break;
					case 4: f_.src = Box2D(128, 0, 32, 48); break;
					case 5: f_.src = Box2D(160, 0, 32, 48); break;
					case 6: f_.src = Box2D(192, 0, 32, 48); break;
					case 7: f_.src = Box2D(224, 0, 32, 48); break;
				}
				break;
			case FireBallState::Explode:
				switch (f_.animCnt / 7 % 4)
				{
					case 0: f_.src = Box2D(0, 384, 64, 64); break;
					case 1: f_.src = Box2D(64, 384, 64, 64); break;
					case 2: f_.src = Box2D(128, 384, 64, 64); break;
					case 3: f_.src = Box2D(192, 384, 64, 64); break;
				}
				break;
		}
	}

	void fireBall_UpdateDrawBase(FireBall& f_) 
	{
		if (f_.state == FireBallState::Normal || f_.state == FireBallState::Hit)
		{
			int dW = fireBallDrawW * f_.scale;
			int dH = fireBallDrawH * f_.scale;
			f_.drawBase = Box2D(-dW / 2, -dH / 2, dW, dH);

			if (!f_.isDropping && f_.drawBase.h >= 0 && f_.state == FireBallState::Hit)
			{
				f_.drawBase.y = -f_.drawBase.y;
				f_.drawBase.h = -f_.drawBase.h;
			}
		}
		else if (f_.state == FireBallState::Explode)
		{
			f_.drawBase = Box2D(-32, -32, 64, 64);
		}
	}
	
	void hpBar_Initialize()
	{
		playerCurrentHP = playerHP;
		enemyCurrentHP = enemyHP;
		playerHPScale,enemyHPScale = 0.0f;
	}
	
	void hpBar_Update()
	{
		playerHPAnim++;
		if (playerCurrentHP <= 0)
		{
			playerHPScale = 0;
			playerCurrentHP = 0;
		}
		else
		{
			playerHPScale = float(playerCurrentHP) / float(playerHP);
		}
		
		if (enemyCurrentHP <= 0)
		{
			enemyHPScale = 0;
			enemyCurrentHP = 0;
		}
		else
		{
			enemyHPScale = float(enemyCurrentHP) / float(enemyHP);
		}
	}

	void hpBar_Render()
	{
		//Frame
		Box2D hpFrameDraw[2] =
		{
			Box2D(1060, 58, 45, 350),
			Box2D(1175, 58, 45, 350)
		};
		Box2D hpFrameSrc[2] =
		{
			Box2D(142, 438, 18, 106),
			Box2D(538, 438, 18, 106)
		};
		hpFrameImg->Draw(hpFrameDraw[0], hpFrameSrc[0]);
		hpFrameImg->Draw(hpFrameDraw[1], hpFrameSrc[1]);

		//Icon
		Box2D hpIconDraw[2] = 
		{
			Box2D(1050, 456, 64, 64),
			Box2D(1164, 460, 64, 64)
		};
		Box2D hpIconSrc[2] =
		{
			Box2D(192, 32, 32, 32),
			Box2D(224, 96, 32, 31)
		};
		hpIconImg->Draw(hpIconDraw[0], hpIconSrc[0]);
		hpIconImg->Draw(hpIconDraw[1], hpIconSrc[1]);

		//PlayerHPAlert
		if (playerCurrentHP <= playerHP / 2)
		{
			Box2D hpAlertDraw(1060, 58, 45, 350);
			Box2D hpAlertSrc(0, 0, 45, 350);
			Color hpAlertColor;
			hpBar_Anim(hpAlertColor);
			hpAlertImg->Draw(hpAlertDraw, hpAlertSrc, hpAlertColor);
		}

		//PlayerHP
		Box2D playerHPDraw(1060, 58.0f + 350.0f * (1 - playerHPScale), 45, 350.0f * playerHPScale);
		Box2D playerHPSrc(32, 438.0f + 106.0f * (1 - playerHPScale), 18, 106.0f * playerHPScale);
		playerHPImg->Draw(playerHPDraw, playerHPSrc);

		//EnemyHP
		Box2D enemyHPDraw(1175, 58.0f + 350.0f * (1 - enemyHPScale), 45, 350.0f * enemyHPScale);
		Box2D enemyHPSrc(428, 438 + 106.0f * (1 - enemyHPScale), 18, 106.0f * enemyHPScale);
		enemyHPImg->Draw(enemyHPDraw, enemyHPSrc);

		//PlayerHPShow
		Box2D playerHPTextBox;
		string playerHPText  = to_string(playerCurrentHP) + "/" + to_string(playerHP);
		if (playerCurrentHP > 9) 
		{
			playerHPTextBox = Box2D(1045, 420, 84, 24);
		}
		else 
		{
			playerHPTextBox = Box2D(1045 + 12, 420, 84, 24);
		}
		showPlayerHP->Draw(playerHPTextBox, playerHPText, ML::Color(1.0f, 1.0f, 1.0f, 1.0f));

		//EnemyHPShow
		Box2D enemyHPTextBox;
		string enemyHPText = to_string(enemyCurrentHP) + "/" + to_string(enemyHP);
		if (enemyCurrentHP > 99)
		{
			enemyHPTextBox = Box2D(1150, 420, 108, 24);
		}
		else if (enemyCurrentHP > 9)
		{
			enemyHPTextBox = Box2D(1150 + 12, 420, 108, 24);
		}
		else
		{
			enemyHPTextBox = Box2D(1150 + 24, 420, 108, 24);
		}
		showEnemyHP->Draw(enemyHPTextBox, enemyHPText, ML::Color(1.0f, 1.0f, 1.0f, 1.0f));
	}

	void hpBar_Anim(Color& phc_)
	{
		switch (playerHPAnim / 4 % 7)
		{
			case 0: phc_ = Color(0.75f, 0.0f, 1.0f, 1.0f); break;
			case 1: phc_ = Color( 0.5f, 0.0f, 1.0f, 1.0f); break;
			case 2: phc_ = Color(0.25f, 0.0f, 1.0f, 1.0f); break;
			case 3: phc_ = Color( 0.0f, 0.0f, 1.0f, 1.0f); break;
			case 4: phc_ = Color(0.25f, 0.0f, 1.0f, 1.0f); break;
			case 5: phc_ = Color( 0.5f, 0.0f, 1.0f, 1.0f); break;
			case 6: phc_ = Color(0.75f, 0.0f, 1.0f, 1.0f); break;
		}
	}

	void result_Initialize()
	{
		victory = false;
		defeat = false;
	}

	void result_Update()
	{
		if (playerCurrentHP <= 0)
		{
			defeat = true;
		}
		else if (enemyCurrentHP <= 0 && !victory)
		{
			allDead();
			victoryDelayTimer++;
			if (victoryDelayTimer >= 45)
			{
				victory = true;
				victoryDelayTimer = 0;
			}
		}
	}

	void allDead()
	{
		for (int f = 0; f < fireballCnt; f++)
		{
			fireBall[f].state = FireBallState::Hit;
			fireBall[f].vx = 0.0f;
			fireBall[f].vy = -fBDropSpeed;
		}
		for (int e = 0; e < enemyCnt; e++)
		{
			enemy[e].state = State::Hit;
		}
	}

	void result_Render()
	{
		Box2D resultBGDraw(0, 0, 1000, 720);
		Box2D bgColorSrc(0, 0, 100, 100);
		Box2D resultDraw(300, 120, 400, 480);
		if (victory)
		{
			Box2D resultSrc(0, 0, 400, 480);
			Color bgColor(0.3f, 0.0f, 1.0f, 0.0f);
			resultBGImg->Draw(resultBGDraw, bgColorSrc, bgColor);
			victoryImg->Draw(resultDraw, resultSrc);
		}
		else if (defeat)
		{
			Box2D resultSrc(0, 0, 400, 480);
			Color bgColor(0.3f, 1.0f, 0.0f, 0.0f);
			resultBGImg->Draw(resultBGDraw, bgColorSrc, bgColor);
			defeatImg->Draw(resultDraw, resultSrc);
		}
	}

	void gameInitialize()
	{
		srand((unsigned int)time(NULL));

		mciSendString(TEXT("open \".\\data\\Music\\Level1BGM.mp3\" alias Level1BGM"), NULL, 0, NULL);
		mciSendString(TEXT("play Level1BGM repeat"), NULL, 0, NULL);
		PlaySound(NULL, NULL, 0);
		isPlaying = false; //resultSound

		victoryImg = Image::Create("./data/Image/Victory.png");
		defeatImg = Image::Create("./data/Image/Defeat.png");
		resultBGImg = Image::Create("./data/Image/ColorW.png");
		playerImg[0] = Image::Create("./data/Image/Player.png");
		playerImg[1] = Image::Create("./data/Image/Player_Move.png");
		playerImg[2] = Image::Create("./data/Image/Player_Attack.png");
		playerImg[3] = Image::Create("./data/Image/Player_Stun.png");
		defendImg = Image::Create("./data/Image/Player_DefendEffect.png");
		fbMonImg = Image::Create("./data/Image/FireBallMon.png");
		fireBallImg[0] = Image::Create("./data/Image/FireBall.png");
		fireBallImg[1] = Image::Create("./data/Image/FireBall_Explode.png");
		villageImg = Image::Create("./data/Image/Village.png");
		backgroundImg = Image::Create("./data/Image/MeadowBG.png");
		itemFrameImg = Image::Create("./data/Image/ItemFrame.png");
		enemyWalkImg[0] = Image::Create("./data/Image/Enemy_Wizard.png");
		enemyAtkImg[0] = Image::Create("./data/Image/Enemy_Wizard_Attack.png");
		enemyDeathImg[0] = Image::Create("./data/Image/Enemy_Wizard_Death.png");
		enemyWalkImg[1] = Image::Create("./data/Image/Enemy_Fireworm.png");
		enemyAtkImg[1] = Image::Create("./data/Image/Enemy_Fireworm_Attack.png");
		enemyDeathImg[1] = Image::Create("./data/Image/Enemy_Fireworm_Death.png");
		enemyWalkImg[2] = Image::Create("./data/Image/Enemy_Skeleton.png");
		enemyAtkImg[2] = Image::Create("./data/Image/Enemy_Skeleton_Attack.png");
		enemyDeathImg[2] = Image::Create("./data/Image/Enemy_Skeleton_Death.png");
		enemyWalkImg[3] = Image::Create("./data/Image/Enemy_Bat.png");
		enemyAtkImg[3] = Image::Create("./data/Image/Enemy_Bat_Attack.png");
		enemyDeathImg[3] = Image::Create("./data/Image/Enemy_Bat_Death.png");
		enemyDoorImg = Image::Create("./data/Image/Castle_Door.png");
		castleTileImg = Image::Create("./data/Image/Castle_Tile.png");
		statusBGImg = Image::Create("./data/Image/StatusBG.png");
		hpFrameImg = Image::Create("./data/Image/HP_Bars.png");
		hpIconImg = Image::Create("./data/Image/HP_Icon.png");
		hpAlertImg = Image::Create("./data/Image/HPAlert.png");
		playerHPImg = Image::Create("./data/Image/HP_Bars.png");
		enemyHPImg = Image::Create("./data/Image/HP_Bars.png");

		showPlayerHP = Font::Create("POP", 12, 24);
		showEnemyHP = Font::Create("POP", 12, 24);

		result_Initialize();
		castle_Initialize(castleBuild);
		castle_Load(castleBuild, 1);
		player_Initialize(player, 500, 600);
		for (int i = 0; i < enemyDoorCnt; i++)
		{
			enemyDoor_Initialize(enemyDoor[i], doorPosX[i] + 32, 266 + 48);
		}
		for (int i = 0; i < enemyCnt; i++)
		{
			enemy[i].state = State::Non;
		}
		fbMon_Initialize(fbMon, 500, 64);
		for (int i = 0; i < fireballCnt; i++)
		{
			fireBall[i].state = FireBallState::Non;
		}
		hpBar_Initialize();
	}
	
	void gameUpdate()
	{	
		castle_Update(castleBuild);
		player_Update(player);
		for (int i = 0; i < enemyDoorCnt; i++)
		{
			enemyDoor_Update(enemyDoor[i]);
		}
		for (int i = 0; i < enemyCnt; i++)
		{
			enemy_Update(enemy[i]);
		}
		fbMon_Update(fbMon);
		for (int i = 0; i < fireballCnt; i++)
		{
			fireBall_Update(fireBall[i]);
		}
		hpBar_Update();
	}
}
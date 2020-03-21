#pragma once
#include "GameObject.h"
#include "Weapon.h"

#define SIMON_WALKING_SPEED		0.1f 
//0.1f
#define SIMON_JUMP_SPEED_Y		0.5f
#define SIMON_JUMP_DEFLECT_SPEED 0.2f
#define SIMON_GRAVITY			0.002f
#define SIMON_DIE_DEFLECT_SPEED	 0.5f

#define SIMON_STATE_IDLE			0
#define SIMON_STATE_WALKING_RIGHT	100
#define SIMON_STATE_WALKING_LEFT	200
#define SIMON_STATE_JUMP			300
#define SIMON_STATE_DIE				400
#define SIMON_STATE_ATTACK			500

#define SIMON_ANI_BIG_IDLE_RIGHT		0
#define SIMON_ANI_BIG_IDLE_LEFT			1
//#define SIMON_ANI_SMALL_IDLE_RIGHT		2
//#define SIMON_ANI_SMALL_IDLE_LEFT			3

#define SIMON_ANI_BIG_WALKING_RIGHT			2
#define SIMON_ANI_BIG_WALKING_LEFT			3
//#define SIMON_ANI_SMALL_WALKING_RIGHT		6
//#define SIMON_ANI_SMALL_WALKING_LEFT		7
#define SIMON_ANI_DIE				4
#define SIMON_ANI_ATTACK_RIGHT		5
#define SIMON_ANI_ATTACK_LEFT		6

#define	SIMON_LEVEL_SMALL	1
#define	SIMON_LEVEL_BIG		2

#define SIMON_BIG_BBOX_WIDTH  22
#define SIMON_BIG_BBOX_HEIGHT 30

#define SIMON_SMALL_BBOX_WIDTH  13
#define SIMON_SMALL_BBOX_HEIGHT 15

#define SIMON_UNTOUCHABLE_TIME 5000


class CSimon : public CGameObject
{
	int level;
	int untouchable;
	bool isJump;
	bool isAttack;
	DWORD untouchable_start;
	static CSimon* __instance;
	//unordered_map<int, CWeapon*> weapons;
public: 
	CSimon();
	static CSimon* GetInstance();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *colliable_objects = NULL);
	virtual void Render();
	void SetState(int state);
	void SetLevel(int l) { level = l; }
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount(); }
	void SetJump(bool a) { isJump = a; }
	bool GetJump() { return isJump; }
	int GetState() { return state; }
	int GetNx() { return nx; }

	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
};
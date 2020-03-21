#pragma once
#include "GameObject.h"
//#include "Textures.h"
#include "Weapon.h"

#define WHIP_HEIGHT 64
#define WHIP_WIDTH 160

#define WHIP_NOT_ATTACK 0
#define WHIP_ATTACK 1 

#define WHIP_ANI_ATTACK_RIGHT 100
#define WHIP_ANI_ATTACK_LEFT  200


class CWhip2 : public CWeapon
{
	int _level = 1;
	static CWhip2* __instance;
public:

	static CWhip2* GetInstance();
	CWhip2();
	void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	//void setUpLevel();
	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};
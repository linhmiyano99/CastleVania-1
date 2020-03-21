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


class CWhip : public CWeapon
{
	int _level=1;
	static CWhip* __instance;
public:

	static CWhip* GetInstance();
	CWhip();
	void SetPosition(float simon_x, float simon_y);
	void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	void setDefaultLevel();
	//void setUpLevel();
	void Reset() { _level = 1; animation = animations[0]; }
	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	void CollisionWithObject(DWORD dt, vector<LPGAMEOBJECT>& listObj);
	CAnimation* GetAnimation() { return animation; }
};
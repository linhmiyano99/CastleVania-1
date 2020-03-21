#pragma once
#include "GameObject.h"
///#include "Textures.h"
///#include "Sound.h"

#define WEAPON_HEIGHT 64
#define WEAPON_WIDTH 160

#define WEAPON_NOT_EXSIST 0
#define WEAPON_EXSIST 1 


class CWeapon : public CGameObject
{
protected:
	bool isRender;
	//Sound* sound;
public:
	CWeapon(){/* sound = new Sound();*/ }
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void CollisionWithObject(DWORD dt, vector<LPGAMEOBJECT>& listObj);
	virtual CAnimation* GetAnimation() { if (animations.size() > 0)return animation; }
	virtual void Reset() {}
};

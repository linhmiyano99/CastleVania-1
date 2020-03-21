#include "whip2.h"
//#include "Torch.h"
//#include "Enemy.h"
#include "Game.h"
//#include "Boss.h"
#include "Simon.h"
CWhip2* CWhip2::__instance = NULL;

CWhip2* CWhip2::GetInstance()
{
	if (__instance == NULL) __instance = new CWhip2();
	return __instance;
}

CWhip2::CWhip2() :CWeapon()
{
	_level = 1;
	AddAnimation(3000);
	AddAnimation(4000);
	//AddAnimation(602);
	CGameObject::animation = CGameObject::animations[0];
}
void CWhip2::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vector<LPGAMEOBJECT> listObj;
	// Collision logic with items

}
void CWhip2::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (animation->GetCurrentFrame() < 2)
		return;
	if (_level == 1 || _level == 2)
	{
		left = x;
		right = x + 120;
		top = y;
		bottom = y + 20;
	}
	else
	{
		left = x;
		right = x + 145;
		top = y;
		bottom = y + 20;

	}
}
void CWhip2::Render()
{
	//	int ani;

	if (animations.size() > 1)
		animations[0]->Render(100, 100);
}

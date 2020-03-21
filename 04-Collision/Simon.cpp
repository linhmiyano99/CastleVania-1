#include <algorithm>
#include "debug.h"

#include "Simon.h"
#include "Game.h"
#include"whip2.h"
#include"Whip.h"
#include "Goomba.h"
#include "Define.h"


CSimon* CSimon::__instance = NULL;

CSimon* CSimon::GetInstance()
{
	if (__instance == NULL) __instance = new CSimon();
	return __instance;
}
CSimon::CSimon() :CGameObject()
{
		level = SIMON_LEVEL_BIG;
		AddAnimation(1001);		// idle right big
		AddAnimation(2001);		// idle left big

		AddAnimation(1002);		// walk right big
		AddAnimation(2002);		// walk left big

		AddAnimation(5099);		// die

		AddAnimation(1003);//attack right
		AddAnimation(2003);//attack left
		untouchable = 0;
		//weapons[0] = CWhip2::GetInstance();
}
void CSimon::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	// Calculate dx, dy 
	CGameObject::Update(dt);

	// Simple fall down
	vy += SIMON_GRAVITY*dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	//weapons[eType::Whip]->SetPosition(50.0f, 0);

	// turn off collision when die 
	if (state!=SIMON_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

	// reset untouchable timer if untouchable time has passed
	if ( GetTickCount() - untouchable_start > SIMON_UNTOUCHABLE_TIME) 
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	// No collision occured, proceed normally
	if (coEvents.size()==0)
	{
		x += dx; 
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);

		// block 
		x += min_tx*dx + nx*0.4f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
		y += min_ty*dy + ny*0.4f;
		
		if (nx!=0) vx = 0;
		if (ny!=0) vy = 0;

		// Collision logic with Goombas
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CGoomba *>(e->obj)) // if e->obj is Goomba 
			{
				CGoomba *goomba = dynamic_cast<CGoomba *>(e->obj);

				// jump on top >> kill Goomba and deflect a bit 
				if (e->ny < 0)
				{
					if (goomba->GetState()!= GOOMBA_STATE_DIE)
					{
						goomba->SetState(GOOMBA_STATE_DIE);
						vy = -SIMON_JUMP_DEFLECT_SPEED;
					}
				}
				else if (e->nx != 0)
				{
					if (untouchable==0)
					{
						if (goomba->GetState()!=GOOMBA_STATE_DIE)
						{
							if (level > SIMON_LEVEL_SMALL)
							{
								level = SIMON_LEVEL_SMALL;
								StartUntouchable();
							}
							else 
								SetState(SIMON_STATE_DIE);
						}
					}
				}
			}
		}
		GetPosition(x, y);
		//CGame::GetInstance()->GetCamPos(x,y);
		//SetPosition(0, 0);
		if (x<0)
		{
			SetPosition(50.0f, 0);
		}
	}

	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CSimon::Render()
{
	//CWhip* whip = CWhip::GetInstance();
	int ani;
	if (state == SIMON_STATE_DIE)
		ani = SIMON_ANI_DIE;
	else if (isAttack==true)
	{
		/*if (isAttack == true)
		{*/
			if (vx == 0)
			{
				if (nx > 0) ani = SIMON_ANI_ATTACK_RIGHT;
				else ani = SIMON_ANI_ATTACK_LEFT;
				//if(state==SIMON_STATE_ATTACK)
				// CWhip2::GetInstance()->Render();
				 //CWhip2 *w = CWhip2::GetInstance();
				CWhip2::GetInstance()->Render();


				//CWhip::GetInstance()->Render();
				//Whip->
			}
		//}
	}
	else
	if (level == SIMON_LEVEL_BIG)
	{
		if (vx == 0)
		{
			if (nx>0) ani = SIMON_ANI_BIG_IDLE_RIGHT;
			else ani = SIMON_ANI_BIG_IDLE_LEFT;
			
		}
		else if (vx > 0) 
			ani = SIMON_ANI_BIG_WALKING_RIGHT; 
		else ani = SIMON_ANI_BIG_WALKING_LEFT;
	}
	//else if (level == SIMON_LEVEL_SMALL)
	//{
	//	/*if (vx == 0)
	//	{
	//		if (nx>0) ani = SIMON_ANI_SMALL_IDLE_RIGHT;
	//		else ani = SIMON_ANI_SMALL_IDLE_LEFT;
	//	}
	//	else if (vx > 0)
	//		ani = SIMON_ANI_SMALL_WALKING_RIGHT;
	//	else ani = SIMON_ANI_SMALL_WALKING_LEFT;*/
	//}

	int alpha = 255;
	if (untouchable) alpha = 128;
	animations[ani]->Render(x, y, alpha);
	
	RenderBoundingBox();
	if (animations[ani]->GetCurrentFrame() == 2)isAttack = false;
}

void CSimon::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case SIMON_STATE_WALKING_RIGHT:
		vx = SIMON_WALKING_SPEED;
		nx = 1;
		break;
	case SIMON_STATE_WALKING_LEFT: 
		vx = -SIMON_WALKING_SPEED;
		nx = -1;
		break;
	case SIMON_STATE_JUMP: 
		vy = -SIMON_JUMP_SPEED_Y;
	case SIMON_STATE_IDLE: 
		vx = 0;
		break;
	case SIMON_STATE_DIE:
		vy = -SIMON_DIE_DEFLECT_SPEED;
		break;
	case SIMON_STATE_ATTACK:
		isAttack = true;
		vx = 0;
		break;
	}
}

void CSimon::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	left = x;
	top = y; 

	if (level==SIMON_LEVEL_BIG)
	{
		right = x + SIMON_BIG_BBOX_WIDTH;
		bottom = y + SIMON_BIG_BBOX_HEIGHT;
	}
	else
	{
		right = x + SIMON_SMALL_BBOX_WIDTH;
		bottom = y + SIMON_SMALL_BBOX_HEIGHT;
	}
}


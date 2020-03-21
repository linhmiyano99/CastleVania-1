/* =============================================================
	INTRODUCTION TO GAME PROGRAMMING SE102
	
	SAMPLE 04 - COLLISION

	This sample illustrates how to:

		1/ Implement SweptAABB algorithm between moving objects
		2/ Implement a simple (yet effective) collision frame work

	Key functions: 
		CGame::SweptAABB
		CGameObject::SweptAABBEx
		CGameObject::CalcPotentialCollisions
		CGameObject::FilterCollision

		CGameObject::GetBoundingBox
		
================================================================ */

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "debug.h"
#include "Game.h"
#include "GameObject.h"
#include "Textures.h"

#include "Simon.h"
#include "Brick.h"
#include "Goomba.h"
#include"Whip.h"

#include<iostream>
#include<fstream>
#include<string>
#include<queue>

#include"whip2.h"


#define WINDOW_CLASS_NAME L"SampleWindow"
#define MAIN_WINDOW_TITLE L"04 - Collision"

#define BACKGROUND_COLOR D3DCOLOR_XRGB(255, 255, 200)
#define SCREEN_WIDTH 520
#define SCREEN_HEIGHT 340

#define MAX_FRAME_RATE 120

#define ID_TEX_SIMON 0
#define ID_TEX_ENEMY 10
#define ID_TEX_MISC 20
#define ID_TEX_MAP 30
#define ID_TEX_WHIP 40

CGame *game;
CSimon* simon;
CGoomba *goomba;
//CWhip *whip;

vector<LPGAMEOBJECT> objects;

class CSampleKeyHander: public CKeyEventHandler
{
	virtual void KeyState(BYTE *states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
};

CSampleKeyHander * keyHandler; 

void CSampleKeyHander::OnKeyDown(int KeyCode)
{
	DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);
	switch (KeyCode)
	{
	case DIK_SPACE:
		simon->SetState(SIMON_STATE_JUMP);
		break;
	case DIK_A: // reset
		simon->SetState(SIMON_STATE_IDLE);
		simon->SetLevel(SIMON_LEVEL_BIG);
		simon->SetPosition(50.0f,0.0f);
		simon->SetSpeed(0, 0);
		break;
	}
}

void CSampleKeyHander::OnKeyUp(int KeyCode)
{
	DebugOut(L"[INFO] KeyUp: %d\n", KeyCode);
}

void CSampleKeyHander::KeyState(BYTE *states)
{
	// disable control key when simon die 
	if (simon->GetState() == SIMON_STATE_DIE) return;
	if (game->IsKeyDown(DIK_RIGHT))
		simon->SetState(SIMON_STATE_WALKING_RIGHT);
	else if (game->IsKeyDown(DIK_LEFT))
		simon->SetState(SIMON_STATE_WALKING_LEFT);
	else if (game->IsKeyDown(DIK_Z))
	{
		simon->SetState(SIMON_STATE_ATTACK);
		//whip->SetState(WHIP_STATE_ATTACK);
	}
	
		simon->SetState(SIMON_STATE_IDLE);
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

/*
	Load all game resources 
	In this example: load textures, sprites, animations and SIMON object

	TO-DO: Improve this function by loading texture,sprite,animation,object from file
*/
void LoadResources()
{
	CTextures * textures = CTextures::GetInstance();

	textures->Add(ID_TEX_SIMON, L"textures\\Simon.png",D3DCOLOR_XRGB(0, 128, 102));
	textures->Add(ID_TEX_MISC, L"textures\\misc.png", D3DCOLOR_XRGB(176, 224, 248));
	textures->Add(ID_TEX_ENEMY, L"textures\\enemies.png", D3DCOLOR_XRGB(3, 26, 110));
	textures->Add(ID_TEX_MAP, L"textures\\tilemap.png", D3DCOLOR_XRGB(3, 26, 110));
	textures->Add(ID_TEX_WHIP, L"textures\\Whip.png", D3DCOLOR_XRGB(3, 26, 110));
	textures->Add(ID_TEX_BBOX, L"textures\\bbox.png", D3DCOLOR_XRGB(255, 255, 255));

	CSprites * sprites = CSprites::GetInstance();
	CAnimations * animations = CAnimations::GetInstance();
	
	LPDIRECT3DTEXTURE9 texSimon = textures->Get(ID_TEX_SIMON);
	LPDIRECT3DTEXTURE9 texMap = textures->Get(ID_TEX_MAP);
	LPDIRECT3DTEXTURE9 texWhip = textures->Get(ID_TEX_WHIP);


	int l = 0, r = 32, t = 0, b = 32, id=1;
	while (l != 13*32 && t != 32*5 && r != 14*32 && b != 32*6)
	{
		for ( int i = 1; i <= 37; i++)
		{
			sprites->Add(id, l, t, r, b, texMap);
			id++;
			l = l + 32;
			r = r + 32;
		}
		t = t + 32;
		b = b + 32;
	}

	int flag = 0;
	id = 100;
	int arr[5];
	ifstream file_mario;
	file_mario.open("Simon.txt");
	if (file_mario.is_open())
	{
		while (!file_mario.eof())
		{
			while (file_mario >> id)
			{
				arr[flag] = id;
				flag++;
				if (flag == 5)
				{
					sprites->Add(arr[0], arr[1], arr[2], arr[3], arr[4], texSimon);
					flag = 0;
				}
			}
		}
	}
	file_mario.close();
	file_mario.clear();

	id = 200;
	file_mario.open("Whip.txt");
	if (file_mario.is_open())
	{
		while (!file_mario.eof())
		{
			while (file_mario >> id)
			{
				arr[flag] = id;
				flag++;
				if (flag == 5)
				{
					sprites->Add(arr[0], arr[1], arr[2], arr[3], arr[4], texWhip);
					flag = 0;
				}
			}
		}
	}
	file_mario.close();
	file_mario.clear();


	LPDIRECT3DTEXTURE9 texMisc = textures->Get(ID_TEX_MISC);
	sprites->Add(20001, 408, 225, 424, 241, texMisc);

	LPDIRECT3DTEXTURE9 texEnemy = textures->Get(ID_TEX_ENEMY);
	sprites->Add(30001, 5, 14, 21, 29, texEnemy);
	sprites->Add(30002, 25, 14, 41, 29, texEnemy);

	sprites->Add(30003, 45, 21, 61, 29, texEnemy); // die sprite

	LPANIMATION ani;

	ani = new CAnimation(100);	// idle big right
	ani->Add(101);
	animations->Add(1001, ani);

	ani = new CAnimation(100);	// idle big left
	ani->Add(100);
	animations->Add(2001, ani);

	ani = new CAnimation(100);	// walk right big
	ani->Add(101);
	ani->Add(201);
	ani->Add(301);
	animations->Add(1002, ani);

	ani = new CAnimation(100);	// // walk left big
	ani->Add(100);
	ani->Add(200);
	ani->Add(300);
	animations->Add(2002, ani);

	ani = new CAnimation(100);		// Simon die
	ani->Add(500);
	animations->Add(5099, ani);

	ani = new CAnimation(100);	// attack left big
	ani->Add(600);
	ani->Add(700);
	ani->Add(800);
	animations->Add(2003, ani);

	ani = new CAnimation(100);	// walk right big
	ani->Add(601);
	ani->Add(701);
	ani->Add(801);
	animations->Add(1003, ani);

	ani = new CAnimation(100);	// whip attack left big
	ani->Add(1000); // where is sprite 1000 ?//
	ani->Add(2000);
	ani->Add(3000);
	animations->Add(3000, ani);

	ani = new CAnimation(100);	// whip attack right big
	ani->Add(1001);
	ani->Add(2002);
	ani->Add(3003);
	animations->Add(4000, ani);

	ani = new CAnimation(100);		// brick
	ani->Add(20001);
	animations->Add(601, ani);

	ani = new CAnimation(300);		// Goomba walk
	ani->Add(30001);
	ani->Add(30002);
	animations->Add(701, ani);

	ani = new CAnimation(1000);		// Goomba dead
	ani->Add(30003);
	animations->Add(702, ani);


	
	//simon->AddAnimation(1001);		// idle right big
	//simon->AddAnimation(2001);		// idle left big

	//simon->AddAnimation(1002);		// walk right big
	//simon->AddAnimation(2002);		// walk left big

	//simon->AddAnimation(5099);		// die

	//simon->AddAnimation(1003);//attack right
	//simon->AddAnimation(2003);//attack left
	simon = CSimon::GetInstance();
	simon->SetPosition(50.0f, 0);
	//whip = new CWhip();
	/*whip->AddAnimation(10000);
	whip->AddAnimation(20000);*/
	float sx, sy;
	simon->GetPosition(sx,sy);
	//whip->setDefaultLevel();
	//whip->SetPosition(sx, sy);
	objects.push_back(simon);
	//objects.push_back(whip);


	for (int i = 0; i < 3000; i++)
	{
		CBrick *brick = new CBrick();
		brick->AddAnimation(601);
		brick->SetPosition(0 + i*16.0f, 150);
		objects.push_back(brick);
	}

	// and Goombas 
	/*for (int i = 0; i < 4; i++)
	{
		goomba = new CGoomba();
		goomba->AddAnimation(701);
		goomba->AddAnimation(702);
		goomba->SetPosition(200 + i*60, 135);
		goomba->SetState(GOOMBA_STATE_WALKING);
		objects.push_back(goomba);
	}*/

}

/*
	Update world status for this frame
	dt: time period between beginning of last frame and beginning of this frame
*/
void Update(DWORD dt)
{
	// We know that simon is the first object in the list hence we won't add him into the colliable object list
	// TO-DO: This is a "dirty" way, need a more organized way 

	vector<LPGAMEOBJECT> coObjects;
	for (int i = 1; i < objects.size(); i++)
	{
		coObjects.push_back(objects[i]);
	}

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(dt,&coObjects);
	}


	// Update camera to follow simon
	float cx, cy;
	simon->GetPosition(cx, cy);
	//whip->SetPosition(cx, cy);
	cx -= SCREEN_WIDTH / 2;
	cy -= SCREEN_HEIGHT / 2;
	if(cx < 500/ 2 && cx>0)
	CGame::GetInstance()->SetCamPos(cx, 0.0f /*cy*/);
}

/*
	Render a frame 
*/
void Render()
{
	LPDIRECT3DDEVICE9 d3ddv = game->GetDirect3DDevice();
	LPDIRECT3DSURFACE9 bb = game->GetBackBuffer();
	LPD3DXSPRITE spriteHandler = game->GetSpriteHandler();

	if (d3ddv->BeginScene())
	{
		// Clear back buffer with a color
		d3ddv->ColorFill(bb, NULL, BACKGROUND_COLOR);

		spriteHandler->Begin(D3DXSPRITE_ALPHABLEND);
		/////////////////
		CSprites* sprites = CSprites::GetInstance();
		int x = 0, y = 0;
		int flag = 1;
		int i, j;
		ifstream file_entrance("Tile1.txt");
		int number;
		queue<int>entr;
		if (file_entrance.is_open())
		{
			while (!file_entrance.eof())
			{
				file_entrance >> number;
				entr.push(number);
			}
		}
		
			for (i = 0; i < 6 * 32; i = i + 32)
			{
				for (j = 0; j < 24 * 32; j = j + 32)
				{
					if (!entr.empty())
					{
						sprites->Get(entr.front())->Draw(j, i);
						entr.pop();
					}
				}
			}
		////////////////////
		for (int i = 0; i < objects.size(); i++)
			objects[i]->Render();
	
		//whip->Render(); // ?
		spriteHandler->End();
		d3ddv->EndScene();
	}

	// Display back buffer content to the screen
	d3ddv->Present(NULL, NULL, NULL, NULL);
}

HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = hInstance;

	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	HWND hWnd =
		CreateWindow(
			WINDOW_CLASS_NAME,
			MAIN_WINDOW_TITLE,
			WS_OVERLAPPEDWINDOW, // WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			ScreenWidth,
			ScreenHeight,
			NULL,
			NULL,
			hInstance,
			NULL);

	if (!hWnd) 
	{
		OutputDebugString(L"[ERROR] CreateWindow failed");
		DWORD ErrCode = GetLastError();
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

int Run()
{
	MSG msg;
	int done = 0;
	DWORD frameStart = GetTickCount();
	DWORD tickPerFrame = 1000 / MAX_FRAME_RATE;

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) done = 1;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		DWORD now = GetTickCount();

		// dt: the time between (beginning of last frame) and now
		// this frame: the frame we are about to render
		DWORD dt = now - frameStart;

		if (dt >= tickPerFrame)
		{
			frameStart = now;

			game->ProcessKeyboard();
			
			Update(dt);
			Render();
		}
		else
			Sleep(tickPerFrame - dt);	
	}

	return 1;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd = CreateGameWindow(hInstance, nCmdShow, SCREEN_WIDTH, SCREEN_HEIGHT);

	game = CGame::GetInstance();
	game->Init(hWnd);

	keyHandler = new CSampleKeyHander();
	game->InitKeyboard(keyHandler);


	LoadResources();

	SetWindowPos(hWnd, 0, 0, 0, SCREEN_WIDTH*2, SCREEN_HEIGHT*2, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	Run();

	return 0;
}
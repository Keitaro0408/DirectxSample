#include <windows.h>
#include <mmsystem.h>
#include <d3dx9.h>

#define TITLE 	TEXT("Basic of Game")
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

/**
*@mainpage
*ゲーム開発研究部サンプルコード
*/
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")
#define SAFE_RELEASE(p) {if(p){(p)->Release(); (p)=NULL;}}

struct CUSTOMVERTEX
{
	FLOAT	x, y, z, rhw;
	DWORD	color;
	FLOAT	tu, tv;
};


enum TEXTURE
{
	BACKGROUND_TEX,
	PLAYER_TEX,
	TEXMAX
};


LPDIRECT3DTEXTURE9 pTexture[TEXMAX];	//	画像の情報を入れておく為のポインタ配列

IDirect3DDevice9*	pD3Device;		//	Direct3Dのデバイス
D3DPRESENT_PARAMETERS D3dPresentParameters;		//	パラメーター

D3DDISPLAYMODE D3DdisplayMode;
IDirect3D9*			pDirect3D;		//	Direct3Dのインターフェイス

/**
*制御処理
*/
void Control(void)
{
	static float playerPos = 0.f;

}

void PosSet(CUSTOMVERTEX* _vertex,D3DXVECTOR2 _pos, float scale)
{
	_vertex[0].x = _pos.x - scale;
	_vertex[0].y = _pos.y - scale;
	_vertex[1].x = _pos.x + scale;
	_vertex[1].y = _pos.y + scale;
	_vertex[2].x = _pos.x + scale;
	_vertex[2].y = _pos.y + scale;
	_vertex[3].x = _pos.x + scale;
	_vertex[3].y = _pos.y + scale;
}

/**
*描画処理
*/
void Render(void)
{
	//頂点情報を入れる--------------------------------------
	CUSTOMVERTEX backGround[4]
	{
		{	 0.f,   0.f, 1.f,1.f, 0xFFFFFFFF, 0.f, 0.f },
		{ 1280.f,   0.f, 1.f,1.f, 0xFFFFFFFF, 1.f, 0.f },
		{ 1280.f, 720.f, 1.f,1.f, 0xFFFFFFFF, 1.f, 1.f },
		{	 0.f, 720.f, 1.f,1.f, 0xFFFFFFFF, 0.f, 1.f }
	};

	CUSTOMVERTEX player[4]
	{
		{ 500.f, 250.f, 1.f, 1.f, 0xFFFFFFFF, 0.f, 0.f },
		{ 700.f, 250.f, 1.f, 1.f, 0xFFFFFFFF, 1.f, 0.f },
		{ 700.f, 450.f, 1.f, 1.f, 0xFFFFFFFF, 1.f, 1.f },
		{ 500.f, 450.f, 1.f, 1.f, 0xFFFFFFFF, 0.f, 1.f }
	};
	//-----------------------------------------------------


	//画面の消去
	pD3Device->Clear(0, NULL,
					 D3DCLEAR_TARGET,
					 D3DCOLOR_XRGB(0x00, 0x00, 0x00),
					 1.0, 0);

	//描画の開始
	pD3Device->BeginScene();

	pD3Device->SetTexture(0, pTexture[BACKGROUND_TEX]);
	pD3Device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, backGround, sizeof(CUSTOMVERTEX));
	

	pD3Device->SetTexture(0, pTexture[PLAYER_TEX]);
	pD3Device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, player, sizeof(CUSTOMVERTEX));

	//描画の終了
	pD3Device->EndScene();
	//表示
	pD3Device->Present(NULL, NULL, NULL, NULL);
}

/**
*メッセージ処理
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_DESTROY:	//	ウインドウが閉じられた時とか
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}

/**
*メモリ開放
*/
VOID FreeDx()
{
	for (int i = 0; i <= TEXMAX; i++)
	{
		SAFE_RELEASE(pTexture[i]);
	}
	SAFE_RELEASE(pD3Device);
	SAFE_RELEASE(pDirect3D);
}

/**
*メインルーチン
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, int nCmdShow) {
	MSG msg;
	WNDCLASS winc;
	HWND hWnd;

	//Windows情報の設定
	winc.style = CS_HREDRAW | CS_VREDRAW;
	winc.lpfnWndProc = WndProc;
	winc.cbClsExtra = winc.cbWndExtra = 0;
	winc.hInstance = hInstance;
	winc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winc.hCursor = LoadCursor(NULL, IDC_ARROW);
	winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	winc.lpszMenuName = NULL;
	winc.lpszClassName = TITLE;	//クラス名
	//Windowの登録
	if (!RegisterClass(&winc)) return 0;
	//Windowの生成
	int dH = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME) * 2;
	int dW = GetSystemMetrics(SM_CXFRAME) * 2;
	//	ウィンドウを作る
	hWnd = CreateWindow(
		TITLE,								//ウィンドウのクラス名
		TITLE, 							//ウィンドウのタイトル
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,	//ウィンドウスタイル
		CW_USEDEFAULT,						// ウィンドウの横方向の位置x
		CW_USEDEFAULT,						// ウィンドウの縦方向の位置y
		1280 + dW,							// Width（幅）
		720 + dH,							// Height（高さ）
		NULL,
		NULL,
		hInstance,							// アプリケーションインスタンスのハンドル
		NULL
		);
	if (!hWnd) return 0;

	//DirectX オブジェクトの生成
	pDirect3D = Direct3DCreate9(
		D3D_SDK_VERSION);

	//Display Mode の設定
	pDirect3D->GetAdapterDisplayMode(
		D3DADAPTER_DEFAULT,
		&D3DdisplayMode);

	ZeroMemory(&D3dPresentParameters,
		sizeof(D3DPRESENT_PARAMETERS));
	D3dPresentParameters.BackBufferFormat = D3DdisplayMode.Format;
	D3dPresentParameters.BackBufferCount = 1;
	D3dPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	D3dPresentParameters.Windowed = TRUE;

	//デバイスを作る
	pDirect3D->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&D3dPresentParameters, &pD3Device);

	//描画設定
	pD3Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	pD3Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);  //SRCの設定
	pD3Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pD3Device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);

	pD3Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pD3Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	pD3Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	pD3Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pD3Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	pD3Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	
	//頂点に入れるデータを設定
	pD3Device->SetFVF(D3DFVF_CUSTOMVERTEX);

	D3DXCreateTextureFromFile(
		pD3Device,
		"RabbitHouse.png",
		&pTexture[BACKGROUND_TEX]);

	D3DXCreateTextureFromFile(
		pD3Device,
		"robo.png",
		&pTexture[PLAYER_TEX]);

	DWORD SyncOld = timeGetTime();	//	システム時間を取得
	DWORD SyncNow;

	timeBeginPeriod(1);
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT) {
		Sleep(1);
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			SyncNow = timeGetTime();
			if (SyncNow - SyncOld >= 1000 / 60) {	//	1秒間に60回この中に入るはず
				Control();
				Render();
				SyncOld = SyncNow;
			}
		}
	}
	timeEndPeriod(1);

	FreeDx();
	return (int)msg.wParam;
}

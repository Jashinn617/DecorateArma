#include "DxLib.h"
#include "EffekseerForDXLib.h"

#include "Common/SoundSave.h"

#include "Effect/Effekseer3DManager.h"

#include "Utility/Game.h"
#include "Utility/SoundManager.h"

#include "Scene/SceneManager.h"

#include <memory>

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// ウィンドウに表示するタイトル名設定
	SetWindowText(Game::kTitleText);

	// ウィンドウモードの設定
	ChangeWindowMode(Game::kWindowMode);

	// スクリーンサイズの設定
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kClorDepth);

	// ＤＸライブラリ初期化処理
	if (DxLib_Init() == -1)
	{
		// エラーが起きたら直ちに終了
		return -1;
	}

	// 描画先を裏画面にする
	SetDrawScreen(DX_SCREEN_BACK);

	// Effekseerの初期化
	Effekseer_Init(8000);

	// フルスクリーンウインドウの切り替えでリソースが消えるのを防ぐ
	SetChangeScreenModeGraphicsSystemResetFlag(false);

	// DXライブラリのデバイスロストした時のコールバックを設定する
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	// Zバッファを使用する
	SetUseZBuffer3D(true);
	// Zバッファへの書き込みを行う
	SetWriteZBuffer3D(true);

	// ポリゴンの裏側を描画しない
	SetUseBackCulling(true);

	// エフェクトのロード
	Effekseer3DManager::GetInstance();
	// サウンドのロード
	SoundManager::GetInstance();
	// 音量データのロード
	SoundSave::GetInstance().Load();

	// ライト座標の設定
	SetLightPosition(Game::kLightPos);
	// ライト方向の設定
	SetLightDirection(Game::kLightDirection);

	// カメラの視野角の設定
	SetupCamera_Perspective(Game::kFov);

	// シーン管理ポインタ作成
	std::shared_ptr<SceneManager> pScene = std::make_shared<SceneManager>();

	// シーン初期化
	pScene->Init();

	while (ProcessMessage() == 0)
	{
		// このフレームの開始時刻の保存
		LONGLONG time = GetNowHiPerformanceCount();
		// 描画を行う前に画面をクリアする
		ClearDrawScreen();

		/*float fps= GetFPS();
		DrawFormatString(900, 20, 0xffffff, "fps:%2.2f", fps);*/

		// ゲームの処理
		// シーンポインタがnullptrだったら強制終了する
		if (!pScene->Update()) break;
		// 描画
		pScene->Draw();

		// エフェクトの更新
		Effekseer3DManager::GetInstance().Update();
		// エフェクトの描画
		Effekseer3DManager::GetInstance().Draw();

		// 画面が切り替わるのを待つ
		ScreenFlip();

		if (CheckHitKey(KEY_INPUT_ESCAPE)) // escキーが押された場合
		{
			// シーン終了処理
			pScene->End();
			// 強制終了
			break;
		}

		// fpsを固定する
		while (GetNowHiPerformanceCount() - time < static_cast<int>(Game::kStandardTimePer1Frame)) {}
	}

	// Effekseer終了処理
	Effkseer_End();

	// ＤＸライブラリ使用の終了処理
	DxLib_End();

	// ソフトの終了 
	return 0;
}
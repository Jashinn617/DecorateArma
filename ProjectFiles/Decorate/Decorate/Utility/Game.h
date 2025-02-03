#pragma once
#include "DxLib.h"

/// <summary>
/// ゲーム全体で使う定数を定義するしておくnamespace
/// </summary>
namespace Game
{
	const char* const kTitleText = "DecorateArma";		// ウィンドウ名

#ifdef _DEBUG
	constexpr bool kWindowMode = true;					// ウィンドウモード
#else
	constexpr bool kWindowMode = false;					// フルスクリーン
#endif // _DEBUG

	constexpr int kScreenWidth = 1920;					// 画面横サイズ
	constexpr int kScreenHeight = 1080;					// 画面縦サイズ
	constexpr int kClorDepth = 32;						// カラービット数

	constexpr float kStandardTimePer1Frame = 16666.6f;				// 1フレームあたりの標準時間(現在60fps)
	constexpr float kFov = 45.0f * DX_PI_F / 180.0f;				// カメラの視野角
	
	constexpr VECTOR kLightPos = { -1000.0f,2000.0f,-1000.0f };		// ライト座標
	constexpr VECTOR kLightDirection = { -1.0f,-1.0f,-1.0f };		// ライト方向

	/*ステージの種類*/
	enum class StageKind
	{
		StageTest,	// テスト用ステージ
		Stage1,		// ステージ1

		StageNum,	// ステージ数
	};
}
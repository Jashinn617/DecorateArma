#include "SceneManager.h"
#include "SceneDebug.h"
#include "SceneTitle.h"

#include "../Utility/Pad.h"
#include "../Utility/Game.h"

#include <cassert>

namespace
{
	constexpr float kBarPosX = 0;									// 処理バーの座標X
	constexpr float kStringPosX = 10;								// バーの説明用文字列の座標X
	constexpr float kUpdateBarPosY = Game::kScreenHeight * 0.8f;	// 更新処理バーの座標Y
	constexpr float kDrawBarPosY = Game::kScreenHeight * 0.9f;		// 描画処理バーの座標Y

	constexpr unsigned int kUpdateBarColor = 0x0000ff;				// 更新処理バーの色
	constexpr unsigned int kDrawBarColor = 0xff0000;				// 描画処理バーの色
	constexpr unsigned int kStringColor = 0xffffff;					// 文字色
}

SceneManager::SceneManager():
	m_pScene(nullptr),
	m_updateTime(0),
	m_drawTime(0)
{
	/*処理無し*/
}

SceneManager::~SceneManager()
{
	/*処理無し*/
}

void SceneManager::Init()
{
	/*最初のシーンのメモリを確保する*/
#ifdef _DEBUG
	// デバッグ用シーンに飛ぶ
	m_pScene = std::make_shared<SceneDebug>();
#else
	// タイトルシーンに飛ぶ
	m_pScene = std::make_shared<SceneTitle>();
#endif // DEBUG

	// シーンの初期化
	m_pScene->Init();
}

bool SceneManager::Update()
{
#ifdef _DEBUG
	// 更新前ローディング時間取得
	LONGLONG start = GetNowHiPerformanceCount();
#endif // _DEBUG

	// シーンがnullptrだった場合は止まる
	assert(m_pScene);

	// パッド情報の更新処理
	Pad::Update();

	// 次のシーンの取得、シーンの更新処理
	std::shared_ptr<SceneBase> pNext = m_pScene->Update();

	// 次のシーンが存在していなかった場合
	if (!pNext)
	{
		// 現在処理中のシーンの終了処理を行う
		m_pScene->End();
		// falseを返す
		return false;
	}

#ifdef _DEBUG
	// スペースが押された場合
	if (CheckHitKey(KEY_INPUT_SPACE))
	{
		// デバッグシーンに飛ぶ
		pNext = std::make_shared<SceneDebug>();
	}
#endif

	// 次のシーンが現在のシーンでは無かった場合
	if (pNext != m_pScene)
	{
		// 現在処理中のシーンの終了処理を行う
		m_pScene->End();

		// 新しいシーンの開始処理を行う
		m_pScene = pNext;
		m_pScene->Init();
	}

#ifdef _DEBUG
	// 更新時の処理速度を測る
	m_updateTime = static_cast<float>(GetNowHiPerformanceCount() - start);
#endif // _DEBUG

	// ここまで来たらシーンは存在するため、trueを返す
	return true;
}

void SceneManager::Draw()
{
#ifdef _DEBUG
	// 描画のローディング時間取得
	LONGLONG start = GetNowHiPerformanceCount();
#endif // _DEBUG

	// シーンがnullだった場合は止まる
	assert(m_pScene);

	// 描画処理
	m_pScene->Draw();

#ifdef _DEBUG
	// 描画時の処理速度を測る
	m_drawTime = static_cast<float>(GetNowHiPerformanceCount() - start);

	/*処理バーの描画*/
	// 更新処理バー
	float rate = m_updateTime / Game::kStandardTimePer1Frame;
	float width = Game::kScreenWidth * rate;
	// 描画
	DrawBox(static_cast<int>(kBarPosX), static_cast<int>(kUpdateBarPosY), static_cast<int>(width), Game::kScreenHeight, kUpdateBarColor, true);
	DrawFormatString(static_cast<int>(kStringPosX), static_cast<int>(kUpdateBarPosY), kStringColor, "更新処理");
	// 描画処理バー
	rate = (m_updateTime + m_drawTime) / Game::kStandardTimePer1Frame;
	width = Game::kScreenWidth * rate;
	// 描画
	DrawBox(static_cast<int>(kBarPosX), static_cast<int>(kDrawBarPosY), static_cast<int>(width), Game::kScreenHeight, kDrawBarColor, true);
	DrawFormatString(static_cast<int>(kStringPosX), static_cast<int>(kDrawBarPosY), kStringColor, "描画処理");
#endif // _DEBUG
}

void SceneManager::End()
{
	// シーンがnullptrだった場合は止める
	assert(m_pScene);
	// 終了処理
	m_pScene->End();
}
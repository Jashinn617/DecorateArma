#include "SceneStage.h"

#include "SceneClear.h"
#include "SceneGameOver.h"
#include "SceneOption.h"

#include "../Object/ObjectManager.h"

#include "../Utility/Pad.h"
#include "../Utility/SoundManager.h"


#include <string>
#include <cassert>

namespace
{
	constexpr float kCameraNear = 5.0f;		// カメラ手前クリップ距離
	constexpr float kCameraFar = 5000.0f;	// カメラ奥クリップ距離

	constexpr int kStageNum = 2;			// ステージ数

	// ステージによってBGMを変える
	const std::string kStageBGMPath[kStageNum] =
	{
		"Stage1Scene",
		"Stage1Scene",
	};
}

SceneStage::SceneStage(Game::StageKind stage):
	m_isGameClear(false),
	m_isGameOver(false),
	m_stageKind(stage),
	m_pObject(std::make_shared<ObjectManager>(stage)),
	m_pNextScene(nullptr)
{
	/*処理無し*/
}

SceneStage::~SceneStage()
{
	// BGMを止める
	SoundManager::GetInstance().DesignationStopSound("BossScene");
	SoundManager::GetInstance().DesignationStopSound(kStageBGMPath[static_cast<int>(m_stageKind)].c_str());
}

void SceneStage::Init()
{
	// BGMを流す
	SoundManager::GetInstance().Play(kStageBGMPath[static_cast<int>(m_stageKind)].c_str(), false);
}

std::shared_ptr<SceneBase> SceneStage::Update()
{
	// オブジェクトの更新
	m_pObject->Update();

	// プレイヤーが死んだ場合
	if (m_pObject->IsGameOver())
	{
		// ゲームオーバーシーンに遷移
		return std::make_shared<SceneGameOver>(m_stageKind);
	}
	
	// ボスを倒した場合
	if (m_pObject->IsGameClear() && !m_isGameClear)
	{
		// ゲームクリアシーンに遷移
		return std::make_shared<SceneClear>(m_stageKind);
	}

	// スタートボタンを押したらオプション画面を出す
	if (Pad::IsTrigger(PAD_INPUT_8))
	{
		return std::make_shared<SceneOption>(shared_from_this());
	}

	// ボスバトルになったらBGMを変える
	if (m_pObject->IsBossButtle())
	{
		// BGMを止める
		SoundManager::GetInstance().DesignationStopSound(kStageBGMPath[static_cast<int>(m_stageKind)]);
		// ボスバトルBGMを流す
		SoundManager::GetInstance().Play("BossScene", false);
	}
	

	return shared_from_this();
}

void SceneStage::Draw()
{
	// オブジェクトの描画
	m_pObject->Draw();

#ifdef _DEBUG
	// デバッグ用シーン名描画
	if (static_cast<int>(m_stageKind) == 0)
	{
		DrawFormatString(0, 0, 0xffffff, "StageTest");
	}
	else
	{
		DrawFormatString(0, 0, 0xffffff, "Stage%d", static_cast<int>(m_stageKind));
	}
#endif // _DEBUG
}

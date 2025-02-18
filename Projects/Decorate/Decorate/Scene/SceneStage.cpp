#include "SceneStage.h"

#include "SceneClear.h"
#include "SceneGameOver.h"
#include "SceneGear.h"
#include "SceneOption.h"
#include "SceneSelect.h"

#include "../Object/ObjectManager.h"
#include "../Object/Player/Player.h"

#include "../Utility/Gear.h"
#include "../Utility/Pad.h"
#include "../Utility/SoundManager.h"

#include <string>
#include <cassert>

namespace
{
	constexpr float kCameraNear = 5.0f;		// カメラ手前クリップ距離
	constexpr float kCameraFar = 5000.0f;	// カメラ奥クリップ距離
	constexpr int kInfoPosX = 1500;			// 操作説明画像X座標
	constexpr int kInfoPosY = 0;			// 操作説明画像Y座標

	constexpr int kStageNum = 2;			// ステージ数

	constexpr int kClearBackAlpha = 200;	// クリア時間の背景の暗さ

	const std::string kInfoFilePath = "Data/Image/Info.png";	// 操作説明画像ファイルパス

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
	m_sceneType(SceneType::Main),
	m_pGear(std::make_shared<Gear>(stage)),
	m_pObject(std::make_shared<ObjectManager>(stage, m_pGear)),
	m_nextScene(nullptr)
{
	// 画像のロード
	m_infoH = LoadGraph(kInfoFilePath.c_str());
}

SceneStage::~SceneStage()
{
	// BGMを止める
	SoundManager::GetInstance().DesignationStopSound("BossScene");
	SoundManager::GetInstance().DesignationStopSound(kStageBGMPath[static_cast<int>(m_stageKind)].c_str());

	// 画面のデリート
	DeleteGraph(m_infoH);
}

void SceneStage::Init()
{
	// BGMを流す
	SoundManager::GetInstance().Play(kStageBGMPath[static_cast<int>(m_stageKind)].c_str(), false);
}

std::shared_ptr<SceneBase> SceneStage::Update()
{
	m_nextScene = shared_from_this();

	// シーンタイプによって処理を変える
	switch (m_sceneType)
	{
	case SceneStage::Main:
		// オブジェクトの更新
		m_pObject->Update();

		// プレイヤーが死んだ場合
		if (m_pObject->IsGameOver())
		{
			// ゲームオーバーシーンに遷移
			m_nextScene = std::make_shared<SceneGameOver>(m_stageKind);
		}
		// ボスを倒した場合
		if (m_pObject->IsGameClear() && !m_isGameClear)
		{
			// ゲームクリアシーンに遷移
			//return std::make_shared<SceneClear>(m_stageKind);
			m_sceneType = SceneType::Clear;
		}
		// スタートボタンを押したらオプション画面を出す
		if (Pad::IsTrigger(PAD_INPUT_8))
		{
			m_nextScene = std::make_shared<SceneOption>(shared_from_this(), m_pGear);
		}

		// ボスバトルになったらBGMを変える
		if (m_pObject->IsBossButtle())
		{
			// BGMを止める
			SoundManager::GetInstance().DesignationStopSound(kStageBGMPath[static_cast<int>(m_stageKind)]);
			// ボスバトルBGMを流す
			SoundManager::GetInstance().Play("BossScene", false);
		}
		break;
	case SceneStage::Clear:

		UpdateClearScene();
		
		break;
	default:
		break;
	}

	return m_nextScene;
}

void SceneStage::Draw()
{
	// オブジェクトの描画
	m_pObject->Draw();

	// 操作説明画像描画
	DrawGraph(kInfoPosX, kInfoPosY, m_infoH, true);

	// クリアの時のみ描画
	if (m_sceneType)
	{
		DrawClearScene();
	}

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

void SceneStage::SetPlayerStatus()
{
	m_pObject->GetPlayer()->SetStatus();
}

void SceneStage::UpdateClearScene()
{
	// 仮
	if (Pad::IsTrigger(PAD_INPUT_1))
	{
		m_nextScene = std::make_shared<SceneSelect>();
	}

}

void SceneStage::DrawClearScene()
{
	// 背景を暗くする
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, kClearBackAlpha);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

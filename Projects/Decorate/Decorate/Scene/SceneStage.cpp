#include "SceneStage.h"

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
	
	constexpr int kStageNum = 2;			// ステージ数

	constexpr int kClearBackAlpha = 200;	// クリア時間の背景の暗さ

	constexpr int kMouseCursorSpeed = 60;				// マウスカーソル速度
	constexpr float kMouseCursorSinSpeed = 0.04f;		// マウスカーソル拡縮速度
	constexpr float kMouseCursorAnimSwing = 0.12f;		// マウスカーソル拡縮幅
	constexpr float kNormalMouseCursorExtRate = 1.0f;	// 通常時マウスカーソル拡大率

	/*画像座標関係*/
	constexpr int kInfoPosX = 1500;									// 操作説明画像X座標
	constexpr int kInfoPosY = 0;									// 操作説明画像Y座標
	constexpr int kClearTextPosX = 260;								// クリアテキスト画像X座標
	constexpr int kClearTextPosY = 50;								// クリアテキスト画像Y座標
	constexpr int kBoxPosX = 190;									// ボックス画像X座標
	constexpr int kBoxPosY = 220;									// ボックス画像Y座標
	constexpr int kCursorLeftPosX = 348;							// カーソル画像左側X座標
	constexpr int kCursorRightPosX = kCursorLeftPosX + 692;			// カーソル画像右側X座標
	constexpr int kCursorPosY = 650;								// カーソル画像Y座標
	constexpr int kMouseCursorLeftPosX = kCursorLeftPosX + 430;		// マウスカーソル画像左側X座標
	constexpr int kMouseCursorRightPosX = kCursorRightPosX + 430;	// マウスカーソル画像右側X座標
	constexpr int kMouseCursorPosY = kCursorPosY + 230;				// マウスカーソル画像Y座標

	const std::string kInfoFilePath = "Data/Image/Info.png";					// 操作説明画像ファイルパス
	const std::string kClearTextPath = "Data/Image/Result/ClearText.png";		// クリアテキスト画像ファイルパス
	const std::string kBoxPath = "Data/Image/Result/Box.png";					// ボックス画像ファイルパス
	const std::string kCursorPath = "Data/Image/Result/Cursor.png";				// カーソル画像ファイルパス
	const std::string kMouseCursorPath = "Data/Image/Result/MouseCursor.png";	// マウスカーソル画像ファイルパス

	// ステージによってBGMを変える
	const std::string kStageBGMPath[kStageNum] =
	{
		"Stage1Scene",
		"Stage1Scene",
	};
}

SceneStage::SceneStage(Game::StageKind stage):
	m_mouseCursorPosX(0),
	m_isGameClear(false),
	m_isGameOver(false),
	m_mouseCursorSinCount(0.0f),
	m_mouseCursorScaleSize(0.0f),
	m_mouseCursorScaleRate(0.0f),
	m_isLeft(true),
	m_isCursorMove(false),
	m_stageKind(stage),
	m_sceneType(SceneType::Main),
	m_pGear(std::make_shared<Gear>(stage)),
	m_pObject(std::make_shared<ObjectManager>(stage, m_pGear)),
	m_nextScene(nullptr)
{
	// 画像のロード
	LoadImg();
}

SceneStage::~SceneStage()
{
	// BGMを止める
	SoundManager::GetInstance().DesignationStopSound("BossScene");
	SoundManager::GetInstance().DesignationStopSound(kStageBGMPath[static_cast<int>(m_stageKind)].c_str());

	// 画面のデリート
	DeleteImg();
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
	case SceneType::Main:

		// ボスを倒した場合
		if (m_pObject->IsGameClear() && !m_isGameClear)
		{
			// ゲームクリアシーンに遷移
			m_sceneType = SceneType::Clear;
			// 全サウンドを止める(歩行SE等が残る可能性があるため)		
			SoundManager::GetInstance().StopAllSound();

			// 以下の処理はしない
			break;
		}
		// プレイヤーが死んだ場合
		if (m_pObject->IsGameOver())
		{
			// ゲームオーバーシーンに遷移
			m_nextScene = std::make_shared<SceneGameOver>(m_stageKind);

			// 以下の処理はしない
			break;

		}

		// オブジェクトの更新
		m_pObject->Update();
	
		// スタートボタンを押した場合
		if (Pad::IsTrigger(PAD_INPUT_8))
		{
			// オプション画面を出す
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
	case SceneType::Clear:

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
	if (m_sceneType == SceneType::Clear)
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
		DrawFormatString(0, 0, 0xffffff, "Stage:%d", static_cast<int>(m_stageKind));
	}
#endif // _DEBUG
}

void SceneStage::SetPlayerStatus()
{
	m_pObject->GetPlayer()->SetStatus();
}

void SceneStage::UpdateClearScene()
{
	// 左右ボタンを押されたらカーソルを移動する
	if (Pad::IsTrigger(PAD_INPUT_LEFT))
	{
		// サウンドを流す
		if (!m_isLeft)
		{
			SoundManager::GetInstance().Play("CursorMove", true);
			m_isCursorMove = true;
		}
		m_isLeft = true;
	}
	else if (Pad::IsTrigger(PAD_INPUT_RIGHT))
	{
		// サウンドを流す
		if (!m_isLeft)
		{
			SoundManager::GetInstance().Play("CursorMove", true);
			m_isCursorMove = true;
		}
		m_isLeft = false;
	}

	// カーソルが左にあった場合
	if (m_isLeft)
	{
		// Aボタンが押されたらセレクトに戻る
		if (Pad::IsTrigger(PAD_INPUT_1))
		{
			m_nextScene = std::make_shared<SceneSelect>();
		}

		// マウスカーソルを動かす
		m_mouseCursorPosX -= kMouseCursorSpeed;
		if (m_mouseCursorPosX <= kMouseCursorLeftPosX)
		{
			m_mouseCursorPosX = kMouseCursorLeftPosX;
			m_isCursorMove = false;
		}

	}
	// カーソルが右にあった場合
	else
	{
		// Aボタンが押されたらステージに戻る
		if (Pad::IsTrigger(PAD_INPUT_1))
		{
			m_nextScene = std::make_shared<SceneStage>(m_stageKind);
		}

		// マウスカーソルを動かす
		m_mouseCursorPosX += kMouseCursorSpeed;
		if (m_mouseCursorPosX >= kMouseCursorRightPosX)
		{
			m_mouseCursorPosX = kMouseCursorRightPosX;
			m_isCursorMove = false;
		}
	}

	// マウスカーソル拡縮処理
	m_mouseCursorSinCount += kMouseCursorSinSpeed;
	m_mouseCursorScaleRate = sinf(m_mouseCursorSinCount) * kMouseCursorAnimSwing;
	if (!m_isCursorMove)
	{
		m_mouseCursorScaleSize = kNormalMouseCursorExtRate + m_mouseCursorScaleRate;
	}
	else
	{
		m_mouseCursorScaleSize = kNormalMouseCursorExtRate;
	}
}

void SceneStage::DrawClearScene()
{
	// 背景を暗くする
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, kClearBackAlpha);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// テキスト描画
	DrawGraph(kClearTextPosX, kClearTextPosY, m_clearTextH, true);

	// ボックス描画
	DrawGraph(kBoxPosX, kBoxPosY, m_boxH, true);

	// カーソル描画
	if (m_isLeft)
	{
		DrawGraph(kCursorLeftPosX, kCursorPosY, m_cursorH, true);
	}
	else
	{
		DrawGraph(kCursorRightPosX, kCursorPosY, m_cursorH, true);
	}
	// マウスカーソル描画
	DrawRotaGraph(m_mouseCursorPosX, kMouseCursorPosY,
		m_mouseCursorScaleSize, 0.0,
		m_mouseCursorH, true);
}

void SceneStage::LoadImg()
{
	m_infoH = LoadGraph(kInfoFilePath.c_str());
	assert(m_infoH != -1);
	m_clearTextH = LoadGraph(kClearTextPath.c_str());
	assert(m_clearTextH != -1);
	m_boxH = LoadGraph(kBoxPath.c_str());
	assert(m_boxH != -1);
	m_cursorH = LoadGraph(kCursorPath.c_str());
	assert(m_cursorH != -1);
	m_mouseCursorH = LoadGraph(kMouseCursorPath.c_str());
	assert(m_mouseCursorH != -1);
}

void SceneStage::DeleteImg()
{
	DeleteGraph(m_infoH);
	DeleteGraph(m_clearTextH);
	DeleteGraph(m_boxH);
	DeleteGraph(m_cursorH);
	DeleteGraph(m_mouseCursorH);
}
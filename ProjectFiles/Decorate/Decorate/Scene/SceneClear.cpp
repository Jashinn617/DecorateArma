#include "SceneClear.h"

#include "SceneSelect.h"
#include "SceneStage.h"

#include "../Utility/Pad.h"

#include <cassert>

namespace
{
	const char* const kTextPath = "Data/Image/Clear/GameClear.png";		// テキスト画像ファイルパス
	const char* const kOneMorePath = "Data/Image/OneMore.png";			// もう一度画像ファイルパス
	const char* const kSelectPath = "Data/Image/Select.png";			// セレクト画像ファイルパス
	const char* const kResultPath = "Data/Image/Clear/Result.png";		// リザルト画像ファイルパス
	const char* const kModelPath = "Data/Model/Player/Player.mv1";		// モデルファイルパス

	constexpr int kAnimNum = 1;											// モデルのアニメーション番号
	constexpr float kAnimSpeed = 1.0f;									// アニメーション速度

	constexpr float kNear = 5.0f;										// カメラの一番近い描画距離
	constexpr float kFar = 500.0f;										// カメラの一番遠い描画距離
	constexpr int kTextPosX = 550;
	constexpr int kTextPosY = 50;

	constexpr int kSelectPosX = 1000;
	constexpr int kOneMorePosX = kSelectPosX + 550;
	constexpr int kBoxPosY = 850;

	constexpr float kBoxSinSpeed = 0.07f;								// ボックス拡縮速度
	constexpr float kBoxAnimSwing = 0.03f;								// ボックス拡縮幅
	constexpr float kNormalBoxExtRate = 1.0f;							// 通常時ボックス拡大率

	constexpr VECTOR kCameraPos = { 0.0f,120.0f,0.0f };					// カメラ座標
	constexpr VECTOR kModelPos = { 30.0f,0.0f,130.0f };					// モデル座標
	constexpr VECTOR kTargetPos = { 0.0f,60.0f,100.0f };				// ターゲット座標
	constexpr VECTOR kModelScale = { 0.5f,0.5f,0.5f };					// モデルスケール
}

SceneClear::SceneClear(Game::StageKind stageKind):
	m_animPlayTime(0),
	m_boxSinCount(0.0),
	m_expansionBox(0.0),
	m_oneMoreBoxExtRate(0.0),
	m_selectBoxExtRate(0.0),
	m_isLeft(true),
	m_stageKind(stageKind)
{
	// 画像ロード
	m_textH = LoadGraph(kTextPath);
	// ロードに失敗したら止める
	assert(m_textH != -1);
	// 以下同じ処理
	m_oneMoreBoxH = LoadGraph(kOneMorePath);
	assert(m_oneMoreBoxH != -1);
	m_selectBoxH = LoadGraph(kSelectPath);
	assert(m_selectBoxH != -1);
	m_resultBoxH = LoadGraph(kResultPath);
	assert(m_resultBoxH != -1);
	// モデルロード
	m_modelH = MV1LoadModel(kModelPath);
	assert(m_modelH != -1);
	// アニメーションのアタッチ
	m_animIndex = MV1AttachAnim(m_modelH, kAnimNum,-1,false);
	// アニメーションの総再生時間の取得
	m_animTotalTime = MV1GetAttachAnimTotalTime(m_modelH, m_animIndex);
}

SceneClear::~SceneClear()
{
	// 画像デリート
	DeleteGraph(m_textH);
	DeleteGraph(m_oneMoreBoxH);
	DeleteGraph(m_selectBoxH);
	DeleteGraph(m_resultBoxH);
	MV1DeleteModel(m_modelH);
}

void SceneClear::Init()
{
	// カメラ設定
	SetCameraNearFar(kNear, kFar);
	SetCameraPositionAndTarget_UpVecY(kCameraPos, kTargetPos);

	// モデル設定初期化
	MV1SetPosition(m_modelH, kModelPos);
	MV1SetScale(m_modelH, kModelScale);
}

std::shared_ptr<SceneBase> SceneClear::Update()
{
	m_nextScene = shared_from_this();

	// モデル更新
	UpdateModel();

	// ボックス更新
	UpdateBox();

	// カーソル更新
	UpdateCursor();

	return m_nextScene;
}

void SceneClear::Draw()
{
	// モデル描画
	DrawModel();

	// テキスト描画
	DrawGraph(kTextPosX, kTextPosY, m_textH, true);

	// ボックス描画
	DrawCursorBox();
}

void SceneClear::End()
{
}

void SceneClear::UpdateBox()
{
	// 選択中ボックス拡縮処理
	m_boxSinCount += kBoxSinSpeed;
	m_expansionBox = sinf(m_boxSinCount) * kBoxAnimSwing;

	// 左側のボックスが選ばれていた場合
	if (m_isLeft)
	{
		m_oneMoreBoxExtRate = kNormalBoxExtRate;
		m_selectBoxExtRate = kNormalBoxExtRate + m_expansionBox;

		// Aボタンが押された場合はセレクトに飛ぶ
		if (Pad::IsTrigger(PAD_INPUT_1))
		{
			m_nextScene = std::make_shared<SceneSelect>();
		}
	
	}
	// 右側のボックスが選ばれていた場合
	else
	{
		m_oneMoreBoxExtRate = kNormalBoxExtRate + m_expansionBox;
		m_selectBoxExtRate = kNormalBoxExtRate;

		// Aボタンが押された場合は直前のステージに飛ぶ
		if (Pad::IsTrigger(PAD_INPUT_1))
		{
			m_nextScene = std::make_shared<SceneStage>(m_stageKind);
		}
	}
}

void SceneClear::DrawCursorBox()
{
	// セレクト
	DrawRotaGraph(kSelectPosX, kBoxPosY,
		m_selectBoxExtRate, 0.0,
		m_selectBoxH, true);

	// もう一度
	DrawRotaGraph(kOneMorePosX, kBoxPosY,
		m_oneMoreBoxExtRate, 0.0,
		m_oneMoreBoxH, true);
}

void SceneClear::UpdateCursor()
{
	// 左右ボタンが押されたら上下を入れ替える
	if (Pad::IsTrigger(PAD_INPUT_LEFT) || Pad::IsTrigger(PAD_INPUT_RIGHT))
	{
		m_isLeft = !m_isLeft;
	}
}

void SceneClear::UpdateModel()
{
	// アニメーションを進める
	m_animPlayTime += kAnimSpeed;

	// 再生時間が総再生時間になったらアニメーションをループさせる
	if (m_animPlayTime >= m_animTotalTime)
	{
		m_animPlayTime -= m_animTotalTime;
	}

	// 再生時間の設定
	MV1SetAttachAnimTime(m_modelH, m_animIndex, m_animPlayTime);
}

void SceneClear::DrawModel()
{
	// モデル描画
	MV1DrawModel(m_modelH);
}

#include "SceneGameOver.h"

#include "SceneSelect.h"
#include "SceneStage.h"

#include "../Utility/Pad.h"
#include "../Utility/SoundManager.h"

#include <cassert>

namespace
{
	const char* const kTextPath = "Data/Image/GameOver/GameOver.png";	// テキスト画像ファイルパス
	const char* const kOneMorePath = "Data/Image/OneMore.png";			// もう一度画像ファイルパス
	const char* const kSelectPath = "Data/Image/Select.png";			// セレクト画像ファイルパス
	const char* const kResultPath = "Data/Image/Clear/Result.png";		// リザルト画像ファイルパス
	const char* const kModelPath = "Data/Model/Player/Player.mv1";		// モデルファイルパス
	const char* const kFfoModelPath = "Data/Model/ShotWeapon/UFO.mv1";  // UFOモデルファイルパス

	constexpr int kImgDispSpeed = 7;									// 画像表示速度
	constexpr int kMaxAlpha = 255;										// 画像の最大透明度

	constexpr int kAnimNum = 8;											// モデルのアニメーション番号
	constexpr float kAnimSpeed = 1.0f;									// アニメーション速度

	constexpr float kNear = 5.0f;										// カメラの一番近い描画距離
	constexpr float kFar = 500.0f;										// カメラの一番遠い描画距離
	constexpr int kTextPosX = 550;
	constexpr int kTextPosY = 50;

	constexpr int kSelectPosX = 580;
	constexpr int kOneMorePosX = kSelectPosX + 800;
	constexpr int kBoxPosY = 500;
	constexpr float kModelRotY = 125 * DX_PI_F / 180.0f;				// モデルの角度

	constexpr float kUfoSinSpeed = 0.04f;								// UFO昇降速度
	constexpr float kUfoSinSwing = 0.08f;								// UFO昇降幅
	constexpr float kUfoSpeed = 0.2f;									// UFO移動速度

	constexpr float kBoxSinSpeed = 0.07f;								// ボックス拡縮速度
	constexpr float kBoxAnimSwing = 0.03f;								// ボックス拡縮幅
	constexpr float kNormalBoxExtRate = 1.0f;							// 通常時ボックス拡大率

	constexpr VECTOR kCameraPos = { 0.0f,15.0f,0.0f };					// カメラ座標
	constexpr VECTOR kModelPos = { 40.0f,0.0f,100.0f };					// モデル座標
	constexpr VECTOR kTargetPos = { 0.0f,25.0f,100.0f };				// ターゲット座標
	constexpr VECTOR kModelScale = { 0.5f,0.5f,0.5f };					// モデルスケール
	constexpr VECTOR kModelRot = { 0.0f,kModelRotY,0.0f };				// モデル角度
	constexpr VECTOR kUfoStartPos = { 100.0f,20.0f,100.0f };			// UFOの初期座標
	constexpr VECTOR kUfoGoalPos = { 0.0f,20.0f,100.0f };				// UFOの最終座標
	constexpr VECTOR kUfoScale = { 0.2f,0.2f,0.2f };					// UFOのスケール
}

SceneGameOver::SceneGameOver(Game::StageKind stageKind):
	m_animPlayTime(0),
	m_imgAlpha(0),
	m_boxSinCount(0.0),
	m_expansionBox(0.0),
	m_oneMoreBoxExtRate(0.0),
	m_selectBoxExtRate(0.0),
	m_ufoSinCount(0.0),
	m_ufoSinPosY(0.0),
	m_isLeft(true),
	m_isImg(false),
	m_isUfoMove(false),
	m_isFallDownSE(false),
	m_ufoPos(kUfoStartPos),
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
	m_ufoModelH = MV1LoadModel(kFfoModelPath);
	assert(m_ufoModelH != -1);
	// アニメーションのアタッチ
	m_animIndex = MV1AttachAnim(m_modelH, kAnimNum, -1, false);
	// アニメーションの総再生時間の取得
	m_animTotalTime = MV1GetAttachAnimTotalTime(m_modelH, m_animIndex);
	
	

	// SEを鳴らす
	SoundManager::GetInstance().Play("Dead", false);
}

SceneGameOver::~SceneGameOver()
{
	// 画像デリート
	DeleteGraph(m_textH);
	DeleteGraph(m_oneMoreBoxH);
	DeleteGraph(m_selectBoxH);
	DeleteGraph(m_resultBoxH);
	MV1DeleteModel(m_modelH);
}

void SceneGameOver::Init()
{
	// カメラ設定
	SetCameraNearFar(kNear, kFar);
	SetCameraPositionAndTarget_UpVecY(kCameraPos, kTargetPos);

	// モデル設定初期化
	MV1SetPosition(m_modelH, kModelPos);
	MV1SetScale(m_modelH, kModelScale);
	MV1SetRotationXYZ(m_modelH, kModelRot);
	MV1SetPosition(m_ufoModelH, m_ufoPos);
	MV1SetScale(m_ufoModelH, kUfoScale);
}

std::shared_ptr<SceneBase> SceneGameOver::Update()
{
	m_nextScene = shared_from_this();

	// モデル更新
	UpdateModel();

	// UFO更新
	UpdateUFO();

	// ボックス更新
	UpdateBox();

	// カーソル更新
	UpdateCursor();	

	// 最初のSEが流れ終わったら次のSEを流す
	if (!SoundManager::GetInstance().IsDesignationChackPlaySound("Dead") && !m_isFallDownSE)
	{
		SoundManager::GetInstance().Play("FallDown", false);
		m_isFallDownSE = true;
	}

	// 画像表示フラグが立っていたら少しずつ画像をフェードインしていく
	if (m_isImg)
	{
		// 画像の不透明度を上げる
		m_imgAlpha += kImgDispSpeed;
		// 画像の不透明度が255以上になったら処理を終わる
		if (m_imgAlpha >= kMaxAlpha)
		{
			m_imgAlpha = kMaxAlpha;
			m_isImg = false;
		}
	}

	return m_nextScene;
}

void SceneGameOver::Draw()
{
	// モデル描画
	DrawModel();

	// 透明度の設定
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_imgAlpha);
	// テキスト描画
	DrawGraph(kTextPosX, kTextPosY, m_textH, true);
	// ボックス描画
	DrawCursorBox();
	// 透明度の設定を戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneGameOver::End()
{
	// サウンドを止める
	SoundManager::GetInstance().DesignationStopSound("GameOverScene");
}

void SceneGameOver::UpdateBox()
{
	// 選択中ボックス拡縮処理
	m_boxSinCount += kBoxSinSpeed;
	m_expansionBox = sinf(m_boxSinCount) * kBoxAnimSwing;

	// 左側のボックスが選ばれていた場合
	if (m_isLeft)
	{
		m_oneMoreBoxExtRate = kNormalBoxExtRate;
		m_selectBoxExtRate = kNormalBoxExtRate + m_expansionBox;

		if (m_isImg || m_imgAlpha >= kMaxAlpha)
		{
			// Aボタンが押された場合はセレクトに飛ぶ
			if (Pad::IsTrigger(PAD_INPUT_1))
			{
				m_nextScene = std::make_shared<SceneSelect>();
				// SEを流す
				SoundManager::GetInstance().Play("SelectSelect", false);
			}
		}
	}
	// 右側のボックスが選ばれていた場合
	else
	{
		m_oneMoreBoxExtRate = kNormalBoxExtRate + m_expansionBox;
		m_selectBoxExtRate = kNormalBoxExtRate;

		if (m_isImg || m_imgAlpha >= kMaxAlpha)
		{
			// Aボタンが押された場合は直前のステージに飛ぶ
			if (Pad::IsTrigger(PAD_INPUT_1))
			{
				m_nextScene = std::make_shared<SceneStage>(m_stageKind);
				// SEを流す
				SoundManager::GetInstance().Play("StageSelect", false);
			}
		}	
	}
}

void SceneGameOver::DrawCursorBox()
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

void SceneGameOver::UpdateCursor()
{
	// 左右ボタンが押されたら上下を入れ替える
	if (Pad::IsTrigger(PAD_INPUT_LEFT) || Pad::IsTrigger(PAD_INPUT_RIGHT))
	{
		m_isLeft = !m_isLeft;
	}
}

void SceneGameOver::UpdateModel()
{
	// アニメーションを進める
	m_animPlayTime += kAnimSpeed;

	// 再生時間が総再生時間になったらアニメーションを止める
	if (m_animPlayTime >= m_animTotalTime)
	{
		m_animPlayTime = m_animTotalTime;
		// BGMを流す
		SoundManager::GetInstance().Play("GameOverScene", false);
		// 画像表示フラグを立てる
		m_isImg = true;
		// UFOを動かす
		m_isUfoMove = true;
	}

	// 再生時間の設定
	MV1SetAttachAnimTime(m_modelH, m_animIndex, m_animPlayTime);
}

void SceneGameOver::UpdateUFO()
{
	if (!m_isUfoMove) return;

	// UFOの上下移動
	m_ufoSinCount += kUfoSinSpeed;
	m_ufoSinPosY = sinf(m_ufoSinCount) * kUfoSinSwing;
	m_ufoPos.y += m_ufoSinPosY;

	// UFOの移動
	m_ufoPos.x -= kUfoSpeed;
	if (m_ufoPos.x <= kUfoGoalPos.x)
	{
		m_ufoPos.x = kUfoGoalPos.x;
	}
	
	// UFOの座標更新
	MV1SetPosition(m_ufoModelH, m_ufoPos);
}

void SceneGameOver::DrawModel()
{
	// モデル描画
	MV1DrawModel(m_modelH);
	MV1DrawModel(m_ufoModelH);
}

#include "SceneGameOver.h"

#include "SceneSelect.h"
#include "SceneStage.h"

#include "../Utility/Pad.h"
#include "../Utility/SoundManager.h"

#include <string>
#include <cassert>

namespace
{
	const std::string kTextPath = "Data/Image/Result/GameOverText.png";			// テキスト画像ファイルパス
	const std::string kBoxPath = "Data/Image/Result/Box.png";					// ボックス画像ファイルパス
	const std::string kCursorPath = "Data/Image/Result/Cursor.png";				// カーソル画像ファイルパス
	const std::string kMouseCursorPath = "Data/Image/Result/MouseCursor.png";	// マウスカーソル画像ファイルパス
	const std::string kModelPath = "Data/Model/Player/Player.mv1";				// モデルファイルパス

	constexpr int kImgDispSpeed = 7;									// 画像表示速度
	constexpr int kMaxAlpha = 255;										// 画像の最大透明度

	constexpr int kAnimNum = 9;											// モデルのアニメーション番号
	constexpr float kAnimSpeed = 0.5f;									// モデルのアニメーション速度
	constexpr float kModelRotY = -45 * DX_PI_F / 180.0f;				// モデルの角度

	constexpr float kNear = 5.0f;										// カメラの一番近い描画距離
	constexpr float kFar = 500.0f;										// カメラの一番遠い描画距離

	constexpr int kMouseCursorSpeed = 60;								// マウスカーソル速度
	constexpr float kMouseCursorSinSpeed = 0.04f;						// マウスカーソル拡縮速度
	constexpr float kMouseCursorAnimSwing = 0.12f;						// マウスカーソル拡縮幅
	constexpr float kNormalMouseCursorExtRate = 1.0f;					// 通常時マウスカーソル拡大率

	/*座標関係*/
	// 画像
	constexpr int kTextPosX = 500;										// クリアテキスト画像X座標
	constexpr int kTextPosY = 50;										// クリアテキスト画像Y座標
	constexpr int kBoxPosX = 190;										// ボックス画像X座標
	constexpr int kBoxPosY = 220;										// ボックス画像Y座標
	constexpr int kCursorLeftPosX = 348;								// カーソル画像左側X座標
	constexpr int kCursorRightPosX = kCursorLeftPosX + 692;				// カーソル画像右側X座標
	constexpr int kCursorPosY = 650;									// カーソル画像Y座標
	constexpr int kMouseCursorLeftPosX = kCursorLeftPosX + 430;			// マウスカーソル画像左側X座標
	constexpr int kMouseCursorRightPosX = kCursorRightPosX + 430;		// マウスカーソル画像右側X座標
	constexpr int kMouseCursorPosY = kCursorPosY + 230;					// マウスカーソル画像Y座標
	// モデル
	constexpr VECTOR kCameraPos = { 0.0f,15.0f,0.0f };					// カメラ座標
	constexpr VECTOR kModelPos = { 40.0f,0.0f,100.0f };					// モデル座標
	constexpr VECTOR kTargetPos = { 0.0f,25.0f,100.0f };				// ターゲット座標
	constexpr VECTOR kModelScale = { 0.5f,0.5f,0.5f };					// モデルスケール
	constexpr VECTOR kModelRot = { 0.0f,kModelRotY,0.0f };				// モデル角度
}

SceneGameOver::SceneGameOver(Game::StageKind stageKind):
	m_mouseCursorPosX(0),
	m_animPlayTime(0),
	m_imgAlpha(0),
	m_mouseCursorSinCount(0.0),
	m_mouseCursorScaleSize(0.0),
	m_mouseCursorScaleRate(0.0),
	m_isLeft(true),
	m_isImg(false),
	m_isFallDownSE(false),
	m_isCursorMove(false),
	m_stageKind(stageKind)
{
	// 画像ロード
	LoadImg();
	// モデルロード
	LoadModel();

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
	DeleteImg();
	// モデルデリート
	DeleteModel();
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
}

std::shared_ptr<SceneBase> SceneGameOver::Update()
{
	m_nextScene = shared_from_this();

	// モデル更新
	UpdateModel();
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
	DrawGraph(kBoxPosX, kBoxPosY, m_boxH, true);
	// カーソル描画
	DrawCursor();
	// 透明度の設定を戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneGameOver::End()
{
	// サウンドを止める
	SoundManager::GetInstance().DesignationStopSound("GameOverScene");
}

void SceneGameOver::UpdateCursor()
{
	// 画像が完全に移っていなかった場合は処理をしない
	if (!m_isImg || m_imgAlpha < kMaxAlpha) return;

	// 左右ボタンが押されたらカーソルを移動する
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
		if (m_isLeft)
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
	}

	// 再生時間の設定
	MV1SetAttachAnimTime(m_modelH, m_animIndex, m_animPlayTime);
}

void SceneGameOver::DrawCursor()
{
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

void SceneGameOver::DrawModel()
{
	// モデル描画
	MV1DrawModel(m_modelH);
}

void SceneGameOver::LoadImg()
{
	m_textH = LoadGraph(kTextPath.c_str());
	assert(m_textH != -1);
	m_boxH = LoadGraph(kBoxPath.c_str());
	assert(m_boxH != -1);
	m_cursorH = LoadGraph(kCursorPath.c_str());
	assert(m_cursorH != -1);
	m_mouseCursorH = LoadGraph(kMouseCursorPath.c_str());
	assert(m_mouseCursorH != -1);
}

void SceneGameOver::LoadModel()
{
	m_modelH = MV1LoadModel(kModelPath.c_str());
	assert(m_modelH != -1);
}

void SceneGameOver::DeleteImg()
{
	DeleteGraph(m_textH);
	DeleteGraph(m_boxH);
	DeleteGraph(m_cursorH);
	DeleteGraph(m_mouseCursorH);
}

void SceneGameOver::DeleteModel()
{
	MV1DeleteModel(m_modelH);
}
#include "SceneTitle.h"

#include "SceneSelect.h"

#include "../Utility/Game.h"
#include "../Utility/Pad.h"
#include "../Utility/SoundManager.h"

#include "../Shader/ToonShader.h"

#include <string>
#include <cassert>

namespace
{
    const std::string kTitlePath = "Data/Image/Title/";                 // 画像ファイルパス
    const std::string kLogoPath = kTitlePath + "TitleLogo.png";         // タイトルロゴ
    const std::string kPressPath = kTitlePath + "PressButton.png";      // プレスボタンロゴ
    const std::string kModelPath = "Data/Model/Player/Player.mv1";  	// モデルファイルパス
	const std::string kFfoModelPath = "Data/Model/ShotWeapon/UFO.mv1";  // UFOモデルファイルパス

	constexpr int kBackImgNum = 6;									// 背景画像数
    constexpr int kBackBackNum = 1;		// 後ろの背景の数
    constexpr int kMiddleBackNum = 4;	// 真ん中の背景の数
    constexpr int kFrontBackNum = 1;	// 手前の背景の数

    constexpr int kIndexBackNum = 2;		// スクロールに必要な背景数
    constexpr int kScrollSpeedBack = 1;		// スクロール速度奥
    constexpr int kScrollSpeedMiddle = 2;	// スクロール速度真ん中
    constexpr int kScrollSpeedFront = 3;	// スクロール速度手前

    // 背景画像パス
    const std::string kBackPath[kBackImgNum] =
    {
        kTitlePath + "Background/back1.png",
        kTitlePath + "Background/back2.png",
        kTitlePath + "Background/back3.png",
        kTitlePath + "Background/back4.png",
        kTitlePath + "Background/back5.png",
        kTitlePath + "Background/back6.png",
    };

    constexpr int kAnimNum = 1;										// モデルのアニメーション番号
    constexpr float kAnimSpeed = 0.7f;								// アニメーション速度

    constexpr float kNear = 5.0f;									// カメラの一番近い描画距離
    constexpr float kFar = 500.0f;									// カメラの一番遠い描画距離

	constexpr int kFadeSpeed = 3;									// フェード速度

	constexpr int kFadeInMax = 255;									// フェードインの最大値

    constexpr int kLogoPosX = static_cast<int>(Game::kScreenWidth * 0.01f);           // ロゴX座標
    constexpr int kLogoPosY = static_cast<int>(Game::kScreenHeight * 0.02f);          // ロゴY座標
    constexpr int kPressPosX = static_cast<int>(Game::kScreenWidth * 0.22f);          // プレスX座標
    constexpr int kPressPosY = static_cast<int>(Game::kScreenHeight * 0.7f);          // プレスY座標

    constexpr int kPressAlphaUpSpeed = 3;					         // プレスボタンの透明度が上がる速度
    constexpr int kPressAlphaDownSpeed = 5;					        // プレスボタンの透明度が下がる速度

    constexpr float kModelRotY = 90 * DX_PI_F / 180.0f;			    // モデルの角度

	constexpr float kPressAlphaMax = 255.0f;						// プレスボタンの最大透明度
	constexpr float kPressAlphaMin = 0.0f;						    // プレスボタンの最小透明度
    constexpr float kSinSpeed = 0.04f;							    // UFO昇降速度
    constexpr float kSinSwing = 0.08f;							    // UFO昇降幅

    constexpr VECTOR kCameraPos = { 0.0f,65.0f,70.0f };				// カメラ座標
    constexpr VECTOR kModelPos = { 20.0f,0.0f,110.0f };				// モデル座標
    constexpr VECTOR kTargetPos = { 0.0f,65.0f,100.0f }; 			// ターゲット座標
    constexpr VECTOR kModelScale = { 0.5f,0.5f,0.5f };				// モデルスケール
    constexpr VECTOR kModelRot = { 0.0f,kModelRotY,0.0f };			// モデル角度
	constexpr VECTOR kUfoStartPos = { 15.0f,65.0f,130.0f };			// UFOの初期座標
	constexpr VECTOR kUfoScale = { 0.2f,0.2f,0.2f };				// UFOのスケール
}

SceneTitle::SceneTitle():
	m_scrollXBack(0),
	m_scrollXMiddle(0),
	m_scrollXFront(0),
    m_pressAlpha(0),
	m_fadeAlpha(kFadeInMax),
	m_animPlayTime(0),
	m_ufoSinCount(0.0f),
	m_ufoSinPosY(0.0f),
    m_isPressAlphaUp(true),
	m_isFadeIn(false),
	m_isFadeOut(true),
	m_isSceneEnd(false),
	m_ufoPos(kUfoStartPos),
	m_pToonShader(std::make_shared<ToonShader>())
{
    /*画像ロード*/
    for (int i = 0; i < kBackImgNum; i++)
    {
        m_backH.push_back(LoadGraph(kBackPath[i].c_str()));
        // ロードに失敗したら止める
        assert(m_backH[i] != -1);
    }
    m_titleLogoH = LoadGraph(kLogoPath.c_str());
    assert(m_titleLogoH != -1);
    m_pressTextH = LoadGraph(kPressPath.c_str());
    assert(m_pressTextH);

    /*モデルロード*/
	m_modelH = MV1LoadModel(kModelPath.c_str());
    assert(m_modelH != -1);
	m_ufoModelH = MV1LoadModel(kFfoModelPath.c_str());
	assert(m_ufoModelH != -1);
    // アニメーションのアタッチ
    m_animIndex = MV1AttachAnim(m_modelH, kAnimNum, -1, false);
    // アニメーションの総再生時間の取得
    m_animTotalTime = MV1GetAttachAnimTotalTime(m_modelH, m_animIndex);

    // モデルの頂点タイプの取得
    for (int i = 0; i < MV1GetTriangleListNum(m_modelH); i++)
    {
        // 頂点タイプの取得
        m_vertexShaderType.push_back(MV1GetTriangleListVertexType(m_modelH, i));
    }
	for (int i = 0; i < MV1GetTriangleListNum(m_ufoModelH); i++)
	{
		// 頂点タイプの取得
		m_ufoVertexShaderType.push_back(MV1GetTriangleListVertexType(m_ufoModelH, i));
	}

    // 背景画像サイズ取得
    GetGraphSize(m_backH[0], &m_backWidth, &m_backHeight);
}

SceneTitle::~SceneTitle()
{
    /*画像デリート*/
    for (auto& img : m_backH)
    {
        DeleteGraph(img);
    }
    DeleteGraph(m_titleLogoH);
    DeleteGraph(m_pressTextH);
	MV1DeleteModel(m_modelH);
	MV1DeleteModel(m_ufoModelH);

    // サウンドを止める
    SoundManager::GetInstance().DesignationStopSound("TitleScene");
}

void SceneTitle::Init()
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

    // BGMを流す
	SoundManager::GetInstance().Play("TitleScene", false);
}

std::shared_ptr<SceneBase> SceneTitle::Update()
{
    // モデル更新
	UpdateModel();

    // 背景スクロール値の更新
    m_scrollXBack -= kScrollSpeedBack;
    m_scrollXMiddle -= kScrollSpeedMiddle;
    m_scrollXFront -= kScrollSpeedFront;

	// プレスボタンの透明度変更処理
    PressFade();

	// UFOの昇降処理
	m_ufoSinCount += kSinSpeed;
	m_ufoSinPosY = sinf(m_ufoSinCount) * kSinSwing;
    m_ufoPos.y += m_ufoSinPosY;
	// UFOの座標更新
	MV1SetPosition(m_ufoModelH, m_ufoPos);

    // Aボタンが押されたらセレクト画面に遷移する
    if (Pad::IsTrigger(PAD_INPUT_1))
    {
		// SEを流す
		SoundManager::GetInstance().Play("TitleStart", false);
		// フェードイン
		m_isFadeIn = true;
    }

	// シーン終了処理
    if (m_isSceneEnd)
    {
        return std::make_shared<SceneSelect>();
    }

    // フェード
	FadeIn();
	FadeOut();

    return shared_from_this();
}

void SceneTitle::Draw()
{
	// 背景描画
	DrawBack();

    // モデル描画
    DrawModel();

    // ロゴ描画
    DrawGraph(kLogoPosX, kLogoPosY, m_titleLogoH, true);

    // プレスボタン描画
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_pressAlpha);
    DrawGraph(kPressPosX, kPressPosY, m_pressTextH, true);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// フェード描画(白)
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_fadeAlpha);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0xffffff, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);


#ifdef _DEBUG
    DrawFormatString(0, 0, 0xffffff, "Title");
#endif // _DEBUG
}

void SceneTitle::End()
{
   /*処理無し*/
}

void SceneTitle::PressFade()
{
	// プレスボタンの透明度変更
    if (m_isPressAlphaUp)
    {
        // 透明度を上げる
        m_pressAlpha += kPressAlphaUpSpeed;
		// 透明度が最大値になったら透明度を下げる
        if (m_pressAlpha >= kPressAlphaMax)
        {
			m_isPressAlphaUp = false;
        }
    }
    else
    {
		// 透明度を下げる
        m_pressAlpha -= kPressAlphaDownSpeed;
		// 透明度が最小値になったら透明度を上げる
        if (m_pressAlpha <= kPressAlphaMin)
        {
			m_isPressAlphaUp = true;
        }
    }
}

void SceneTitle::UpdateModel()
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

void SceneTitle::DrawModel()
{
    // モデルをフレームごとに描画する
    // プレイヤー
    for (int i = 0; i < MV1GetTriangleListNum(m_modelH); i++)
    {
        // シェーダの設定
        m_pToonShader->SetShader(m_vertexShaderType[i]);

        // 描画
        MV1DrawTriangleList(m_modelH, i);
    }
    // シェーダを使わない設定にする
    m_pToonShader->ShaderEnd();

	// UFO
	for (int i = 0; i < MV1GetTriangleListNum(m_ufoModelH); i++)
	{
		// シェーダの設定
		m_pToonShader->SetShader(m_ufoVertexShaderType[i]);
		// 描画
		MV1DrawTriangleList(m_ufoModelH, i);
	}
	// シェーダを使わない設定にする
	m_pToonShader->ShaderEnd();
}

void SceneTitle::DrawBack()
{
	// 描画した背景数
	int drawBack = 0;
	/*スクロール処理
	手前側に来るほどスクロールが早くなる*/
	// 後ろ
	int scroll = m_scrollXBack % m_backWidth;
	for (int i = 0; i < kBackBackNum; i++)
	{
		for (int index = 0; index < kIndexBackNum; index++)
		{
			DrawGraph(-scroll + index * -m_backWidth,
				0, m_backH[drawBack + i], true);
		}
	}
	// 真ん中
	drawBack += kBackBackNum;
	scroll = m_scrollXMiddle % m_backWidth;
	for (int i = 0; i < kMiddleBackNum; i++)
	{
		for (int index = 0; index < kIndexBackNum; index++)
		{
			DrawGraph(-scroll + index * -m_backWidth,
				0, m_backH[drawBack + i], true);
		}
	}
	// 手前
	drawBack += kMiddleBackNum;
	scroll = m_scrollXFront % m_backWidth;
	for (int i = 0; i < kFrontBackNum; i++)
	{
		for (int index = 0; index < kIndexBackNum; index++)
		{
			DrawGraph(-scroll + index * -m_backWidth,
				0, m_backH[drawBack + i], true);
		}
	}
}

void SceneTitle::FadeIn()
{
	// フェードイン中でないなら処理をしない
	if (!m_isFadeIn) return;
	// フェードイン処理
	m_fadeAlpha += kFadeSpeed;
    // フェード仕切ったら場面転換
	if (m_fadeAlpha >= kFadeInMax)
	{
		m_fadeAlpha = kFadeInMax;
		m_isFadeIn = false;
		m_isSceneEnd = true;
	}

}

void SceneTitle::FadeOut()
{
	// フェードアウト中でないなら処理をしない
    if (!m_isFadeOut) return;
    // フェードイン中だったら処理をやめる
	if (m_isFadeIn) return;

	// フェードアウト処理
	m_fadeAlpha -= kFadeSpeed;
    // 完全にフェードが明けたら処理をやめる
	if (m_fadeAlpha <= 0)
	{
		m_fadeAlpha = 0;
		m_isFadeOut = false;
	}

}

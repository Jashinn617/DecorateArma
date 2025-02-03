#pragma once
#include "SceneBase.h"

#include <vector>

class ToonShader;

/// <summary>
/// タイトル画面
/// </summary>
class SceneTitle : public SceneBase
{
public:	// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneTitle();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~SceneTitle();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() override final;

	/// <summary>
	/// 更新
	/// </summary>
	/// <returns>次のシーン</returns>
	virtual std::shared_ptr<SceneBase> Update() override final;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() override final;

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void End() override final;

	/// <summary>
	/// シーン種類の取得
	/// </summary>
	/// <returns>シーンの種類</returns>
	virtual SceneKind GetSceneKind() override final { return SceneKind::Gear; }

private:	// 関数
	/// <summary>
	/// プレステキストの透明度変更
	/// </summary>
	void PressFade();

	/// <summary>
	/// モデル更新
	/// </summary>
	void UpdateModel();

	/// <summary>
	/// モデル描画
	/// </summary>
	void DrawModel();

	/// <summary>
	/// 背景描画
	/// </summary>
	void DrawBack();

	/// <summary>
	/// フェードイン
	/// </summary>
	void FadeIn();

	/// <summary>
	/// フェードアウト
	/// </summary>
	void FadeOut();



private:	// 変数
	std::vector<int> m_backH;					// 背景画像ハンドル
	int m_modelH;								// モデルハンドル
	int m_ufoModelH;							// UFOモデルハンドル
	int m_titleLogoH;							// タイトルロゴ
	int m_pressTextH;							// プレスボタンテキスト
	int m_animIndex;							// アニメーションインデックス

	int m_backWidth;							// 背景画像横幅
	int m_backHeight;							// 背景画像縦幅

	int m_scrollXBack;							// 奥スクロール値
	int m_scrollXMiddle;						// 中スクロール値
	int m_scrollXFront;							// 手前スクロール値

	int m_pressAlpha;							// プレスボタンの透明度
	int m_fadeAlpha;							// フェードの透明度

	float m_animTotalTime;						// アニメーションの総再生時間
	float m_animPlayTime;						// アニメーションの再生時間

	float m_ufoSinCount;						// UFOの上下移動カウント
	float m_ufoSinPosY;							// UFOのサイン計算に使うY座標

	bool m_isPressAlphaUp;						// プレスボタンの透明度が上がっているか
	bool m_isFadeIn;							// フェードイン中か
	bool m_isFadeOut;							// フェードアウト中か
	bool m_isSceneEnd;							// シーン終了か

	VECTOR m_ufoPos;							// UFOの座標


	std::vector<int> m_vertexShaderType;		// 頂点シェーダタイプ
	std::vector<int> m_ufoVertexShaderType;		// UFOの頂点シェーダタイプ

	std::shared_ptr<ToonShader>	m_pToonShader;	// トゥーンシェーダ
};


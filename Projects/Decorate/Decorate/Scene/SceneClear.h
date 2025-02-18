#pragma once
#include "SceneBase.h"

#include "../Utility/Game.h"

class SceneClear : public SceneBase
{
public:	// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="stageKind">ステージの種類</param>
	SceneClear(Game::StageKind stageKind);

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~SceneClear();

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
	/// ボックス更新
	/// </summary>
	void UpdateBox();

	/// <summary>
	/// ボックス描画
	/// </summary>
	void DrawCursorBox();

	/// <summary>
	/// カーソル更新
	/// </summary>
	void UpdateCursor();

	/// <summary>
	/// モデル更新
	/// </summary>
	void UpdateModel();

	/// <summary>
	/// モデル描画
	/// </summary>
	void DrawModel();

private:	// 変数
	int m_modelH;					// モデルハンドル
	int m_textH;					// テキスト画像ハンドル
	int m_oneMoreBoxH;				// もう一度ボックス画像ハンドル
	int m_selectBoxH;				// セレクトボックス画像ハンドル
	int m_resultBoxH;				// リザルトボックス画像ハンドル
	int m_animIndex;				// アニメーション

	float m_animTotalTime;			// アニメーションの総再生時間
	float m_animPlayTime;			// アニメーションの再生時間

	float m_boxSinCount;			// ボックス拡縮カウント
	float m_expansionBox;			// ボックス拡縮サイズ

	float m_oneMoreBoxExtRate;		// もう一度ボックス拡大率
	float m_selectBoxExtRate;		// セレクトボックス拡大率

	bool m_isLeft;					// カーソルが左にあるかどうか

	Game::StageKind m_stageKind;	// ステージの種類

	std::shared_ptr<SceneBase> m_nextScene;	// 次のシーン

};


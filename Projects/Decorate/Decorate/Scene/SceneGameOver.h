#pragma once
#include "SceneBase.h"

#include "../Utility/Game.h"


class SceneGameOver : public SceneBase
{
public:	// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="stageKind">ステージの種類</param>
	SceneGameOver(Game::StageKind stageKind);

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~SceneGameOver();

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
	/// カーソル更新
	/// </summary>
	void UpdateCursor();

	/// <summary>
	/// モデル更新
	/// </summary>
	void UpdateModel();

	/// <summary>
	/// カーソル描画
	/// </summary>
	void DrawCursor();

	/// <summary>
	/// モデル描画
	/// </summary>
	void DrawModel();

	/// <summary>
	/// 画像ロード
	/// </summary>
	void LoadImg();

	/// <summary>
	/// モデルロード
	/// </summary>
	void LoadModel();

	/// <summary>
	/// 画像デリート
	/// </summary>
	void DeleteImg();

	/// <summary>
	/// モデルデリート
	/// </summary>
	void DeleteModel();

private:	// 変数
	int m_modelH;					// モデルハンドル

	int m_textH;					// テキスト画像ハンドル
	int m_boxH;						// ボックス画像ハンドル
	int m_cursorH;					// カーソル画像ハンドル
	int m_mouseCursorH;				// マウスカーソル画像ハンドル
	int m_mouseCursorPosX;			// マウスカーソルX座標

	int m_animIndex;				// アニメーション
	int m_imgAlpha;					// 画像の不透明度

	float m_animTotalTime;			// アニメーションの総再生時間
	float m_animPlayTime;			// アニメーションの再生時間

	float m_mouseCursorSinCount;	// マウスカーソル拡縮カウント
	float m_mouseCursorScaleSize;	// マウスカーソル拡縮サイズ
	float m_mouseCursorScaleRate;	// マウスカーソル拡大率

	bool m_isLeft;					// カーソルが左にあるかどうか
	bool m_isImg;					// 画像を表示するかどうか
	bool m_isFallDownSE;			// 倒れるSEを流したかどうか
	bool m_isCursorMove;			// カーソル移動しているかどうか

	Game::StageKind m_stageKind;	// ステージの種類

	std::shared_ptr<SceneBase> m_nextScene;	// 次のシーン
};


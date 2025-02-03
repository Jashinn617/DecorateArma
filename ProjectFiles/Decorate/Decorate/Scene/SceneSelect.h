#pragma once
#include "SceneBase.h"

#include <vector>

/// <summary>
/// セレクト画面
/// </summary>
class SceneSelect : public SceneBase
{
public:	// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneSelect();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SceneSelect();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init()override final;

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
	virtual SceneKind GetSceneKind() override final { return SceneKind::Select; }

private:	// 列挙型
	/// <summary>
	/// シーンタイプ
	/// </summary>
	enum class SceneType
	{
		Normal,			// 通常
		StageSelect,	// ステージ選択
	};

private:	// 関数
	/// <summary>
	/// カーソル更新
	/// </summary>
	void UpdateCursor();

	/// <summary>
	/// ボックス更新
	/// </summary>
	void UpdateBox();

	/// <summary>
	/// ステージ選択更新
	/// </summary>
	void UpdateStageSelect();

	/// <summary>
	/// 背景描画
	/// </summary>
	void DrawBack();

	/// <summary>
	/// ボックス描画
	/// </summary>
	void DrawCursorBox();

	/// <summary>
	/// テキスト描画
	/// </summary>
	void DrawTextInfo();

	/// <summary>
	/// ステージ選択描画
	/// </summary>
	void DrawStageSelect();

	/// <summary>
	/// 画像ロード
	/// </summary>
	void ImgLoad();

	/// <summary>
	/// 画像デリート
	/// </summary>
	void ImgDelete();

private:	// 変数
	std::vector<int> m_backH;		// 背景画像ハンドル
	std::vector<int> m_selectBoxH;	// 選択ボックスハンドル
	std::vector<int> m_textInfoH;		// テキスト画像ハンドル
	int m_stageSelectBoxH;			// ステージ選択ボックスハンドル
	int m_gearBoxH;					// 装備ボックスハンドル
	int m_optionBoxH;				// オプションボックスハンドル
	int m_gameEndBoxH;				// ゲーム終了ボックスハンドル
	int m_descriptionBoxH;			// 説明文ボックスハンドル
	int m_selectButtonH;			// セレクトボタンハンドル
	int m_decisionButtonH;			// 決定ボタンハンドル
	int m_selectTextH;				// 選択テキスト画像ハンドル
	int m_arrowH;					// 矢印画像ハンドル

	int m_backWidth;				// 背景画像横幅
	int m_backHeight;				// 背景画像縦幅

	int m_scrollXBack;				// 奥スクロール値
	int m_scrollXMiddle;			// 中スクロール値
	int m_scrollXFront;				// 手前スクロール値

	int m_cursorCount;				// カーソルカウント

	float m_boxSinCount;			// ボックス拡縮カウント

	float m_expansionBoxExtRate;	// ボックスの拡縮サイズ

	float m_stageSelectBoxExtRate;	// ステージ選択ボックス拡大率
	float m_gearBoxExtRate;		// 装備ボックス拡大率
	float m_optionBoxExtRate;		// オプションボックス拡大率
	float m_gameEndBoxExtRate;		// ゲーム終了ボックス拡大率

	bool m_isCursorUp;				// カーソルが上にいるか

	SceneType m_sceneType;			// シーンタイプ

	std::shared_ptr<SceneBase> m_nextScene;		// 次のシーン
};
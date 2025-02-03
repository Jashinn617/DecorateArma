#pragma once
#include "SceneBase.h"

#include <memory>
#include <array>

/// <summary>
/// オプション画面
/// </summary>
class SceneOption : public SceneBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pPrevScene">前のシーン</param>
	SceneOption(std::shared_ptr<SceneBase> prevScene);

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~SceneOption();

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
	virtual SceneKind GetSceneKind() override final { return SceneKind::Option; }

private:	// 関数
	/// <summary>
	/// BGMの音量変更
	/// </summary>
	void  SettingBGM();

	/// <summary>
	/// SEの音量変更
	/// </summary>
	void SettingSE();

	/// <summary>
	/// カーソル更新
	/// </summary>
	void UpdateCursor();

	/// <summary>
	/// セレクト時の描画
	/// </summary>
	void DrawSelect();

	/// <summary>
	/// ステージ時の描画
	/// </summary>
	void DrawStage();

	/// <summary>
	/// 音量描画
	/// </summary>
	void DrawVol();

private:	// 変数
	int m_baseBoxH;								// 大ボックス画像ハンドル
	int m_normalBoxH;							// 小ボックス画像ハンドル
	int m_normalBox2H;							// 小ボックス2画像ハンドル
	int m_mainTextH;							// テキスト画像ハンドル
	int m_bgmTextH;								// BGMテキスト画像ハンドル
	int m_seTextH;								// SEテキスト画像ハンドル
	int m_backSelectTextH;						// セレクトに戻るテキスト画像ハンドル
	int m_backStageTextH;						// ゲームに戻るテキスト画像ハンドル
	int m_cursorH;								// カーソル画像ハンドル
	int m_volumeArrowH;							// 音量矢印画像ハンドル
	int m_offVolumeH;							// 音量OFF画像ハンドル
	int m_onVolumeH;							// 音量ON画像ハンドル

	int m_cursorCount;							// カーソルカウント

	int m_bgmVol;								// BGM音量
	int m_seVol;								// SE音量

	bool m_isSceneEnd;							// シーンが終わったかどうか

	std::shared_ptr<SceneBase> m_prevScene;		// 前のシーン
	std::shared_ptr<SceneBase> m_nextScene;		// 次のシーン
};


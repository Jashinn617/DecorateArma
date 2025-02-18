#pragma once
#include "SceneBase.h"

#include "../Object/CharacterBase.h"

#include <array>
#include <vector>

class Gear;

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
	/// コンストラクタ
	/// </summary>
	/// <param name="prevScene">前のシーン</param>
	/// <param name="pGear">装備クラスポインタ/param>
	SceneOption(std::shared_ptr<SceneBase> prevScene, std::shared_ptr<Gear> pGear);

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

	/// <summary>
	/// ステータス設定
	/// </summary>
	void SetStatus();

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

	/// <summary>
	/// ステータスの数値の描画
	/// </summary>
	void DrawStatusNum();

	/// <summary>
	/// 画像のロード
	/// </summary>
	void ImgLoad();

	/// <summary>
	/// 画像のデリート
	/// </summary>
	void ImgDelete();

private:	// 変数
	std::vector<int> m_numH;					// 数字画像ハンドル
	int m_baseBoxH;								// 大ボックス画像ハンドル
	int m_mainTextH;							// テキスト画像ハンドル
	int m_bgmH;									// BGMテキスト画像ハンドル
	int m_seH;									// SEテキスト画像ハンドル
	int m_backSelectH;							// セレクトに戻るテキスト画像ハンドル
	int m_backStageH;							// ゲームに戻るテキスト画像ハンドル
	int m_gearH;								// 装備画像ハンドル
	int m_statusBoxH;							// ステータスボックス画像ハンドル
	int m_lifeH;								// ライフ画像ハンドル
	int m_powerH;								// 攻撃力画像ハンドル
	int m_shotH;								// 射撃力画像ハンドル
	int m_guardH;								// 防御力画像ハンドル
	int m_leftCursorH;							// カーソル画像ハンドル
	int m_rightCursorH;							// カーソル画像ハンドル
	int m_volumeArrowH;							// 音量矢印画像ハンドル
	int m_offVolumeH;							// 音量OFF画像ハンドル
	int m_onVolumeH;							// 音量ON画像ハンドル
	int m_LBH;									// LBボタン画像ハンドル
	int m_RBH;									// RBボタン画像ハンドル

	int m_cursorCount;							// カーソルカウント

	int m_bgmVol;								// BGM音量
	int m_seVol;								// SE音量

	bool m_isSceneEnd;							// シーンが終わったかどうか
	bool m_isLeft;								// カーソルが左側にあるかどうか

	std::shared_ptr<Gear> m_pGear;				// 装備クラスポインタ
	std::shared_ptr<SceneBase> m_prevScene;		// 前のシーン
	std::shared_ptr<SceneBase> m_nextScene;		// 次のシーン
	CharacterBase::StatusData m_statusData;		// プレイヤーのステータス情報
};
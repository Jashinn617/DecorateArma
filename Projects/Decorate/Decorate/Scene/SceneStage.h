#pragma once
#include "SceneBase.h"

#include "../Utility/Game.h"

#include <list>
#include <array>

class ObjectManager;
class Gear;

/// <summary>
/// ステージシーンクラス
/// </summary>
class SceneStage : public SceneBase
{
public:		// 列挙型
	/// <summary>
	/// シーンの種類
	/// </summary>
	enum SceneType
	{
		Main,	// メイン
		Clear,	// クリア
	};

public:		// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="stage">ステージの種類</param>
	SceneStage(Game::StageKind stage);

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~SceneStage();

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
	virtual void End() override final {/*処理無し*/ };

	/// <summary>
	/// ステージの種類の取得
	/// </summary>
	/// <returns>ステージの種類</returns>
	virtual SceneKind GetSceneKind() override final { return SceneKind::Stage; }

	/// <summary>
	/// プレイヤーのステータス情報の再設定
	/// </summary>
	void SetPlayerStatus();

private:	// 関数
	/// <summary>
	/// クリア更新
	/// </summary>
	void UpdateClearScene();

	/// <summary>
	/// クリア描画
	/// </summary>
	void DrawClearScene();

private:	// 変数
	int m_infoH;									// 操作説明画像ハンドル
	bool m_isGameClear;								// ゲームクリアしたかどうか
	bool m_isGameOver;								// ゲームオーバーになったかどうか

	Game::StageKind m_stageKind;					// ステージの種類
	SceneType m_sceneType;							// シーンタイプ

	std::shared_ptr<Gear> m_pGear;					// 装備ポインタ
	std::shared_ptr<ObjectManager> m_pObject;		// オブジェクトマネージャー
	std::shared_ptr<SceneBase> m_nextScene;		// 次のシーンのポインタ
};
#pragma once
#include "SceneBase.h"

#include <array>
#include <string>

/// <summary>
/// デバッグ専用シーン
/// 各シーンに飛ぶクラス
/// </summary>
class SceneDebug : public SceneBase
{
public:		// 列挙型
	/// <summary>
	/// シーンのタイプ
	/// </summary>
	enum class SceneType
	{
		Debug,		// デバッグ
		Test,		// テスト
		Title,		// タイトル
		Select,		// セレクト
		Gear,		// 装備品
		Stage1,		// ステージ1
		Clear,		// クリア
		GameOver,	// ゲームオーバー

		SceneNum,	// シーン数
	};

public:		// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneDebug();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~SceneDebug() {/*処理無し*/ }

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() override final {/*処理無し*/ }

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
	virtual void End() override final {/*処理無し*/ }

	/// <summary>
	/// シーンの種類の取得
	/// </summary>
	/// <returns>シーンの種類</returns>
	virtual SceneKind GetSceneKind() override final { return SceneKind::Debug; }

private:	// 関数
	/// <summary>
	/// カーソル更新
	/// </summary>
	void UpdateCursor();

	/// <summary>
	/// 次のシーンに移動する処理
	/// </summary>
	/// <returns>次のシーン</returns>
	std::shared_ptr<SceneBase> MoveNextScene();

private:	// 変数
	int m_cursorCount;		// カーソルカウント
	std::array<std::string, static_cast<int>(SceneType::SceneNum)> m_sceneString;	// シーン移動用に使う文字
};


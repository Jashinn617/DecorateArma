#pragma once
#include "DxLib.h"

#include <memory>

/// <summary>
/// 各シーンの基底クラス
/// </summary>
class SceneBase: public std::enable_shared_from_this<SceneBase>
{
public:	// 列挙型

	// シーンの種類
	enum class SceneKind
	{
		Debug,		// デバッグ
		Title,		// タイトル
		Select,		// セレクト
		Gear,		// 装備
		Stage,		// ステージ
		Clear,		// クリア
		GameOver,	// ゲームオーバー
		Option,		// オプション
	};

public:	// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneBase() {/*処理無し*/ }

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SceneBase(){/*処理無し*/ }

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	/// <returns>次のシーン</returns>
	virtual std::shared_ptr<SceneBase> Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void End() = 0;

	/// <summary>
	/// シーンの種類の取得
	/// </summary>
	/// <returns>シーンの種類</returns>
	virtual SceneKind GetSceneKind() = 0;
};
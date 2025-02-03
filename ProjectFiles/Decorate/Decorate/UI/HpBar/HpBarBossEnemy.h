#pragma once
#include "HpBarBase.h"

/// <summary>
/// ボス敵のHPバー
/// </summary>
class HpBarBossEnemy : public HpBarBase
{
public:	// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="hp">体力</param>
	HpBarBossEnemy(const int& hp);

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~HpBarBossEnemy();

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override final;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override final;

private:	// 変数
	int m_textH;		// テキスト画像ハンドル
	int m_frameH;		// フレーム画像ハンドル
};


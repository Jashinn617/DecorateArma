#pragma once
#include "HpBarBase.h"
/// <summary>
/// 敵のHPバー
/// </summary>
class HpBarEnemy : public HpBarBase
{
public:	// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="hp">体力</param>
	/// <param name="pos">敵座標</param>
	HpBarEnemy(const int& hp, const VECTOR& pos);

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~HpBarEnemy();

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

private:	// 変数
	const VECTOR* m_pos;	// 敵座標
};


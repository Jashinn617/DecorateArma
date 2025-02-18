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
	/// <param name="pos">座標</param>
	/// <param name="height">高さ</param>
	HpBarEnemy(const int& hp, const VECTOR& pos, float height);

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
	float m_hpBarHeight;	// HPバーを表示する高さ
	const VECTOR* m_pos;	// 敵座標
};


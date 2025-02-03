#pragma once
#include "EnemyBase.h"

/// <summary>
/// ワイバーン
/// </summary>
class EnemyWyvern : public EnemyBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pos">初期座標</param>
	EnemyWyvern(VECTOR pos);

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~EnemyWyvern();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override final;
};


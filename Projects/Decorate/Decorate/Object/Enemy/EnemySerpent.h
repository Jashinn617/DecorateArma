#pragma once
#include "EnemyBase.h"

/// <summary>
/// 蛇
/// </summary>
class EnemySerpent : public EnemyBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pos">初期座標</param>
	EnemySerpent(VECTOR pos);

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~EnemySerpent();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override final;
};
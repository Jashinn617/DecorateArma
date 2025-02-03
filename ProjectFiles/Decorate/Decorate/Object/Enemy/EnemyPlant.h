#pragma once
#include "EnemyBase.h"

/// <summary>
/// 植物モンスター
/// </summary>
class EnemyPlant : public EnemyBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pos">初期座標</param>
	EnemyPlant(VECTOR pos);

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~EnemyPlant();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override final;
};


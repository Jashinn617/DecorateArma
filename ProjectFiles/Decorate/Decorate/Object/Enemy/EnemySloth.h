#pragma once
#include "EnemyBase.h"

/// <summary>
/// ナマケモノ
/// </summary>
class EnemySloth : public EnemyBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pos">初期座標</param>
	EnemySloth(VECTOR pos);

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~EnemySloth();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override final;
};


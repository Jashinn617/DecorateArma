#pragma once
#include "EnemyBase.h"

/// <summary>
/// 虫
/// </summary>
class EnemyBug : public EnemyBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pos">初期座標</param>
	EnemyBug(VECTOR pos);

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~EnemyBug();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override final;
};


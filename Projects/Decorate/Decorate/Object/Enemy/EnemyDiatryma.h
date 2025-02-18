#pragma once
#include "EnemyBase.h"

/// <summary>
/// ディアトリマ
/// </summary>
class EnemyDiatryma : public EnemyBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pos">初期座標</param>
	EnemyDiatryma(VECTOR pos);

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~EnemyDiatryma();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override final;
};


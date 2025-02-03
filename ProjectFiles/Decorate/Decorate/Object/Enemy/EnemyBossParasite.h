#pragma once
#include "EnemyBossBase.h"

/// <summary>
/// ボス1
/// 寄生モンスター
/// </summary>
class EnemyBossParasite : public EnemyBossBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pos">初期座標</param>
	EnemyBossParasite(VECTOR pos);

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~EnemyBossParasite();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override final;

	/// <summary>
	/// 敵の種類の取得
	/// </summary>
	/// <returns>種類</returns>
	virtual EnemyType GetEnemyType() const { return EnemyType::Boss; }

private:	// 関数
	virtual void HardAttack() override final;

private:	// 変数
	std::shared_ptr<Time> m_pHardAttackTime;		// 強攻撃を出すまでの時間
	std::shared_ptr<Time> m_pHardAttackEffectTime;	// 強攻撃の効果時間


};
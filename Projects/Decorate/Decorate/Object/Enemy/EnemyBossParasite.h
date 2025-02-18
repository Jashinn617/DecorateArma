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
	/// ダメージ処理
	/// </summary>
	/// <param name="targetPos">攻撃を当てた相手の座標</param>
	/// <param name="damagePoint">ダメージ量</param>
	/// <param name="isInvincible">無敵時間を発動させるかどうか</param>
	void OnDamage(VECTOR targetPos, int damagePoint, bool isInvincible = true) override;

	/// <summary>
	/// 敵の種類の取得
	/// </summary>
	/// <returns>種類</returns>
	virtual EnemyType GetEnemyType() const { return EnemyType::Boss; }

private:	// 関数
	virtual void HardAttack() override final;

private:	// 変数
	bool m_isHardAttackEffect;						// 強攻撃のエフェクトを出したかどうか
	
	std::shared_ptr<Time> m_pHardAttackTime;		// 強攻撃を出すまでの時間
	std::shared_ptr<Time> m_pHardAttackEffectTime;	// 強攻撃の効果時間


};
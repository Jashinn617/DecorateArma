#pragma once
#include "EnemyBase.h"


/// <summary>
/// ボス敵の基底クラス
/// </summary>
class EnemyBossBase : public EnemyBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	EnemyBossBase();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~EnemyBossBase() {/*処理無し*/ };

	/// <summary>
	/// 2D描画
	/// </summary>
	void Draw2D() override final;

	/// <summary>
	/// 敵の種類の取得(ボス)
	/// </summary>
	/// <returns>種類</returns>
	virtual EnemyType GetEnemyType() const { return EnemyType::Boss; }

	/// <summary>
	/// 強攻撃当たり判定
	/// </summary>
	virtual void OnHardAttack(CharacterBase* pPlayer);


protected:	// 関数
	/// <summary>
	/// 近距離攻撃のみ
	/// </summary>
	virtual void UpdateMeleeAttackState() override;

	
	/// <summary>
	/// 強攻撃
	/// ボスによって処理が違う
	/// </summary>
	virtual void HardAttack() {/*処理無し*/ };

protected:	// 変数
	bool m_isHardAttack;			// 強攻撃中かどうか

	std::shared_ptr<CollisionShape> m_pHardAttackCol;	// 強攻撃当たり判定

private:	// 関数
	/// <summary>
	/// プレイヤーを見つけた際の処理
	/// </summary>
	virtual void SearchPlayer() override final;

private:	// 変数
	int m_attackProbability;		// 攻撃確率
};
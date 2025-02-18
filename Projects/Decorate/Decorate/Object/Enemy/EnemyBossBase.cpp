#include "EnemyBossBase.h"

#include "../Model.h"

#include "../../Utility/Time.h"
#include "../../Utility/CollisionShape.h"

#include "../../UI/HpBar/HpBarBase.h"

#include <random>
#include <Windows.h>

namespace
{
	constexpr int kAttackAnimsNum = 3;			// 攻撃アニメーションの数
	constexpr int kMaxProbability = 100;		// 確率
	constexpr int kHardAttackProbability = 30;	// 強攻撃の確率
	constexpr int kHardDamage = 20;				// 強攻撃ダメージ追加数
}

EnemyBossBase::EnemyBossBase() :
	m_attackProbability(0),
	m_isHardAttack(false),
	m_isHardAttackCol(false),
	m_isRand(false)
{
}

void EnemyBossBase::Draw2D()
{
	// ボス戦がプレイヤーを発見していなければ何もしない
	if (!m_isFinding) return;

	// HPバー描画
	m_pHpBar->Draw();
}


void EnemyBossBase::OnHardAttack(CharacterBase* pPlayer)
{
	// 攻撃中でないときは判定しない
	if (!m_isAttack) return;
	// 強攻撃中でないときは判定しない
	if (!m_isHardAttackCol) return;

#ifdef _DEBUG
	// 当たり判定デバッグ表示
	m_pHardAttackCol->DebugDraw(0xff00ff);
#endif // _DEBUG

	// 衝突判定
	if (m_pHardAttackCol->IsCollide(pPlayer->GetCollShape()))
	{
		// 当たっていたらダメージを与える
		pPlayer->OnDamage(m_characterInfo.pos,
			m_statusData.meleeAtk + kHardDamage);
	}
}

void EnemyBossBase::UpdateMeleeAttackState()
{
	// 攻撃中は移動をしない
	m_moveSpeed = 0.0f;

	// 攻撃間隔時間は攻撃しない
	if (!m_attackIntervalTime->Update())
	{
		// アニメーション再生
		m_pModel->ChangeAnim(m_animData.idle, true, false, 1);
		UpdateMoveDirection();
		UpdateAngle();
		return;
	}

	if (!m_isRand)
	{
		// 強攻撃か通常攻撃かを決める
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<int> urd(0, kMaxProbability);
		m_attackProbability = urd(mt);
	}

	// 強攻撃
	if (m_attackProbability < kHardAttackProbability)
	{
		if (!m_isRand)
		{
			m_isAttack = true;
			m_isHardAttack = true;
			m_isRand = true;
		}		
		HardAttack();
	}
	// 通常攻撃
	else
	{
		if (!m_isRand)
		{
			m_isAttack = true;
			m_isRand = true;
		}		
		// アニメーション再生
		m_pModel->ChangeAnim(m_animData.attack1, false, false, 2);
	}

	// 攻撃が終わったら移動状態に戻る
	if (m_pModel->IsAnimEnd())
	{
		m_pModel->ChangeAnim(m_animData.walk, true, false, 1);
		m_updateFunc = &EnemyBossBase::UpdateMoveState;
		m_isAttack = false;
		m_isHardAttack = false;
		m_isRand = false;
	}
}

void EnemyBossBase::SearchPlayer()
{
	// ボス戦フラグを立てる
	m_isFinding = true;
	// ボス戦突入SEを鳴らす
}
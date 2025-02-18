#include "EnemyBossParasite.h"

#include "../Model.h"
#include "../ObjectManager.h"

#include "../../Common/CsvLoad.h"

#include "../../Effect/Effekseer3DManager.h"


#include "../../Utility/CollisionShape.h"
#include "../../Utility/Time.h"

#include "../../UI/HpBar/HpBarBase.h"
#include "../../UI/HpBar/HpBarBossEnemy.h"

#include <cassert>

namespace
{
	constexpr int kAttackInterval = 100;					// 攻撃感覚
	constexpr int kHardAttackTime = 110;					// 強攻撃を出すまでの時間
	constexpr int kHardAttackEffectTime = 40;				// 強攻撃効果時間
	constexpr float kRadius = 40.0f;						// 当たり判定半径
	constexpr float kCapsuleRadius = 30.0f;					// カプセルの半径
	constexpr float kRotSpeed = 9.0f;						// 回転速度
	constexpr VECTOR kModelScale = { 1.5f,1.5f,1.5f };		// モデルスケール
	constexpr float kSearchRadius = 800.0f;					// 索敵範囲の半径
	constexpr float kSearchHeight = 70.0f;					// 索敵範囲の高さ
	constexpr float kAttackRadius = 15.0f;					// 攻撃当たり判定の半径
	constexpr float kMeleeAttackRange = 150.0f;				// 近距離攻撃射程
	constexpr float kHardAttackRadius = 200.0f;				// 強攻撃範囲半径
	constexpr float kHardAttackHeight = 50.0f;				// 強攻撃範囲高さ
	const char* const kName = "BossParasite";				// 名前
}

EnemyBossParasite::EnemyBossParasite(VECTOR pos):
	m_isHardAttackEffect(false)
{
	// 座標初期化
	m_characterInfo.pos = pos;

	// ステータス初期化
	CsvLoad::GetInstance().StatusLoad(m_statusData, kName);
	// アニメーション設定
	CsvLoad::GetInstance().AnimLoad(m_animData, kName);

	// 移動速度設定
	InitMoveSpeed(m_statusData.spd);

	// 回転速度設定
	m_moveData.rotSpeed = kRotSpeed;

	// 攻撃範囲設定
	m_meleeAttackRange = kMeleeAttackRange;

	// 攻撃タイプ設定
	m_attackType = AttackType::Melee;

	// 索敵範囲設定
	m_pSearchRange = std::make_shared<CollisionShape>(m_characterInfo.pos, kSearchRadius, kSearchHeight);
	// 強攻撃範囲設定
	m_pHardAttackCol = std::make_shared<CollisionShape>(m_characterInfo.pos, kHardAttackRadius, kHardAttackHeight);
	// 攻撃間隔
	m_attackIntervalTime = std::make_shared<Time>(kAttackInterval);
	// 強攻撃までの時間
	m_pHardAttackTime = std::make_shared<Time>(kHardAttackTime);
	// 攻撃効果時間
	m_pHardAttackEffectTime = std::make_shared<Time>(kHardAttackEffectTime);

	// HPバーポインタ作成
	m_pHpBar = std::make_shared<HpBarBossEnemy>(m_statusData.hp);

	// 敵タイプ設定
	m_enemyType = EnemyType::Boss;
}

EnemyBossParasite::~EnemyBossParasite()
{
	/*処理無し*/
}

void EnemyBossParasite::Init()
{
	// モデル取得
	m_pModel = std::make_shared<Model>(m_modelH);

	// モデル頂点の取得
	m_topFrameIndex = MV1SearchFrame(m_pModel->GetModelHandle(), "mixamorig:HeadTop_End");
	assert(m_topFrameIndex != -1);
	assert(m_topFrameIndex != -2);
	// モデル底辺の取得
	m_bottomFrameIndex = MV1SearchFrame(m_pModel->GetModelHandle(), "Armature");
	assert(m_bottomFrameIndex != -1);
	assert(m_bottomFrameIndex != -2);

	// 当たり判定範囲のポジション
	m_characterInfo.topPos = MV1GetFramePosition(m_pModel->GetModelHandle(), m_topFrameIndex);
	m_characterInfo.bottomPos = MV1GetFramePosition(m_pModel->GetModelHandle(), m_bottomFrameIndex);

	// 当たり判定ポインタ作成
	m_pCollShape = std::make_shared<CollisionShape>(m_characterInfo.topPos, m_characterInfo.bottomPos, kRadius);

	// 攻撃当たり判定作成
	m_melleAttack.CollTopFrameIndex = MV1SearchFrame(m_pModel->GetModelHandle(),
		"mixamorig:RightForeArm");
	assert(m_melleAttack.CollTopFrameIndex != -1);
	assert(m_melleAttack.CollTopFrameIndex != -2);
	m_melleAttack.CollBottomFrameIndex = MV1SearchFrame(m_pModel->GetModelHandle(),
		"mixamorig:RightHandMiddle4_end");
	assert(m_melleAttack.CollBottomFrameIndex != -1);
	assert(m_melleAttack.CollBottomFrameIndex != -2);

	m_melleAttack.CollTop = MV1GetFramePosition(m_pModel->GetModelHandle(),
		m_melleAttack.CollTopFrameIndex);

	// 攻撃当たり判定ポインタ作成
	m_pMelleAttackCol = std::make_shared<CollisionShape>(m_melleAttack.CollTop,
		m_melleAttack.CollBottom, kAttackRadius);

	// アニメーション設定
	m_pModel->SetAnim(m_animData.idle, true, true);
	// スケール設定
	m_pModel->SetScale(kModelScale);
	// 回転量初期化
	m_pModel->SetRot(m_characterInfo.rot);
	// 座標初期化
	m_pModel->SetPos(m_characterInfo.pos);

	// モデルの頂点タイプの取得
	for (int i = 0; i < MV1GetTriangleListNum(m_pModel->GetModelHandle()); i++)
	{
		// 頂点タイプの取得
		m_vertexShaderType.push_back(MV1GetTriangleListVertexType(m_pModel->GetModelHandle(), i));
	}
}

void EnemyBossParasite::OnDamage(VECTOR targetPos, int damagePoint, bool isInvincible)
{
	// ダメージ中は処理をしない
	if (m_isDamage) return;

	// HPを減らす
	m_statusData.hp -= damagePoint;

	// HPが0以下になって死亡状態でなかった場合
	if (m_statusData.hp <= 0 && !m_isDead)
	{
		// 死亡する
		m_isDead = true;
		// アイテムドロップ
		m_pObjectManager->GetGear()->ObtainItemOnStage();
	}
	// ダメージ状態になる
	m_isDamage = true;
}

void EnemyBossParasite::HardAttack()
{
	if (!m_isHardAttack) return;

	// アニメーション再生
	m_pModel->ChangeAnim(m_animData.hardAttack, false, false, 2);

	// 一定時間経過したら周りにダメージを与える
	if (m_pHardAttackTime->Update())
	{
		// フラグを立てる
		m_isHardAttackCol = true;

		// エフェクトを流す
		if (!m_isHardAttackEffect)
		{
			Effekseer3DManager::GetInstance().Add("ParasiteHardAttack",
				Effekseer3DManager::PlayType::Normal, this, m_characterInfo.pos);
			m_isHardAttackEffect = true;
		}

		// 一定時間経過したら消す
		if (m_pHardAttackEffectTime->Update())
		{
			m_pHardAttackTime->Reset();
			m_pHardAttackEffectTime->Reset();
			m_isHardAttack = false;
			m_isHardAttackCol = false;
			m_isHardAttackEffect = false;
		}
	}
}
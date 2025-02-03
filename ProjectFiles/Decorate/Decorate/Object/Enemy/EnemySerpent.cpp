#include "EnemySerpent.h"
#include "EShot.h"

#include "../Model.h"

#include "../../Common/CsvLoad.h"

#include "../../Utility/CollisionShape.h"
#include "../../Utility/Time.h"

#include "../../UI/HpBar/HpBarBase.h"
#include "../../UI/HpBar/HpBarEnemy.h"

#include <cassert>

namespace
{
	constexpr int kAttackInterval = 80;						// 攻撃感覚
	constexpr float kRadius = 120.0f;						// 当たり判定半径
	constexpr float kHeight = 100.0f;						// 高さ
	constexpr float kRotSpeed = 9.0f;						// 回転速度
	constexpr VECTOR kModelScale = { 0.8f,0.8f,0.8f };		// モデルスケール
	constexpr float kSearchRadius = 600.0f;					// 索敵範囲の半径
	constexpr float kSearchHeight = 70.0f;					// 索敵範囲の高さ
	constexpr float kShotAttackRange = 500.0f;				// 遠距離攻撃射程
	const char* const kName = "Serpent";						// 名前
}

EnemySerpent::EnemySerpent(VECTOR pos)
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
	m_shotAttackRange = kShotAttackRange;

	// 攻撃タイプ設定
	m_attackType = AttackType::Shot;

	// 索敵範囲設定
	m_pSearchRange = std::make_shared<CollisionShape>(m_characterInfo.pos, kSearchRadius, kSearchHeight);

	// 攻撃間隔
	m_attackIntervalTime = std::make_shared<Time>(kAttackInterval);

	// HPバーポインタ作成
	m_pHpBar = std::make_shared<HpBarEnemy>(m_statusData.hp, m_characterInfo.pos);

	// 遠距離攻撃
	m_pShot = std::make_shared<EShot>(m_statusData.shotAtk);

	// 重力を適応しない
	m_isGravity = false;
}

EnemySerpent::~EnemySerpent()
{
	/*処理無し*/
}

void EnemySerpent::Init()
{
	// モデル取得
	m_pModel = std::make_shared<Model>(m_modelH);
	// 当たり判定ポインタ作成
	m_pCollShape = std::make_shared<CollisionShape>(m_characterInfo.pos, kRadius, kHeight);

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

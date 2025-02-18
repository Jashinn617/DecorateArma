#include "ObjectManager.h"

#include "../Object/ObjectBase.h"
#include "../Object/Field.h"
#include "../Object/Camera.h"
#include "../Object/Player/Player.h"
#include "../Utility/Gear.h"
#include "../Object/Enemy/EnemyBase.h"

#include "../Utility/Collision.h"

#include "../Common/Spawn.h"

#include "../Shader/ToonShader.h"
#include "../Shader/ShadowMapShader.h"

namespace
{
	constexpr float kLockOnRange = 10000.0f;					// ロックオンできる範囲

	constexpr float kMinLockOnValidRangeMin = -DX_PI_F * 0.4f;	// ロックオンの有効範囲
	constexpr float kMinLockOnValidRangeMax = DX_PI_F * 0.4f;	// ロックオンの有効範囲
	constexpr float kMaxLockOnValidRangeMin = -DX_PI_F * 0.8f;	// ロックオンの有効範囲
	constexpr float kMaxLockOnValidRangeMax = DX_PI_F * 0.8f;	// ロックオンの有効範囲
}

ObjectManager::ObjectManager(Game::StageKind stageKind, std::shared_ptr<Gear> pGear) :
	m_isBossDead(false),
	m_isGameClear(false),
	m_pLockOnEnemy(nullptr),
	m_pCollision(std::make_shared<Collision>()),
	m_pSpawn(std::make_shared<Spawn>(stageKind,this)),
	m_pToon(std::make_shared<ToonShader>()),
	m_pGear(pGear),
	m_pShadowMapShader(std::make_shared<ShadowMapShader>())
{
	// プレイヤーの追加
	AddObject(new Player);
	// フィールド追加
	AddObject(new Field(stageKind));
	// オブジェクト出現
	m_pSpawn->Init();
}

ObjectManager::~ObjectManager()
{
	// オブジェクトポインタ解放
	std::list<ObjectBase*>::iterator it = m_pObject.begin();
	while (it != m_pObject.end())
	{
		auto obj = (*it);

		delete obj;
		obj = nullptr;
		it = m_pObject.erase(it);
	}
}

void ObjectManager::Update()
{
	// オブジェクト配列の最初のイテレータを取得する
	std::list<ObjectBase*>::iterator it = m_pObject.begin();
	while (it != m_pObject.end())
	{
		// オブジェクトの更新
		auto obj = (*it);
		obj->Update();

		// オブジェクトのタイプが敵だった場合
		if (obj->GetColType() == ObjectBase::ColType::Enemy)
		{
			// かつその敵がボスだった場合
			if (dynamic_cast<EnemyBase*>(obj)->GetEnemyType() == EnemyBase::EnemyType::Boss)
			{
				/*プレイヤー発見状態かどうかを調べる
				発見状態だった場合はボス戦状態になる*/
				m_isBossButtle = dynamic_cast<EnemyBase*>(obj)->IsFinding();
			}
		}

		// オブジェクトが存在していない場合
		if (!obj->GetInfo().isExist)
		{
			// オブジェクトのタイプが敵だった場合
			if (obj->GetColType() == ObjectBase::ColType::Enemy)
			{
				// 死んだのがボスで、ボスの死亡フラグが立っていなかった場合
				if (dynamic_cast<EnemyBase*>(obj)->GetEnemyType() == EnemyBase::EnemyType::Boss
				&& !m_isBossDead)
				{
					// ボスの死亡フラグを立てる
					m_isBossDead = true;
				}
			}		

			// プレイヤー以外
			if (obj->GetColType() != ObjectBase::ColType::Player)
			{
				// ロックオンされていた場合
				if (m_pLockOnEnemy == obj)
				{
					// ロックオン状態を無効にする
					m_pLockOnEnemy = nullptr;
				}

				// ポインタを解放する
				delete obj;
				obj = nullptr;
				it = m_pObject.erase(it);
			}			
		}
		else
		{
			// イテレータを進める
			it++;
		}
	}


#ifdef _DEBUG

	// エンターキーが押された場合
	if (CheckHitKey(KEY_INPUT_RETURN))
	{
		// 強制クリア
		m_isBossDead = true;
	}

#endif // _DEBUG

	// ボスが死亡しているかつゲームクリアフラグが立っていなかった場合
	if (m_isBossDead && !m_isGameClear)
	{
		// 一定時間立った場合
		m_isGameClear = true;

		// クリア後装備保存
		m_pGear->StageClear();
		m_pGear->SaveGear();

		// 全オブジェクトのステージクリア処理をする
		for (auto& obj : m_pObject)
		{
			// ステージクリア処理を行う
			obj->StageClear();
		}

	}

	// ステージクリアフラグが立っていたらこれ以上更新処理を行わない
	if (m_isGameClear) return;

	// 当たり判定
	for (auto& myObj : m_pObject)
	{
		for (auto& targetObj : m_pObject)
		{
			// 同じオブジェクトの場合は当たり判定の処理をしない
			if (myObj == targetObj)continue;
			// 当たり判定の処理
			m_pCollision->UpdateCollision(myObj, targetObj);
		}
	}
}

void ObjectManager::Draw()
{
	// カメラ位置リセット
	GetPlayer()->GetCamera()->ResetCamera();

	// オブジェクト描画
	for (auto& obj : m_pObject)
	{
		obj->Draw(m_pToon);
	}

	// シャドウマップの書き込み開始
	m_pShadowMapShader->WriteStart(GetPlayer()->GetPos());

	// フィールドにシャドウマップを張り付ける為、まずフィールド以外に処理を行う
	for (auto& obj : m_pObject)
	{
		if (obj->GetColType() != ObjectBase::ColType::Field)
		{
			obj->ShadowMapDraw(m_pShadowMapShader);
		}
	}

	// 終了
	m_pShadowMapShader->WriteEnd();

	// カメラ位置リセット
	GetPlayer()->GetCamera()->ResetCamera();

	// フィールドにシャドウマップを張り付ける準備をする
	m_pShadowMapShader->SetShaderField(GetPlayer()->GetPos());
	for (auto& obj : m_pObject)
	{
		// ここではフィールドだけ描画する
		if (obj->GetColType() == ObjectBase::ColType::Field)
		{
			obj->ShadowMapDraw(m_pShadowMapShader);
		}
	}
	// 作業終了
	m_pShadowMapShader->WriteEnd();

	// ステージクリア時は描画しない
	if (!m_isGameClear)
	{
		// カメラ位置リセット
		GetPlayer()->GetCamera()->ResetCamera();

		// 2D描画
		for (auto& obj : m_pObject)
		{
			obj->Draw2D();
		}
		m_pGear->Draw();
	}

	// カメラ位置リセット
	GetPlayer()->GetCamera()->ResetCamera();
}

Player* const ObjectManager::GetPlayer()
{
	// 全オブジェクトを検索してプレイヤーを見つける
	for (auto& obj : m_pObject)
	{
		if (obj->GetColType() != ObjectBase::ColType::Player) continue;

		return dynamic_cast<Player*>(obj);
	}

	return nullptr;
}

void ObjectManager::InitLockOnEnemy()
{
	// ロックオン出来る範囲の設定
	float nearEnemyToPlayerVec = kLockOnRange;
	// カメラの向いている方向の取得
	float cameraAngle = GetPlayer()->GetCamera()->GetCameraAngleX();

	// ロックオンしている敵のポインタを空にする
	m_pLockOnEnemy = nullptr;

	for (auto& obj : m_pObject)
	{
		// 当たり判定の種類がエネミーじゃなかったらcontinueする
		if (obj->GetColType() != ObjectBase::ColType::Enemy) continue;

		// 敵からプレイヤーまでのベクトルを求める
		VECTOR enemyToPlayerVec = VSub(GetPlayer()->GetInfo().pos, obj->GetInfo().pos);

		// 敵からプレイヤーまでの角度を求める
		float enemyToPlayerAngle = static_cast<float>(atan2(enemyToPlayerVec.x, enemyToPlayerVec.z));
		// カメラの角度と敵からプレイヤーまでの角度の差を求める
		float differenceAngle = cameraAngle - enemyToPlayerAngle;

		// ロックオンできる範囲に入っているかどうか調べる
		bool isLockOnRangeMin = differenceAngle < kMinLockOnValidRangeMax && differenceAngle > kMinLockOnValidRangeMin;
		bool isLockOnRangeMax = differenceAngle > kMaxLockOnValidRangeMax || differenceAngle < kMaxLockOnValidRangeMin;

		// 角度の違いによってロックオンできる敵かどうかを判断する
		if (isLockOnRangeMin || isLockOnRangeMax)
		{
			// 敵からプレイヤーまでの距離を求める
			float EnemyToPlayerLen = VSize(dynamic_cast<EnemyBase*>(obj)->GetEnemyToPlayerVec());

			// 今見ている敵が今までに見た敵よりも近くにいる場合
			if (nearEnemyToPlayerVec > EnemyToPlayerLen)
			{
				// 一番近い距離を設定する
				nearEnemyToPlayerVec = EnemyToPlayerLen;
				// 今見ている敵をロックオンする敵にする
				m_pLockOnEnemy = dynamic_cast<EnemyBase*>(obj);
			}
		}
	}
}

bool ObjectManager::IsGameOver() const
{
	// 全オブジェクトを検索してプレイヤーを見つける
	for (auto& obj : m_pObject)
	{
		if (obj->GetColType() == ObjectBase::ColType::Player)
		{
			// 死亡状態かどうかを返す
			return dynamic_cast<Player*>(obj)->IsDead();
		}
	}

	// もしプレイヤーが見つからなかった場合はfalseを返す
	return false;
}
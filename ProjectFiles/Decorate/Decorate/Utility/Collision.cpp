#include "Collision.h"

#include "../Object/ObjectBase.h"
#include "../Object/Player/Player.h"
#include "../Object/Player/Shot.h"
#include "../Object/Enemy/EnemyBase.h"
#include "../Object/Enemy/EnemyBossBase.h"
#include "../Object/Camera.h"
#include "../Object/RecoveryItem.h"

Collision::Collision()
{
	/*処理無し*/
}

Collision::~Collision()
{
	/*処理無し*/
}

void Collision::UpdateCollision(ObjectBase* my, ObjectBase* target)
{
	// 存在しない物体は当たらない
	if (!my->GetInfo().isExist && !target->GetInfo().isExist) return;
	// 種類が設定設定されてなければには何もぶつからない
	if (my->GetColType() == ObjectBase::ColType::None) return;

	// オブジェクトタイプがプレイヤーの場合
	if (my->GetColType() == ObjectBase::ColType::Player)
	{
		// ターゲットのオブジェクトがフィールドの場合
		if (target->GetColType() == ObjectBase::ColType::Field)
		{
			// カメラがフィールドにめり込まないようにする
			dynamic_cast<Player*>(my)->GetCamera()->ColUpdate(target);
			my->MoveCollField(target);
			return;
		}

		// ターゲットのオブジェクトがエネミーの場合
		if (target->GetColType() == ObjectBase::ColType::Enemy)
		{
			// プレイヤーが回避状態の時はキャラクター同士の衝突判定を行わない
			if (!dynamic_cast<Player*>(my)->IsDodge())
			{
				// キャラクター同士衝突判定
				dynamic_cast<CharacterBase*>(my)->MoveCollCharacter(dynamic_cast<CharacterBase*>(target));
			}
			// ショット衝突判定
			dynamic_cast<Player*>(my)->GetShot()->OnAttack(dynamic_cast<CharacterBase*>(target));
			// 攻撃衝突判定
			dynamic_cast<Player*>(my)->OnAttack(dynamic_cast<CharacterBase*>(target));
			// 強攻撃衝突判定
			dynamic_cast<Player*>(my)->OnHardAttack(dynamic_cast<CharacterBase*>(target));

			return;
		}
	}

	// オブジェクトタイプがエネミーの場合
	if (my->GetColType() == ObjectBase::ColType::Enemy)
	{
		// ターゲットのオブジェクトがフィールドの場合
		if (target->GetColType() == ObjectBase::ColType::Field)
		{
			my->MoveCollField(target);
			return;
		}

		// ターゲットのオブジェクトがプレイヤーの場合
		if (target->GetColType() == ObjectBase::ColType::Player)
		{

			// プレイヤーが回避状態の時はキャラクター同士の衝突判定を行わない
			if (!dynamic_cast<Player*>(target)->IsDodge())
			{
				// キャラクター同士衝突判定
				dynamic_cast<CharacterBase*>(my)->MoveCollCharacter(dynamic_cast<CharacterBase*>(target));
			}
			// 索敵範囲衝突判定
			dynamic_cast<EnemyBase*>(my)->OnSearch(target);
			// 近距離攻撃衝突判定
			dynamic_cast<EnemyBase*>(my)->OnAttack(dynamic_cast<CharacterBase*>(target));
			// ボス敵だった場合
			if (dynamic_cast<EnemyBase*>(my)->GetEnemyType() == EnemyBase::EnemyType::Boss)
			{
				// 強攻撃衝突判定
				dynamic_cast<EnemyBossBase*>(my)->OnHardAttack(dynamic_cast<CharacterBase*>(target));
			}
			return;
		}

		// ターゲットのオブジェクトがエネミーの場合
		if (target->GetColType() == ObjectBase::ColType::Enemy)
		{
			// キャラクター同士衝突判定
			dynamic_cast<CharacterBase*>(my)->MoveCollCharacter(dynamic_cast<CharacterBase*>(target));
		}
	}

	// オブジェクトタイプがアイテムの場合
	if (my->GetColType() == ObjectBase::ColType::Item)
	{
		// ターゲットのオブジェクトがプレイヤーの場合
		if (target->GetColType() == ObjectBase::ColType::Player)
		{
			// プレイヤーに近付くか判定
			dynamic_cast<RecoveryItem*>(my)->OnApproach(target);
			// 衝突判定
			dynamic_cast<RecoveryItem*>(my)->OnGet(dynamic_cast<Player*>(target));
		}
	}
}

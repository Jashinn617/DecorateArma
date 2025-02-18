#pragma once
#include "DxLib.h"

#include "../CharacterBase.h"

#include "../../Utility/CharacterData.h"

class PlayerState;
class Shot;
class Camera;
class Time;
class RecoveryItem;

/// <summary>
/// プレイヤー
/// </summary>
class Player : public CharacterBase
{
public:		// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Player();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override final{/*処理無し*/ }

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override final;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="pToonShader">トゥーンシェーダポインタ</param>
	virtual void Draw(std::shared_ptr<ToonShader> pToonShader) override final;

	/// <summary>
	/// 2D画像描画
	/// </summary>
	void Draw2D() override final;


	/// <summary>
	/// ダメージ処理
	/// </summary>
	/// <param name="targetPos">攻撃を当てた相手の座標</param>
	/// <param name="damagePoint">ダメージ量</param>
	/// <param name="isInvincible">無敵時間を発動させるかどうか</param>
	void OnDamage(VECTOR targetPos, int damagePoint, bool isInvincible = true) override final;

	/// <summary>
	/// 当たり判定の種類の取得
	/// </summary>
	/// <returns>当たり判定の種類</returns>
	ColType GetColType()const override final { return ColType::Player; }

	/// <summary>
	/// カメラ取得
	/// </summary>
	/// <returns>カメラポインタ</returns>
	const std::shared_ptr<Camera> GetCamera()const { return m_pCamera; }

	/// <summary>
	/// ショット取得
	/// </summary>
	/// <returns>ショットポインタ</returns>
	const std::shared_ptr<Shot> GetShot()const { return m_pShot; }

	/// <summary>
	/// ステイトごとの初期化
	/// </summary>
	void InitState();

	/// <summary>
	/// ステータス情報の設定
	/// </summary>
	void SetStatus();

	/// <summary>
	/// 攻撃処理
	/// </summary>
	void OnAttack(CharacterBase* pEnemy);

	/// <summary>
	/// 強攻撃処理
	/// </summary>
	/// <param name="pEnemy">敵ポインタ</param>
	void OnHardAttack(CharacterBase* pEnemy);

	/// <summary>
	/// 回復処理
	/// </summary>
	/// <param name="recoveryNum">回復量</param>
	void OnRecovery(int recoveryNum);

	/// <summary>
	/// 回避状態かどうかの取得
	/// </summary>
	/// <returns>回避状態の有無</returns>
	bool IsDodge() { return m_isDodge; }

private:	// 関数
	/// <summary>
	/// 角度更新
	/// </summary>
	void UpdateAngle();

	/// <summary>
	/// 移動方向更新
	/// </summary>
	void UpdateMoveDirection();

	/// <summary>
	/// カメラ更新
	/// </summary>
	void UpdateCamera();

	/// <summary>
	/// 移動処理
	/// </summary>
	/// <returns>移動値</returns>
	VECTOR Move();

	/// <summary>
	/// 回避初期化
	/// </summary>
	void InitDodge();

	/// <summary>
	/// ステイトごとの更新
	/// </summary>
	void UpdateState();

	/// <summary>
	/// 攻撃更新
	/// </summary>
	void UpdateAttack();

	/// <summary>
	/// 強攻撃更新
	/// </summary>
	void UpdateHardAttack();

	/// <summary>
	/// 回避更新
	/// </summary>
	void UpdateDodge();

	/// <summary>
	/// 攻撃座標更新
	/// </summary>
	/// <param name="angle"></param>
	void UpdateAttackPosition(float angle);

private:	// 変数
	int m_maxHp;									// 最大HP
	int m_attackCount;								// 連続で攻撃した回数

	int m_attackEffectIndex;						// 攻撃時エフェクト座標用フレーム番号
	int m_hardAttackIndex;							// 強攻撃座標用フレーム番号

	bool m_isColl;									// 攻撃が当たったか
	bool m_isAttack;								// 攻撃中か
	bool m_isHardAttack;							// 強攻撃中か
	bool m_isNextAttack;							// 次の攻撃が実行されるかどうか
	bool m_isLockOn;								// ロックオン状態かどうか
	bool m_isDodge;									// 回避中かどうか

	VECTOR m_moveDirection;							// 移動方向
	VECTOR m_attackPos;								// 攻撃座標
	VECTOR m_hardAttackPos;							// 強攻撃座標
	VECTOR m_dodgeDirection;						// 回避方向
	VECTOR m_prevDodgeDirection;					// 回避前に向いていた方向
	VECTOR m_attackEffectPos;						// 攻撃時エフェクト座標

	std::shared_ptr<PlayerState> m_pState;			// ステイトポインタ
	std::shared_ptr<Camera> m_pCamera;				// カメラポインタ
	std::shared_ptr<Shot> m_pShot;					// ショット
	std::shared_ptr<CollisionShape> m_attackColl;	// 剣の当たり判定
	std::shared_ptr<CollisionShape> m_hardAtkColl;	// 強攻撃当たり判定

	std::shared_ptr<Time> m_attackStanTime;			// 攻撃間隔時間
	std::shared_ptr<Time> m_invincibleTime;			// 無敵時間
	std::shared_ptr<Time> m_dodgeTime;				// 回避時間
};
#pragma once
#include "ObjectBase.h"

class HpBarBase;
class Gear;

/// <summary>
/// オブジェクトの中でもキャラクターの基底クラス
/// </summary>
class CharacterBase : public ObjectBase
{
public:	// 構造体

	/// <summary>
	/// ステータス情報
	/// </summary>
	struct StatusData
	{
		int hp = 0;				// 体力
		int meleeAtk = 0;		// 近距離攻撃力
		int shotAtk = 0;		// 遠距離攻撃力
		int def = 0;			// 防御力
		float spd = 0.0f;		// 速度
	};

	/// <summary>
	/// 移動ステータス情報
	/// </summary>
	struct MoveStatusData
	{
		float walkSpeed = 0.0f;		// 歩き速度
		float dashSpeed = 0.0f;		// 走り速度
		float acc = 0.0f;	// 加速度
		float rotSpeed = 0.0f;		// 回転速度
	};

	/// <summary>
	/// アニメーション情報
	/// </summary>
	struct AnimData
	{
		int8_t idle = 0;			// 待機
		int8_t walk = 0;			// 歩き
		int8_t run = 0;				// 走り
		int8_t attack1 = 0;			// 攻撃1
		int8_t attack2 = 0;			// 攻撃2
		int8_t attack3 = 0;			// 攻撃3
		int8_t hardAttack = 0;		// 強攻撃
		int8_t damage = 0;			// ダメージ
		int8_t death = 0;			// 死亡
		int8_t jumpStart = 0;		// ジャンプ時
		int8_t jumpIdle = 0;		// ジャンプ中
	};

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	CharacterBase();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~CharacterBase();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() override = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() override = 0;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="pToonShader">トゥーンシェーダポインタ</param>
	virtual void Draw(std::shared_ptr<ToonShader> pToonShader)override = 0;

	/// <summary>
	/// ダメージ処理
	/// </summary>
	/// <param name="targetPos">攻撃を当てた相手の座標</param>
	/// <param name="damagePoint">ダメージ量</param>
	/// <param name="isInvincible">無敵時間を発動させるかどうか</param>
	virtual void OnDamage(VECTOR targetPos,int damagePoint,bool isInvincible  = true) {/*処理無し*/ }

	/// <summary>
	/// キャラクター同士の当たり判定を考慮した移動処理
	/// </summary>
	/// <param name="pTarget">相手のキャラクターポインタ</param>
	void MoveCollCharacter(CharacterBase* pTarget);

	/// <summary>
	/// 地面に当たっているときの処理
	/// </summary>
	void HitGround();

	/// <summary>
	/// 重力による落下処理
	/// </summary>
	void UpdateGravity();

	/// <summary>
	/// 角度の更新
	/// </summary>
	/// <returns>角度</returns>
	float GetAngle() const { return m_angle; }

	/// <summary>
	/// ダメージを受けたかどうかを返す
	/// </summary>
	/// <returns>ダメージを受けたかどうか</returns>
	bool IsDamage()const { return m_isDamage; }

	/// <summary>
	/// 攻撃を受けていない状態に戻す
	/// </summary>
	void OffDamage() { m_isDamage = false; }

	/// <summary>
	/// 死亡状態かどうかを返す
	/// </summary>
	/// <returns>死亡状態かどうか</returns>
	bool IsDead()const { return m_isDead; }

protected:	// 関数
	/// <summary>
	/// 角度を滑らかに変化させる関数
	/// </summary>
	/// <param name="nowAngle">現在の角度</param>
	/// <param name="nextAngle">目標角度</param>
	void SmoothAngle(float& nowAngle, float nextAngle);

protected:	// 変数
	float m_fallSpeed;								// 落下速度

	bool m_isDead;									// 死亡したかどうか
	bool m_isGravity;								// 重力を適応するかどうか
	bool m_isResetAttack;							// 攻撃判定の初期化(一度攻撃の当たった敵のもう一度当てる為)
	bool m_isDamage;								// 攻撃を受けたかどうか
	
	std::shared_ptr<HpBarBase> m_pHpBar;			// HPバーポインタ
	std::shared_ptr<Gear> m_pGear;						// 装備品

	StatusData m_statusData{};			// ステータス情報
	MoveStatusData m_moveData{};		// 移動情報
	AnimData m_animData{};				// アニメーション情報
};
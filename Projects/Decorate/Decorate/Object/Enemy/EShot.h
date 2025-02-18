#pragma once

#include "DxLib.h"

#include <memory>
#include <vector>

class Model;
class Time;
class CollisionShape;
class CharacterBase;

/// <summary>
/// 敵の弾
/// </summary>
class EShot
{
public:		// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="atkPoint">攻撃力</param>
	EShot(int atkPoint);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~EShot();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 攻撃処理
	/// </summary>
	/// <param name="pPlayer">プレイヤーポインタ</param>
	void OnAttack(CharacterBase* pPlayer);

	/// <summary>
	/// 弾生成
	/// </summary>
	/// <param name="pos">自分の座標</param>
	/// <param name="targetPos">ターゲット座標</param>
	void Make(VECTOR pos, VECTOR targetPos);

private:	// 構造体
	/// <summary>
	/// 弾情報
	/// </summary>
	struct Bullet
	{
		bool isExist = false;						// 存在フラグ
		VECTOR pos = VGet(0.0f,0.0f,0.0f);			// 座標
		VECTOR direction = VGet(0.0f, 0.0f, 0.0f);	// 進む方向
		std::shared_ptr<Time> vanishTime;			// 消えるまでの時間
		std::shared_ptr<CollisionShape> coll;		// 当たり判定
		std::shared_ptr<Model> model;				// モデル
	};

private:	// 変数
	int m_atkPoint;									// 攻撃力
	VECTOR m_pos;									// 座標
	std::vector<Bullet> m_bullet;					// 弾構造体
	std::shared_ptr<Time> m_pBulletIntervalTime;	// 次の球が発射されるまでの時間
};
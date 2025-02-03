#pragma once

class ObjectBase;

/// <summary>
/// 当たり判定用クラス
/// </summary>
class Collision
{
public:	// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Collision();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Collision();

	/// <summary>
	/// 当たり判定更新処理
	/// </summary>
	/// <param name="my">自身</param>
	/// <param name="target">相手</param>
	void UpdateCollision(ObjectBase* my, ObjectBase* target);
};


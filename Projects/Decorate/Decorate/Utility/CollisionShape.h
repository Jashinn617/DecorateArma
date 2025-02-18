#pragma once
#include "DxLib.h"

#include <memory>

/// <summary>
/// 当たり判定用図形
/// </summary>
class CollisionShape
{
public:	// 列挙型
	/// <summary>
	/// 図形タイプ
	/// </summary>
	enum class ShapeType
	{
		Sphere,		// 球
		Capsule,	// カプセル
	};

public:	// 関数
	/// <summary>
	/// コンストラクタ(カプセル)
	/// </summary>
	/// <param name="topPos">判定始点</param>
	/// <param name="bottomPos">判定終点</param>
	/// <param name="radisu">半径</param>
	CollisionShape(const VECTOR& topPos, const VECTOR& bottomPos, float radius);

	/// <summary>
	/// コンストラクタ(球)
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="radius">半径</param>
	/// <param name="height">高さ</param>
	CollisionShape(const VECTOR& pos, float radius, float height);

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~CollisionShape();

	/// <summary>
	/// 当たり判定のデバッグ表示
	/// </summary>
	/// <param name="color">図形の色</param>
	void DebugDraw(unsigned int color);

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="target">相手の当たり判定図形</param>
	/// <returns>当たっているかどうか</returns>
	bool IsCollide(const std::shared_ptr<CollisionShape> target) const;

	/// <summary>
	/// 球の中心座標の取得
	/// </summary>
	/// <returns>円の中心</returns>
	const VECTOR& GetCenter() const { return *m_pos; }

	/// <summary>
	/// 判定始点の取得
	/// </summary>
	/// <returns>判定始点</returns>
	const VECTOR& GetTopPos()const { return *topPos; }

	/// <summary>
	/// 判定終点の取得
	/// </summary>
	/// <returns>判定終点</returns>
	const VECTOR& GetBottomPos()const { return *bottomPos; }

	/// <summary>
	/// 半径の取得
	/// </summary>
	/// <returns>半径</returns>
	float GetRadius()const { return m_radius; }

	/// <summary>
	/// 図形タイプの取得
	/// </summary>
	/// <returns>図形タイプ</returns>
	ShapeType GetShapeType()const { return m_shapeType; }

private:	// 関数

	/// <summary>
	/// 球同士の衝突判定
	/// </summary>
	/// <param name="target">相手の当たり判定図形</param>
	/// <returns>当たっているかどうか</returns>
	bool SphereToSphereCollide(const std::shared_ptr<CollisionShape> target)const;

	/// <summary>
	/// カプセル同士の衝突判定
	/// </summary>
	/// <param name="target">相手の当たり判定図形</param>
	/// <returns>当たっているかどうか</returns>
	bool CapsuleToCapsuleCollide(const std::shared_ptr<CollisionShape> target)const;

	/// <summary>
	/// カプセルと球の衝突判定
	/// </summary>
	/// <param name="target">相手の当たり判定図形</param>
	/// <returns>当たっているかどうか</returns>
	bool CapsuleToSphereCollide(const std::shared_ptr<CollisionShape> target)const;

	/// <summary>
	/// 球とカプセルの衝突判定
	/// </summary>
	/// <param name="target">相手の当たり判定図形</param>
	/// <returns>当たっているかどうか</returns>
	bool SphereToCapsuleCollide(const std::shared_ptr<CollisionShape> target)const;

private:	// 変数
	const float m_radius;			// 半径
	const float m_height;			// 高さ
	const VECTOR* m_pos;			// 中心座標
	const VECTOR* topPos;			// 判定始点
	const VECTOR* bottomPos;		// 判定終点

	ShapeType m_shapeType;			// 図形タイプ
};


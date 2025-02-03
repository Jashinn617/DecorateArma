#include "CollisionShape.h"

CollisionShape::CollisionShape(const VECTOR& topPos, const VECTOR& bottomPos, float radius):
	m_radius(radius),
	m_height(0.0f),
	m_pos(nullptr),
	topPos(&topPos),
	bottomPos(&bottomPos),
	m_shapeType(ShapeType::Capsule)
{
	/*処理無し*/
}

CollisionShape::CollisionShape(const VECTOR& pos, float radius, float height):
	m_radius(radius),
	m_height(height),
	m_pos(&pos),
	topPos(nullptr),
	bottomPos(nullptr),
	m_shapeType(ShapeType::Sphere)
{
	/*処理無し*/
}

CollisionShape::~CollisionShape()
{
	/*処理無し*/
}

void CollisionShape::DebugDraw(unsigned int color)
{
	// デバッグ表示
#ifdef _DEBUG
	/*図形のタイプによって処理を変える*/
	switch (m_shapeType)
	{
	case CollisionShape::ShapeType::Sphere:	// 球
		// 中央座標の取得
		VECTOR center = VAdd(*m_pos, VGet(0.0f, m_height, 0.0f));
		// 描画
		DrawSphere3D(center, m_radius, 32, color, color, false);
		break;
	case CollisionShape::ShapeType::Capsule: // カプセル
		// 描画
		DrawCapsule3D(*topPos, *bottomPos,
			m_radius, 32, color, color, false);
		break;
	default:
		break;
	}
#endif // _DEBUG
}

bool CollisionShape::IsCollide(const std::shared_ptr<CollisionShape> target) const
{
	/*自分と相手の図形のタイプによって処理を変える*/
	// どっちも球
	if (m_shapeType == ShapeType::Sphere && target->GetShapeType() == ShapeType::Sphere)
	{
		return SphereToSphereCollide(target);
	}
	// どっちもカプセル
	if (m_shapeType == ShapeType::Capsule && target->GetShapeType() == ShapeType::Capsule)
	{
		return CapsuleToCapsuleCollide(target);
	}
	// 自分が球で相手がカプセル
	if (m_shapeType == ShapeType::Sphere && target->GetShapeType() == ShapeType::Capsule)
	{
		return SphereToCapsuleCollide(target);
	}
	// 自分がカプセルで相手が球
	{
		return CapsuleToSphereCollide(target);
	}

	// エラー
#ifdef _DEBUG
	DrawString(0, 300, "図形タイプが正常でない", 0xff0000);
#endif // _DEBUG

	return false;
}

bool CollisionShape::SphereToSphereCollide(const std::shared_ptr<CollisionShape> target) const
{
	// 真ん中の座標
	VECTOR center = VAdd(*m_pos, VGet(0.0f, m_height, 0.0f));
	// 相手の真ん中の座標
	VECTOR targetCenter = VAdd(*target->m_pos, VGet(0.0f, target->m_height, 0.0f));

	// 相手の中心と自分の中心の距離の計算
	float distance = static_cast<float>(fabs(VSize(VSub(targetCenter, center))));
	// 半径の合計の計算
	float radius = m_radius + target->m_radius;

	// 相手の中心と自分の中心の距離が半径の合計以下だった場合は当たっている
	return distance <= radius;
}

bool CollisionShape::CapsuleToCapsuleCollide(const std::shared_ptr<CollisionShape> target) const
{
	// 当たり判定
	return HitCheck_Capsule_Capsule(*topPos, *bottomPos, m_radius,
		target->GetTopPos(), target->GetBottomPos(), target->GetRadius());
}

bool CollisionShape::CapsuleToSphereCollide(const std::shared_ptr<CollisionShape> target) const
{
	// 相手の真ん中の座標
	VECTOR targetCenter = VAdd(*target->m_pos, VGet(0.0f, target->m_height, 0.0f));

	// 判定
	return HitCheck_Sphere_Capsule(targetCenter, target->GetRadius(),
		*topPos, *bottomPos, m_radius);
}

bool CollisionShape::SphereToCapsuleCollide(const std::shared_ptr<CollisionShape> target) const
{
	// 真ん中の座標
	VECTOR center = VAdd(*m_pos, VGet(0.0f, m_height, 0.0f));

	// 判定
	return HitCheck_Sphere_Capsule(center, m_radius,
		target->GetTopPos(), target->GetBottomPos(), target->GetRadius());
}
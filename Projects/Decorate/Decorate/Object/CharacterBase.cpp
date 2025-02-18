#include "CharacterBase.h"

#include "../Utility/CollisionShape.h"

namespace
{
	constexpr int kMaxColHitPolyNum = 2000;				// 最大当たり判定ポリゴン数
	constexpr int kMaxColHitTryNum = 16;				// 壁に押し出す処理の最大試行回数
	constexpr float kColHitSlideLength = 1.0f;			// 壁押し出し時にスライドさせる距離
	constexpr float kGravity = 0.8f;					// 重力
	constexpr float kFixGravity = 0.5f;					// 重力が急激に速度を変えないように調整する数
	constexpr float kFallMaxSpeed = -15.0f;				// 最大落下速度
	constexpr float kChangeAngleSpeed = 0.25f;			// 滑らかに角度を移動させる速度
	constexpr float kAngleDiffMax = DX_PI_F + 0.3f;		// 角度の最大差
	constexpr float kAngleDiffMin = -DX_PI_F - 0.3f;	// 角度の最小差
	constexpr float kSlideLength = 5.0f;				// 押し出しの大きさ	
}

CharacterBase::CharacterBase():
	m_fallSpeed(0.0f),
	m_isDead(false),
	m_isGravity(true),
	m_isResetAttack(false),
	m_isDamage(false)
{
	/*処理無し*/
}

CharacterBase::~CharacterBase()
{
	/*処理無し*/
}

void CharacterBase::MoveCollCharacter(CharacterBase* pTarget)
{
	// 自分か相手が死亡状態の時は判定をしない
	if (m_isDead || pTarget->IsDead())return;

	// オブジェクト同士の当たり判定をする
	// オブジェクト同士が当たっていなかった場合は何もしない
	if (!GetCollShape()->IsCollide(pTarget->GetCollShape()))return;

	// 移動前の座標を保存する(座標 - 移動ベクトル)
	VECTOR prevPos = VSub(m_characterInfo.pos, m_characterInfo.vec);

	VECTOR nextPos;		// 移動後の座標
	VECTOR slideVec;	// 相手のオブジェクトを考慮して移動する座標

	// 移動方向と相手の座標から自分の座標を引いたベクトルに垂直なベクトルを計算する
	slideVec = VCross(m_characterInfo.vec,
		VNorm(VSub(m_characterInfo.pos, pTarget->GetInfo().pos)));

	// 上で出したベクトルを使って、相手方向への移動成分を除く
	slideVec = VCross(VNorm(VSub(m_characterInfo.pos, pTarget->GetInfo().pos)),
		slideVec);

	// 相手方向への移動成分を除いたベクトルを足す
	nextPos = VAdd(prevPos, slideVec);

	// 押し出し処理
	for (int i = 0; i < kMaxColHitTryNum; i++)
	{
		// 相手と当たっていなかったら終わる
		if (!GetCollShape()->IsCollide(pTarget->GetCollShape()))return;

		// 相手とのベクトルの計算
		VECTOR move = VSub(m_characterInfo.pos, pTarget->GetInfo().pos);
		// 上で出したベクトルを使って相手との距離を取得する
		float vecSize = VSize(move);
		// ベクトルを正規化してスライドさせる距離を調性する
		move = VScale(move, (1.0f / (vecSize * kSlideLength)));

		// 相手方向への移動成分を除いたベクトルにスライドする距離を足す
		nextPos = VAdd(nextPos, move);

		// 座標を保存する
		m_characterInfo.pos = nextPos;
	}

	// 新しい座標を保存する
	m_characterInfo.pos = nextPos;
}

void CharacterBase::HitGround()
{
	// 落下速度を0にする
	m_fallSpeed = 0.0f;
}

void CharacterBase::UpdateGravity()
{
	// 重力が適応されなかったら何もしない
	if (!m_isGravity) return;

	// 落ちる速度が最大落下速度を超えないようにする
	m_fallSpeed = max(m_fallSpeed - (kGravity * kFixGravity), kFallMaxSpeed);

	// 落下処理
	m_characterInfo.vec.y = m_fallSpeed;
}

void CharacterBase::SmoothAngle(float& nowAngle, float nextAngle)
{
	// -180度以下になったら角度が大きくなりすぎないように360度足す
	if (nextAngle < -DX_PI_F)
	{
		nextAngle += DX_TWO_PI_F;
	}
	// 180度以上になったら角度が小さくなりすぎないように360度引く
	if (nextAngle > DX_PI_F)
	{
		nextAngle -= DX_TWO_PI_F;
	}

	// 角度の差を求める
	float angleDiff = nowAngle - nextAngle;

	// 角度の差が最大差より大きかった場合
	if (angleDiff > kAngleDiffMax)
	{
		// 角度の差が小さくなりすぎないように360度引く
		nowAngle -= DX_TWO_PI_F;
	}
	// 角度の差が最大差より小さかった場合
	else if (angleDiff < kAngleDiffMin)
	{
		// 角度の差が角度が大きくなりすぎないように360度足す
		nowAngle += DX_TWO_PI_F;
	}

	// 角度の差が小さい場合は何もせずにそのまま次の角度を返す
	if (angleDiff < kChangeAngleSpeed && angleDiff > -kChangeAngleSpeed)
	{
		nowAngle = nextAngle;
		return;
	}

	/*それ以外の場合は少しずつ角度を変化させる*/
	else if (angleDiff > kChangeAngleSpeed)
	{
		nowAngle -= kChangeAngleSpeed;
		return;
	}
	else if (angleDiff < kChangeAngleSpeed)
	{
		nowAngle += kChangeAngleSpeed;
		return;
	}
}

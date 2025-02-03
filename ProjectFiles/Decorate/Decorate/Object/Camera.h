#pragma once
#include "DxLib.h"

#include "ObjectBase.h"

#include <memory>

class LockOnTarget;
class CollisionShape;

/// <summary>
/// カメラ
/// </summary>
class Camera
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Camera();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Camera();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="targetPos">ターゲット座標</param>
	/// <param name="isLockOn">ロックオン状態かどうか</param>
	void Update(VECTOR targetPos, bool isLockOn);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// カメラの座標のリセット
	/// </summary>
	void ResetCamera();

	/// <summary>
	/// 当たり判定の更新
	/// </summary>
	/// <param name="Field">フィールドポインタ</param>
	void ColUpdate(ObjectBase* Field);

	/// <summary>
	/// カメラの水平角度の取得
	/// </summary>
	/// <returns>カメラ水平角度</returns>
	float GetCameraAngleX() { return m_angleH; }

	/// <summary>
	/// 当たり判定の図形の取得
	/// </summary>
	/// <returns>当たり判定図形ポインタ</returns>
	const std::shared_ptr<CollisionShape> GetCircle() const { return m_pCollShape; }

	/// <summary>
	/// 座標の取得
	/// </summary>
	/// <returns>カメラ座標</returns>
	const VECTOR& GetPos() const { return m_nextPos; }

	/// <summary>
	/// 前フレームのカメラの座標の取得
	/// </summary>
	/// <returns>前フレームカメラ座標</returns>
	const VECTOR& GetPrevPos()const { return m_prevPos; }

	/// <summary>
	/// ロックオンしている敵座標の設定
	/// </summary>
	/// <param name="lockOnEnemyPos">ロックオンしている敵座標</param>
	void SetLockOnEnemyPos(VECTOR lockOnEnemyPos) { m_lockOnEnemyPos = lockOnEnemyPos; }

	/// <summary>
	/// ロックオンしている敵座標の取得
	/// </summary>
	/// <returns>ロックオンしている敵座標</returns>
	VECTOR GetLockOnEnemyPos() { return m_lockOnEnemyPos; }

	/// <summary>
	/// ロックオン状態かどうかの取得
	/// </summary>
	/// <returns>ロックオン状態かどうか</returns>
	bool IsLockOn() { return m_isLockOn; }

private:	// 関数
	/// <summary>
	/// 角度更新
	/// </summary>
	/// <param name="isLockOn">ロックオン状態かどうか</param>
	void UpdateAngle(bool isLockOn);

	/// <summary>
	/// 通常時更新
	/// </summary>
	/// <param name="targetPos">ターゲット座標</param>
	void NormalUpdate(VECTOR targetPos);

	/// <summary>
	/// ロックオン時更新
	/// </summary>
	/// <param name="targetPos">ターゲット座標</param>
	void LockOnUpdate(VECTOR targetPos);

	/// <summary>
	/// 座標の更新
	/// </summary>
	void UpdatePos();

	/// <summary>
	/// 当たり判定を考慮した座標更新
	/// </summary>
	void FixPos();

	/// <summary>
	/// 押し出し処理
	/// </summary>
	void FixPosInternal();

private:	// 変数
	float m_angleH;									// 水平角度
	float m_angleV;									// 垂直角度
	float m_cameraToTargetLenght;					// カメラからターゲットまでの距離
	VECTOR m_pos;									// 座標
	VECTOR m_nextPos;								// 次の座標
	VECTOR m_prevPos;								// 前の座標
	VECTOR m_targetPos;								// カメラからターゲットの座標
	MATRIX m_rotX;									// 回転用行列X
	MATRIX m_rotY;									// 回転用行列Y
	std::shared_ptr<CollisionShape> m_pCollShape;	// 当たり判定ポインタ

private:	// ロックオン用変数
	bool m_isLockOn;								// ロックオンしているかどうか
	VECTOR m_lockOnEnemyPos;						// ロックオンした敵の座標

	std::shared_ptr<LockOnTarget> m_pLockOnTarget;	// ロックオン処理ポインタ

private:	// 当たり判定用変数
	bool m_isMove;		// 移動したかどうか
	bool m_isPolyHit;	// ポリゴンに当たったかどうか

	MV1_COLL_RESULT_POLY_DIM m_hitDim{};									// 当たり判定結果代入用ポリゴン構造体
	MV1_COLL_RESULT_POLY* m_pPolyIndex[ColInfo::kMaxColHitPolyNum]{};	// 壁ポリゴンと判断されたポリゴンのアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* m_pPoly;											// ポリゴンの構造体にアクセスする為に使用するポインタ
	HITRESULT_LINE m_lineRes{};												// 線分とポリゴンとの当たり判定の結果を代入する構造体
};
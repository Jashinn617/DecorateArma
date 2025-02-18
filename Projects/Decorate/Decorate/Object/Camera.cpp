#include "Camera.h"

#include "ObjectBase.h"
#include "Model.h"
#include "LockOnTarget.h"

#include "../Utility/CollisionShape.h"

namespace
{
	constexpr int kMaxInputNum = 1000;													// アナログ入力状態最大値
	constexpr int kMaxColHitTryNum = 50;												// カメラの押し出し試行回数
	constexpr float kNear = 5.0f;														// カメラの一番近い描画距離
	constexpr float kFar = 7000.0f;														// カメラの一番遠い描画距離
	constexpr float kCameraAngleSpeedX = 0.1f;											// カメラの旋回速度X
	constexpr float kCameraAngleSpeedY = 0.05f;											// カメラの旋回速度Y
	constexpr float kCameraAngleVMax = DX_PI_F / 2.0f - 0.8f;							// カメラの最大垂直角度
	constexpr float kCameraAngleVMin = -DX_PI_F / 2.0f + 0.6f;							// カメラの最小垂直角度
	constexpr float kCameraViewpointHeight = 150.0f;									// ターゲットから注視点までの高さ
	constexpr float kCameraToTargetLenghtMax = 400.0f;									// カメラからターゲットまでの最大距離
	constexpr float kCameraToTargetLenghtMin = 10.0f;									// カメラからターゲットまでの最小距離
	constexpr float kCameraToTargetLenghtSpeed = 5.0f;									// カメラからターゲットまでの距離を変える速度
	constexpr float kSize = 50.0f;														// 当たり判定用のカメラのサイズ
	
	constexpr float kCameraFollowSpeed = 0.2f;											// カメラが付いてくる速度
	constexpr float kPrevCameraFollowSpeed = 1.0f - kCameraFollowSpeed;					// カメラの初速度
	constexpr float kCameraTargetFollowSpeed = 0.2f;									// カメラが注視点を追いかける速度
	constexpr float kPrevCameraTargetFollowSpeed = 1.0f - kCameraTargetFollowSpeed;		// カメラが注視点を追いかける初速度

	/*ロックオン時*/
	constexpr float kLockOnAngleH = -DX_PI_F * 0.5f + 0.1f;										// ロックオン時にカメラを少し傾けるための値
	constexpr float kLockOnAngleV = -DX_PI_F / 2.0f + 1.3f;										// ロックオン時にカメラを少し傾けるための値
	constexpr float kCameraTargetLockOnFollowSpeed = 0.5f;										// ロックオン時にカメラが付いてくる速度
	constexpr float kPrevCameraTargetLockOnFollowSpeed = 1.0f - kCameraTargetLockOnFollowSpeed;	// ロックオン時にカメラが付いてくる初速度
}

Camera::Camera():
	m_angleH(0.0f),
	m_angleV(0.0f),
	m_cameraToTargetLenght(0.0f),
	m_pos{0.0f,0.0f,0.0f},
	m_nextPos{0.0f,0.0f,0.0f},
	m_prevPos{0.0f,0.0f,0.0f},
	m_targetPos{0.0f,0.0f,0.0f},
	m_rotX(MGetRotX(m_angleV)),
	m_rotY(MGetRotY(m_angleH)),
	m_pCollShape(std::make_shared<CollisionShape>(m_nextPos,kSize,0.0f)),
	m_isLockOn(false),
	m_pLockOnTarget(std::make_shared<LockOnTarget>()),
	m_isMove(false),
	m_isPolyHit(false),
	m_pPoly(nullptr)
{
	/*処理無し*/
}

Camera::~Camera()
{
	/*処理無し*/
}

void Camera::Init()
{
	/*処理無し*/
}

void Camera::Update(VECTOR targetPos, bool isLockOn)
{
	// 移動前の座標を保存する
	m_prevPos = m_nextPos;

	// 注視点座標を設定する
	VECTOR viewpointPos;
	viewpointPos = targetPos;
	// ターゲット座標に注視点の高さを足す
	viewpointPos.y += kCameraViewpointHeight;

	// ロックオン状態状態かどうかの確認
	m_isLockOn = isLockOn;

	// 角度更新
	UpdateAngle(m_isLockOn);

	if (m_isLockOn)	// ロックオン時
	{
		// カメラのロックオン時の更新
		LockOnUpdate(viewpointPos);
		m_pLockOnTarget->Update(m_lockOnEnemyPos);
	}
	else // 通常時
	{
		// 通常時更新
		NormalUpdate(viewpointPos);
	}
	// 座標の確定
	UpdatePos();
}

void Camera::Draw()
{
	if (m_isLockOn) // ロックオン時
	{
		// ロックオン画像の描画
		m_pLockOnTarget->Draw();
	}

#ifdef _DEBUG
	// デバッグ表示
	DrawFormatString(0, 120, 0xffffff, "カメラ座標：%f,%f,%f", m_prevPos.x, m_prevPos.y, m_prevPos.z);
	DrawFormatString(0, 60, 0xffffff, "ターゲット座標：%f,%f,%f", m_targetPos.x, m_targetPos.y, m_targetPos.z);
	DrawFormatString(0, 80, 0xffffff, "GetTargetPos:%f,%f,%f", GetCameraTarget().x, GetCameraTarget().y, GetCameraTarget().z);
	DrawFormatString(0, 220, 0x000000, "ロックオン状態敵座標：%f,%f,%f", m_lockOnEnemyPos.x, m_lockOnEnemyPos.y, m_lockOnEnemyPos.z);
#endif // _DEBUG
}

void Camera::ResetCamera()
{
	// カメラの表示設定
	SetCameraNearFar(kNear, kFar);
	// 座標設定
	SetCameraPositionAndTarget_UpVecY(m_pos, m_targetPos);
}

void Camera::ColUpdate(ObjectBase* pField)
{
	// カメラの周囲にあるステージポリゴンを取得する
	// 検出する範囲は移動距離も考慮する
	m_hitDim = MV1CollCheck_Capsule(pField->GetModel()->GetModelHandle(),
		-1, m_nextPos, m_prevPos,
		m_pCollShape->GetRadius());

	if (m_hitDim.HitNum == 0)
	{
		// 検出したプレイヤーの周囲のポリゴン情報を開放する
		MV1CollResultPolyDimTerminate(m_hitDim);

		// カメラからターゲットまでの最小距離を出す
		m_cameraToTargetLenght = min(m_cameraToTargetLenght + kCameraToTargetLenghtSpeed, kCameraToTargetLenghtMax);

		// カメラの座標を決定する
		UpdatePos();

		m_hitDim = MV1CollCheck_Capsule(pField->GetModel()->GetModelHandle(),
			-1, m_nextPos, m_prevPos,
			m_pCollShape->GetRadius());
	}

	for (int i = 0; i < m_hitDim.HitNum; i++)
	{
		m_pPolyIndex[i] = &m_hitDim.Dim[i];
	}

	// カメラの押し出し処理
	FixPosInternal();

	// 検出したプレイヤーの周囲のポリゴン情報を開放する
	MV1CollResultPolyDimTerminate(m_hitDim);

	// カメラの座標を決定する
	UpdatePos();

	// カメラの情報を反映させる
	m_pos = VAdd(VScale(m_prevPos, kPrevCameraFollowSpeed), VScale(m_nextPos, kCameraFollowSpeed));
	SetCameraPositionAndTarget_UpVecY(m_pos, m_targetPos);
}

void Camera::UpdateAngle(bool isLockOn)
{
	// ロックオン状態の時は何もしない
	if (isLockOn) return;

	// パッドのアナログ情報の取得
	DINPUT_JOYSTATE input;

	// 入力情報の初期化
	input.Rx = 0;
	input.Ry = 0;

	// 入力情報の取得
	GetJoypadDirectInputState(DX_INPUT_PAD1, &input);

	// 回転を1～1000から0.001～1の範囲にする
	// 後でカメラ感度も用意する(一旦3にしておく)
	float rotX = 0.00020f * input.Rx * 3;
	float rotY = 0.00017f * input.Ry * 3;

	// カメラの回転スピードをかける
	rotX *= kCameraAngleSpeedX;
	rotY *= kCameraAngleSpeedY;

	m_angleH += rotX;
	if (input.Rx < 0.0f)
	{
		// -180°以下になったら角度地が大きくなりすぎないように360°を足す
		if (m_angleH < -DX_PI_F)
		{
			m_angleH += DX_TWO_PI_F;
		}
	}
	if (input.Rx > 0.0f)
	{
		// 180°以上になったら角度地が大きくなりすぎないように360°を引く
		if (m_angleH > DX_PI_F)
		{
			m_angleH -= DX_TWO_PI_F;
		}
	}

	m_angleV -= rotY;
	// 一定角度以下ににはならないようにする
	if (m_angleV < kCameraAngleVMin)
	{
		m_angleV = kCameraAngleVMin;
	}
	// 一定角度以上にはならないようにする
	if (m_angleV > kCameraAngleVMax)
	{
		m_angleV = kCameraAngleVMax;
	}
}

void Camera::NormalUpdate(VECTOR targetPos)
{
	// ターゲット座標更新
	m_targetPos.x = (m_targetPos.x * kPrevCameraTargetFollowSpeed) + (targetPos.x * kCameraTargetFollowSpeed);
	m_targetPos.y = (m_targetPos.y * kPrevCameraTargetFollowSpeed) + (targetPos.y * kCameraTargetFollowSpeed);
	m_targetPos.z = (m_targetPos.z * kPrevCameraTargetFollowSpeed) + (targetPos.z * kCameraTargetFollowSpeed);
}

void Camera::LockOnUpdate(VECTOR targetPos)
{
	// ターゲット座標更新
	m_targetPos.x = (m_targetPos.x * kPrevCameraTargetFollowSpeed) + (targetPos.x * kCameraTargetFollowSpeed);
	m_targetPos.y = (m_targetPos.y * kPrevCameraTargetFollowSpeed) + (targetPos.y * kCameraTargetFollowSpeed);
	m_targetPos.z = (m_targetPos.z * kPrevCameraTargetFollowSpeed) + (targetPos.z * kCameraTargetFollowSpeed);
	// プレイヤーからカメラまでの方向ベクトルを出す
	VECTOR playerToCameraVec = VNorm(VSub(m_targetPos, m_nextPos));
	// プレイヤーから敵までの方向ベクトルを出す
	VECTOR playerToEnemyVec = VNorm(VSub(m_lockOnEnemyPos, targetPos));

	// ロックオン時のカメラの方向ベクトルを出す
	VECTOR lockOnCameraVec = VAdd(VScale(playerToCameraVec, kPrevCameraTargetLockOnFollowSpeed),
		VScale(playerToEnemyVec, kCameraTargetLockOnFollowSpeed));
	// カメラを少し傾ける
	m_angleH = static_cast<float>(atan2(-lockOnCameraVec.z, lockOnCameraVec.x) + kLockOnAngleH);

	// プレイヤーから敵までの角度（垂直方向）を求める
	float targetAngleV = static_cast<float>(atan2(playerToEnemyVec.y, 
		sqrt(playerToEnemyVec.x * playerToEnemyVec.x + playerToEnemyVec.z * playerToEnemyVec.z)));
	// 上下方向の角度制限
	const float minAngleV = -DX_PI_F * 0.2f;
	const float maxAngleV = DX_PI_F * 0.1f;
	// m_angleVの補間
	m_angleV = (m_angleV * kPrevCameraFollowSpeed) + (targetAngleV * kCameraFollowSpeed);
	// 角度制限の適用
	m_angleV = max(min(m_angleV, maxAngleV), minAngleV);

	// カメラ座標を出す
	VECTOR cameraPos = VAdd(VScale(m_prevPos, kPrevCameraTargetLockOnFollowSpeed),
		VScale(m_nextPos, kCameraTargetLockOnFollowSpeed));

	// カメラ座標の設定
	SetCameraPositionAndTarget_UpVecY(cameraPos, m_targetPos);
}

void Camera::UpdatePos()
{
	// 垂直方向の回転(X軸)
	MATRIX RotX = MGetRotX(m_angleV);
	// 水平方向の回転(Y軸)
	MATRIX RotY = MGetRotY(m_angleH);

	/*カメラの座標の計算
	(X軸にカメラとプレイヤーとの距離分だけ伸びたベクトルを
	垂直方向回転させた後に水平方向回転して
	それに注視点の座標を足す)*/
	m_nextPos = VAdd(VTransform(VTransform(VGet(0.0f, 0.0f, m_cameraToTargetLenght), RotX), RotY), m_targetPos);
}

void Camera::FixPos()
{
	// 壁に当たったかどうかのフラグをfalseにしておく
	m_isPolyHit = false;

	// 移動したかどうか
	if (m_isMove)
	{
		// 壁ポリゴンの数だけ繰り返す
		for (int i = 0; i < m_hitDim.HitNum; i++)
		{
			// i番目の壁ポリゴンのアドレスを壁ポチゴンポインタ配列から取得
			m_pPoly = m_pPolyIndex[i];

			// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
			if (!HitCheck_Capsule_Triangle(m_nextPos, m_prevPos, m_pCollShape->GetRadius(),
				m_pPoly->Position[0], m_pPoly->Position[1],
				m_pPoly->Position[2])) continue;

			// ここに来たら当たっている
			m_isPolyHit = true;

			// カメラが当たっているとき
			while (m_isPolyHit)
			{
				m_cameraToTargetLenght = max(m_cameraToTargetLenght--, kCameraToTargetLenghtMin);
				// カメラの座標更新
				UpdatePos();

				// ポリゴンとプレイヤーが当たっていなかったらループから抜ける
				if (!HitCheck_Capsule_Triangle(m_nextPos, m_prevPos, m_pCollShape->GetRadius(),
					m_pPoly->Position[0], m_pPoly->Position[1],
					m_pPoly->Position[2]))
				{
					m_isPolyHit = false;
				}
			}

			// 新たな移動座標で壁ポリゴンと当たっていないかどうかを判定する
			bool isHitWallPolygom = false;
			for (int j = 0; j < m_hitDim.HitNum; j++)
			{
				// j番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
				m_pPoly = m_pPolyIndex[j];

				// 当たっていたらループから抜ける
				if (!HitCheck_Capsule_Triangle(m_nextPos, m_prevPos, m_pCollShape->GetRadius(),
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
				{
					isHitWallPolygom = true;
					break;
				}
			}

			// すべてのポリゴンと当たっていなかったらループから抜ける
			if (!isHitWallPolygom)
			{
				m_isPolyHit = false;
				break;
			}
		}
	}
	else // 移動していなかった場合
	{
		// 壁ポリゴンの数だけ繰り返す
		for (int i = 0; i < m_hitDim.HitNum; i++)
		{
			// i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
			m_pPoly = m_pPolyIndex[i];

			// ポリゴンに当たっていたあr当たったフラグを立てたうえでループから抜ける
			if (HitCheck_Capsule_Triangle(m_nextPos, m_prevPos, m_pCollShape->GetRadius(),
				m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
			{
				m_isPolyHit = true;
				break;
			}
		}
	}
}

void Camera::FixPosInternal()
{
	// 壁からの押し出し処理を試みる最大数だけ繰り返す
	for (int i = 0; i < kMaxColHitTryNum; i++)
	{
		// 当たる可能性のある壁ポリゴンをすべて見る
		bool isHitWall = false;

		// 壁ポリゴンの数だけ繰り返す
		for (int j = 0; j < m_hitDim.HitNum; j++)
		{
			// j番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
			m_pPoly = m_pPolyIndex[j];

			m_cameraToTargetLenght = max(m_cameraToTargetLenght--, kCameraToTargetLenghtMin);

			// 座標更新
			UpdatePos();

			// 移動したうえで壁ポリゴンと接触しているかどうかを判定
			for (int k = 0; k < m_hitDim.HitNum; k++)
			{
				// 当たったらループから抜ける
				m_pPoly = m_pPolyIndex[k];
				if (HitCheck_Capsule_Triangle(m_nextPos, m_prevPos, m_pCollShape->GetRadius(),
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
				{
					isHitWall = true;
					break;
				}
			}

			// すべてのポリゴンと当たっていなかったらループから抜ける
			if (!isHitWall) break;
		}
		// ループ終了
		if (!isHitWall)break;
	}
}
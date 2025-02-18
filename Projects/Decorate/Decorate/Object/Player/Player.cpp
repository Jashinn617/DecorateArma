#include "Player.h"
#include "PlayerState.h"
#include "Shot.h"

#include "../ObjectManager.h"
#include "../Model.h"
#include "../Camera.h"
#include "../Enemy/EnemyBase.h"

#include "../../Effect/Effekseer3DManager.h"

#include "../../Shader/ToonShader.h"

#include "../../Utility/CollisionShape.h"
#include "../../Utility/MoveDirectionVec.h"
#include "../../Utility/Time.h"
#include "../../Utility/Pad.h"
#include "../../Utility/SoundManager.h"

#include "../../UI/HpBar/HpBarBase.h"
#include "../../UI/HpBar/HpBarPlayer.h"

#include "../../Common/CsvLoad.h"

#include <cassert>


namespace
{
	const char* const kFileName = "Data/Model/Player/Player.mv1";	// プレイヤーモデルファイルパス
	const char* const kName = "Player";								// キャラクター名

	// 時間
	constexpr int kAttackStanTime = 20;						// 攻撃間隔時間
	constexpr int kInvincibleTime = 150;					// 無敵時間
	constexpr int kDodgeTime = 15;							// 回避時間

	// 攻撃力の段階ごとに攻撃エフェクトを分ける用の数値(三段階に分ける)
	constexpr int kFirstStepPower = 0;						// 一段階目
	constexpr int kSecondStagePower = 25;					// 二段階目
	constexpr int kThirdStagePower = 35;					// 三段階目

	constexpr float kHardAttackRate = 1.5f;					// 強攻撃時の攻撃力倍率

	// 速度関係
	constexpr float kMoveSpeedDashRate = 1.7f;				// ダッシュ時速度
	constexpr float kAccelerationRate = 0.5f;				// 加速度
	constexpr float kDodgeSpeedRate = 3.0f;					// 回避時速度
	constexpr float kGravity = 0.8f;						// 重力
	constexpr float kFallMaxSpeed = -15.0f;					// 最大落下速度
	constexpr float kNowVecNum = 0.8f;						// 現在の方向
	constexpr float kNextVecNum = 0.2f;						// 進みたい方向
	constexpr float kAngleSpeed = 0.02f;					// 回転速度

	// 衝突判定関係
	constexpr float kAttackDistance = 120.0f;				// 攻撃距離
	constexpr float kHeight = 140.0f;						// 高さ
	constexpr float kSize = 50.0f;							// モデルサイズ
	constexpr float kCapsuleRadius = 30.0f;					// 衝突判定用カプセル半径
	constexpr float kAttackRadius = 70.0f;					// 攻撃判定用球の半径
	constexpr float kAttackHeight = 0.0f;					// 攻撃判定用球の高さ
	constexpr float kHardRadius = 60.0f;					// 強攻撃当たり判定球の半径
	constexpr float kHardHeight = 0.0f;						// 強攻撃当たり判定球の高さ

	constexpr VECTOR kScaleVec = { 1.2f,1.2f,1.2f };			// モデルスケール
	constexpr VECTOR kEffectPosControl = { 0.0f,70.0f,0.0f };	// エフェクト座標調節用

	/// <summary>
	/// アニメーション切り替え速度
	/// </summary>
	enum kAnimChangeFrameNum
	{
		Idle = 2,
		Walk = 1,
		Dash = 1,
		Attack1 = 1,
		Attack2 = 1,
		Attack3 = 1,
		HardAttack = 1,
		JumpStart = 1,
		JumpIdle = 1,
		Avoidance = 1,
		Damage = 1,
	};
}

Player::Player() :
	m_attackCount(0),
	m_isColl(false),
	m_isAttack(false),
	m_isHardAttack(false),
	m_isNextAttack(false),
	m_isLockOn(false),
	m_isDodge(false),
	m_moveDirection{ 0.0f,0.0f,0.0f },
	m_attackPos{ 0.0f,0.0f,0.0f },
	m_dodgeDirection{ 0.0f,0.0f,0.0f },
	m_prevDodgeDirection{ 0.0f,0.0f,0.0f },
	m_pState(std::make_shared<PlayerState>(this)),
	m_pCamera(std::make_shared<Camera>()),
	m_attackStanTime(std::make_shared<Time>(kAttackStanTime)),
	m_invincibleTime(std::make_shared<Time>(kInvincibleTime)),
	m_dodgeTime(std::make_shared<Time>(kDodgeTime))
{
	// アニメーションロード
	CsvLoad::GetInstance().AnimLoad(m_animData, kName);

	// ショット作成
	m_pShot = std::make_shared<Shot>(this, m_statusData.shotAtk);
	// ステータス情報初期化
	SetStatus();

	/*移動速度初期化*/
	m_moveData.walkSpeed = m_statusData.spd;
	m_moveData.dashSpeed = m_statusData.spd * kMoveSpeedDashRate;
	m_moveData.acc = m_statusData.spd * kAccelerationRate;
	m_moveData.rotSpeed = kAngleSpeed;

	// モデルポインタ作成
	m_pModel = std::make_shared<Model>(kFileName);
	// モデルスケール設定
	m_pModel->SetScale(kScaleVec);
	// アニメーション初期化
	m_pModel->SetAnim(m_animData.idle, true, false);

	// モデル頂点の取得
	m_topFrameIndex = MV1SearchFrame(m_pModel->GetModelHandle(), "mixamorig:HeadTop_End");
	assert(m_topFrameIndex != -1);
	assert(m_topFrameIndex != -2);
	// モデル底辺の取得
	m_bottomFrameIndex = MV1SearchFrame(m_pModel->GetModelHandle(), "mixamorig:RightToeBase");
	assert(m_bottomFrameIndex != -1);
	assert(m_bottomFrameIndex != -2);
	// 攻撃時エフェクト座標の取得
	m_attackEffectIndex = MV1SearchFrame(m_pModel->GetModelHandle(), "mixamorig:RightHandMiddle1");
	assert(m_attackEffectIndex != -1);
	assert(m_attackEffectIndex != -2);
	// 座標設定
	m_attackEffectPos = MV1GetFramePosition(m_pModel->GetModelHandle(), m_attackEffectIndex);

	// 攻撃座標設定
	m_hardAttackIndex = MV1SearchFrame(m_pModel->GetModelHandle(), "mixamorig:RightFoot");
	assert(m_hardAttackIndex != -1);
	assert(m_hardAttackIndex != -2);
	m_hardAttackPos = MV1GetFramePosition(m_pModel->GetModelHandle(), m_hardAttackIndex);

	// 初期ステイトの設定(待機状態から)
	m_pState->SetState(PlayerState::StateKind::Idle);

	/*情報初期化*/
	m_characterInfo.pos = VGet(0.0f, 0.0f, 0.0f);
	m_characterInfo.topPos = MV1GetFramePosition(m_pModel->GetModelHandle(), m_topFrameIndex);
	m_characterInfo.bottomPos = MV1GetFramePosition(m_pModel->GetModelHandle(), m_bottomFrameIndex);
	m_characterInfo.vec = VGet(0.0f, 0.0f, 0.0f);
	m_characterInfo.rot = VGet(0.0f, 0.0f, 0.0f);
	m_characterInfo.modelH = -1;
	m_characterInfo.isExist = true;
	m_objSize = kSize;
	m_angle = 0.0f;

	/*当たり判定ポインタ作成*/
	// プレイヤー
	m_pCollShape = std::make_shared<CollisionShape>(m_characterInfo.topPos, m_characterInfo.bottomPos, kCapsuleRadius);
	// 通常攻撃
	m_attackColl = std::make_shared<CollisionShape>(m_attackEffectPos, kAttackRadius, kAttackHeight);
	// 強攻撃
	m_hardAtkColl = std::make_shared<CollisionShape>(m_hardAttackPos, kHardRadius, kHardHeight);

	// 最大HP設定
	m_maxHp = m_statusData.hp;

	// HPバー作成
	m_pHpBar = std::make_shared<HpBarPlayer>(m_statusData.hp);

	// モデルの頂点タイプの取得
	for (int i = 0; i < MV1GetTriangleListNum(m_pModel->GetModelHandle()); i++)
	{
		// 頂点タイプの取得
		m_vertexShaderType.push_back(MV1GetTriangleListVertexType(m_pModel->GetModelHandle(), i));
	}
}

Player::~Player()
{
	/*処理無し*/
}

void Player::Update()
{
	// ステイト更新
	m_pState->Update();
	UpdateState();

	// 移動更新
	m_characterInfo.vec = Move();
	// 重力による落下処理
	UpdateGravity();

	// アニメーション更新
	m_pModel->Update();
	// モデル座標の設定
	m_pModel->SetPos(m_characterInfo.pos);
	// モデル回転の設定
	m_pModel->SetRot(VGet(0.0f, m_angle, 0.0f));

	// HPバー更新
	m_pHpBar->Update();

	// カメラ更新
	UpdateCamera();

	// カメラの角度行列の取得
	MATRIX rotMtx = MGetRotY(m_pCamera->GetCameraAngleX());
	// ショットにカメラの角度行列を渡す
	m_pShot->SetCameraRot(rotMtx);
	// 遠距離攻撃武器更新
	m_pShot->Update(m_characterInfo.pos, m_angle);

	// フレーム座標の更新
	m_characterInfo.topPos = MV1GetFramePosition(m_pModel->GetModelHandle(), m_topFrameIndex);
	m_characterInfo.bottomPos = MV1GetFramePosition(m_pModel->GetModelHandle(), m_bottomFrameIndex);
	UpdateAttackPosition(m_angle);
	m_hardAttackPos = MV1GetFramePosition(m_pModel->GetModelHandle(), m_hardAttackIndex);

	// ダメージ中の場合は攻撃を受けない
	if (m_isDamage)
	{
		// 一定時間経過した場合
		if (m_invincibleTime->Update())
		{
			// タイマーリセット
			m_invincibleTime->Reset();
			// ダメージ中フラグをfalseにする
			m_isDamage = false;
		}
	}

#ifdef _DEBUG
	// バックスペースキーが押された場合
	if (CheckHitKey(KEY_INPUT_BACK))
	{
		// HPを減らす
		OnDamage(VGet(0.0f, 0.0f, 0.0f), 10);
	}
#endif // _DEBUG
}

void Player::Draw(std::shared_ptr<ToonShader> pToonShader)
{
	if (m_isAttack)
	{
		// デバッグ表示
		m_attackColl->DebugDraw(0x0000ff);

	}

	// モデルをフレームごとに描画する
	for (int i = 0; i < MV1GetTriangleListNum(m_pModel->GetModelHandle()); i++)
	{
		// シェーダの設定
		pToonShader->SetShader(m_vertexShaderType[i]);

		// 描画
		MV1DrawTriangleList(m_pModel->GetModelHandle(), i);
	}
	// シェーダを使わない設定にする
	pToonShader->ShaderEnd();

	// 遠距離攻撃武器描画
	m_pShot->Draw();
}

void Player::Draw2D()
{
	// HPバー描画
	m_pHpBar->Draw();

	// カメラ2D部分の描画
	m_pCamera->Draw();

#ifdef _DEBUG
	// デバッグ用描画
	DrawFormatString(0, 20, 0xffffff, "プレイヤー座標：%f,%f,%f"
		, m_characterInfo.pos.x, m_characterInfo.pos.y, m_characterInfo.pos.z);
	DrawFormatString(0, 180, 0xffffff, "攻撃カウント：%d", m_attackCount);

	DrawFormatString(1200, 20, 0xffffff, "HP:%d", m_statusData.hp);
	DrawFormatString(1200, 40, 0xffffff, "MATK:%d", m_statusData.meleeAtk);
	DrawFormatString(1200, 80, 0xffffff, "SATK:%d", m_statusData.shotAtk);
	DrawFormatString(1200, 100, 0xffffff, "DEF:%d", m_statusData.def);
#endif // _DEBUG
}

void Player::OnDamage(VECTOR targetPos, int damagePoint, bool isInvincible)
{
	// ダメージ中は処理をしない
	if (m_isDamage) return;
	// 回避中は処理をしない
	if (m_isDodge) return;

	VECTOR EffectPos = VAdd(m_characterInfo.pos, kEffectPosControl);

	// エフェクトを流す
	Effekseer3DManager::GetInstance().Add("Damage",
		Effekseer3DManager::PlayType::Normal, this, EffectPos);
	Effekseer3DManager::GetInstance().Add("Damage2",
		Effekseer3DManager::PlayType::Normal, this, EffectPos);

	// ダメージを受けた状態にする
	m_isDamage = true;

	// 攻撃状態のリセット
	m_isAttack = false;
	m_isHardAttack = false;
	m_isColl = false;
	m_attackStanTime->Reset();

	// HPを減らす
	m_statusData.hp -= damagePoint;

	// HPが0以下になって死亡状態でなかった場合
	if (m_statusData.hp <= 0 && !m_isDead)
	{
		// 死亡する
		m_isDead = true;
	}
	// ダメージ状態にする
	m_pState->OnDamage();
}

void Player::UpdateAngle()
{
	// ダメージ中は処理をしない
	if (m_pState->GetState() == PlayerState::StateKind::Damage) return;
	// 回避中は処理をしない
	if (m_isDodge) return;

	float nextAngle = 0.0f;

	// ロックオン状態で、ダッシュ状態ではなかった場合は
	// ロックオンされている敵の方向を向くようにする
	if (m_isLockOn && m_pState->GetState() != PlayerState::StateKind::Dash)
	{
		// ロックオンされている敵がいなかったら返る
		if (m_pObjectManager->GetLockOnEnemy() == nullptr) return;
		// プレイヤーから敵までのベクトルを出す
		VECTOR enemyToPlayer = VSub(m_pObjectManager->GetLockOnEnemy()->GetInfo().pos, m_characterInfo.pos);

		// 目標角度の計算
		nextAngle = static_cast<float>(atan2(-enemyToPlayer.z, enemyToPlayer.x) - DX_PI_F * 0.5f);
	}
	else
	{
		// 目標角度の計算(ベクトル(z,x)の角度 + 90°+ カメラ角度)
		nextAngle = static_cast<float>(atan2(static_cast<double>(m_moveDirection.z), static_cast<double>(m_moveDirection.x))
			+ DX_PI_F * 0.5f + m_pCamera->GetCameraAngleX());
	}

	// 角度を滑らかに変更する
	SmoothAngle(m_angle, nextAngle);
}

void Player::UpdateMoveDirection()
{
	// ダメージ中は処理をしない
	if (m_pState->GetState() == PlayerState::StateKind::Damage) return;

	// 移動方向ベクトルクラスの作成
	MoveDirectionVec moveDirectionVec;
	// 移動方向更新
	moveDirectionVec.Update();

	// 進みたい方向と現在の方向の線形補完
	m_moveDirection = VAdd(VScale(m_moveDirection, kNowVecNum),
		VScale(moveDirectionVec.GetDirectionVec(), kNextVecNum));

	// 進みたい方向のY軸を省く
	m_moveDirection.y = 0.0f;
}

void Player::UpdateCamera()
{
	// 左スティックが押し込まれた場合
	if (Pad::IsTrigger(PAD_INPUT_10))
	{
		// SEを鳴らす
		if (m_isLockOn)// OFF
		{
			SoundManager::GetInstance().Play("LockOff", true);
		}
		else// ON
		{
			SoundManager::GetInstance().Play("LockOn", true);
		}

		// ロックオン状態が切り替わる
		m_isLockOn = !m_isLockOn;
		

		// ロックオン状態だった場合
		if (m_isLockOn)
		{
			m_pObjectManager->InitLockOnEnemy();
		}
	}

	// ロックオンされていた敵が消えたらロックオンを解除する
	if (m_pObjectManager->GetLockOnEnemy() != nullptr)
	{
		// 存在フラグが立っていなかった場合
		if (!m_pObjectManager->GetLockOnEnemy()->GetInfo().isExist)
		{
			// ロックオン状態を解除する
			m_isLockOn = false;
		}
	}
	else
	{
		// ロックオン状態を解除する
		m_isLockOn = false;
	}

	// ロックオン状態の場合
	if (m_isLockOn)
	{
		// カメラに敵の座標を渡す
		m_pCamera->SetLockOnEnemyPos(m_pObjectManager->GetLockOnEnemy()->GetInfo().pos);
	}

	// カメラ更新
	m_pCamera->Update(m_characterInfo.pos, m_isLockOn);
}

VECTOR Player::Move()
{
	// 移動速度が0の場合は何もしない
	if (m_moveSpeed == 0.0f)return VGet(0.0f, 0.0f, 0.0f);

	// 移動方向更新
	UpdateMoveDirection();
	// 角度更新
	UpdateAngle();

	// 移動ベクトルの生成
	VECTOR move = VNorm(m_moveDirection);

	// ダメージ中の処理
	if (m_pState->GetState() == PlayerState::StateKind::Damage)
	{
		return move;
	}

	// 移動ベクトルに速度をかける
	// X軸は反転させる(カメラの角度を変えた時に移動方向がおかしくならないように)
	move.x *= -m_moveSpeed;
	move.z *= m_moveSpeed;

	/*カメラの角度によって進む方向を変える*/
	// カメラの角度行列を取得する
	MATRIX rotMtx = MGetRotY(m_pCamera->GetCameraAngleX());

	// 移動ベクトルとカメラ角度行列をかける
	move = VTransform(move, rotMtx);

	return move;
}

void Player::InitDodge()
{
	// 攻撃音を止める
	SoundManager::GetInstance().DesignationStopSound("Attack1");
	SoundManager::GetInstance().DesignationStopSound("Attack2");
	SoundManager::GetInstance().DesignationStopSound("Attack3");
	SoundManager::GetInstance().DesignationStopSound("HardAttack");

	// 回避前の方向を保存しておく
	m_prevDodgeDirection = m_moveDirection;

	// 回避方向を決める
	// スティックが入力されていたらその方向に回避する
	if (Pad::IsPress(PAD_INPUT_LEFT) || Pad::IsPress(PAD_INPUT_RIGHT) ||
	Pad::IsPress(PAD_INPUT_UP) || Pad::IsPress(PAD_INPUT_DOWN))
	{
		m_dodgeDirection = m_moveDirection;
	}
	// 入力されていなかったら後ろに回避する
	else
	{
		// 後ろに回避する
		m_dodgeDirection = VScale(m_moveDirection, -1.0f);
	}

	// 回避中フラグを立てる
	m_isDodge = true;
	// 攻撃状態の解除
	m_isAttack = false;
	m_isHardAttack = false;
	m_isNextAttack = false;
	m_attackStanTime->Reset();
}

void Player::InitState()
{
	/*現在のステイトによって初期化処理を変える*/
	switch (m_pState->GetState())
	{
	case PlayerState::StateKind::Attack:

		m_attackStanTime->Reset();		// 硬直時間のリセット
		m_attackCount = 0;				// 攻撃回数のリセット

		//	攻撃フラグを立てる
		m_isAttack = true;
		// 次の攻撃をするかどうかのフラグをfalseにする
		m_isNextAttack = false;
		// 攻撃判定リセットフラグを立てる
		m_isResetAttack = true;

		break;

	case PlayerState::StateKind::HardAttack:

		// 強攻撃フラグを立てる
		m_isHardAttack = true;

		break;

	case PlayerState::StateKind::Dodge:	// 回避
		InitDodge();

		break;
	default:
		// 上記以外だった場合は何もしない
		break;
	}
}

void Player::SetStatus()
{
	// ステータスの設定
	CsvLoad::GetInstance().StatusLoad(m_statusData, kName);
	// ステータス追加
	CsvLoad::GetInstance().AddStatusLoad(m_statusData, kName);
	// ショットの攻撃力の再設定
	m_pShot->SetAttackPoint(m_statusData.shotAtk);
}

void Player::OnAttack(CharacterBase* pEnemy)
{
	// 攻撃中以外または衝突中は処理をしない
	if (!m_isAttack || m_isColl) return;

	// 衝突判定
	if (m_attackColl->IsCollide(pEnemy->GetCollShape()))
	{
		// 当たっていたらダメージを与える
		pEnemy->OnDamage(m_characterInfo.pos, m_statusData.meleeAtk, true);
		m_isColl = true;
		m_isAttack = true;
	}
}

void Player::OnHardAttack(CharacterBase* pEnemy)
{
	// 強攻撃中以外は処理をしない
	if (!m_isHardAttack) return;

	// デバッグ表示
	m_hardAtkColl->DebugDraw(0xff00ff);

	// 衝突判定
	if (m_hardAtkColl->IsCollide(pEnemy->GetCollShape()))
	{
		// 当たっていたらダメージを与える
		pEnemy->OnDamage(m_characterInfo.pos,
			static_cast<int>(m_statusData.meleeAtk * kHardAttackRate));
	}
}

void Player::OnRecovery(int recoveryNum)
{
	// HP回復
	m_statusData.hp += recoveryNum;
	// 最大HP以上にはならないようにする
	if (m_statusData.hp >= m_maxHp)
	{
		m_statusData.hp = m_maxHp;
	}
	// HPバー回復
	m_pHpBar->OnRecoveryHp(recoveryNum);
}

void Player::UpdateState()
{
	/*現在のステイトによって更新処理を変える*/
	switch (m_pState->GetState())
	{
	case PlayerState::StateKind::Idle:	// 待機
		// 段々減速する
		m_moveSpeed = max(m_moveSpeed - m_moveData.acc, 0.0f);
		// アニメーションを待機状態に変更する
		m_pModel->ChangeAnim(m_animData.idle, true, false, kAnimChangeFrameNum::Idle);
		break;

	case PlayerState::StateKind::Walk:	// 歩き
		// 移動速度を歩き状態の速度に変更する
		m_moveSpeed = min(m_moveSpeed + m_moveData.acc, m_moveData.walkSpeed);
		// アニメーションを歩きアニメーションに変更する
		m_pModel->ChangeAnim(m_animData.walk, true, false, kAnimChangeFrameNum::Walk);
		// SEを鳴らす
		SoundManager::GetInstance().Play("Walk", false);
		break;

	case PlayerState::StateKind::Dash:	// ダッシュ
		// 移動速度をダッシュ時の速度にする
		m_moveSpeed = min(m_moveSpeed + m_moveData.acc, m_moveData.dashSpeed);
		// アニメーションをダッシュアニメーションに変更する
		m_pModel->ChangeAnim(m_animData.run, true, false, kAnimChangeFrameNum::Dash);
		// SEを鳴らす
		SoundManager::GetInstance().Play("Dash", false);
		break;

	case PlayerState::StateKind::Attack:	// 攻撃
		// 攻撃更新
		UpdateAttack();
		break;

	case PlayerState::StateKind::HardAttack:	// 強攻撃
		// 強攻撃更新
		UpdateHardAttack();
		break;

	case PlayerState::StateKind::Dodge:	// 回避
		// 攻撃状態の解除
		m_isAttack = false;
		m_isHardAttack = false;
		// 回避更新
		UpdateDodge();
		break;

	case PlayerState::StateKind::Damage:	// ダメージ
		// 攻撃状態の解除
		m_isAttack = false;
		m_isHardAttack = false;
		// アニメーションの変更
		m_pModel->ChangeAnim(m_animData.damage, false, false, kAnimChangeFrameNum::Damage);
		// アニメーションが終わったら移動状態に変更する
		if (m_pModel->IsAnimEnd())
		{
			m_pModel->ChangeAnim(m_animData.idle, true, false, kAnimChangeFrameNum::Idle);
			if (m_statusData.hp <= 0)return;
			m_pState->EndState();
		}
		break;

	default:
		// 上記以外だった場合は何もしない
		break;
	}
}

void Player::UpdateAttack()
{
	// 止まる
	m_moveSpeed = 0.0f;

	// エフェクトを流す
	// 現在の攻撃力によってエフェクトを変化させる
	if (kFirstStepPower <= m_statusData.meleeAtk && m_statusData.meleeAtk < kSecondStagePower) // 一段階目
	{
		Effekseer3DManager::GetInstance().Add("Attack1",
		Effekseer3DManager::PlayType::Normal, this, m_attackEffectPos);
	}
	else if (kSecondStagePower <= m_statusData.meleeAtk && m_statusData.meleeAtk < kThirdStagePower) // 二段階目
	{
		Effekseer3DManager::GetInstance().Add("Attack2",
		Effekseer3DManager::PlayType::Normal, this, m_attackEffectPos);
	}
	else if (kThirdStagePower <= m_statusData.meleeAtk)	// 三段階目
	{
		Effekseer3DManager::GetInstance().Add("Attack3",
		Effekseer3DManager::PlayType::Normal, this, m_attackEffectPos);
	}

	// 攻撃回数によってアニメーションを変更する
	switch (m_attackCount % 3)
	{
	case 0:
		m_pModel->ChangeAnim(m_animData.attack1, false, false, kAnimChangeFrameNum::Attack1);
		break;

	case 1:
		m_pModel->ChangeAnim(m_animData.attack2, false, false, kAnimChangeFrameNum::Attack2);
		break;

	case 2:
		m_pModel->ChangeAnim(m_animData.attack3, false, false, kAnimChangeFrameNum::Attack3);
		break;

	default:
		break;
	}

	if (m_attackStanTime->Update())
	{
		// 攻撃中に攻撃ボタンが押された場合
		if (Pad::IsTrigger(PAD_INPUT_3) && !m_isNextAttack)
		{
			// 次に攻撃するフラグを立てる
			m_isNextAttack = true;
		}
	}

	// アニメーションが終わった段階で次に攻撃するフラグが立っていなかった場合
	if (m_pModel->IsAnimEnd() && !m_isNextAttack)
	{
		// 衝突フラグをfalseにする
		m_isColl = false;
		// 攻撃を終了する
		m_isAttack = false;
		// 硬直時間をリセットする
		m_attackStanTime->Reset();
		// 攻撃カウントをリセットする
		m_attackCount = 0;
		// ステイトを終了する
		m_pState->EndState();
	}
	// アニメーションが終わった段階で次に攻撃するフラグが立っていた場合
	if (m_pModel->IsAnimEnd() && m_isNextAttack)
	{
		// 衝突フラグをfalseにする
		m_isColl = false;
		// 攻撃判定の初期化
		m_isResetAttack = true;
		// 硬直時間のリセット
		m_attackStanTime->Reset();
		// 次の攻撃をするフラグをfalseにする
		m_isNextAttack = false;
		// 攻撃カウントを増やす
		m_attackCount++;
		// SEを鳴らす(攻撃カウントによってSEを変える)
		switch (m_attackCount % 3)
		{
		case 0:
			SoundManager::GetInstance().Play("Attack1", true);
			break;
		case 1:
			SoundManager::GetInstance().Play("Attack2", true);
			break;
		case 2:
			SoundManager::GetInstance().Play("Attack3", true);
			break;
		default:
			break;
		}
		
	}
}

void Player::UpdateHardAttack()
{
	// 止まる
	m_moveSpeed = 0.0f;

	// エフェクトを流す
	// 現在の攻撃力によってエフェクトを変化させる
	if (kFirstStepPower <= m_statusData.meleeAtk && m_statusData.meleeAtk < kSecondStagePower) // 一段階目
	{
		Effekseer3DManager::GetInstance().Add("HardAttack1",
		Effekseer3DManager::PlayType::Normal, this, m_hardAttackPos);
	}
	else if (kSecondStagePower <= m_statusData.meleeAtk && m_statusData.meleeAtk < kThirdStagePower) // 二段階目
	{
		Effekseer3DManager::GetInstance().Add("HardAttack2",
		Effekseer3DManager::PlayType::Normal, this, m_hardAttackPos);
	}
	else if (kThirdStagePower <= m_statusData.meleeAtk)	// 三段階目
	{
		Effekseer3DManager::GetInstance().Add("HardAttack3",
		Effekseer3DManager::PlayType::Normal, this, m_hardAttackPos);
	}

	// アニメーションを再生する
	m_pModel->ChangeAnim(m_animData.hardAttack, false, false, kAnimChangeFrameNum::HardAttack);

	// アニメーションが終わった場合
	if (m_pModel->IsAnimEnd())
	{
		// ステイト終了
		m_pState->EndState();
		// 強攻撃フラグをfalseにする
		m_isHardAttack = false;
	}
}

void Player::UpdateDodge()
{
	// 回避処理
	// スピードの調整
	m_moveSpeed = m_moveData.walkSpeed * kDodgeSpeedRate;
	// 方向の固定
	m_moveDirection = m_dodgeDirection;

	// アニメーションを再生する
	m_pModel->ChangeAnim(m_animData.hardAttack, false, false, kAnimChangeFrameNum::HardAttack);

	// 回避時間が一定になったら場合
	if (m_dodgeTime->Update())
	{
		// ステイト終了
		m_pState->EndState();
		// フラグをfalseにする
		m_isDodge = false;
		// 回避時間のリセット
		m_dodgeTime->Reset();
		// 方向の調整
		m_moveDirection = m_prevDodgeDirection;
		// 攻撃判定の初期化
		m_isColl = false;
		m_isResetAttack = true;
	}
}

void Player::UpdateAttackPosition(float angle)
{
	// 向いている方向を出す
	VECTOR vec = VGet(sinf(angle + DX_PI_F), 0.0f, cosf(angle + DX_PI_F));
	// 前に出す
	vec = VScale(vec, kAttackDistance);
	// 出したベクトルを現在位置に足す
	m_attackPos = VAdd(m_characterInfo.bottomPos, vec);

	m_attackEffectPos = MV1GetFramePosition(m_pModel->GetModelHandle(), m_attackEffectIndex);
}
#include "Shot.h"

#include "Player.h"

#include "../Camera.h"
#include "../CharacterBase.h"
#include "../Model.h"

#include "../../Utility/CollisionShape.h"
#include "../../Utility/Time.h"
#include "../../Utility/Pad.h"
#include "../../Utility/SoundManager.h"

#include "../../Shader/ToonShader.h"

#include <cmath>
#include <cassert>

namespace
{
	const char* const kFileName = "Data/Model/ShotWeapon/UFO.mv1";					// モデルファイルパス
	const char* const kBulletFileName = "Data/Model/ShotWeapon/Bullet/Shot.mv1";	// 弾ファイルパス
	
	constexpr float kPlayerDistance = 60.0f;					// プレイヤーとの距離
	constexpr float kPlayerToHeight = 150.0f;					// プレイヤーからの高さ
	constexpr float kPlayerLeftBack = DX_PI_F * 5.0f / 4.0f;	// プレイヤーの左後ろ
	constexpr float kSinSpeed = 0.03f;							// 昇降速度
	constexpr float kSinSwing = 4.0f;							// 昇降幅
	constexpr VECTOR kScaleVec = { 1.0f,1.0f,1.0f };			// スケール

	constexpr int kNum = 50;								// 弾数
	constexpr int kVanishTime = 50;							// 弾が消えるまでの時間
	constexpr int kIntervalTime = 5;						// 次の球が発射されるまでの時間
	constexpr float kEnemyHeight = 50.0f;					// ロックオンした敵に当てる弾の高さ調整
	constexpr float kRadius = 10;							// 弾の半径
	constexpr float kSpeed = 15.0f;							// 弾の速度
	constexpr float kStartPosHeight = 5.0f;					// 弾を発射する高さ
	constexpr VECTOR kScale = { 0.4f,0.4f,0.4f };			// 弾のスケール
}

Shot::Shot(Player* pPlayer, int atkPoint):
	m_atkPoint(atkPoint),
	m_sinCount(0.0f),
	m_sinPosY(0.0f),
	m_pos{0.0f,0.0f,0.0f},
	m_cameraRotMtx(MGetRotY(0)),
	m_pPlayer(pPlayer),
	m_pModel(std::make_shared<Model>(kFileName)),
	m_pBulletIntervalTime(std::make_shared<Time>(kIntervalTime)),
	m_pToonShader(std::make_shared<ToonShader>())
{
	// 弾のモデルハンドルの取得
	int bulletH = MV1LoadModel(kBulletFileName);
	assert(bulletH != -1);

	// 弾の生成
	m_bullet.resize(kNum);
	for (auto& bullet : m_bullet)
	{
		// 存在フラグ初期化
		bullet.isExist = false;
		// 座標初期化
		bullet.pos = VGet(0.0f, 0.0f, 0.0f);
		// 進む向きの初期化
		bullet.direction = VGet(0.0f, 0.0f,0.0f);
		// 消えるまでの時間の生成
		bullet.vanishTime = std::make_shared<Time>(kVanishTime);
		// 当たり判定の生成
		bullet.coll = std::make_shared<CollisionShape>(bullet.pos, kRadius, 0.0f);
		// モデルの生成
		bullet.model = std::make_shared<Model>(bulletH);
		// 弾のスケールの設定
		bullet.model->SetScale(kScale);
	}

	// モデルスケール設定
	m_pModel->SetScale(kScaleVec);

	// モデルの頂点タイプの取得
	for (int i = 0; i < MV1GetTriangleListNum(m_pModel->GetModelHandle()); i++)
	{
		// 頂点タイプの取得
		m_vertexShaderType.push_back(MV1GetTriangleListVertexType(m_pModel->GetModelHandle(), i));
	}
	for (int i = 0; i < MV1GetTriangleListNum(m_bullet[0].model->GetModelHandle()); i++)
	{
		// 頂点タイプの取得
		m_bulletVertexShaderType.push_back(MV1GetTriangleListVertexType(m_bullet[0].model->GetModelHandle(), i));
	}
}

Shot::~Shot()
{
	/*処理無し*/
}

void Shot::Update(const VECTOR& playerPos, float playerRot)
{
	// プレイヤーの左後ろ計算
	float angle = playerRot + kPlayerLeftBack;

	// オフセット計算
	float offsetX = cosf(angle) * kPlayerDistance;
	float offsetZ = sinf(angle) * kPlayerDistance;

	// 物体の位置の設定
	m_pos = VGet(playerPos.x + offsetX,
		playerPos.y + kPlayerToHeight,
		playerPos.z + offsetZ);

	// 昇降
	m_sinCount += kSinSpeed;
	m_sinPosY = sinf(m_sinCount) * kSinSwing;
	m_pos.y += m_sinPosY;

	// モデル座標更新
	m_pModel->SetPos(m_pos);
	// モデル回転更新
	m_pModel->SetRot(VGet(0.0f, playerRot, 0.0f));

	// 弾生成
	MakeBullet();
	// 弾更新
	UpdateBullet();
}

void Shot::Draw()
{
	// モデルをフレームごとに描画する
	for (int i = 0; i < MV1GetTriangleListNum(m_pModel->GetModelHandle()); i++)
	{
		// シェーダの設定
		m_pToonShader->SetShader(m_vertexShaderType[i]);

		// 描画
		MV1DrawTriangleList(m_pModel->GetModelHandle(), i);
	}
	// シェーダを使わない設定にする
	m_pToonShader->ShaderEnd();

	for (auto& bullet : m_bullet)
	{
		// 存在していた場合は描画する
		if (bullet.isExist)
		{
			// モデル描画
			// モデルをフレームごとに描画する
			for (int i = 0; i < MV1GetTriangleListNum(bullet.model->GetModelHandle()); i++)
			{
				// シェーダの設定
				m_pToonShader->SetShader(m_bulletVertexShaderType[i]);

				// 描画
				MV1DrawTriangleList(bullet.model->GetModelHandle(), i);
			}
			// シェーダを使わない設定にする
			m_pToonShader->ShaderEnd();
			// 当たり判定の描画
			//bullet.coll->DebugDraw(0xff0000);
		}
	}

#ifdef _DEBUG
	DrawFormatString(0, 160, 0xffffff, "遠距離攻撃武器座標：%f,%f,%f"
	, m_pos.x, m_pos.y, m_pos.z);
#endif // _DEBUG
}

void Shot::OnAttack(CharacterBase* pEnemy)
{
	for (auto& bullet : m_bullet)
	{
		// 衝突判定
		if (bullet.coll->IsCollide(pEnemy->GetCollShape()) && bullet.isExist)
		{
			// 当たっていたらダメージを与える
			pEnemy->OnDamage(m_pos, m_atkPoint, false);

			// 弾を消す
			bullet.isExist = false;	
		}
		else
		{
			continue;
		}
	}
}

void Shot::UpdateBullet()
{
	// 存在していたら進める
	for (auto& bullet : m_bullet)
	{
		if (bullet.isExist)
		{
			// 一定時間経ったら消す
			if (bullet.vanishTime->Update())
			{
				bullet.isExist = false;
				bullet.vanishTime->Reset();
			}
			else
			{
				// 弾を撃つ
				bullet.pos.x += bullet.direction.x * kSpeed;
				bullet.pos.y += bullet.direction.y * kSpeed;
				bullet.pos.z += bullet.direction.z * kSpeed;

				// モデル座標更新
				bullet.model->SetPos(bullet.pos);
			}
		}
	}

}

void Shot::MakeBullet()
{
	// LBボタンが押された場合
	if (Pad::IsPress(PAD_INPUT_5))
	{
		// 前の弾が発射されてから一定時間経った場合
		if (m_pBulletIntervalTime->Update())
		{
			// 弾を一つ一つ見ていく
			for (auto& bullet : m_bullet)
			{
				// 存在していなかったら発射する
				if (!bullet.isExist)
				{
					// 存在フラグを立てる
					bullet.isExist = true;
					// 位置の決定
					bullet.pos = m_pos;
					bullet.pos.y += kStartPosHeight;
					// モデル座標更新
					bullet.model->SetPos(bullet.pos);
					// SEを鳴らす
					SoundManager::GetInstance().Play("Shot", true);

					// カメラがロックオン状態だった時
					if (m_pPlayer->GetCamera()->IsLockOn())
					{
						/*ロックオンしている敵に向けて撃つ*/
						// ターゲット座標取得
						VECTOR targetPos = m_pPlayer->GetCamera()->GetLockOnEnemyPos();
						// 高さ調整
						targetPos.y += kEnemyHeight;

						// 進む方向の決定
						bullet.direction = VNorm(VSub(targetPos, bullet.pos));
					}
					// 通常時
					else
					{		
						/*カメラから見て前に飛ばす*/

						// 進む方向の決定
						bullet.direction = VGet(-m_cameraRotMtx.m[2][0],
							-m_cameraRotMtx.m[2][1],
							-m_cameraRotMtx.m[2][2]);
					}
					break;
				}
			}
			m_pBulletIntervalTime->Reset();
		}		
	}
}
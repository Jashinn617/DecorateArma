#include "RecoveryItem.h"
#include "Model.h"

#include "Player/Player.h"

#include "../Utility/CollisionShape.h"

#include "../Shader/ToonShader.h"

namespace
{
	constexpr int kRecoveryNum = 30;					// HP回復量
	constexpr float kRadius = 35.0f;					// 半径
	constexpr float kHeight = -5.0f;					// 高さ
	constexpr float kApproachRadius = 200.0f;			// 近付く範囲の半径
	constexpr float kApproachHeight = -5.0f;			// 近付く範囲の高さ
	constexpr float kFlyHeight = 90.0f;					// プレイヤーの方向に飛んでいく高さ
	constexpr float kSpeed = 25.0f;						// 近付く速度
	constexpr float kSinSpeed = 0.04f;					// 昇降速度
	constexpr float kSinSwing = 0.5f;					// 昇降幅
	constexpr float kRotX = 20.0f * DX_PI_F / 180.0f;	// モデルX角度
	constexpr float kRotSpeed = 0.03f;					// 回転速度
	constexpr VECTOR kScale = { 0.3f,0.3f,0.3f };		// モデルスケール
}

RecoveryItem::RecoveryItem(VECTOR pos):
	m_sinCount(0.0f),
	m_sinPosY(0.0f),
	m_rotY(0.0f),
	m_isApproaching(0.0f)
{
	// 座標設定
	m_characterInfo.pos = pos;
}

RecoveryItem::~RecoveryItem()
{
}

void RecoveryItem::Init()
{
	// モデル設定
	m_pModel = std::make_shared<Model>(m_modelH);
	// 当たり判定作成(球)
	m_pCollShape = std::make_shared<CollisionShape>(m_characterInfo.pos, kRadius, kHeight);
	// 近付く範囲の当たり判定作成(球)
	m_pApproachColl = std::make_shared<CollisionShape>(m_characterInfo.pos, kApproachRadius, kApproachHeight);

	// モデル座標設定
	m_pModel->SetPos(m_characterInfo.pos);
	// モデルスケール設定
	m_pModel->SetScale(kScale);
	// モデル角度設定
	m_pModel->SetRot(VGet(kRotX, 0.0f, 0.0f));

	// モデルの頂点タイプの取得
	for (int i = 0; i < MV1GetTriangleListNum(m_pModel->GetModelHandle()); i++)
	{
		// 頂点タイプの取得
		m_vertexShaderType.push_back(MV1GetTriangleListVertexType(m_pModel->GetModelHandle(), i));
	}
}

void RecoveryItem::Update()
{
	// プレイヤーを追いかけていないときは上下にふわふわ動く
	if (!m_isApproaching)
	{
		// 昇降
		m_sinCount += kSinSpeed;
		m_sinPosY = sinf(m_sinCount) * kSinSwing;
		m_characterInfo.pos.y += m_sinPosY;

		// 回転
		m_rotY += kRotSpeed;
	}


	// モデル座標更新
	m_pModel->SetPos(m_characterInfo.pos);
	// モデル角度更新
	m_pModel->SetRot(VGet(kRotX, m_rotY, 0.0f));
}

void RecoveryItem::Draw(std::shared_ptr<ToonShader> pToonShader)
{
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

	// 当たり判定デバッグ表示
	//m_pCollShape->DebugDraw(0xff00ff);
	// プレイヤーに近付く範囲の当たり判定デバッグ表示
	//m_pApproachColl->DebugDraw(0x00ffff);
}

void RecoveryItem::OnGet(Player* pPlayer)
{
	// 衝突判定
	if (m_pCollShape->IsCollide(pPlayer->GetCollShape()))
	{
		// 回復処理
		pPlayer->OnRecovery(kRecoveryNum);
		// 存在を消す
		m_characterInfo.isExist = false;
	}
}

void RecoveryItem::OnApproach(ObjectBase* pPlayer)
{
	// 衝突判定
	if (m_pApproachColl->IsCollide(pPlayer->GetCollShape()))
	{
		// プレイヤーまでのベクトルを求める
		VECTOR itemToPlayer = VSub(pPlayer->GetInfo().pos, m_characterInfo.pos);
		// 高さ調整
		itemToPlayer.y += kFlyHeight;
		// プレイヤーがいる方向を出す
		VECTOR direction = VNorm(itemToPlayer);

		// その方向に進む
		m_characterInfo.pos = VAdd(m_characterInfo.pos, VScale(direction, kSpeed));
	}
}
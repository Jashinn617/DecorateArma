#include "ObjectBase.h"

#include "CharacterBase.h"
#include "Model.h"

#include "../Utility/CollisionShape.h"

#include "../Shader/ShadowMapShader.h"

namespace
{
	constexpr int kMaxColHitPolyNum = 2000;		// 最大当たり判定ポリゴン数
	constexpr int kMaxColHitTryNum = 16;		// 壁押し出し処理の最大試行回数
	constexpr float kColHitSlideLength = 1.0f;	// 壁押し出し時にスライドさせる距離
	constexpr float kMove = 0.01f;				// 移動したかを判断するための変数
	constexpr float kWallPolyBorder = 0.4f;		// 壁ポリゴンか床ポリゴンかを判断する為の変数
	constexpr float kWallPolyHeight = 20.0f;	// 壁ポリゴンと判断するための高さ変数
	constexpr float kHeadHeight = 40.0f;		// 頭の高さ
}

ObjectBase::ObjectBase() :
	m_modelH(-1),
	m_topFrameIndex(-1),
	m_bottomFrameIndex(-1),
	m_objSize(0.0f),
	m_angle(0.0f),
	m_moveSpeed(0.0f),
	m_pModel(nullptr),
	m_pCollShape(nullptr),
	m_pObjectManager(nullptr),
	m_wallNum(0),
	m_floorNum(0),
	m_isMove(false),
	m_isPolyHit(false),
	m_prevPos{ 0.0f,0.0f,0.0f },
	m_nextPos{ 0.0f,0.0f,0.0f },
	m_pPoly(nullptr)
{
	// キャラクター情報の初期化
	m_characterInfo.pos = VGet(0.0f, 0.0f, 0.0f);
	m_characterInfo.vec = VGet(0.0f, 0.0f, 0.0f);
	m_characterInfo.rot = VGet(0.0f, 0.0f, 0.0f);
	m_characterInfo.isExist = true;
}

ObjectBase::~ObjectBase()
{
	/*処理無し*/
}

void ObjectBase::End()
{
	/*ポインタの解放*/
	*m_pWallPoly = nullptr;
	*m_pFloorPoly = nullptr;
	m_pPoly = nullptr;
}

void ObjectBase::ShadowMapDraw(std::shared_ptr<ShadowMapShader> pShadowMapShader)
{
	// モデルポインタが無い場合は何もしない
	if (m_pModel == nullptr)return;

	for (int i = 0; i < MV1GetTriangleListNum(m_pModel->GetModelHandle()); i++)
	{
		// 頂点タイプの取得
		int type = MV1GetTriangleListVertexType(m_pModel->GetModelHandle(), i);
		// シェーダの設定
		pShadowMapShader->SetShader(type);
		// 描画
		MV1DrawTriangleList(m_pModel->GetModelHandle(), i);
	}
	// シェーダを使わない設定に戻す
	pShadowMapShader->ShaderEnd();

}

void ObjectBase::MoveCollField(ObjectBase* pField)
{
	// 移動前の座標を保存する
	m_prevPos = m_characterInfo.pos;
	// 移動後の座標を計算する
	m_nextPos = VAdd(m_prevPos, m_characterInfo.vec);

	/*周囲にあるステージポリゴンを移動距離を考慮して取得する
	(モデルハンドル,
	コリジョン情報を更新するフレーム番号,当たり判定用の球の中心座標,
	球の半径)*/
	m_hitDin = MV1CollCheck_Sphere(pField->GetModel()->GetModelHandle(),
		-1, m_characterInfo.pos,
		dynamic_cast<CharacterBase*>(this)->GetCollShape()->GetRadius() + VSize(m_characterInfo.vec));

	// X,Z方向への移動量が0.01f以下だった場合は移動をしていないことにする
	if (fabs(m_characterInfo.vec.x) <= kMove || fabs(m_characterInfo.vec.z) > kMove)
	{
		m_isMove = false;
	}
	else // 0.01f以上だったら移動をしたことにする
	{
		m_isMove = true;
	}

	// 壁と床の当たり判定を行う
	CheckWallAndFloor();
	// 壁との当たり判定から位置修正処理を行う
	FixPosWithWall();
	// 床との当たり判定から位置修正処理を行う
	FixPosWithFloor();

	// 新しい座標を保存する
	m_characterInfo.pos = m_nextPos;

	// 検出したプレイヤーの周囲のポリゴン情報を解放する(後始末)
	MV1CollResultPolyDimTerminate(m_hitDin);
}

void ObjectBase::CheckWallAndFloor()
{
	// 壁ポリゴンと床ポリゴンの数を初期化する
	m_wallNum = 0;
	m_floorNum = 0;

	// 検出されたポリゴンの数だけ繰り返す
	for (int i = 0; i < m_hitDin.HitNum; i++)
	{
		/*ポリゴンの法線のY成分がほぼ縦に垂直かどうかで
		壁ポリゴンか床ポリゴンかを判断する
		(0.4f以上斜めになっていたら床ポリゴン判定にする)*/
		if (m_hitDin.Dim[i].Normal.y < kWallPolyBorder &&
			m_hitDin.Dim[i].Normal.y > -kWallPolyBorder) // 壁だった場合
		{
			// 壁ポリゴンと判断された場合でも、
			// プレイヤーのY座標よりも高いポリゴンのみと当たり判定を行う
			if (m_hitDin.Dim[i].Position[0].y > GetInfo().pos.y + kWallPolyHeight ||
				m_hitDin.Dim[i].Position[1].y > GetInfo().pos.y + kWallPolyHeight ||
				m_hitDin.Dim[i].Position[2].y > GetInfo().pos.y + kWallPolyHeight)
			{
				// ポリゴンの数が限界数に達していなかった場合
				if (m_wallNum < ColInfo::kMaxColHitPolyNum)
				{
					// ポリゴンの構造体のアドレスを壁ポリゴンポインタに保存する
					m_pWallPoly[m_wallNum] = &m_hitDin.Dim[i];
					// 壁ポリゴンの数を足す
					m_wallNum++;
				}
			}
		}
		else	// 床ポリゴンの場合
		{
			// ポリゴンの数が限界数に達していなかった場合
			if (m_floorNum < ColInfo::kMaxColHitPolyNum)
			{
				// ポリゴンの構造体のアドレスを床ポリゴンポインタに保存する
				m_pFloorPoly[m_floorNum] = &m_hitDin.Dim[i];
				// 床ポリゴンの数を足す
				m_floorNum++;
			}
		}
	}
}

void ObjectBase::FixPosWithWall()
{
	// 壁ポリゴンが無かった場合は何もしない
	if (m_wallNum == 0)return;

	/*壁ポリゴンとの当たり判定の処理*/

	// 壁に当たったかどうかのフラグをfalseにしておく
	m_isPolyHit = false;

	/*移動しているかどうかで処理を分岐する*/
	if (m_isMove)	// 移動していた場合
	{
		// 壁ポリゴンの数だけ繰り返す
		for (int i = 0; i < m_wallNum; i++)
		{
			// i番目の壁のポリゴンのアドレスを壁ポリゴンポインタ配列から取得する
			m_pPoly = m_pWallPoly[i];

			// ポリゴンとキャラクターが当たっていなかったら次のカウントに行く
			if (!HitCheck_Capsule_Triangle(m_nextPos,
				VAdd(m_nextPos,
					VGet(0.0f, dynamic_cast<CharacterBase*>(this)->GetCollShape()->GetRadius(), 0.0f)),
				dynamic_cast<CharacterBase*>(this)->GetCollShape()->GetRadius(),
				m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2])) continue;

			// 当たっていなかったら当たっているフラグを立てる
			m_isPolyHit = true;

			/*壁を考慮した移動量を外積を使って出す*/
			// スライドさせる距離
			VECTOR slideVec;
			// 進行方向ベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを出す
			slideVec = VCross(m_characterInfo.vec, m_pPoly->Normal);
			/*↑で出したベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを出し、
			元の移動成分から壁方向の移動成分を抜いたベクトルを出す*/
			slideVec = VCross(m_pPoly->Normal, slideVec);
			// 移動前の座標にスライドさせる距離を足したものを次の座標にする
			m_nextPos = VAdd(m_prevPos, slideVec);

			/*新しい移動座標で壁ポリゴンと当たっていないかを判定する*/
			// 壁と当たっているかどうかのフラグを作る
			bool isHitWallPoly = false;
			// 壁ポリゴンの数だけ繰り返す
			for (int j = 0; j < m_wallNum; j++)
			{
				// j番目の壁のポリゴンのアドレスを壁ポリゴンポインタ配列から取得する
				m_pPoly = m_pWallPoly[j];

				// ポリゴンとキャラクターが当たっていた場合
				if (HitCheck_Capsule_Triangle(m_nextPos,
					VAdd(m_nextPos,
						VGet(0.0f, dynamic_cast<CharacterBase*>(this)->GetCollShape()->GetRadius(), 0.0f)),
					dynamic_cast<CharacterBase*>(this)->GetCollShape()->GetRadius(),
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
				{
					// 当たっているフラグを立てる
					isHitWallPoly = true;
					// ループから抜ける
					break;
				}
			}
			// 全てのポリゴンと当たっていなかった場合
			if (!isHitWallPoly)
			{
				// 当たっているフラグをfalseにする
				m_isPolyHit = false;
				// ループから抜ける
				break;
			}
		}
	}
	else	// 移動していなかった場合
	{
		// 壁ポリゴンの数だけ繰り返す
		for (int i = 0; i < m_wallNum; i++)
		{
			// i番目の壁のポリゴンのアドレスを壁ポリゴンポインタ配列から取得する
			m_pPoly = m_pWallPoly[i];

			// ポリゴンとキャラクターが当たっていた場合
			if (HitCheck_Capsule_Triangle(m_nextPos,
				VAdd(m_nextPos,
					VGet(0.0f, dynamic_cast<CharacterBase*>(this)->GetCollShape()->GetRadius(), 0.0f)),
				dynamic_cast<CharacterBase*>(this)->GetCollShape()->GetRadius(),
				m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
			{
				// 当たっているフラグを立てる
				m_isPolyHit = true;
				// ループを抜ける
				break;
			}
		}
	}

	// 壁に当たっていた場合壁からの押し出し処理をする
	if (m_isPolyHit) FixPosWithWallInternal();
}

void ObjectBase::FixPosWithWallInternal()
{
	// 壁からの押し出し処理を限界まで繰り返す
	for (int i = 0; i < ColInfo::kMaxColHitTryNum; i++)
	{
		/*当たる可能性のある壁ポリゴンを全て見る*/
		// 壁と当たっているかどうかのフラグ
		bool isHitWall = false;

		// 壁ポリゴンの数だけ繰り返す
		for (int j = 0; j < m_wallNum; j++)
		{
			// j番目の壁のポリゴンのアドレスを壁ポリゴンポインタ配列から取得する
			m_pPoly = m_pWallPoly[j];

			// ポリゴンとキャラクターが当たっていなかったら次のカウントに行く
			if (!HitCheck_Capsule_Triangle(m_nextPos,
				VAdd(m_nextPos,
					VGet(0.0f, dynamic_cast<CharacterBase*>(this)->GetCollShape()->GetRadius(), 0.0f)),
				dynamic_cast<CharacterBase*>(this)->GetCollShape()->GetRadius(),
				m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2])) continue;

			// 当たっていた場合は基底距離分、壁の法線方向に移動させる
			m_nextPos = VAdd(m_nextPos, VScale(m_pPoly->Normal, kColHitSlideLength));

			// 移動したうえで壁ポリゴンと接触しているかどうかを判定する
			for (int k = 0; k < m_wallNum; k++)
			{
				m_pPoly = m_pWallPoly[k];
				// 当たっていた場合
				if (HitCheck_Capsule_Triangle(m_nextPos,
					VAdd(m_nextPos,
						VGet(0.0f, dynamic_cast<CharacterBase*>(this)->GetCollShape()->GetRadius(), 0.0f)),
					dynamic_cast<CharacterBase*>(this)->GetCollShape()->GetRadius(),
					m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]))
				{
					// 当たっているフラグを立てる
					isHitWall = true;
					// ループを抜ける
					break;
				}
			}
			// 全てのポリゴンと当たっていなかったらループを終わる
			if (!isHitWall) break;
		}
		// ループを終わる
		if (!isHitWall) break;
	}
}

void ObjectBase::FixPosWithFloor()
{
	// 床ポリゴンが無い場合は何もしない
	if (m_floorNum == 0) return;

	/*床ポリゴンとの当たり判定*/
	// 当たったかどうかのフラグの初期化
	bool isHitFlag = false;

	/*床ポリゴンとの当たり判定*/
	// 一番高い床ポリゴンにぶつけるための判定用変数を初期化する
	float polyMaxPosY = 0.0f;

	// 床ポリゴンに当たったかどうかのフラグを倒しておく
	m_isPolyHit = false;

	// 床ポリゴンの数だけ繰り返す
	for (int i = 0; i < m_floorNum; i++)
	{
		// i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得する
		m_pPoly = m_pFloorPoly[i];

		// 足先から頭の高さまでの間でポリゴンと接触しているかどうかを判定する
		m_lineRes = HitCheck_Line_Triangle(VAdd(m_nextPos, VGet(0.0f, kHeadHeight, 0.0f)),
			m_nextPos, m_pPoly->Position[0], m_pPoly->Position[1], m_pPoly->Position[2]);

		// 当たっていなかったら何もせずに次のカウントに行く
		if (!m_lineRes.HitFlag) continue;

		// 既に当たったポリゴンがあり、かつ今まで検出した
		// 床ポリゴンより低い場合は何もせずに次のカウントに行く
		if (m_isPolyHit && polyMaxPosY > m_lineRes.Position.y) continue;

		// ポリゴンに当たったフラグを立てる
		m_isPolyHit = true;

		// 接触したY座標を保存する
		polyMaxPosY = m_lineRes.Position.y;
	}

	if (m_isPolyHit)	// 床に当たっていた場合
	{
		// 接触したポリゴンで一番高いY座標をプレイヤーのY座標にする
		m_nextPos.y = polyMaxPosY;
		dynamic_cast<CharacterBase*>(this)->HitGround();
	}
}
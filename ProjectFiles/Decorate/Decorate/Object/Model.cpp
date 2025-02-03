#include "Model.h"

#include <assert.h>

Model::Model(const char* fileName) :
	m_modelH(-1),
	m_animSpeed(0),
	m_pos{ 0,0,0 },
	m_animChangeFrame(0),
	m_animChangeFrameTotal(0),
	m_animChangeSpeed(0),
	m_colFrameIndex(-1),
	m_isUseCol(false),
	m_isUpdateCol(false)
{
	// モデルの読み込み
	m_modelH = MV1LoadModel(fileName);
	// ロードが失敗したら止まる
	assert(m_modelH != -1);

	// アニメーション情報のクリア
	ClearAnimData(m_prevAnim);
	ClearAnimData(m_nextAnim);
}

Model::Model(int modelH) :
	m_modelH(-1),
	m_animSpeed(0),
	m_pos{ 0,0,0 },
	m_animChangeFrame(0),
	m_animChangeFrameTotal(0),
	m_animChangeSpeed(0),
	m_colFrameIndex(-1),
	m_isUseCol(false),
	m_isUpdateCol(false)
{
	// モデルのコピー
	m_modelH = MV1DuplicateModel(modelH);
	// ロードが失敗したら止まる
	assert(m_modelH != -1);

	// アニメーション情報のクリア
	ClearAnimData(m_prevAnim);
	ClearAnimData(m_nextAnim);
}

Model::~Model()
{
	// 当たり判定の情報を使用していた場合
	if (m_isUseCol)
	{
		// 当たり判定の情報を削除する
		MV1TerminateCollInfo(m_modelH, -1);
		// 当たり判定情報を使用しない
		m_isUseCol = false;
	}
	// モデルのデリート
	MV1DeleteModel(m_modelH);
}

void Model::Update()
{
	// アニメーションを進める
	m_animSpeed++;
	// アニメーションが一周した場合
	if (m_animSpeed >= m_animChangeFrameTotal)
	{
		// アニメーションの更新をする
		UpdateAnim(m_prevAnim);
		UpdateAnim(m_nextAnim);
		m_animSpeed = 0;
	}

	// 指定フレームかけてアニメーションを変更する
	m_animChangeFrame++;
	if (m_animChangeFrame > m_animChangeFrameTotal)
	{
		m_animChangeFrame = m_animChangeFrameTotal;
	}
	// アニメーションのブレンド率の設定
	UpdateAnimBlendRate();

	// 当たり判定データの更新
	if (m_isUseCol && m_isUpdateCol)
	{
		// コリジョン情報の構築
		MV1RefreshCollInfo(m_modelH, m_colFrameIndex);
	}
}

void Model::Draw()
{
	// モデルの描画
	MV1DrawModel(m_modelH);
}

void Model::SetPos(VECTOR pos)
{
	// 位置情報の設定
	m_pos = pos;
	MV1SetPosition(m_modelH, m_pos);
}

void Model::SetScale(VECTOR scale)
{
	// サイズの設定
	MV1SetScale(m_modelH, scale);
}

void Model::SetRot(VECTOR rot)
{
	// 回転情報の設定
	MV1SetRotationXYZ(m_modelH, rot);
}

void Model::SetAnim(int animNo, bool isLoop, bool isForceChange)
{
	// animNoと同じアニメーションの再生中は再生されない設定だった場合かつ、
	// 既にアニメーションが再生されていた場合は再生しない
	if ((!isForceChange) && (m_nextAnim.animNo == animNo))return;

	// 以前のアニメーションが残っていた場合は終了する
	if (m_prevAnim.attachNo != -1)
	{
		MV1DetachAnim(m_modelH, m_prevAnim.attachNo);
		ClearAnimData(m_prevAnim);
	}
	if (m_nextAnim.attachNo != -1)
	{
		MV1DetachAnim(m_modelH, m_prevAnim.attachNo);
		ClearAnimData(m_nextAnim);
	}

	// 新しいアニメーションを設定する
	m_nextAnim.animNo = animNo;
	m_nextAnim.attachNo = MV1AttachAnim(m_modelH, animNo, -1, false);
	m_nextAnim.totalTime = MV1GetAttachAnimTotalTime(m_modelH, m_nextAnim.attachNo);
	m_nextAnim.isLoop = isLoop;

	// 変更に掛けるフレーム数を設定する
	m_animChangeFrameTotal = 1;
	m_animChangeFrame = 1;
}

void Model::ChangeAnim(int animNo, bool isLoop, bool isForceChange, int changeFrameNum)
{
	// animNoと同じアニメーションの再生中は再生されない設定だった場合かつ、
	// 既にアニメーションが再生されていた場合は再生しない
	if ((!isForceChange) && (m_nextAnim.animNo == animNo))return;

	// 以前のアニメーションが残っていた場合は終了する
	if (m_prevAnim.attachNo != -1)
	{
		MV1DetachAnim(m_modelH, m_prevAnim.attachNo);
		ClearAnimData(m_prevAnim);
	}
	// 現在再生中のアニメーションを古くする
	m_prevAnim = m_nextAnim;

	// 新しいアニメーションを設定する
	m_nextAnim.animNo = animNo;
	m_nextAnim.attachNo = MV1AttachAnim(m_modelH, animNo, -1, false);
	m_nextAnim.totalTime = MV1GetAttachAnimTotalTime(m_modelH, m_nextAnim.attachNo);
	m_nextAnim.isLoop = isLoop;

	// 変更に掛けるフレーム数を設定する
	m_animChangeFrameTotal = changeFrameNum;
	m_animChangeFrame = 0;

	// アニメーションのブレンド率の設定
	UpdateAnimBlendRate();
}

bool Model::IsAnimEnd()
{
	// Loopアニメの場合はfalseを返す
	if (m_nextAnim.isLoop)return false;

	// 現在のアニメーションの再生時間
	float time = MV1GetAttachAnimTime(m_modelH, m_nextAnim.attachNo);
	// 現在のアニメーションの再生時間がアニメーションの総再生時間よりも
	// 大きかったらtrueを返す
	if (time >= m_nextAnim.totalTime) return true;

	return false;
}

void Model::ClearAnimData(AnimData& anim)
{
	anim.animNo = -1;
	anim.attachNo = -1;
	anim.totalTime = 0.0f;
	anim.isLoop = false;
}

void Model::UpdateAnim(AnimData anim, float animSpeed)
{
	// アニメーションが設定されていなかった場合は何もしない
	if (anim.attachNo == -1)return;

	// アニメーションの更新
	float nowTime = MV1GetAttachAnimTime(m_modelH, anim.attachNo);
	nowTime += animSpeed;
	if (nowTime > anim.totalTime)
	{
		if (anim.isLoop)
		{
			// アニメーションをループさせる
			nowTime -= anim.totalTime;
		}
		else
		{
			nowTime = anim.totalTime;
		}
	}
	// アニメーションの再生時間の設定
	MV1SetAttachAnimTime(m_modelH, anim.attachNo, nowTime);
}

void Model::UpdateAnimBlendRate()
{
	// アニメーション変化のフレーム数に応じたブレンド率を設定する
	float rate = static_cast<float>(m_animChangeFrame) / static_cast<float>(m_animChangeFrameTotal);
	// ブレンド率が1以上にならないようにする
	if (rate > 1.0f)rate = 1.0f;

	// ブレンド率の設定
	MV1SetAttachAnimBlendRate(m_modelH, m_prevAnim.attachNo, 1.0f - rate);
	MV1SetAttachAnimBlendRate(m_modelH, m_nextAnim.attachNo, rate);
}
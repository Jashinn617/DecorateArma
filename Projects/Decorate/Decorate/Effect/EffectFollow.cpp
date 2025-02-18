#include "EffectFollow.h"

EffectFollow::EffectFollow(const char* fileName, ObjectBase* pObj):
	m_pObj(pObj)
{
	// 情報設定
	SetData(fileName);
	m_data.playH = PlayEffekseer3DEffect(m_data.H);
	// 再生
	Play();
}

EffectFollow::~EffectFollow()
{
	// エフェクトの停止
	StopEffekseer3DEffect(m_data.playH);
}

void EffectFollow::Update()
{
	// エフェクトの終了フラグが立っていたら何もしない
	if (m_isEnd) return;

	// エフェクトの再生が終了したら終了する
	if (IsEffekseer3DEffectPlaying(m_data.playH) != 0)
	{
		m_isEnd = true;
		return;
	}

	// オブジェクトのポインタがnullになったら再生終了
	if (m_pObj == nullptr)
	{
		m_isEnd = true;
		return;
	}

	// オブジェクトが存在していなかったら再生終了
	if (!m_pObj->GetInfo().isExist)
	{
		m_isEnd = true;
		return;
	}

	// エフェクト再生
	Play();
}

void EffectFollow::Play()
{
	// リザルトの初期化
	int result = 0;

	// エフェクトの角度の設定
	SetRotationPlayingEffekseer3DEffect
	(m_data.playH,
		m_pObj->GetInfo().rot.x,
		m_pObj->GetInfo().rot.y,
		m_pObj->GetInfo().rot.z);

	// エフェクトの座標の設定
	SetPosPlayingEffekseer3DEffect
	(m_data.playH,
		m_pObj->GetInfo().pos.x,
		m_pObj->GetInfo().pos.y,
		m_pObj->GetInfo().pos.z);

	// エフェクトの大きさの設定
	result = SetScalePlayingEffekseer3DEffect(
		m_data.playH,
		m_data.size,
		m_data.size,
		m_data.size);

	// エフェクトの再生速度の設定
	result = SetSpeedPlayingEffekseer3DEffect
	(m_data.playH, m_data.speed);

	assert(result != -1);
}
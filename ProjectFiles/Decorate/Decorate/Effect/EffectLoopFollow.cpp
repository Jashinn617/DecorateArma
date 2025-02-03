#include "EffectLoopFollow.h"

EffectLoopFollow::EffectLoopFollow(const char* fileName, ObjectBase* pObj):
	m_pObj(pObj)
{
	// 情報設定
	SetData(fileName);
}

EffectLoopFollow::~EffectLoopFollow()
{
	// エフェクトの停止
	StopEffekseer3DEffect(m_data.playH);
}

void EffectLoopFollow::Update()
{
	// エフェクトの終了フラグが立っていたら何もしない
	if (m_isEnd) return;

	// オブジェクトの存在がなくなったらエフェクトの再生を終了する
	if (!m_pObj->GetInfo().isExist)
	{
		m_isEnd = true;
		return;
	}

	// エフェクト再生
	Play();

	// エフェクト角度設定
	SetRotationPlayingEffekseer3DEffect(
		m_data.playH, m_pObj->GetInfo().rot.x,
		m_pObj->GetInfo().rot.y,
		m_pObj->GetInfo().rot.z);

	// エフェクトの座標設定
	SetPosPlayingEffekseer3DEffect(
		m_data.playH, m_pObj->GetInfo().pos.x,
		m_pObj->GetInfo().pos.y,
		m_pObj->GetInfo().pos.z);
}

void EffectLoopFollow::Play()
{
	// リザルトの初期化
	int result = 0;

	// エフェクトが再生中でなかった場合は再生する
	if (IsEffekseer3DEffectPlaying(m_data.playH) == 0)return;

	// エフェクト再生
	result = m_data.playH = PlayEffekseer3DEffect(m_data.H);
	assert(result != -1);

	// エフェクトサイズ設定
	result = SetScalePlayingEffekseer3DEffect(
		m_data.playH, m_data.size,
		m_data.size, m_data.size);

	// エフェクトの再生速度の設定
	result = SetSpeedPlayingEffekseer3DEffect(m_data.playH, m_data.speed);
}
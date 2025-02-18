#include "EffectNormal.h"

EffectNormal::EffectNormal(const char* fileName, VECTOR pos, VECTOR rot):
	m_pos(pos),
	m_rot(rot)
{
	// 情報設定
	SetData(fileName);
	// 再生
	Play();
}

EffectNormal::~EffectNormal()
{
	// エフェクトの停止
	StopEffekseer3DEffect(m_data.playH);
}

void EffectNormal::Update()
{
	// エフェクトの終了フラグが立っていたら何もしない
	if (m_isEnd) return;

	// オブジェクトの存在がなくなったらエフェクトの再生を終了する
	if (IsEffekseer3DEffectPlaying(m_data.playH) != 0)
	{
		m_isEnd = true;
		return;
	}
}

void EffectNormal::Play()
{
	// リザルトの初期化
	int result = 0;
	// エフェクトの再生
	result = m_data.playH = PlayEffekseer3DEffect(m_data.H);
	assert(IsEffekseer3DEffectPlaying(m_data.playH) == 0);

	// エフェクトの座標の設定
	SetPosPlayingEffekseer3DEffect
	(m_data.playH,
		m_pos.x,
		m_pos.y,
		m_pos.z);

	// エフェクトの角度の設定
	SetRotationPlayingEffekseer3DEffect
	(m_data.playH,
		m_rot.x,
		m_rot.y,
		m_rot.z);

	// エフェクトの大きさの設定
	result = SetScalePlayingEffekseer3DEffect(
		m_data.playH,
		m_data.size,
		m_data.size,
		m_data.size);

	// エフェクトの再生速度の設定
	result = SetSpeedPlayingEffekseer3DEffect
	(m_data.playH, m_data.speed);
}

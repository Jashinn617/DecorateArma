#include "SoundManager.h"

#include "../Common/SoundSave.h"

namespace
{
	constexpr int kVolumeRateOne = 51;	// 1音量の値
}

SoundManager::SoundManager()
{
	// サウンドデータのロード
	CsvLoad::GetInstance().SoundLoad(m_data);
}

SoundManager::~SoundManager()
{
	/*処理無し*/
}

void SoundManager::Play(const char* name, bool isSameStream)
{
	if (!isSameStream)
	{
		// 同じサウンドが流れていた場合は処理を終了する
		if (CheckSoundMem(m_data[name].handle))return;

		if (m_data[name].isBgm)
		{
			// サウンドを流す
			PlaySoundMem(m_data[name].handle, DX_PLAYTYPE_LOOP);

			// 音量調整
			float vol = (static_cast<float>(m_data[name].volRate) / 255.0f);
			ChangeVolumeSoundMem(static_cast<int>(SoundSave::GetInstance().GetBGMVol() * kVolumeRateOne * vol),
				m_data[name].handle);
		}
		else
		{
			// サウンドを流す
			PlaySoundMem(m_data[name].handle, DX_PLAYTYPE_BACK);

			// 音量調整
			float vol = (static_cast<float>(m_data[name].volRate) / 255.0f);
			ChangeVolumeSoundMem(static_cast<int>(SoundSave::GetInstance().GetSEVol() * kVolumeRateOne * vol),
				m_data[name].handle);
		}
	}
	else
	{
		// サウンドを流す
		PlaySoundMem(m_data[name].handle, DX_PLAYTYPE_BACK);

		// 音量設定
		float vol = (static_cast<float>(m_data[name].volRate) / 255.0f);
		ChangeVolumeSoundMem(static_cast<int>(SoundSave::GetInstance().GetSEVol() * kVolumeRateOne * vol),
			m_data[name].handle);
	}
}

void SoundManager::StopAllSound()
{
	StopMusic();
}

void SoundManager::StopAllSE()
{
	for (auto& se : m_data)
	{
		// サウンドの種類がSEでなければ何もしない
		if (se.second.isBgm) continue;

		// サウンドの停止
		StopSoundMem(se.second.handle);
	}
}

void SoundManager::DesignationStopSound(std::string str)
{
	// 指定したサウンドを停止させる
	StopSoundMem(m_data[str].handle);
}

bool SoundManager::IsDesignationChackPlaySound(std::string str)
{
	// 指定したサウンドが流れているか調べる
	// 流れていたらtrue
	if (CheckSoundMem(m_data[str].handle)) return true;

	// 流れていなかったらfalseを返す
	return false;
}

void SoundManager::ChangeBGMVolume()
{
	// BGMの音量変更
	for (auto& bgm : m_data)
	{
		// サウンドの種類がBGMでなければ何もしない
		if (!bgm.second.isBgm) continue;

		ChangeVolumeSoundMem(SoundSave::GetInstance().GetBGMVol() * kVolumeRateOne,
			bgm.second.handle);
	}
}

void SoundManager::ChangeSEVolume()
{
	// SEの音量変更
	for (auto& se : m_data)
	{
		// サウンドの種類がSEでなければ何もしない
		if (se.second.isBgm) continue;

		ChangeVolumeSoundMem(SoundSave::GetInstance().GetSEVol() * kVolumeRateOne,
			se.second.handle);
	}
}

#pragma once

#include "CharacterData.h"
#include "../Common/CsvLoad.h"

#include <string>
#include <unordered_map>

using namespace CharacterData;

/// <summary>
/// サウンド管理を行うクラス
/// </summary>
class SoundManager
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~SoundManager();

	/// <summary>
	/// SoundManager参照用関数
	/// </summary>
	/// <returns>実態</returns>
	static SoundManager& GetInstance()
	{
		// 唯一の実態
		static SoundManager instance;

		// 実態を返す
		return instance;
	}

	/// <summary>
	/// 指定のサウンドを流す
	/// </summary>
	/// <param name="name">サウンド名</param>
	/// <param name="isSameStream">同じサウンドが流れていてもサウンドを流すか</param>
	void Play(const char* name, bool isSameStream);

	/// <summary>
	/// 全てのサウンドの停止
	/// </summary>
	void StopAllSound();

	/// <summary>
	/// 全てのSEの停止
	/// </summary>
	void StopAllSE();

	/// <summary>
	/// 指定のサウンドの停止
	/// </summary>
	/// <param name="str">指定するサウンド名</param>
	void DesignationStopSound(std::string str);

	/// <summary>
	/// 指定のサウンドが流れているかを調べる
	/// </summary>
	/// <param name="str">指定するサウンド名</param>
	/// <returns>指定のサウンドが流れているかどうか</returns>
	bool IsDesignationChackPlaySound(std::string str);

	/// <summary>
	/// BGMの音量変更
	/// </summary>
	void ChangeBGMVolume();

	/// <summary>
	/// SEの音量変更
	/// </summary>
	void ChangeSEVolume();

private:	// 構造体
	/// <summary>
	/// 変更した音量情報をファイルに書き込む
	/// </summary>
	struct SoundConfigInfo
	{
		char signature[5];			// SND_
		float version;				// 1.0
		unsigned short volumeSE;	// 0～255
		unsigned short volumeBGM;	// 0～255
	};

private:	// 関数
	/// <summary>
	/// コンストラクタ
	/// シングルトンパターンなのでprivateに置く
	/// </summary>
	SoundManager();
	// コピーと代入の禁止
	SoundManager(const SoundManager&) = delete;
	void operator=(const SoundManager&) = delete;

private:	// 変数
	std::unordered_map<std::string, LoadData::SoundData> m_data;	// サウンド情報
};
#pragma once

#include <vector>
#include <string>

class SoundSave
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~SoundSave();

	/// <summary>
	/// 実態の取得
	/// </summary>
	/// <returns>実態</returns>
	static SoundSave& GetInstance()
	{
		// 唯一の実態
		static SoundSave instance;

		// 参照を返す
		return instance;
	}

	/// <summary>
	/// データの読み込み
	/// </summary>
	void Load();

	/// <summary>
	/// データの書き込み
	/// </summary>
	void Write();

	/// <summary>
	/// BGM音量変更
	/// </summary>
	/// <param name="bgmVol">BGM音量</param>
	void ChangeBGMVol(int bgmVol);

	/// <summary>
	/// SE音量変更
	/// </summary>
	/// <param name="seVol">SE音量</param>
	void ChangeSEVol(int seVol);

	/// <summary>
	/// セーブデータを新規作成して上書きする(新規保存)
	/// </summary>
	void CreateNewData();

	/// <summary>
	/// データのクリア
	/// </summary>
	void ClearData();

	/// <summary>
	/// BGM音量取得
	/// </summary>
	/// <returns></returns>
	int GetBGMVol() const { return m_data.volBgm; }

	/// <summary>
	/// SE音量取得
	/// </summary>
	/// <returns></returns>
	int GetSEVol() const { return m_data.volSe; }


private:	// 構造体
	struct SaveData
	{
		int volBgm = 3;	// BGM音量0～5
		int volSe = 3;	// SE音量0～5
	};

private:	// 関数
	/// <summary>
	/// コンストラクタ
	/// シングルトンパターンなのでprivateに置く
	/// </summary>
	SoundSave();
	// コピーと代入を禁止する
	SoundSave(const SoundSave&) = delete;
	void operator=(const SoundSave&) = delete;

	/// <summary>
	/// 区切り文字が検出されるまで文字をプッシュバックする
	/// </summary>
	/// <param name="input">抜き出す文字</param>
	/// <param name="delimiter">区切り文字</param>
	/// <returns>検出されるまでの文字列</returns>
	std::vector<std::string> Split(std::string& input, char delimiter);

private:	// 変数
	SaveData m_data;
};


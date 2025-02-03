#pragma once
#include "DxLib.h"
#include "EffekseerForDXLib.h"

#include "../Common/CsvLoad.h"

#include <unordered_map>
#include <list>
#include <string>

class ObjectBase;
class EffectBase;

class Effekseer3DManager
{
public:	// 列挙型
	/// <summary>
	/// 再生タイプ
	/// </summary>
	enum class PlayType
	{
		LoopFollow,	// ループ、追従
		Loop,		// ループのみ
		Flollw,		// 追従のみ
		Normal,		// 通常再生
	};

public:	// エフェクトベースクラス
	class EffectBase
	{
	public:
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~EffectBase() {/*処理無し*/ };

		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update() {/*処理無し*/ };

		/// <summary>
		/// エフェクト情報の設定
		/// </summary>
		/// <param name="FileName"></param>
		void SetData(const char* FileName)
		{
			m_data = GetInstance().m_data[FileName];
		}

		/// <summary>
		/// エフェクトが再生終了したかどうか
		/// </summary>
		/// <returns>エフェクトが再生終了したか</returns>
		virtual bool IsEnd() { return m_isEnd; }

		/// <summary>
		/// エフェクト情報の取得
		/// </summary>
		/// <returns>エフェクト情報</returns>
		LoadData::EffectData GetData()const { return m_data; }

	protected:	// 関数
		/// <summary>
		/// エフェクトの再生
		/// </summary>
		virtual void Play() {/*処理無し*/ };

	protected:	// 変数
		bool m_isEnd;					// エフェクトが再生終了したかどうか
		LoadData::EffectData m_data;	// エフェクト情報
	};

public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Effekseer3DManager();

	/// <summary>
	/// SoundManager参照用関数
	/// </summary>
	/// <returns>実態</returns>
	static Effekseer3DManager& GetInstance();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// エフェクト追加
	/// </summary>
	/// <param name="fileName">ファイルパス</param>
	/// <param name="type">エフェクトタイプ</param>
	/// <param name="pObject">オブジェクトポインタ</param>
	/// <param name="pos">座標</param>
	/// <param name="rot">角度</param>
	void Add(const char* fileName, PlayType type, ObjectBase* pObject = nullptr,
		VECTOR pos = { 0.0f,0.0f,0.0f }, VECTOR rot = { 0.0f,0.0f,0.0f });

	/// <summary>
	/// エフェクト消去
	/// </summary>
	void Delete();

private:	// 関数
	/// <summary>
	/// コンストラクタ
	/// シングルトンパターンなのでprivateに置く
	/// </summary>
	Effekseer3DManager();
	// コピーと代入の禁止
	Effekseer3DManager(const Effekseer3DManager&) = delete;
	void operator=(const Effekseer3DManager&) = delete;

private:	// 変数
	std::unordered_map<std::string, LoadData::EffectData> m_data;	// エフェクトの情報テーブル

	std::list<std::shared_ptr<EffectBase>> m_pEffect;				// エフェクト管理リスト
};
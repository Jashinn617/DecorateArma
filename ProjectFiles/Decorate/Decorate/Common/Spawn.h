#pragma once
#include "../Utility/Game.h"

#include <string>
#include <vector>
#include <list>

class ObjectManager;
class ObjectBase;


/// <summary>
/// 敵とアイテムの発生処理
/// </summary>
class Spawn
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="stageKind"></param>
	/// <param name="pObjectManager"></param>
	Spawn(Game::StageKind stageKind, ObjectManager* pObjectManager);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Spawn();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 出現
	/// </summary>
	void Advent();

private:	// 構造体
	/// <summary>
	/// 敵情報
	/// </summary>
	struct Data
	{
		ObjectBase* pObj = nullptr;	// オブジェクトベースポインタ
		int index = 0;					// 番号
		std::string name{};				// 名前
	};

private:	// 関数
	/// <summary>
	/// Csvファイルのロード
	/// </summary>
	void CsvLoad();

	/// <summary>
	/// 敵の追加
	/// </summary>
	/// <param name="createNum">番号</param>
	/// <param name="kind">種類</param>
	/// <param name="path">ファイルパス</param>
	/// <param name="name">名前</param>
	/// <param name="pos">座標</param>
	/// <param name="moveSpeed">移動速度</param>
	void Add(int createNum, std::string kind, std::string name, std::string path, VECTOR pos);

	/// <summary>
	/// 区切り文字が検出されるまで文字をプッシュバックする
	/// </summary>
	/// <param name="input">抜き出す文字</param>
	/// <param name="delimiter">区切り文字</param>
	/// <returns>検出されるまでの文字列</returns>
	std::vector<std::string> Split(std::string& input, char delimiter);

private:	// 変数
	int m_createNum;					// 出現番号
	const char* m_fileName;				// ファイル名
	std::list<Data*> m_pData;			// 情報
	std::string m_line;					// ファイル内の文字列
	ObjectManager* m_pObjectManager;	// オブジェクトマネージャーポインタ
};


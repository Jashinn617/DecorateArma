#include "Spawn.h"

#include "../Object/ObjectBase.h"
#include "../Object/ObjectManager.h"

#include "../Object/RecoveryItem.h"

#include "../Object/Enemy/EnemyBase.h"
#include "../Object/Enemy/EnemyPlant.h"
#include "../Object/Enemy/EnemySloth.h"
#include "../Object/Enemy/EnemyWyvern.h"
#include "../Object/Enemy/EnemyBug.h"
#include "../Object/Enemy/EnemyDiatryma.h"
#include "../Object/Enemy/EnemySerpent.h"
#include "../Object/Enemy/EnemyBossParasite.h"

#include <fstream>
#include <sstream>
#include <cassert>

namespace
{
	// ステージ毎にスポーンするものを変える
	const char* const kCsvDataFile[static_cast<int>(Game::StageKind::StageNum)] =
	{
		"Data/CsvFile/SpawnFile/TestSpawn.csv",
		"Data/CsvFile/SpawnFile/Stage1Spawn.csv",
	};
}

Spawn::Spawn(Game::StageKind stageKind, ObjectManager* pObjectManager):
	m_createNum(0),
	m_fileName(kCsvDataFile[(static_cast<int>(stageKind))]),
	m_pObjectManager(pObjectManager)
{
	// 生成
	CsvLoad();
}

Spawn::~Spawn()
{
	// 情報ポインタの解放
	std::list<Data*>::iterator it = m_pData.begin();
	while (it != m_pData.end())
	{
		auto obj = (*it);

		delete obj;
		obj = nullptr;
		it = m_pData.erase(it);
	}
}

void Spawn::Init()
{
	// 出現
	Advent();
}

void Spawn::Advent()
{
	for (auto& data : m_pData)
	{
		// オブジェクトの追加
		m_pObjectManager->AddObject(data->pObj);
		// 初期化
		data->pObj->Init();
	}
}

void Spawn::CsvLoad()
{
	// ファイル名
	std::ifstream ifs(m_fileName);

	while (getline(ifs, m_line))
	{
		// csvデータの1行を','で複数の文字列の分割する
		std::vector<std::string> strvec = Split(m_line, ',');

		// strvec[0]		：出現番号		int
		// strvec[1]		：種類			string
		// strvec[2]		：名前			string
		// strvec[3]		：ファイルパス	string
		// strvec[4]		：X座標			float
		// strvec[5]		：Y座標			float
		// strvec[6]		：Z座標			float

		/*読み取ったデータを変換する*/
		// 座標
		VECTOR pos = VGet(stof(strvec[4]), stof(strvec[5]), stof(strvec[6]));
		// 高さ調整

		// 追加
		Add(stoi(strvec[0]), strvec[1], strvec[2], strvec[3], pos);
	}
}

void Spawn::Add(int createNum, std::string kind, std::string name, std::string path, VECTOR pos)
{
	std::string objModelFile;

	if (kind == "Item")
	{
		// モデルファイルパス
		objModelFile = "Data/Model/Item/" + path + ".mv1";
	}
	else if (kind == "Enemy")
	{
		// モデルファイルパス
		objModelFile = "Data/Model/Enemy/" + path + ".mv1";
	}
	// モデルファイル名
	const char* const modelFile = objModelFile.c_str();
	// モデルハンドルの取得
	int modelH = MV1LoadModel(modelFile);
	// 取得に失敗したら止める
	assert(modelH != -1);

	if (kind == "Item")
	{
		// nameと同じ種類のアイテムの追加
		if (name == "Recovery")
		{
			m_pData.push_back(new Data);
			m_pData.back()->pObj = new RecoveryItem(pos);
		}
		else
		{
			// 合うアイテムがいなかった場合は何もしない
			return;
		}
	}
	else if (kind == "Enemy")
	{
		// nameと同じ種類の敵の追加
		if (name == "Plant")
		{
			m_pData.push_back(new Data);
			m_pData.back()->pObj = new EnemyPlant(pos);
		}
		else if (name == "Sloth")
		{
			m_pData.push_back(new Data);
			m_pData.back()->pObj = new EnemySloth(pos);
		}
		else if (name == "Wyvern")
		{
			m_pData.push_back(new Data);
			m_pData.back()->pObj = new EnemyWyvern(pos);
		}
		else if (name == "Bug")
		{
			m_pData.push_back(new Data);
			m_pData.back()->pObj = new EnemyBug(pos);
		}
		else if (name == "Diatryma")
		{
			m_pData.push_back(new Data);
			m_pData.back()->pObj = new EnemyDiatryma(pos);
		}
		else if (name == "Serpent")
		{
			m_pData.push_back(new Data);
			m_pData.back()->pObj = new EnemySerpent(pos);
		}
		else if (name == "BossParasite")
		{
			m_pData.push_back(new Data);
			m_pData.back()->pObj = new EnemyBossParasite(pos);
		}
		else
		{
			// 合う敵がいなかった場合は何もしない
			return;
		}
	}

	// モデルハンドルの設定	
	m_pData.back()->pObj->SetModelHandle(modelH);
	// 敵番号の設定
	m_pData.back()->index = createNum;
	// 種類の設定
	m_pData.back()->name = name;
}

std::vector<std::string> Spawn::Split(std::string& input, char delimiter)
{
	std::istringstream stream(input);	// 文字列ストリーム
	std::string filed;					// 一時格納用文字列
	std::vector<std::string> result;	// 分割結果

	// 文字列を区切り文字(コンマ)で分割する
	while (getline(stream, filed, delimiter))
	{
		// 分割した文字の追加
		result.push_back(filed);
	}

	return result;
}
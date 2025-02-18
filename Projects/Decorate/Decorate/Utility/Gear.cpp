#include "Gear.h"

#include "../Common/CsvLoad.h"

#include "Time.h"

#include <iostream>
#include <fstream>
#include <cassert>

#include <random>
#include <Windows.h>

namespace
{
	const char* kCsvFileName = "Data/CsvFile/Gear.csv";					// 保存するファイル名(装備前装備品)
	const char* kEquippedCsvFileName = "Data/CsvFile/EquippedGear.csv";	// 保存するファイル名(装備後装備品)

	constexpr int kItemDropProbability = 40;	// アイテムを手に入れる確率
	constexpr int kMaxProbability = 100;		// 確率最大値
	constexpr int kMaxHandNum = 99;				// 最大手持ち数
	constexpr int kGearNum = 15;				// 装備品の種類数
	constexpr int kUIPosX = 1500;				// 取得時表示UIX座標
	constexpr int kUIStartPosY = 500;			// 取得時表示UI初期Y座標
	constexpr int kUIIntervalPosY = 40;			// 取得時表示UIY座標間隔
	constexpr int kUIDeleteTime = 60;			// UIを消すまでの時間

	// ステージ名
	const char* kStageName[2] =
	{
		"Test",
		"Stage1",
	};

	// 取得装備UI画像パス
	const char* kGetUIPath[kGearNum] =
	{
		"Data/Image/Gear/EquippedUI/HPS.png",
		"Data/Image/Gear/EquippedUI/HPM.png",
		"Data/Image/Gear/EquippedUI/HPL.png",
		"Data/Image/Gear/EquippedUI/MATKS.png",
		"Data/Image/Gear/EquippedUI/MATKM.png",
		"Data/Image/Gear/EquippedUI/MATKL.png",
		"Data/Image/Gear/EquippedUI/SATKS.png",
		"Data/Image/Gear/EquippedUI/SATKM.png",
		"Data/Image/Gear/EquippedUI/SATKL.png",
		"Data/Image/Gear/EquippedUI/DEFS.png",
		"Data/Image/Gear/EquippedUI/DEFM.png",
		"Data/Image/Gear/EquippedUI/DEFL.png",
		"Data/Image/Gear/EquippedUI/AllS.png",
		"Data/Image/Gear/EquippedUI/AllM.png",
		"Data/Image/Gear/EquippedUI/AllL.png",
	};
}

Gear::Gear() :
	m_nowDropProbability(0),
	m_prevDropProbability(0)
{
	// 装備品情報のロード
	CsvLoad::GetInstance().GearDataLoad(m_data);
	// 装備品情報のロード
	CsvLoad::GetInstance().GearEquippedDataLoad(m_equippedData);
}

Gear::Gear(Game::StageKind stageKind):
	m_nowDropProbability(0),
	m_prevDropProbability(0),
	m_UIDeleteTime(std::make_shared<Time>(kUIDeleteTime))
{
	// 装備品情報のロード
	CsvLoad::GetInstance().GearDataLoad(m_data);
	// 装備品情報のロード
	CsvLoad::GetInstance().GearEquippedDataLoad(m_equippedData);
	// ドロップ確率のロード
	CsvLoad::GetInstance().ItemDropProbabilityDataLoad(m_probability, kStageName[static_cast<int>(stageKind)]);
	// 画像ロード
	for (int i = 0; i < kGearNum; i++)
	{
		m_getUIH.push_back(LoadGraph(kGetUIPath[i]));
		assert(m_getUIH[i]);
	}
}

Gear::~Gear()
{
	// 画像デリート
	for (auto& UI : m_getUIH)
	{
		DeleteGraph(UI);
	}
}

void Gear::Draw()
{
	// getGearが1以上だった場合
	if (m_getGear.size() > 0)
	{
		// 一定時間経過したら先頭の要素から消していく
		if (m_UIDeleteTime->Update())
		{
			// 先頭要素を消す
			m_getGear.erase(m_getGear.begin());

			// リセット
			m_UIDeleteTime->Reset();
		}

		// 描画
		for (int i = 0; i < m_getGear.size(); i++)
		{
			// UI描画
			DrawGraph(kUIPosX, kUIStartPosY + (kUIIntervalPosY * i),
				m_getUIH[m_getGear[i]], true);
		}
	}
}

Gear::GearData Gear::GetGearNum(std::string gearName)
{
	for (int i = 0; i < m_data.size(); i++)
	{
		// 指定の装備品を見つけたらそれを返す
		if (m_data[i].name == gearName)
		{
			return m_data[i];
		}
	}

	// 指定した装備品がない場合はエラーなので止める
	assert(false);
	// 警告消す用(ダミー)
	return m_data[0];
}

Gear::GearData Gear::GetGearNum(int gearNum)
{
	return m_data[gearNum];

	// 指定した装備品がなければ止める
	assert(false);
}

void Gear::DecreaseGear(int gearNum)
{
	m_data[gearNum].num--;
}

void Gear::AddEquippedGear(std::string gearName)
{
	// 指定の装備品を探す
	for (int i = 0; i < m_data.size(); i++)
	{
		// 指定の装備品を見つけたら装備品にそれを入れる
		if (m_data[i].name == gearName)
		{
			m_equippedData.push_back(m_data[i]);
			return;
		}
	}
}

void Gear::RemoveGear(int gearNum)
{
	// 装備前装備品のデータを一つ増やす
	// 外す装備品と同じ装備品を探す
	for (int i = 0; i < m_data.size(); i++)
	{
		// 指定の装備品を見つけたら装備品数を増やして
		// 装備中の装備品を消す
		if (m_data[i].name == m_equippedData[gearNum].name)
		{
			m_data[i].num++;
			m_equippedData.erase(m_equippedData.begin() + gearNum);

			// 見つけたら終わる
			return;
		}
	}
}

void Gear::SaveGear()
{
	/*装備品ファイル*/
	// ファイルを上書きモードで開く
	std::ofstream csvFile(kCsvFileName);
	// ファイルが無かったらエラーメッセージを出す
	if (!csvFile)
	{
		std::cerr << "ファイルを開けませんでした：" << kCsvFileName << std::endl;
	}
	// 一行目に数値の種類を書き込む
	csvFile << "アイテム名,HP上昇値,MATK上昇値,SATK上昇値,DEF上昇値,コスト,所持数,装備番号\n";
	// 二行目以降にデータを書き込む
	for (const auto& data : m_data)
	{
		csvFile << data.name << ","
			<< data.upHp << ","
			<< data.upMAttack << ","
			<< data.upSAttack << ","
			<< data.upDef << ","
			<< data.cost << ","
			<< data.num << ","
			<< data.number << "\n";
	}
	// ファイルを閉じる
	csvFile.close();

	/*装備中装備品ファイル*/
	// ファイルを上書きモードで開く
	std::ofstream csvEquippedFile(kEquippedCsvFileName);
	// ファイルが無かったらエラーメッセージを出す
	if (!csvEquippedFile)
	{
		std::cerr << "ファイルを開けませんでした：" << kEquippedCsvFileName << std::endl;
	}
	// 一行目に数値の種類を書き込む
	csvEquippedFile << "アイテム名,HP上昇値,MATK上昇値,SATK上昇値,DEF上昇値,コスト,所持数,装備番号\n";
	// 二行目以降にデータを書き込む
	for (const auto& data : m_equippedData)
	{
		csvEquippedFile << data.name << ","
			<< data.upHp << ","
			<< data.upMAttack << ","
			<< data.upSAttack << ","
			<< data.upDef << ","
			<< data.cost << ","
			<< data.num << ","
			<< data.number << "\n";
	}
	// ファイルを閉じる
	csvEquippedFile.close();
}

int Gear::GetAddHp()
{
	int ans = 0;

	for (auto& data : m_equippedData)
	{
		// 全てのHPを足す
		ans += data.upHp;
	}

	return ans;
}

int Gear::GetAddMAtk()
{
	int ans = 0;

	for (auto& data : m_equippedData)
	{
		// 全ての近接攻撃力を足す
		ans += data.upMAttack;
	}

	return ans;
}

int Gear::GetAddSAtk()
{
	int ans = 0;

	for (auto& data : m_equippedData)
	{
		// 全ての遠距離攻撃力を足す
		ans += data.upSAttack;
	}

	return ans;
}

int Gear::GetAddDef()
{
	int ans = 0;

	for (auto& data : m_equippedData)
	{
		// 全ての防御力を足す
		ans += data.upDef;
	}

	return ans;
}

void Gear::ObtainItemOnStage()
{
	// アイテムを落としたかどうかを調べる
	std::random_device rd;
	std::mt19937 mt(rd());
	// 範囲
	std::uniform_real_distribution<float> urd(0, kMaxProbability);
	float probability = urd(mt);
	// 出た数が確率より大きかったら何もせずに返る
	if (probability > kItemDropProbability) return;

	// 確率を出す
	float rand = urd(mt);
	// 確率初期化
	m_prevDropProbability = -1;
	m_nowDropProbability = m_probability[0];

	for (int i = 0; i < m_probability.size(); i++)
	{
		/*その確率の範囲内にいるかどうかを調べる*/
		if (m_prevDropProbability < rand && rand <= m_nowDropProbability)
		{
			// 範囲内に入っていたら所持アイテムに加えて処理を返す
			m_dorpData.push_back(m_data[i]);

			// 画面右上に何を手に入れたかを描画するようにする
			m_getGear.push_back(m_data[i].number);

			return;
		}
		// 違う場合は次の範囲を設定する
		m_prevDropProbability = m_nowDropProbability;
		m_nowDropProbability += m_probability[i];
	}
}

void Gear::StageClear()
{
	/*手に入れた装備品を持ち物に入れる*/
	// 手に入れた装備品の数だけ回す
	for (int i = 0; i < m_dorpData.size(); i++)
	{
		// 装備品の種類数だけ回す
		for (int j = 0; j < m_data.size(); j++)
		{
			// 同じ装備品を見つけた場合
			if (m_dorpData[i].name == m_data[j].name)
			{
				// 装備中のモノを含めた装備品の数を確かめる
				int equippedNum = 0;
				for (int l = 0; l < m_equippedData.size(); l++)
				{
					if (m_equippedData[l].name == m_dorpData[i].name)
					{
						equippedNum++;
					}
				}
				// 装備品の数が99個以下だった場合は手持ちの装備品を一つ増やす
				if (kMaxHandNum >= m_data[j].num + equippedNum)
				{
					m_data[j].num++;
				}
				// 次の装備品に行く
				break;
			}
		}
	}
}
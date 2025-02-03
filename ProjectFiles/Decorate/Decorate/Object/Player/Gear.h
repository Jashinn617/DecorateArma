#pragma once

#include "../../Utility/Game.h"

#include <string>
#include <vector>
#include <memory>

class Time;

/// <summary>
/// 装備品
/// </summary>
class Gear
{
public:	// 構造体
	/// <summary>
	/// 装備品情報
	/// </summary>
	struct GearData
	{
		std::string name;	// アイテム名
		int upHp;			// 体力の上昇値
		int upMAttack;		// 近接攻撃力の上昇値
		int upSAttack;		// 遠距離攻撃力の上昇値
		int upDef;			// 防御力の上昇値
		int cost;			// コスト
		int num;			// 所持数
		int number;			// UI画像パス
	};

public:	// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Gear();

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="stageKind">ステージの種類</param>
	Gear(Game::StageKind stageKind);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Gear();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 指定した装備品を取得する
	/// </summary>
	/// <param name="gearName">装備品名</param>
	/// <returns>指定の装備品</returns>
	GearData GetGearNum(std::string gearName);

	/// <summary>
	/// 指定した装備品を取得する
	/// </summary>
	/// <param name="gearNum">装備品番号</param>
	/// <returns>指定の装備品</returns>
	GearData GetGearNum(int gearNum);

	/// <summary>
	/// 指定した装備品の数を減らす
	/// </summary>
	/// <param name="gearNum">装備品番号</param>
	void DecreaseGear(int gearNum);

	/// <summary>
	/// 装備品アイテムの追加
	/// </summary>
	/// <param name="gearName">アイテム名</param>
	void AddEquippedGear(std::string gearName);

	/// <summary>
	/// 装備を外す
	/// </summary>
	/// <param name="gearNum">装備品番号</param>
	void RemoveGear(int gearNum);

	/// <summary>
	/// CSVファイルに装備品のデータを書き込む
	/// </summary>
	void SaveGear();

	/// <summary>
	/// 装備中装備データのサイズの取得
	/// </summary>
	/// <returns>装備中装備データのサイズ</returns>
	int GetEquippedGearSize() { return static_cast<int>(m_equippedData.size()); }

	/// <summary>
	/// 指定した装備中装備品を取得する
	/// </summary>
	/// <param name="gearNum">装備品番号</param>
	/// <returns>指定の装備品</returns>
	GearData GetEquippedNum(int gearNum) { return m_equippedData[gearNum]; }

	/// <summary>
	/// 現在の追加HP数を取得する
	/// </summary>
	/// <returns>追加HP数</returns>
	int GetAddHp();

	/// <summary>
	/// 現在の追加近接攻撃力数を取得する
	/// </summary>
	/// <returns>追加近接攻撃力数</returns>
	int GetAddMAtk();

	/// <summary>
	/// 現在の追加遠距離攻撃力数を取得する
	/// </summary>
	/// <returns>追加遠距離攻撃力数</returns>
	int GetAddSAtk();

	/// <summary>
	/// 現在の追加防御力数を取得する
	/// </summary>
	/// <returns>追加防御力数</returns>
	int GetAddDef();

	/// <summary>
	/// ステージでアイテムを手に入れた時の処理
	/// </summary>
	void ObtainItemOnStage();

	/// <summary>
	/// ステージクリア時の処理
	/// </summary>
	void StageClear();

private: // 変数
	int m_nowDropProbability;				// 現在確率
	int m_prevDropProbability;				// 前回確率
	std::vector<int> m_probability;			// アイテムドロップ確率
	std::vector<int> m_getUIH;				// 取得装備UI画像
	std::vector<int> m_getGear;				// 取得した装備
	std::vector<GearData> m_data;			// 装備品情報
	std::vector<GearData> m_equippedData;	// 装備中装備品情報
	std::vector<GearData> m_dorpData;		// ステージ上で拾ったアイテム

	std::shared_ptr<Time> m_UIDeleteTime;	// UIを消していく時間
	 
};


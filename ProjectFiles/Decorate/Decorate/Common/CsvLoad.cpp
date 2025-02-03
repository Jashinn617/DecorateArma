#include "CsvLoad.h"

#include "EffekseerForDXLib.h"

#include <array>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <assert.h>

/// <summary>
/// 区切り文字が検出されるまで文字をプッシュバックする
/// </summary>
/// <param name="input">抜き出す文字</param>
/// <param name="delimiter">区切り文字</param>
/// <returns>検出されるまでの文字列</returns>
std::vector<std::string> Split(std::string& input, char delimiter)
{
	std::istringstream stream(input);
	std::string field;
	std::vector<std::string> result;
	while (getline(stream, field, delimiter)) 
	{
		result.push_back(field);
	}
	return result;
}

CsvLoad::CsvLoad()
{
	/*処理無し*/
}

CsvLoad::~CsvLoad()
{
	/*処理無し*/
}

CsvLoad& CsvLoad::GetInstance()
{
	// 唯一の実態
	static CsvLoad instance;

	return instance;
}

void CsvLoad::AnimLoad(CharacterBase::AnimData& data, const char* characterName)
{
	// ファイル情報の読み込み
	std::ifstream ifs("Data/CsvFile/AnimNum.csv");
	std::string line;

	std::array<int8_t, 12> animData{};

	std::vector<std::string> strVec;

	while (getline(ifs, line))
	{
		// csvデータ１行を、','で複数の文字列に分割
		strVec = Split(line, ',');

		// strvec[0]	: キャラ名	string
		// strvec[1]	: 待機
		// strvec[2]	: 歩き
		// strvec[3]	: 走り
		// strvec[4]    : 攻撃1
		// strvec[5]    : 攻撃2
		// strvec[6]    : 攻撃3
		// strvec[7]    : 強攻撃
		// strvec[8]    : ダメージ
		// strvec[9]    : 死亡
		// strvec[10]	: ジャンプ時
		// strvec[11]	: ジャンプ中

		//参照したいキャラが見つかっていたら処理をやめる
		const char* str = strVec[0].c_str();
		if (strcmp(str, characterName) == 0)
		{
			for (int i = 0; i < animData.size(); i++)
			{
				//キャラクター名情報はいらないので省く
				if (i == 0)continue;
				int animNum = stoi(strVec[i]);
				animData[i] = static_cast<int8_t>(animNum);
			}
			break;
		}
		else
		{
			strVec.erase(strVec.begin(), strVec.end());
		}
	}

	// 情報をアニメデータに入れる
	data.idle = animData[1];
	data.walk = animData[2];
	data.run = animData[3];
	data.attack1 = animData[4];
	data.attack2 = animData[5];
	data.attack3 = animData[6];
	data.hardAttack = animData[7];
	data.damage = animData[8];
	data.death = animData[9];
	data.jumpStart = animData[10];
	data.jumpIdle = animData[11];
}

void CsvLoad::SoundLoad(std::unordered_map<std::string, LoadData::SoundData>& data)
{
	// ファイル情報の読み込み
	std::ifstream ifs("Data/CsvFile/Sound.csv");
	std::string line;
	while (getline(ifs, line))
	{
		// csvデータ１行を、','で複数の文字列に分割
		std::vector<std::string> strvec = Split(line, ',');

		// strvec[0]	: ファイル名	string
		// strvec[1]	: BGMフラグ		int
		// strvec[2]	: 音量			float
		// strvec[3]	: 拡張子		string

		// 文字列を適切なデータ型に変換して格納
		LoadData::SoundData sData{};

		std::string str;

		//BGMかSEかを判断する
		if (stoi(strvec[1]) == 0)	// SE
		{
			sData.isBgm = false;
			str = "Data/Sound/SE/" + strvec[0] + strvec[3];
			//サウンドをロード
			sData.handle = LoadSoundMem(str.c_str());
			assert(sData.handle != -1);
			
		}
		else // BGM
		{
			sData.isBgm = true;
			str = "Data/Sound/BGM/" + strvec[0] + strvec[3];
			//サウンドをロード
			sData.handle = LoadSoundMem(str.c_str());
			assert(sData.handle != -1);
		}
		//ボリュームの大きさを設定
		sData.volRate = stof(strvec[2]);

		//strvec[0]という配列名にデータを保存
		data[strvec[0]] = sData;
	}
}

void CsvLoad::StatusLoad(CharacterBase::StatusData& data, const char* characterName)
{
	// ファイル情報の読み込み
	std::ifstream ifs("Data/CsvFile/Status.csv");
	std::string line;

	std::vector<std::string> strVec;

	while (getline(ifs, line))
	{
		// csvデータ１行を、','で複数の文字列に分割
		strVec = Split(line, ',');

		// strvec[0]	: キャラ名	string
		// strvec[1]	: HP		int
		// strvec[2]	: MATK		int
		// strvec[3]	: SATK		int
		// strvec[4]	: DEF		int
		// strvec[5]	: SPD		float

		//参照したいキャラが見つかっていたら処理をやめる
		const char* str = strVec[0].c_str();
		if (strcmp(str, characterName) == 0) break;

		else
		{
			strVec.erase(strVec.begin(), strVec.end());
		}
	}

	// ステータス情報をステータスデータに入れる
	data.hp = stoi(strVec[1]);
	data.meleeAtk = stoi(strVec[2]);
	data.shotAtk = stoi(strVec[3]);
	data.def = stoi(strVec[4]);
	data.spd = stof(strVec[5]);
}

void CsvLoad::AddStatusLoad(CharacterBase::StatusData& data, const char* characterName)
{
	// ファイル情報の読み込み
	std::ifstream ifs("Data/CsvFile/EquippedGear.csv");
	std::string line;

	// 最初の行はヘッダなので飛ばす
	getline(ifs, line);

	while (getline(ifs, line))
	{
		// csvデータの1行を','で複数の文字列の分割する
		std::vector<std::string> strVec = Split(line, ',');

		// 行のデータが8個あるか調べる
		if (strVec.size() < 8)continue;

		// strvec[0]	: アイテム名	string
		// strvec[1]	: HP上昇値		int
		// strvec[2]	: MATK上昇値	int
		// strvec[3]	: SATK上昇値	int
		// strvec[4]	: DEF上昇値		int
		// strvec[5]	: コスト		int
		// strvec[6]	: 所持数		int
		// strvec[7]	: 装備番号		int

		// ステータス情報をステータスデータに入れる
		data.hp += stoi(strVec[1]);
		data.meleeAtk += stoi(strVec[2]);
		data.shotAtk += stoi(strVec[3]);
		data.def += stoi(strVec[4]);
	}	
}

void CsvLoad::GearDataLoad(std::vector<Gear::GearData>& data)
{
	// ファイル情報の読み込み
	std::ifstream ifs("Data/CsvFile/Gear.csv");
	std::string line;

	
	// 最初の行はヘッダなので飛ばす
	getline(ifs, line);
	

	while (getline(ifs, line))
	{
		// csvデータの1行を','で複数の文字列の分割する
		std::vector<std::string> strVec = Split(line, ',');

		// 行のデータが8個あるか調べる
		if (strVec.size() < 8)continue;

		// strvec[0]	: アイテム名	string
		// strvec[1]	: HP上昇値		int
		// strvec[2]	: MATK上昇値	int
		// strvec[3]	: SATK上昇値	int
		// strvec[4]	: DEF上昇値		int
		// strvec[5]	: コスト		int
		// strvec[6]	: 所持数		int
		// strvec[7]	: 装備番号		int

		// データ作成
		Gear::GearData gearData;
		// 情報を装備品データに入れる
		gearData.name = strVec[0];
		gearData.upHp = stoi(strVec[1]);
		gearData.upMAttack = stoi(strVec[2]);
		gearData.upSAttack = stoi(strVec[3]);
		gearData.upDef = stoi(strVec[4]);
		gearData.cost = stoi(strVec[5]);
		gearData.num = stoi(strVec[6]);
		gearData.number = stoi(strVec[7]);

		// 構造体をベクターに追加する
		data.push_back(gearData);
	}
}

void CsvLoad::GearEquippedDataLoad(std::vector<Gear::GearData>& data)
{
	// ファイル情報の読み込み
	std::ifstream ifs("Data/CsvFile/EquippedGear.csv");
	std::string line;

	// 最初の行はヘッダなので飛ばす
	getline(ifs, line);
	

	while (getline(ifs, line))
	{
		// csvデータの1行を','で複数の文字列の分割する
		std::vector<std::string> strVec = Split(line, ',');

		// 行のデータが8個あるか調べる
		if (strVec.size() < 8)continue;

		// strvec[0]	: アイテム名	string
		// strvec[1]	: HP上昇値		int
		// strvec[2]	: MATK上昇値	int
		// strvec[3]	: SATK上昇値	int
		// strvec[4]	: DEF上昇値		int
		// strvec[5]	: コスト		int
		// strvec[6]	: 所持数		int
		// strvec[7]	: 装備番号		int

		// データ作成
		Gear::GearData gearData;
		// 情報を装備品データに入れる
		gearData.name = strVec[0];
		gearData.upHp = stoi(strVec[1]);
		gearData.upMAttack = stoi(strVec[2]);
		gearData.upSAttack = stoi(strVec[3]);
		gearData.upDef = stoi(strVec[4]);
		gearData.cost = stoi(strVec[5]);
		gearData.num = stoi(strVec[6]);
		gearData.number = stoi(strVec[7]);
		
		// 構造体をベクターに追加する
		data.push_back(gearData);
	}
}

void CsvLoad::ItemDropProbabilityDataLoad(std::vector<int>& data, const char* stageName)
{
	// ファイル情報の読み込み
	std::ifstream ifs("Data/CsvFile/DropProbability.csv");
	std::string line;

	std::vector<std::string>strVec;

	while (getline(ifs, line))
	{
		// csvデータ１行を、','で複数の文字列に分割
		strVec = Split(line, ',');

		// strvec[0]	: ステージ名	string
		// strvec[1]	: HPS			int
		// strvec[2]	: HPM			int
		// strvec[3]	: HPL			int
		// strvec[4]	: MATKS			int
		// strvec[5]	: MATKM			int
		// strvec[6]	: MATKL			int
		// strvec[7]	: SATKS			int
		// strvec[8]	: SATKM			int
		// strvec[9]	: SATKL			int
		// strvec[10]	: DEFS			int
		// strvec[11]	: DEFM			int
		// strvec[12]	: DEFL			int
		// strvec[13]	: AllS			int
		// strvec[14]	: AllM			int
		// strvec[15]	: AllL			int

		//参照したいステージが見つかっていたら処理をやめる
		const char* str = strVec[0].c_str();
		if (strcmp(str, stageName) == 0) break;

		else
		{
			strVec.erase(strVec.begin(), strVec.end());
		}
	}

	// データを入れる
	for (int i = 1; i <= 15; i++)
	{
		data.push_back(stoi(strVec[i]));
	}
	
}

void CsvLoad::EffectLoad(std::unordered_map<std::string, LoadData::EffectData>& data)
{
	// ファイル情報の読み込み
	std::ifstream ifs("Data/CsvFile/Effect.csv");
	std::string line;

	std::vector<std::string> strVec;

	while (getline(ifs, line))
	{
		// csvデータ１行を、','で複数の文字列に分割
		strVec = Split(line, ',');


		// strvec[0]	: エフェクト名	string
		// strvec[1]	: エフェクトサイズ
		// strvec[2]	: エフェクトの再生速度

		// ファイル名設定
		std::string fileName = "Data/Effect/";
		fileName += strVec[0];
		fileName += ".efk";
		const char* file = fileName.c_str();

		// エフェクト情報をデータに入れる
		data[strVec[0]].H = LoadEffekseerEffect(file);
		data[strVec[0]].size = stof(strVec[1]);
		data[strVec[0]].speed = stof(strVec[2]);

		// ロードに失敗したら止める
		assert(data[strVec[0]].H != -1);
	}
}
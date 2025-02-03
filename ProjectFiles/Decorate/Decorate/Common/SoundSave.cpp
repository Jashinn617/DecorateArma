#include "SoundSave.h"

#include "CsvLoad.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include <cassert>

SoundSave::SoundSave()
{
    /*処理無し*/
}

SoundSave::~SoundSave()
{
}

void SoundSave::Load()
{
    // ファイル情報の読み込み
    std::ifstream ifs("Data/CsvFile/Volume.csv");
    std::string line;

    std::vector<std::string> strVec;

    while (getline(ifs, line))
    {
        // csvデータ１行を、','で複数の文字列に分割
        strVec = Split(line, ',');

        // strvec[0]    ： BGM音量
        // strvec[1]    ： SE音量

        // 情報をデータに入れる
        m_data.volBgm = stoi(strVec[0]);
        m_data.volSe = stoi(strVec[1]);
    }
}

void SoundSave::Write()
{
    // ファイルを上書きモードで開く
    std::ofstream csvFile("Data/CsvFile/Volume.csv");
    // ファイルが無かったらエラーメッセージを出す
    if (!csvFile)
    {
        std::cerr << "ファイルを開けませんでした:" << "Data/CsvFile/Volume.csv" << std::endl;
    }
    // データを書き込む
    csvFile << m_data.volBgm << ","
        << m_data.volSe << "\n";
    // ファイルを閉じる
    csvFile.close();
}

void SoundSave::ChangeBGMVol(int bgmVol)
{
    m_data.volBgm = bgmVol;
}

void SoundSave::ChangeSEVol(int seVol)
{
    m_data.volSe = seVol;
}

void SoundSave::CreateNewData()
{
    ClearData();


}

void SoundSave::ClearData()
{
    m_data.volBgm = 3;
    m_data.volSe = 3;
}

std::vector<std::string> SoundSave::Split(std::string& input, char delimiter)
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

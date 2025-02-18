#include "SceneOption.h"

#include "SceneSelect.h"
#include "SceneGear.h"

#include "../Common/SoundSave.h"
#include "../Common/CsvLoad.h"

#include "../Utility/Game.h"
#include "../Utility/Gear.h"
#include "../Utility/Pad.h"
#include "../Utility/SoundManager.h"

#include <string>
#include <assert.h>

namespace
{
	constexpr int kPrevSceneAlpha = 180;	// 前のシーンを暗くする不透明度
	constexpr int kNum = 10;				// 数字の数

	/*ファイルパス関係*/
	const std::string kOptionPath = "Data/Image/Option/";					// 画像ファイルパス
	const std::string kNumberPath = "Number/";								// 数字画像ファイルパス

	const std::string kBaseBoxPath = kOptionPath + "BaseBox.png";			// ボックス画像ファイルパス
	const std::string kMainTextPath = kOptionPath + "MainText.png";			// テキスト画像ファイルパス
	const std::string kBGMPath = kOptionPath + "BGM.png";					// BGM画像ファイルパス
	const std::string kSEPath = kOptionPath + "SE.png";						// SE画像ファイルパス
	const std::string kBackSelectPath = kOptionPath + "BackSelect.png";		// セレクトに戻る画像ファイルパス
	const std::string kBackStagePath = kOptionPath + "BackGame.png";		// ゲームに戻る画像ファイルパス
	const std::string kGearPath = kOptionPath + "Gear.png";					// 装備画像ファイルパス
	const std::string kStatusBoxPath = kOptionPath + "Status.png";			// ステータスボックスファイルパス
	const std::string kLifePath = kOptionPath + "Life.png";					// ライフ画像ファイルパス
	const std::string kPowerPath = kOptionPath + "Power.png";				// 攻撃力画像ファイルパス
	const std::string kShotPath = kOptionPath + "Shot.png";					// 射撃力画像ファイルパス
	const std::string kGuardPath = kOptionPath + "Guard.png";				// 防御力画像ファイルパス
	const std::string kLeftCursorPath = kOptionPath + "LeftCursor.png";		// カーソル画像ファイルパス
	const std::string kRightCursorPath = kOptionPath + "RightCursor.png";	// カーソル画像ファイルパス
	const std::string kVolumeArrowPath = kOptionPath + "VolumeArrow.png";	// 音量矢印画像ファイルパス
	const std::string kOffVolumePath = kOptionPath + "OffVolume.png";		// 音量OFF画像ファイルパス
	const std::string kOnVolumePath = kOptionPath + "OnVolume.png";			// 音量ON画像ファイルパス
	const std::string kLBPath = kOptionPath + "LB.png";						// LBボタン画像ファイルパス
	const std::string kRBPath = kOptionPath + "RB.png";						// RBボタン画像ファイルパス

	constexpr int kBaseBoxPosX = 150;										// ボックス座標X
	constexpr int kBaseBoxPosY = 100;										// ボックス座標Y
	constexpr int kMainTextPosX = 880;										// テキスト座標X
	constexpr int kMainTextPosY = 10;										// テキスト座標Y

	constexpr int kLeftBoxSpace = 200;										// 小ボックス間隔
	constexpr int kLeftBoxPosX = 420;										// 小ボックス座標X
	constexpr int kBGMBoxPosY = 180;										// BGMボックス座標Y
	constexpr int kSEBoxPosY = kBGMBoxPosY + kLeftBoxSpace;					// SEボックス座標Y
	constexpr int kBackSelectBoxPosY = kSEBoxPosY + kLeftBoxSpace;			// セレクトに戻るボックス座標Y
	constexpr int kBackGameBoxPosY = kBackSelectBoxPosY + kLeftBoxSpace;	// ゲームに戻るボックス座標
	constexpr int kLeftArrowPosX = kLeftBoxPosX + 225;						// 左矢印座標X
	constexpr int kRightArrowPosX = kLeftArrowPosX + 405;					// 右矢印座標X
	constexpr int kBGMArrowPosY = kBGMBoxPosY + 25;							// BGM矢印座標Y
	constexpr int kSEArrowPosY = kSEBoxPosY + 25;							// SE矢印座標Y
	constexpr int kLeftCursorPosX = kLeftBoxPosX - 22;						// 左カーソル座標X
	constexpr int kCursorStartPosY = kBGMBoxPosY - 15;						// 左カーソルのスタート座標Y
	constexpr int kCursorSpace = 200;										// 左カーソル間隔

	constexpr int kGearBoxPosX = 1250;										// 装備ボックス座標X
	constexpr int kGearBoxPosY = 300;										// 装備ボックス座標Y
	constexpr int kRightCursorPosX = kGearBoxPosX - 15;						// 右カーソル座標X	
	constexpr int kRightCursorPosY = kGearBoxPosY - 15;						// 右カーソル座標Y
	constexpr int kStatusBoxPosX = 1200;									// ステータスボックス座標X
	constexpr int kStatusBoxPosY = kGearBoxPosY + 200;						// ステータスボックス座標Y
	constexpr int kStatusTextPosX = kStatusBoxPosX + 50;					// ステータステキスト座標X
	constexpr int kStatusTextSpaceY = 70;									// ステータステキスト間隔Y
	constexpr int kLifePosY = kStatusBoxPosY + 40;							// ライフテキスト座標Y
	constexpr int kPowerPosY = kLifePosY + kStatusTextSpaceY;				// 攻撃力テキスト座標Y
	constexpr int kShotPosY = kPowerPosY + kStatusTextSpaceY;				// 遠距離攻撃力テキスト座標Y
	constexpr int kGuardPosY = kShotPosY + kStatusTextSpaceY;				// 防御力テキスト座標Y
	constexpr int kStatusNumSpaseX = 40;									// ステータスの数値の間隔
	constexpr int kThirdStatusNumPosX = kStatusTextPosX + 250;				// ステータス百の位座標X
	constexpr int kSecondStatusNumPosX = kThirdStatusNumPosX + kStatusNumSpaseX;	// ステータス十の位座標X
	constexpr int kFirstStatusNumPosX = kSecondStatusNumPosX + kStatusNumSpaseX;	// ステータス一の位座標X

	constexpr int kStartVolumePosX = kLeftArrowPosX + 70;					// 音量初期座標X
	constexpr int kVolumeSpace = 70;										// 音量間隔
	constexpr int kBGMVolumePosY = kBGMArrowPosY;							// BGM音量座標Y
	constexpr int kSEVolumePosY = kSEArrowPosY;								// SE音量座標Y
	constexpr int kLBPosX = 645;											// RBボタン座標X
	constexpr int kRBPosX = kLBPosX + 435;									// LBボタン座標X
	constexpr int kBGMInfoPosY = 200;										// BGM操作説明ボタン座標Y
	constexpr int kSEInfoPosY = 400;										// SE操作説明ボタン座標Y

	constexpr int kPrevStageCountMax = 4;		// 前のシーンがステージシーンだった場合の最大カーソルカウント数
	constexpr int kPrevSelectCountMax = 3;		// 前のシーンがセレクトシーンだった場合の最大カーソルカウント数
	constexpr int kMaxVol = 5;					// 最大音量数
}

SceneOption::SceneOption(std::shared_ptr<SceneBase> prevScene) :
	m_cursorCount(0),
	m_isSceneEnd(false),
	m_isLeft(true),
	m_pGear(nullptr),
	m_prevScene(prevScene),
	m_nextScene(nullptr)
{
	// 画像ロード
	ImgLoad();

	// 音量設定
	m_bgmVol = SoundSave::GetInstance().GetBGMVol();
	m_seVol = SoundSave::GetInstance().GetSEVol();

	// プレイヤーのステータス取得
	SetStatus();
}

SceneOption::SceneOption(std::shared_ptr<SceneBase> prevScene, std::shared_ptr<Gear> pGear) :
	m_cursorCount(0),
	m_isSceneEnd(false),
	m_isLeft(true),
	m_pGear(pGear),
	m_prevScene(prevScene),
	m_nextScene(nullptr)
{
	// 画像ロード
	ImgLoad();

	// 音量設定
	m_bgmVol = SoundSave::GetInstance().GetBGMVol();
	m_seVol = SoundSave::GetInstance().GetSEVol();

	// プレイヤーのステータス取得
	SetStatus();
}

SceneOption::~SceneOption()
{
	// 画像デリート
	ImgDelete();
	// サウンドデータの保存
	SoundSave::GetInstance().Write();
}

void SceneOption::Init()
{
}

std::shared_ptr<SceneBase> SceneOption::Update()
{
	m_nextScene = shared_from_this();

	// 前のシーンがステージシーンだった場合
	if (m_prevScene->GetSceneKind() == SceneKind::Stage)
	{
		// 左右ボタンが押されたらカーソルが左右に動く
		if (Pad::IsTrigger(PAD_INPUT_LEFT) || Pad::IsTrigger(PAD_INPUT_RIGHT))
		{
			// SEを鳴らす
			SoundManager::GetInstance().Play("CursorMove", true);
			m_isLeft = !m_isLeft;
		}
	}

	// カーソル更新
	UpdateCursor();

	// Bボタンが押された場合画面を閉じる
	if (Pad::IsTrigger(PAD_INPUT_2))
	{
		// SEを鳴らす
		SoundManager::GetInstance().Play("Back", true);

		m_isSceneEnd = true;
	}

	// シーンの終了フラグが立っていたら画面を閉じる
	if (m_isSceneEnd)
	{
		m_nextScene = m_prevScene;
	}

	return m_nextScene;
}

void SceneOption::Draw()
{
	// 前のシーンの画面の描画
	m_prevScene->Draw();
	// 前のシーンの画面を暗くする
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, kPrevSceneAlpha);
	DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 大ボックス描画
	DrawGraph(kBaseBoxPosX, kBaseBoxPosY, m_baseBoxH, true);

	// 前のシーンによって描画するものを変える
	if (m_prevScene->GetSceneKind() == SceneKind::Select)
	{
		DrawSelect();
	}
	else if (m_prevScene->GetSceneKind() == SceneKind::Stage)
	{
		DrawStage();
	}

	// ステータスボックス
	DrawGraph(kStatusBoxPosX, kStatusBoxPosY, m_statusBoxH, true);
	// ステータス
	DrawGraph(kStatusTextPosX, kLifePosY, m_lifeH, true);
	DrawGraph(kStatusTextPosX, kPowerPosY, m_powerH, true);
	DrawGraph(kStatusTextPosX, kShotPosY, m_shotH, true);
	DrawGraph(kStatusTextPosX, kGuardPosY, m_guardH, true);
	DrawStatusNum();

	// 音量描画
	DrawVol();


	// カーソル描画
	if (m_isLeft)
	{
		DrawGraph(kLeftCursorPosX, kCursorStartPosY + (kCursorSpace * m_cursorCount),
		m_leftCursorH, true);
	}
	else
	{
		DrawGraph(kRightCursorPosX, kRightCursorPosY, m_rightCursorH, true);
	}
	


	// テキスト描画
	DrawGraph(kMainTextPosX, kMainTextPosY, m_mainTextH, true);

}

void SceneOption::End()
{
}

void SceneOption::SetStatus()
{
	CsvLoad::GetInstance().StatusLoad(m_statusData, "Player");
	CsvLoad::GetInstance().AddStatusLoad(m_statusData, "Player");
}

void SceneOption::SettingBGM()
{
	// RBが押されたら音量を上げる
	if (Pad::IsTrigger(PAD_INPUT_6))
	{
		m_bgmVol++;
		// 音量が最大だった場合はそれ以上上がらないようにする
		if (m_bgmVol >= kMaxVol) m_bgmVol = kMaxVol;

		// 情報の書き込み
		SoundSave::GetInstance().ChangeBGMVol(m_bgmVol);
		// 音量変更
		SoundManager::GetInstance().ChangeBGMVolume();
		// SEを鳴らす
		SoundManager::GetInstance().Play("VolumeChange", true);
	}

	// LBが押されたら音量を下げる
	if (Pad::IsTrigger(PAD_INPUT_5))
	{
		m_bgmVol--;
		// 音量が最大だった場合はそれ以上上がらないようにする
		if (m_bgmVol <= 0) m_bgmVol = 0;

		// 情報の書き込み
		SoundSave::GetInstance().ChangeBGMVol(m_bgmVol);
		// 音量変更
		SoundManager::GetInstance().ChangeBGMVolume();
		// SEを鳴らす
		SoundManager::GetInstance().Play("VolumeChange", true);
	}
}

void SceneOption::SettingSE()
{
	// RBが押されたら音量を上げる
	if (Pad::IsTrigger(PAD_INPUT_6))
	{
		m_seVol++;
		// 音量が最大だった場合はそれ以上上がらないようにする
		if (m_seVol >= kMaxVol) m_seVol = kMaxVol;

		// 情報の書き込み
		SoundSave::GetInstance().ChangeSEVol(m_seVol);
		// 音量変更
		SoundManager::GetInstance().ChangeSEVolume();
		// SEを鳴らす
		SoundManager::GetInstance().Play("VolumeChange", true);
	}

	// LBが押されたら音量を下げる
	if (Pad::IsTrigger(PAD_INPUT_5))
	{
		m_seVol--;
		// 音量が最大だった場合はそれ以上上がらないようにする
		if (m_seVol <= 0) m_seVol = 0;

		// 情報の書き込み
		SoundSave::GetInstance().ChangeSEVol(m_seVol);
		// 音量変更
		SoundManager::GetInstance().ChangeSEVolume();
		// SEを鳴らす
		SoundManager::GetInstance().Play("VolumeChange", true);
	}
}

void SceneOption::UpdateCursor()
{
	if (m_isLeft)
	{
		// 上ボタンが押されたらカーソルカウントを減らす
		if (Pad::IsTrigger(PAD_INPUT_UP))
		{
			m_cursorCount--;
			// SEを鳴らす
			SoundManager::GetInstance().Play("CursorMove", true);
		}
		// 下ボタンが押されたらカウントを増やす
		if (Pad::IsTrigger(PAD_INPUT_DOWN))
		{
			m_cursorCount++;
			// SEを鳴らす
			SoundManager::GetInstance().Play("CursorMove", true);
		}

		// カウント数が1だった場合
		if (m_cursorCount == 0)
		{
			// BGMの音量変更
			SettingBGM();
		}
		else if (m_cursorCount == 1)
		{
			// SEの音量変更
			SettingSE();
		}
		else if (m_cursorCount == 2)
		{
			// 前のシーンによって処理を変える
			if (m_prevScene->GetSceneKind() == SceneKind::Select)
			{
				// Aボタンが押された場合画面を閉じる
				if (Pad::IsTrigger(PAD_INPUT_1))
				{
					// SEを鳴らす
					SoundManager::GetInstance().Play("Back", true);

					m_isSceneEnd = true;
				}
			}
			else if (m_prevScene->GetSceneKind() == SceneKind::Stage)
			{
				// Aボタンが押された場合はカウント数によって処理を変える
				if (Pad::IsTrigger(PAD_INPUT_1))
				{
					// SEを鳴らす
					SoundManager::GetInstance().Play("Back", true);

					m_nextScene = std::make_shared<SceneSelect>();
				}
			}
		}
		else if (m_cursorCount == 3)
		{
			// Aボタンが押された場合画面を閉じる
			if (Pad::IsTrigger(PAD_INPUT_1))
			{
				// SEを鳴らす
				SoundManager::GetInstance().Play("Back", true);

				m_isSceneEnd = true;
			}
		}

		// カーソルをループさせる
		if (m_prevScene->GetSceneKind() == SceneKind::Select)
		{
			if (m_cursorCount >= kPrevSelectCountMax) m_cursorCount = 0;
			if (m_cursorCount < 0) m_cursorCount = kPrevSelectCountMax - 1;
		}
		else if (m_prevScene->GetSceneKind() == SceneKind::Stage)
		{
			// カーソルをループさせる
			if (m_cursorCount >= kPrevStageCountMax) m_cursorCount = 0;
			if (m_cursorCount < 0) m_cursorCount = kPrevStageCountMax - 1;
		}
	}
	else
	{
		// Aボタンが押された場合
		if (Pad::IsTrigger(PAD_INPUT_1))
		{
			// 前のシーンがステージシーンだった時のみ
			if (m_prevScene->GetSceneKind() == SceneKind::Stage)
			{
				// 装備画面を開く
				m_nextScene = std::make_shared<SceneGear>(shared_from_this(), m_prevScene, m_pGear);
				// SEを鳴らす
				SoundManager::GetInstance().Play("SelectGear", true);
			}
		}
	}
}

void SceneOption::DrawSelect()
{
	// BGM
	DrawGraph(kLeftBoxPosX, kBGMBoxPosY, m_bgmH, true);
	DrawGraph(kRightArrowPosX, kBGMArrowPosY, m_volumeArrowH, true);
	DrawTurnGraph(kLeftArrowPosX, kBGMArrowPosY, m_volumeArrowH, true);
	DrawGraph(kLBPosX, kBGMInfoPosY, m_LBH, true);
	DrawGraph(kRBPosX, kBGMInfoPosY, m_RBH, true);
	// SE
	DrawGraph(kLeftBoxPosX, kSEBoxPosY, m_seH, true);
	DrawGraph(kRightArrowPosX, kSEArrowPosY, m_volumeArrowH, true);
	DrawTurnGraph(kLeftArrowPosX, kSEArrowPosY, m_volumeArrowH, true);
	DrawGraph(kLBPosX, kSEInfoPosY, m_LBH, true);
	DrawGraph(kRBPosX, kSEInfoPosY, m_RBH, true);
	// セレクトに戻る
	DrawGraph(kLeftBoxPosX, kBackSelectBoxPosY, m_backSelectH, true);
}

void SceneOption::DrawStage()
{
	// BGM
	DrawGraph(kLeftBoxPosX, kBGMBoxPosY, m_bgmH, true);
	DrawGraph(kRightArrowPosX, kBGMArrowPosY, m_volumeArrowH, true);
	DrawTurnGraph(kLeftArrowPosX, kBGMArrowPosY, m_volumeArrowH, true);
	DrawGraph(kLBPosX, kBGMInfoPosY, m_LBH, true);
	DrawGraph(kRBPosX, kBGMInfoPosY, m_RBH, true);
	// SE
	DrawGraph(kLeftBoxPosX, kSEBoxPosY, m_seH, true);
	DrawGraph(kRightArrowPosX, kSEArrowPosY, m_volumeArrowH, true);
	DrawTurnGraph(kLeftArrowPosX, kSEArrowPosY, m_volumeArrowH, true);
	DrawGraph(kLBPosX, kSEInfoPosY, m_LBH, true);
	DrawGraph(kRBPosX, kSEInfoPosY, m_RBH, true);
	// セレクトに戻る
	DrawGraph(kLeftBoxPosX, kBackSelectBoxPosY, m_backSelectH, true);
	// ゲームに戻る
	DrawGraph(kLeftBoxPosX, kBackGameBoxPosY, m_backStageH, true);
	// 装備
	DrawGraph(kGearBoxPosX, kGearBoxPosY, m_gearH, true);

}

void SceneOption::DrawVol()
{
	// 音量描画
	// BGM
	for (int i = 0; i < kMaxVol; i++)
	{
		// 音量の数だけONの画像を出す
		// 音量以上になったらOFFの画像を出す
		if (i < m_bgmVol)
		{
			DrawGraph(kStartVolumePosX + (kVolumeSpace * i), kBGMVolumePosY,
			m_onVolumeH, true);
		}
		else
		{
			DrawGraph(kStartVolumePosX + (kVolumeSpace * i), kBGMVolumePosY,
			m_offVolumeH, true);
		}
	}
	// SE
	for (int i = 0; i < kMaxVol; i++)
	{
		// 音量の数だけONの画像を出す
		// 音量以上になったらOFFの画像を出す
		if (i < m_seVol)
		{
			DrawGraph(kStartVolumePosX + (kVolumeSpace * i), kSEVolumePosY,
			m_onVolumeH, true);
		}
		else
		{
			DrawGraph(kStartVolumePosX + (kVolumeSpace * i), kSEVolumePosY,
			m_offVolumeH, true);
		}
	}
}

void SceneOption::DrawStatusNum()
{
	// ライフ
	int hp = m_statusData.hp;
	int first = hp % 10;	// 一の位
	int second = (hp / 10) % 10;	// 十の位
	int third = ((hp / 10) / 10) % 10;	// 百の位
	// 百の位
	DrawGraph(kThirdStatusNumPosX, kLifePosY, m_numH[third], true);
	// 十の位
	DrawGraph(kSecondStatusNumPosX, kLifePosY, m_numH[second], true);
	// 一の位
	DrawGraph(kFirstStatusNumPosX, kLifePosY, m_numH[first], true);

	// 攻撃力
	int power = m_statusData.meleeAtk;
	first = power % 10;	// 一の位
	second = (power / 10) % 10;	// 十の位
	third = ((power / 10) / 10) % 10;	// 百の位
	// 百の位
	DrawGraph(kThirdStatusNumPosX, kPowerPosY, m_numH[third], true);
	// 十の位
	DrawGraph(kSecondStatusNumPosX, kPowerPosY, m_numH[second], true);
	// 一の位
	DrawGraph(kFirstStatusNumPosX, kPowerPosY, m_numH[first], true);

	// 射撃力
	int shot = m_statusData.shotAtk;
	first = shot % 10;	// 一の位
	second = (shot / 10) % 10;	// 十の位
	third = ((shot / 10) / 10) % 10;	// 百の位
	// 百の位
	DrawGraph(kThirdStatusNumPosX, kShotPosY, m_numH[third], true);
	// 十の位
	DrawGraph(kSecondStatusNumPosX, kShotPosY, m_numH[second], true);
	// 一の位
	DrawGraph(kFirstStatusNumPosX, kShotPosY, m_numH[first], true);

	// 防御
	int guard = m_statusData.def;
	first = guard % 10;	// 一の位
	second = (guard / 10) % 10;	// 十の位
	third = ((guard / 10) / 10) % 10;	// 百の位
	// 百の位
	DrawGraph(kThirdStatusNumPosX, kGuardPosY, m_numH[third], true);
	// 十の位
	DrawGraph(kSecondStatusNumPosX, kGuardPosY, m_numH[second], true);
	// 一の位
	DrawGraph(kFirstStatusNumPosX, kGuardPosY, m_numH[first], true);
}

void SceneOption::ImgLoad()
{
	std::string str;
	/*画像ロード*/
	for (int i = 0; i < kNum; i++)
	{
		str = kOptionPath + kNumberPath + std::to_string(i) + ".png";
		m_numH.push_back(LoadGraph(str.c_str()));
		// ロードに失敗したら止める
		assert(m_numH[i]);
	}
	m_baseBoxH = LoadGraph(kBaseBoxPath.c_str());
	assert(m_baseBoxH != -1);
	m_mainTextH = LoadGraph(kMainTextPath.c_str());
	assert(m_mainTextH != -1);
	m_bgmH = LoadGraph(kBGMPath.c_str());
	assert(m_bgmH != -1);
	m_seH = LoadGraph(kSEPath.c_str());
	assert(m_seH != -1);
	m_backSelectH = LoadGraph(kBackSelectPath.c_str());
	assert(m_backSelectH != -1);
	m_backStageH = LoadGraph(kBackStagePath.c_str());
	assert(m_backStageH != -1);
	m_gearH = LoadGraph(kGearPath.c_str());
	assert(m_gearH != -1);
	m_statusBoxH = LoadGraph(kStatusBoxPath.c_str());
	assert(m_statusBoxH != -1);
	m_lifeH = LoadGraph(kLifePath.c_str());
	assert(m_lifeH != -1);
	m_powerH = LoadGraph(kPowerPath.c_str());
	assert(m_powerH != -1);
	m_shotH = LoadGraph(kShotPath.c_str());
	assert(m_shotH != -1);
	m_guardH = LoadGraph(kGuardPath.c_str());
	assert(m_guardH != -1);
	m_leftCursorH = LoadGraph(kLeftCursorPath.c_str());
	assert(m_leftCursorH != -1);
	m_rightCursorH = LoadGraph(kRightCursorPath.c_str());
	assert(m_rightCursorH != -1);
	m_volumeArrowH = LoadGraph(kVolumeArrowPath.c_str());
	assert(m_volumeArrowH != -1);
	m_offVolumeH = LoadGraph(kOffVolumePath.c_str());
	assert(m_offVolumeH != -1);
	m_onVolumeH = LoadGraph(kOnVolumePath.c_str());
	assert(m_onVolumeH != -1);
	m_LBH = LoadGraph(kLBPath.c_str());
	assert(m_LBH != -1);
	m_RBH = LoadGraph(kRBPath.c_str());
	assert(m_RBH != -1);
}

void SceneOption::ImgDelete()
{
	/*画像デリート*/
	DeleteGraph(m_baseBoxH);
	DeleteGraph(m_mainTextH);
	DeleteGraph(m_bgmH);
	DeleteGraph(m_seH);
	DeleteGraph(m_backSelectH);
	DeleteGraph(m_backStageH);
	DeleteGraph(m_gearH);
	DeleteGraph(m_statusBoxH);
	DeleteGraph(m_lifeH);
	DeleteGraph(m_powerH);
	DeleteGraph(m_shotH);
	DeleteGraph(m_guardH);
	DeleteGraph(m_leftCursorH);
	DeleteGraph(m_rightCursorH);
	DeleteGraph(m_volumeArrowH);
	DeleteGraph(m_offVolumeH);
	DeleteGraph(m_onVolumeH);
	DeleteGraph(m_LBH);
	DeleteGraph(m_RBH);
}

#include "SceneOption.h"

#include "SceneSelect.h"

#include "../Common/SoundSave.h"

#include "../Utility/Game.h"
#include "../Utility/Pad.h"
#include "../Utility/SoundManager.h"

#include <string>
#include <assert.h>

namespace
{
	constexpr int kPrevSceneAlpha = 180;	// 前のシーンを暗くする不透明度

	/*ファイルパス関係*/
	const std::string kOptionPath = "Data/Image/Option/";			// 画像ファイルパス

	const std::string kBaseBoxPath = kOptionPath + "BaseBox.png";			// ボックス画像ファイルパス
	const std::string kNormalBoxPath = kOptionPath + "NormalBox.png";		// 小ボックス画像ファイルパス
	const std::string kNormalBox2Path = kOptionPath + "NormalBox2.png";		// 小ボックス画像ファイルパス
	const std::string kMainTextPath = kOptionPath + "MainText.png";			// テキスト画像ファイルパス
	const std::string kBGMTextPath = kOptionPath + "BGM.png";				// BGMテキスト画像ファイルパス
	const std::string kSETextPath = kOptionPath + "SE.png";					// SEテキスト画像ファイルパス
	const std::string kBackSelectTextPath = kOptionPath + "BackSelect.png";	// セレクトに戻るテキスト画像ファイルパス
	const std::string kBackStageTextPath = kOptionPath + "BackGame.png";	// ゲームに戻るテキスト画像ファイルパス
	const std::string kCursorPath = kOptionPath + "Cursor.png";				// カーソル画像ファイルパス
	const std::string kVolumeArrowPath = kOptionPath + "VolumeArrow.png";	// 音量矢印画像ファイルパス
	const std::string kOffVolumePath = kOptionPath + "OffVolume.png";		// 音量OFF画像ファイルパス
	const std::string kOnVolumePath = kOptionPath + "OnVolume.png";			// 音量ON画像ファイルパス

	constexpr int kBaseBoxPosX = 125;										// ボックス座標X
	constexpr int kBaseBoxPosY = 170;										// ボックス座標Y
	constexpr int kNormalBoxSpace = 200;									// 小ボックス間隔
	constexpr int kNormalBoxPosX = 180;										// 小ボックス座標X
	constexpr int kBGMBoxPosY = 230;										// BGMボックス座標Y
	constexpr int kSEBoxPosY = kBGMBoxPosY + kNormalBoxSpace;				// SEボックス座標Y
	constexpr int kBackSelectBoxPosY = kSEBoxPosY + kNormalBoxSpace;		// セレクトに戻るボックス座標Y
	constexpr int kBackGameBoxPosY = kBackSelectBoxPosY + kNormalBoxSpace;	// ゲームに戻るボックス座標
	constexpr int kCursorPosX = 169;										// カーソル座標X
	constexpr int kCursorStartPosY = 220;									// カーソルのスタート座標Y
	constexpr int kCursorSpace = 200;										// カーソル間隔
	constexpr int kMainTextPosX = 720;										// テキスト座標X
	constexpr int kMainTextPosY = 20;										// テキスト座標Y
	constexpr int kTextSpace = 200;											// テキスト間隔
	constexpr int kBGMTextPosX = 205;										// BGMテキスト座標X
	constexpr int kBGMTextPosY = 270;										// BGMテキスト座標Y
	constexpr int kSETextPosX = 240;										// SEテキスト座標X
	constexpr int kSETextPosY = kBGMTextPosY + kTextSpace;					// SEテキスト座標Y
	constexpr int kBackSelectTextPosX = 300;								// セレクトに戻るテキスト座標X
	constexpr int kBackSelectTextPosY = kSETextPosY + kTextSpace;			// セレクトに戻るテキスト座標Y
	constexpr int kBackStageTextPosX = 350;									// ゲームに戻るテキスト座標X
	constexpr int kBackStageTextPosY = kBackSelectTextPosY + kTextSpace;	// ゲームに戻るテキスト座標Y
	constexpr int kLeftArrowPosX = 410;										// 左矢印座標X
	constexpr int kRightArrowPosX = kLeftArrowPosX + 510;					// 右矢印座標X
	constexpr int kBGMArrowPosY = kBGMTextPosY - 10;						// BGM矢印座標Y
	constexpr int kSEArrowPosY = kSETextPosY - 10;							// SE矢印座標Y

	constexpr int kStartVolumePosX = 500;									// 音量座標X
	constexpr int kVolumeSpace = 80;										// 音量間隔
	constexpr int kBGMVolumePosY = kBGMArrowPosY - 15;						// BGM音量座標Y
	constexpr int kSEVolumePosY = kSEArrowPosY - 15;						// SE音量座標Y

	constexpr int kPrevStageCountMax = 4;		// 前のシーンがステージシーンだった場合の最大カーソルカウント数
	constexpr int kPrevSelectCountMax = 3;		// 前のシーンがセレクトシーンだった場合の最大カーソルカウント数
	constexpr int kMaxVol = 5;					// 最大音量数
}

SceneOption::SceneOption(std::shared_ptr<SceneBase> prevScene) :
	m_cursorCount(0),
	m_isSceneEnd(false),
	m_prevScene(prevScene),
	m_nextScene(nullptr)
{
	/*画像ロード*/
	m_baseBoxH = LoadGraph(kBaseBoxPath.c_str());
	// ロードに失敗したら止める
	assert(m_baseBoxH != -1);
	m_normalBoxH = LoadGraph(kNormalBoxPath.c_str());
	assert(m_normalBoxH != -1);
	m_normalBox2H = LoadGraph(kNormalBox2Path.c_str());
	assert(m_normalBox2H != -1);
	m_mainTextH = LoadGraph(kMainTextPath.c_str());
	assert(m_mainTextH != -1);
	m_bgmTextH = LoadGraph(kBGMTextPath.c_str());
	assert(m_bgmTextH != -1);
	m_seTextH = LoadGraph(kSETextPath.c_str());
	assert(m_seTextH != -1);
	m_backSelectTextH = LoadGraph(kBackSelectTextPath.c_str());
	assert(m_backSelectTextH != -1);
	m_backStageTextH = LoadGraph(kBackStageTextPath.c_str());
	assert(m_backStageTextH != -1);
	m_cursorH = LoadGraph(kCursorPath.c_str());
	assert(m_cursorH != -1);
	m_volumeArrowH = LoadGraph(kVolumeArrowPath.c_str());
	assert(m_volumeArrowH != -1);
	m_offVolumeH = LoadGraph(kOffVolumePath.c_str());
	assert(m_offVolumeH != -1);
	m_onVolumeH = LoadGraph(kOnVolumePath.c_str());
	assert(m_onVolumeH != -1);

	// 音量設定
	m_bgmVol = SoundSave::GetInstance().GetBGMVol();
	m_seVol = SoundSave::GetInstance().GetSEVol();
}

SceneOption::~SceneOption()
{
	/*画像デリート*/
	DeleteGraph(m_baseBoxH);
	DeleteGraph(m_normalBoxH);
	DeleteGraph(m_normalBox2H);
	DeleteGraph(m_mainTextH);
	DeleteGraph(m_bgmTextH);
	DeleteGraph(m_seTextH);
	DeleteGraph(m_backSelectTextH);
	DeleteGraph(m_backStageTextH);
	DeleteGraph(m_cursorH);
	DeleteGraph(m_volumeArrowH);
	DeleteGraph(m_offVolumeH);
	DeleteGraph(m_onVolumeH);
}

void SceneOption::Init()
{
}

std::shared_ptr<SceneBase> SceneOption::Update()
{
	m_nextScene = shared_from_this();

	// カーソル更新
	UpdateCursor();

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

	// 音量描画
	DrawVol();


	// カーソル描画
	DrawGraph(kCursorPosX, kCursorStartPosY + (kCursorSpace * m_cursorCount),
		m_cursorH, true);


	// テキスト描画
	DrawGraph(kMainTextPosX, kMainTextPosY, m_mainTextH, true);

}

void SceneOption::End()
{
}

void SceneOption::SettingBGM()
{
	// 右が押されたら音量を上げる
	if (Pad::IsTrigger(PAD_INPUT_RIGHT))
	{
		m_bgmVol++;
		// 音量が最大だった場合はそれ以上上がらないようにする
		if (m_bgmVol >= kMaxVol) m_bgmVol = kMaxVol;

		// 情報の書き込み
		SoundSave::GetInstance().ChangeBGMVol(m_bgmVol);
		// 音量変更
		SoundManager::GetInstance().ChangeBGMVolume();
	}

	// 左が押されたら音量を下げる
	if (Pad::IsTrigger(PAD_INPUT_LEFT))
	{
		m_bgmVol--;
		// 音量が最大だった場合はそれ以上上がらないようにする
		if (m_bgmVol <= 0) m_bgmVol = 0;

		// 情報の書き込み
		SoundSave::GetInstance().ChangeBGMVol(m_bgmVol);
		// 音量変更
		SoundManager::GetInstance().ChangeBGMVolume();
	}
}

void SceneOption::SettingSE()
{
	// 右が押されたら音量を上げる
	if (Pad::IsTrigger(PAD_INPUT_RIGHT))
	{
		m_seVol++;
		// 音量が最大だった場合はそれ以上上がらないようにする
		if (m_seVol >= kMaxVol) m_seVol = kMaxVol;

		// 情報の書き込み
		SoundSave::GetInstance().ChangeSEVol(m_seVol);
		// 音量変更
		SoundManager::GetInstance().ChangeBGMVolume();
	}

	// 左が押されたら音量を下げる
	if (Pad::IsTrigger(PAD_INPUT_LEFT))
	{
		m_seVol--;
		// 音量が最大だった場合はそれ以上上がらないようにする
		if (m_seVol <= 0) m_seVol = 0;

		// 情報の書き込み
		SoundSave::GetInstance().ChangeSEVol(m_seVol);
		// 音量変更
		SoundManager::GetInstance().ChangeBGMVolume();
	}
}

void SceneOption::UpdateCursor()
{
	// 上ボタンが押されたらカーソルカウントを減らす
	if (Pad::IsTrigger(PAD_INPUT_UP)) m_cursorCount--;
	// 下ボタンが押されたらカウントを増やす
	if (Pad::IsTrigger(PAD_INPUT_DOWN)) m_cursorCount++;

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
				// データ保存
				SoundSave::GetInstance().Write();

				m_isSceneEnd = true;
			}
		}
		else if (m_prevScene->GetSceneKind() == SceneKind::Stage)
		{
			// Aボタンが押された場合はカウント数によって処理を変える
			if (Pad::IsTrigger(PAD_INPUT_1))
			{
				// データ保存
				SoundSave::GetInstance().Write();

				m_nextScene = std::make_shared<SceneSelect>();
			}
		}
	}
	else if (m_cursorCount == 3)
	{
		// Aボタンが押された場合画面を閉じる
		if (Pad::IsTrigger(PAD_INPUT_1))
		{
			// データ保存
			SoundSave::GetInstance().Write();

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

void SceneOption::DrawSelect()
{
	// BGM
	DrawGraph(kNormalBoxPosX, kBGMBoxPosY, m_normalBoxH, true);
	DrawGraph(kBGMTextPosX, kBGMTextPosY, m_bgmTextH, true);
	DrawGraph(kRightArrowPosX, kBGMArrowPosY, m_volumeArrowH, true);
	DrawTurnGraph(kLeftArrowPosX, kBGMArrowPosY, m_volumeArrowH, true);
	// SE
	DrawGraph(kNormalBoxPosX, kSEBoxPosY, m_normalBoxH, true);
	DrawGraph(kSETextPosX, kSETextPosY, m_seTextH, true);
	DrawGraph(kRightArrowPosX, kSEArrowPosY, m_volumeArrowH, true);
	DrawTurnGraph(kLeftArrowPosX, kSEArrowPosY, m_volumeArrowH, true);
	// セレクトに戻る
	DrawGraph(kNormalBoxPosX, kBackSelectBoxPosY, m_normalBox2H, true);
	DrawGraph(kBackSelectTextPosX, kBackSelectTextPosY, m_backSelectTextH, true);
}

void SceneOption::DrawStage()
{
	// BGM
	DrawGraph(kNormalBoxPosX, kBGMBoxPosY, m_normalBoxH, true);
	DrawGraph(kBGMTextPosX, kBGMTextPosY, m_bgmTextH, true);
	DrawGraph(kRightArrowPosX, kBGMArrowPosY, m_volumeArrowH, true);
	DrawTurnGraph(kLeftArrowPosX, kBGMArrowPosY, m_volumeArrowH, true);
	// SE
	DrawGraph(kNormalBoxPosX, kSEBoxPosY, m_normalBoxH, true);
	DrawGraph(kSETextPosX, kSETextPosY, m_seTextH, true);
	DrawGraph(kRightArrowPosX, kSEArrowPosY, m_volumeArrowH, true);
	DrawTurnGraph(kLeftArrowPosX, kSEArrowPosY, m_volumeArrowH, true);
	// セレクトに戻る
	DrawGraph(kNormalBoxPosX, kBackSelectBoxPosY, m_normalBox2H, true);
	DrawGraph(kBackSelectTextPosX, kBackSelectTextPosY, m_backSelectTextH, true);
	// ゲームに戻る
	DrawGraph(kNormalBoxPosX, kBackGameBoxPosY, m_normalBox2H, true);
	DrawGraph(kBackStageTextPosX, kBackStageTextPosY, m_backStageTextH, true);
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
#include "SceneSelect.h"

#include "SceneGear.h"
#include "SceneStage.h"
#include "SceneOption.h"
#include "SceneTitle.h"

#include "../Utility/Game.h"
#include "../Utility/Pad.h"
#include "../Utility/SoundManager.h"

#include <string>
#include <cassert>

namespace
{
	// 背景関係
	constexpr int kBackImgNum = 9;			// 背景画像数
	constexpr int kStopBackNum = 3;			// 動かない背景の数
	constexpr int kBackBackNum = 2;			// 後ろの背景の数
	constexpr int kMiddleBackNum = 2;		// 真ん中の背景の数
	constexpr int kFrontBackNum = 2;		// 手前の背景の数
	constexpr int kIndexBackNum = 2;		// スクロールに必要な背景数
	constexpr int kScrollSpeedBack = 1;		// スクロール速度奥
	constexpr int kScrollSpeedMiddle = 2;	// スクロール速度真ん中
	constexpr int kScrollSpeedFront = 3;	// スクロール速度手前

	constexpr int kMaxCursorCount = 2;		// カーソルカウント最大値
	constexpr int kStageNum = 1;			// ステージ数
	constexpr int kTextNum = 4;				// テキスト数

	// 画像座標関係
	constexpr int kStageSelectBoxPosX = static_cast<int>(Game::kScreenWidth * 0.28f);		// ステージ選択ボックス座標X
	constexpr int kStageSelectBoxPosY = static_cast<int>(Game::kScreenHeight * 0.32f);		// ステージ選択ボックス座標Y
	constexpr int kGearBoxPosX = static_cast<int>(Game::kScreenWidth * 0.76f);				// 装備ボックス座標X
	constexpr int kGearBoxPosY = static_cast<int>(Game::kScreenHeight * 0.18f);				// 装備ボックス座標Y
	constexpr int kOptionBoxPosX = static_cast<int>(Game::kScreenWidth * 0.65f);			// オプションボックス座標X
	constexpr int kOptionBoxPosY = static_cast<int>(Game::kScreenHeight * 0.47f);			// オプションボックス座標Y
	constexpr int kGameEndBoxPosX = static_cast<int>(Game::kScreenWidth * 0.87f);			// ゲーム終了ボックス座標X
	constexpr int kGameEndBoxPosY = static_cast<int>(Game::kScreenHeight * 0.47f);			// ゲーム終了ボックス座標Y
	constexpr int kDescriptionBoxPosX = static_cast<int>(Game::kScreenWidth * 0.02f);		// 説明文ボックス座標X
	constexpr int kDescriptionBoxPosY = static_cast<int>(Game::kScreenHeight * 0.65f);		// 説明文ボックス座標Y
	constexpr int kButtonPosX = static_cast<int>(Game::kScreenWidth * 0.8f);				// ボタン座標X
	constexpr int kSelectButtonPosY = static_cast<int>(Game::kScreenHeight * 0.65f);		// 選択ボタン座標Y
	constexpr int kDecisionButtonPosY = static_cast<int>(Game::kScreenHeight * 0.8f);		// 決定ボタン座標Y
	constexpr int kSelectBoxPosX = static_cast<int>(Game::kScreenWidth * 0.092f);			// ステージ選択ボックス座標X
	constexpr int kSelectBoxPosY = static_cast<int>(Game::kScreenHeight * 0.05f);			// ステージ選択ボックス座標Y
	constexpr int kLeftArrowPosX = static_cast<int>(Game::kScreenWidth * 0.005f);			// 左矢印座標X
	constexpr int kRightArrowPosX = static_cast<int>(Game::kScreenWidth * 0.9f);			// 右矢印座標X
	constexpr int kArrowPosY = static_cast<int>(Game::kScreenHeight * 0.39f);				// 矢印座標Y
	constexpr int kTextInfoPosX = static_cast<int>(Game::kScreenWidth * 0.25f);				// テキスト座標X
	constexpr int kTextInfoPosY = static_cast<int>(Game::kScreenWidth * 0.43f);				// テキスト座標Y

	constexpr float kNormalBoxExtRate = 1.0f;								// 通常時ボックス拡大率
	constexpr float kBoxSinSpeed = 0.07f;									// ボックス拡縮速度
	constexpr float kBoxAnimSwing = 0.03f;									// ボックス拡縮幅

	// ファイルパス関係
	const std::string kSelectPath = "Data/Image/Select/";		// 画像フォルダファイルパス
	// 背景画像パス
	const std::string kBackPath[kBackImgNum] =
	{
		"back1.png",
		"back2.png",
		"back3.png",
		"back4.png",
		"back5.png",
		"back6.png",
		"back7.png",
		"back8.png",
		"back9.png",
	};
	// テキスト画像パス
	const std::string kSelectInfoTextPath[kTextNum] =
	{
		"Select.png",
		"Gear.png",
		"Option.png",
		"GameEnd.png",
	};
	// 選択ボックス画像パス
	const std::string kSelectBoxPath[kStageNum] =
	{
		"Stage1.png",
	};
	const std::string kStageSelectBoxPath = kSelectPath + "Box/StageSelect.png";	// ステージ選択ボックスパス
	const std::string kGearBoxPath = kSelectPath + "Box/Gear.png";					// 装備ボックスパス
	const std::string kOptionBoxPath = kSelectPath + "Box/Option.png";				// オプションボックスパス
	const std::string kGameEndBoxPath = kSelectPath + "Box/GameEnd.png";			// ゲーム終了ボックスパス
	const std::string kDescriptionBoxPath = kSelectPath + "Box/Description.png";	// 説明文ボックスパス
	const std::string kSelectButtonPath = kSelectPath + "Box/Select.png";			// 選択ボタンパス
	const std::string kDecisionButtonPath = kSelectPath + "Box/Decision.png";		// 決定ボタンパス
	const std::string kArrowPath = kSelectPath + "StageSelect/Arrow.png";			// 矢印画像パス
}

SceneSelect::SceneSelect() :
	m_scrollXBack(0),
	m_scrollXMiddle(0),
	m_scrollXFront(0),
	m_cursorCount(0),
	m_boxSinCount(0.0),
	m_expansionBoxExtRate(0.0),
	m_stageSelectBoxExtRate(0.0),
	m_gearBoxExtRate(0.0),
	m_optionBoxExtRate(0.0),
	m_gameEndBoxExtRate(0.0),
	m_isCursorUp(true),
	m_sceneType(SceneType::Normal),
	m_nextScene(nullptr)
{
	// 画像ロード
	ImgLoad();
	// 背景画像サイズ取得
	GetGraphSize(m_backH[0], &m_backWidth, &m_backHeight);
}

SceneSelect::~SceneSelect()
{
	// 画像のデリート
	ImgDelete();
	// BGMを止める
	SoundManager::GetInstance().DesignationStopSound("SelectScene");
}

void SceneSelect::Init()
{
	// BGMの再生
	SoundManager::GetInstance().Play("SelectScene", false);
}

std::shared_ptr<SceneBase> SceneSelect::Update()
{
	// 次のシーンの設定(最初は自分のシーンを入れておく)
	m_nextScene = shared_from_this();

	switch (m_sceneType)
	{
	case SceneSelect::SceneType::Normal:
		// カーソル更新
		UpdateCursor();
		// ボックス更新
		UpdateBox();
		// Bボタンが押された場合はタイトルに戻る
		if (Pad::IsTrigger(PAD_INPUT_2))
		{
			m_nextScene = std::make_shared<SceneTitle>();
		}
		break;
	case SceneSelect::SceneType::StageSelect:
		UpdateStageSelect();
		break;
	default:
		break;
	}	

	// スクロール値の更新
	m_scrollXBack += kScrollSpeedBack;
	m_scrollXMiddle += kScrollSpeedMiddle;
	m_scrollXFront += kScrollSpeedFront;

	return m_nextScene;
}

void SceneSelect::Draw()
{
	// 背景描画
	DrawBack();

	switch (m_sceneType)
	{
	case SceneSelect::SceneType::Normal:
		// ボックス描画
		DrawCursorBox();
		// テキスト描画
		DrawTextInfo();
		break;
	case SceneSelect::SceneType::StageSelect:
		DrawStageSelect();
		break;
	default:
		break;
	}
}

void SceneSelect::End()
{
	/*処理無し*/
}

void SceneSelect::UpdateCursor()
{
	// 左ボタンが押されたらカウントを減らす
	if (Pad::IsTrigger(PAD_INPUT_LEFT))
	{
		m_cursorCount--;
		// サウンドを流す
		SoundManager::GetInstance().Play("CursorMove", true);
	}

	// 右ボタンが押されたらカウントを増やす
	else if (Pad::IsTrigger(PAD_INPUT_RIGHT))
	{
		m_cursorCount++;
		// サウンドを流す
		SoundManager::GetInstance().Play("CursorMove", true);
	}


	// カーソルをループさせる
	if (m_cursorCount > kMaxCursorCount) m_cursorCount = 0;
	if (m_cursorCount < 0) m_cursorCount = kMaxCursorCount;

	// 上下ボタンが押されたら上下を入れ替える
	if (Pad::IsTrigger(PAD_INPUT_UP) || Pad::IsTrigger(PAD_INPUT_DOWN))
	{
		m_isCursorUp = !m_isCursorUp;
		// カーソルカウントが0以外だった場合はサウンドを流す
		if (m_cursorCount != 0)
		{
			SoundManager::GetInstance().Play("CursorMove", true);
		}
	}
}

void SceneSelect::UpdateBox()
{
	// 選択中ボックス拡縮処理
	m_boxSinCount += kBoxSinSpeed;
	m_expansionBoxExtRate = sinf(m_boxSinCount) * kBoxAnimSwing;

	// カーソルカウントが0の場合はステージ選択ボックスを選択している
	if (m_cursorCount == 0)
	{
		m_stageSelectBoxExtRate = kNormalBoxExtRate + m_expansionBoxExtRate;
		m_gearBoxExtRate = kNormalBoxExtRate;
		m_optionBoxExtRate = kNormalBoxExtRate;
		m_gameEndBoxExtRate = kNormalBoxExtRate;

		m_isCursorUp = true;

		// Aボタンが押されたら選択画面に飛ぶ
		if (Pad::IsTrigger(PAD_INPUT_1))
		{
			// シーンタイプをステージ選択に変える
			m_sceneType = SceneType::StageSelect;
			// カーソルカウントの初期化
			m_cursorCount = 0;
			// SEを鳴らす
			SoundManager::GetInstance().Play("Select", true);
		}
	}
	else if (m_cursorCount == 1)
	{
		// カーソルが上だった場合は装備ボックスを選択している
		if (m_isCursorUp)
		{
			m_stageSelectBoxExtRate = kNormalBoxExtRate;
			m_gearBoxExtRate = kNormalBoxExtRate + m_expansionBoxExtRate;
			m_optionBoxExtRate = kNormalBoxExtRate;
			m_gameEndBoxExtRate = kNormalBoxExtRate;

			// Aボタンが押されたら装備シーンに飛ぶ
			if (Pad::IsTrigger(PAD_INPUT_1))
			{
				// SEを鳴らす
				SoundManager::GetInstance().Play("Select", true);
				m_nextScene = std::make_shared<SceneGear>(shared_from_this(), shared_from_this());
			}
		}
		// 下だった場合はオプションボックスを選択している
		else
		{
			m_stageSelectBoxExtRate = kNormalBoxExtRate;
			m_gearBoxExtRate = kNormalBoxExtRate;
			m_optionBoxExtRate = kNormalBoxExtRate + m_expansionBoxExtRate;
			m_gameEndBoxExtRate = kNormalBoxExtRate;

			// Aボタンが押されたらオプション画面を出す
			if (Pad::IsTrigger(PAD_INPUT_1))
			{
				// SEを鳴らす
				SoundManager::GetInstance().Play("Select", true);
				m_nextScene = std::make_shared<SceneOption>(shared_from_this());
			}

		}
	}
	else
	{
		// カーソルが上だった場合は装備ボックスを選択している
		if (m_isCursorUp)
		{
			m_stageSelectBoxExtRate = kNormalBoxExtRate;
			m_gearBoxExtRate = kNormalBoxExtRate + m_expansionBoxExtRate;
			m_optionBoxExtRate = kNormalBoxExtRate;
			m_gameEndBoxExtRate = kNormalBoxExtRate;

			// Aボタンが押されたら装備シーンに飛ぶ
			if (Pad::IsTrigger(PAD_INPUT_1))
			{
				// SEを鳴らす
				SoundManager::GetInstance().Play("Select", true);
				m_nextScene = std::make_shared<SceneGear>(shared_from_this(), shared_from_this());
			}
		}
		// 下だった場合はゲーム終了ボックスを選択している
		else
		{
			m_stageSelectBoxExtRate = kNormalBoxExtRate;
			m_gearBoxExtRate = kNormalBoxExtRate;
			m_optionBoxExtRate = kNormalBoxExtRate;
			m_gameEndBoxExtRate = kNormalBoxExtRate + m_expansionBoxExtRate;

			// Aボタンが押されたらゲーム終了
			if (Pad::IsTrigger(PAD_INPUT_1))
			{
				m_nextScene = nullptr;
			}
		}

		
	}
}

void SceneSelect::UpdateStageSelect()
{
	// Bボタンが押されたらシーンタイプを通常に変える
	if (Pad::IsTrigger(PAD_INPUT_2))
	{
		m_sceneType = SceneType::Normal;
		// カーソルカウントの初期化
		m_cursorCount = 0;
	}

	// 右が押されたらカウントを増やす
	if (Pad::IsTrigger(PAD_INPUT_RIGHT))
	{
		m_cursorCount++;
	}
	// 左が押されたらカウントを減らす
	else if (Pad::IsTrigger(PAD_INPUT_LEFT))
	{
		m_cursorCount--;
	}

	// カーソルをループさせる
	if (m_cursorCount > kStageNum - 1) m_cursorCount = 0;
	if (m_cursorCount < 0) m_cursorCount = kStageNum - 1;

	// Aボタンが押されたらステージシーンに飛ぶ
	if (Pad::IsTrigger(PAD_INPUT_1))
	{
		// BGMを止める
		SoundManager::GetInstance().DesignationStopSound("SelectScene");
		// SEを鳴らす
		SoundManager::GetInstance().Play("SelectSelect", true);
		m_nextScene = std::make_shared<SceneStage>(static_cast<Game::StageKind>(m_cursorCount + 1));
	}
}

void SceneSelect::DrawBack()
{
	// 動かない背景
	for (int i = 0; i < kStopBackNum; i++)
	{
		DrawGraph(0, 0, m_backH[i], true);
	}
	// 描画した背景数
	int drawBack = kStopBackNum;
	/*スクロール処理
	手前側に来るほどスクロールが早くなる*/
	// 後ろ
	int scroll = m_scrollXBack % m_backWidth;
	for (int i = 0; i < kBackBackNum; i++)
	{
		for (int index = 0; index < kIndexBackNum; index++)
		{
			DrawGraph(-scroll + index * m_backWidth,
				0, m_backH[drawBack + i], true);
		}
	}

	// 真ん中
	drawBack += kBackBackNum;
	scroll = m_scrollXMiddle % m_backWidth;
	for (int i = 0; i < kMiddleBackNum; i++)
	{
		for (int index = 0; index < kIndexBackNum; index++)
		{
			DrawGraph(-scroll + index * m_backWidth,
				0, m_backH[drawBack + i], true);
		}
	}

	// 手前
	drawBack += kMiddleBackNum;
	scroll = m_scrollXFront % m_backWidth;
	for (int i = 0; i < kFrontBackNum; i++)
	{
		for (int index = 0; index < kIndexBackNum; index++)
		{
			DrawGraph(-scroll + index * m_backWidth,
				0, m_backH[drawBack + i], true);
		}
	}
}

void SceneSelect::DrawCursorBox()
{
	// ステージ選択
	DrawRotaGraph(kStageSelectBoxPosX, kStageSelectBoxPosY,
		m_stageSelectBoxExtRate, 0.0,
		m_stageSelectBoxH, true);
	// 装備
	DrawRotaGraph(kGearBoxPosX, kGearBoxPosY,
		m_gearBoxExtRate, 0.0,
		m_gearBoxH, true);
	// オプション
	DrawRotaGraph(kOptionBoxPosX, kOptionBoxPosY,
		m_optionBoxExtRate, 0.0,
		m_optionBoxH, true);
	// ゲーム終了
	DrawRotaGraph(kGameEndBoxPosX, kGameEndBoxPosY,
		m_gameEndBoxExtRate, 0.0,
		m_gameEndBoxH, true);
	// 説明文
	DrawGraph(kDescriptionBoxPosX, kDescriptionBoxPosY,
		m_descriptionBoxH, true);
	// 選択ボタン
	DrawGraph(kButtonPosX, kSelectButtonPosY,
		m_selectButtonH, true);
	// 決定ボタン
	DrawGraph(kButtonPosX, kDecisionButtonPosY,
		m_decisionButtonH, true);
}

void SceneSelect::DrawTextInfo()
{
	// カーソルカウントが0の場合はステージ選択ボックスを選択している
	if (m_cursorCount == 0)
	{
		DrawGraph(kTextInfoPosX, kTextInfoPosY,
			m_textInfoH[0], true);
	}
	else if (m_cursorCount == 1)
	{
		// カーソルが上だった場合は装備ボックスを選択している
		if (m_isCursorUp)
		{
			DrawGraph(kTextInfoPosX, kTextInfoPosY,
				m_textInfoH[1], true);
		}
		// 下だった場合はオプションボックスを選択している
		else
		{
			DrawGraph(kTextInfoPosX, kTextInfoPosY,
				m_textInfoH[2], true);
		}
	}
	else
	{
		// カーソルが上だった場合は装備ボックスを選択している
		if (m_isCursorUp)
		{
			DrawGraph(kTextInfoPosX, kTextInfoPosY,
				m_textInfoH[1], true);
		}
		// 下だった場合はゲーム終了ボックスを選択している
		else
		{
			DrawGraph(kTextInfoPosX, kTextInfoPosY,
				m_textInfoH[3], true);
		}
	}
}

void SceneSelect::DrawStageSelect()
{
	// ステージ選択ボックス描画
	DrawGraph(kSelectBoxPosX, kSelectBoxPosY, m_selectBoxH[m_cursorCount], true);

	// 矢印描画
	DrawGraph(kRightArrowPosX, kArrowPosY, m_arrowH, true);
	DrawTurnGraph(kLeftArrowPosX, kArrowPosY, m_arrowH, true);
}

void SceneSelect::ImgLoad()
{
	// ループをする時にファイルパスを入れる用の文字列
	std::string path;
	// 背景
	for (int i = 0; i < kBackImgNum; i++)
	{
		// 画像ファイルパスの設定
		path = kSelectPath + "Background/" + kBackPath[i];
		// ハンドルのロード
		m_backH.push_back(LoadGraph(path.c_str()));
		// ロードに失敗したら止める
		assert(m_backH[i] != -1);
	}
	// 選択ボックス
	for (int i = 0; i < kStageNum; i++)
	{
		// 画像ファイルパスの設定
		path = kSelectPath + "StageSelect/" + kSelectBoxPath[i];
		// ハンドルのロード
		m_selectBoxH.push_back(LoadGraph(path.c_str()));
		assert(m_selectBoxH[i] != -1);
	}
	// テキスト
	for (int i = 0; i < kTextNum; i++)
	{
		// 画像ファイルパスの設定
		path = kSelectPath + "Text/" + kSelectInfoTextPath[i];
		// ハンドルのロード
		m_textInfoH.push_back(LoadGraph(path.c_str()));
		assert(m_textInfoH[i] != -1);
	}
	m_stageSelectBoxH = LoadGraph(kStageSelectBoxPath.c_str());
	assert(m_stageSelectBoxH != -1);
	m_gearBoxH = LoadGraph(kGearBoxPath.c_str());
	assert(m_gearBoxH != -1);
	m_optionBoxH = LoadGraph(kOptionBoxPath.c_str());
	assert(m_optionBoxH != -1);
	m_gameEndBoxH = LoadGraph(kGameEndBoxPath.c_str());
	assert(m_gameEndBoxH != -1);
	m_descriptionBoxH = LoadGraph(kDescriptionBoxPath.c_str());
	assert(m_decisionButtonH != -1);
	m_selectButtonH = LoadGraph(kSelectButtonPath.c_str());
	assert(m_selectButtonH != -1);
	m_decisionButtonH = LoadGraph(kDecisionButtonPath.c_str());
	assert(m_decisionButtonH != -1);
	m_arrowH = LoadGraph(kArrowPath.c_str());
	assert(m_arrowH != -1);
}

void SceneSelect::ImgDelete()
{
	for (auto& img : m_backH)
	{
		DeleteGraph(img);
	}
	for (auto& img : m_selectBoxH)
	{
		DeleteGraph(img);
	}
	for (auto& img : m_textInfoH)
	{
		DeleteGraph(img);
	}
	DeleteGraph(m_stageSelectBoxH);
	DeleteGraph(m_gearBoxH);
	DeleteGraph(m_optionBoxH);
	DeleteGraph(m_gameEndBoxH);
	DeleteGraph(m_descriptionBoxH);
	DeleteGraph(m_selectButtonH);
	DeleteGraph(m_decisionButtonH);
	DeleteGraph(m_arrowH);
}

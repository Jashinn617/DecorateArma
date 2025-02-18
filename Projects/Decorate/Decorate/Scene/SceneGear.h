#pragma once
#include "SceneBase.h"

#include "../Object/CharacterBase.h"

#include <vector>

class Gear;

/// <summary>
/// 装備画面
/// </summary>
class SceneGear : public SceneBase
{
public:	// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="prevScene">前のシーン</param>
	/// <param name="prevMainScene">前のメインのシーン</param>
	SceneGear(std::shared_ptr<SceneBase> prevScene, std::shared_ptr<SceneBase> prevMainScene);

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="prevScene">前のシーン</param>
	/// <param name="prevMainScene">前のメインのシーン</param>
	/// <param name="pGear">装備クラスポインタ</param>
	SceneGear(std::shared_ptr<SceneBase> prevScene, std::shared_ptr<SceneBase> prevMainScene,
		std::shared_ptr<Gear> pGear);

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~SceneGear();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() override final;

	/// <summary>
	/// 更新
	/// </summary>
	/// <returns>次のシーン</returns>
	virtual std::shared_ptr<SceneBase> Update() override final;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() override final;

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void End() override final;

	/// <summary>
	/// シーン種類の取得
	/// </summary>
	/// <returns>シーンの種類</returns>
	virtual SceneKind GetSceneKind() override final { return SceneKind::Gear; }

private:	// 関数
	/// <summary>
	/// 装備を付けるカーソル更新
	/// </summary>
	void UpdateEquipCursor();

	/// <summary>
	/// 装備を外すカーソル更新
	/// </summary>
	void UpdateRemoveCursor();

	/// <summary>
	/// コスト更新
	/// </summary>
	/// <returns>コスト数</returns>
	int UpdateCost();

	/// <summary>
	/// コスト描画
	/// </summary>
	void DrawCost();

	/// <summary>
	/// テキスト描画
	/// </summary>
	void DrawGearText();

	/// <summary>
	/// 装備品数描画
	/// </summary>
	void DrawGearNum();

	/// <summary>
	/// 装備中装備描画
	/// </summary>
	void DrawEquippedGear();

	/// <summary>
	/// カーソル描画
	/// </summary>
	void DrawCursor();

	/// <summary>
	/// 装備を付けるカーソル描画
	/// </summary>
	void DrawEquipCursor();

	/// <summary>
	/// 装備を外すカーソル描画
	/// </summary>
	void DrawRemoveCursor();

	/// <summary>
	/// ボックス描画
	/// </summary>
	void DrawBoxes();

	/// <summary>
	/// HP描画
	/// </summary>
	void DrawHp();

	/// <summary>
	/// 近接攻撃力描画
	/// </summary>
	void DrawMAtk();

	/// <summary>
	/// 遠距離攻撃力描画
	/// </summary>
	void DrawSAtk();

	/// <summary>
	/// 防御力描画
	/// </summary>
	void DrawDef();

	/// <summary>
	/// 説明画像描画
	/// </summary>
	void DrawInfo();

	/// <summary>
	/// 背景描画
	/// </summary>
	void DrawBack();

	/// <summary>
	/// 画像のロード
	/// </summary>
	void LoadImg();

	/// <summary>
	/// 画像のデリート
	/// </summary>
	void DeleteImg();

private:	// 変数
	int m_mulMarkH;							// ×マーク画像ハンドル
	int m_slashH;							// スラッシュ画像ハンドル
	int m_cursorH;							// カーソル画像ハンドル
	int m_removeCursorH;					// 装備を外す時カーソル画像ハンドル
	int m_gearBoxH;							// 装備品ボックス画像ハンドル
	int m_explainBoxH;						// 説明文ボックス画像ハンドル
	int m_costTextH;						// コストテキスト画像ハンドル
	int m_equippedBoxH;						// 装備中ボックス画像ハンドル
	int m_statusBoxH;						// ステータスボックス画像ハンドル
	int m_hpTextH;							// HPのテキスト画像ハンドル
	int m_mAtkTextH;						// 近接攻撃力のテキスト画像ハンドル
	int m_sAtkTextH;						// 遠距離攻撃のテキスト画像ハンドル
	int m_defTextH;							// 防御力のテキスト画像ハンドル
	int m_addH;								// プラス記号画像ハンドル
	int m_moveInfoH;						// 選択説明画像ハンドル
	int m_selectInfoH;						// 決定説明画像ハンドル
	int m_backInfoH;						// 戻る説明画像ハンドル
	int m_backgroundH;						// 背景画像ハンドル
	std::vector<int> m_gearH;				// 装備品名画像ハンドル
	std::vector<int> m_numH;				// 数字画像ハンドル
	std::vector<int> m_explainH;			// 説明文画像ハンドル
	std::vector<int> m_equippedUIH;			// 装備中装備UI画像

	int m_gearNumberCount;					// アイテム番号カウント
	int m_lineNumberCount;					// 計算用行番号カウント
	int m_lineCount;						// カーソル行カウント
	int m_removeNumberCount;				// 装備を外す用カウント
	int m_rowCount;							// カーソル列カウント
	int m_backWidth;						// 背景横幅
	int m_backHeight;						// 背景縦幅
	int m_scrollX;							// スクロール値

	std::shared_ptr<Gear> m_pGear;				// 装備品
	std::shared_ptr<SceneBase> m_prevScene;		// 前のシーン
	std::shared_ptr<SceneBase> m_prevMainScene;	// 前のシーンのメインになるシーン
	std::shared_ptr<SceneBase> m_nextScene;		// 次のシーン
	CharacterBase::StatusData m_statusData;		// プレイヤーのステータスデータ
};


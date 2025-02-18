#pragma once
#include "../Utility/CharacterData.h"

#include <memory>
#include <vector>

class Model;
class SetVertexShader;
class ObjectManager;
class CollisionShape;


class ToonShader;
class ShadowMapShader;

namespace ColInfo
{
	constexpr int kMaxColHitPolyNum = 2000;		// 最大当たり判定ポリゴン数
	constexpr int kMaxColHitTryNum = 16;		// 壁押し出し処理の最大試行回数
	constexpr float kColHitSlideLength = 1.0f;	// 壁押し出し時にスライドさせる距離
}

/// <summary>
/// オブジェクトの基底クラス
/// </summary>
class ObjectBase
{
public:	// 列挙型
	/// <summary>
	/// 当たり判定の種類
	/// </summary>
	enum class ColType
	{
		None,	// 無し
		Player,	// プレイヤー
		Enemy,	// エネミー
		Field,	// フィールド
		Item,	// アイテム
	};

public:	// 継承用関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ObjectBase();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~ObjectBase();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="pToonShader">トゥーンシェーダポインタ</param>
	virtual void Draw(std::shared_ptr<ToonShader> pToonShader) = 0;

	/// <summary>
	/// 終了処理
	/// </summary>
	void End();

	/// <summary>
	/// 影の描画
	/// </summary>
	/// <param name="pShadowMapShader">シャドウマップシェーダポインタ</param>
	virtual void ShadowMapDraw(std::shared_ptr<ShadowMapShader> pShadowMapShader);

	/// <summary>
	/// 2Dの描画
	/// </summary>
	virtual void Draw2D() {/*処理無し*/ };

	/// <summary>
	/// ステージクリア時処理
	/// </summary>
	virtual void StageClear() {/*処理無し*/ };

	/// <summary>
	/// 当たり判定用図形ポインタの取得
	/// </summary>
	/// <returns>当たり判定用図形ポインタ</returns>
	const std::shared_ptr<CollisionShape> GetCollShape()const { return m_pCollShape; }

	/// <summary>
	/// 自身の当たり判定の情報の取得
	/// </summary>
	/// <returns>当たり判定情報</returns>
	virtual ColType GetColType() const { return ColType::None; }

public:	// 継承用以外のパブリック関数

	/// <summary>
	/// オブジェクトマネージャーの設定
	/// </summary>
	/// <param name="objectManager">オブジェクトマネージャーポインタ</param>
	void SetObjectManager(ObjectManager* objectManager) { m_pObjectManager = objectManager; }

	/// <summary>
	/// モデルハンドルの設定
	/// </summary>
	/// <param name="handle">モデルハンドル</param>
	void SetModelHandle(int handle) { m_modelH = handle; }

	/// <summary>
	/// モデルクラスの取得
	/// </summary>
	/// <returns>モデルクラス</returns>
	const std::shared_ptr<Model> GetModel()const { return m_pModel; }

	/// <summary>
	/// キャラクター情報の取得
	/// </summary>
	/// <returns>キャラクター情報</returns>
	const CharacterData::CharacterInfo GetInfo()const { return m_characterInfo; }

	/// <summary>
	/// 座標の取得
	/// </summary>
	/// <returns>座標</returns>
	const VECTOR& GetPos()const { return m_characterInfo.pos; }

	/// <summary>
	/// フィールドとの当たり判定を考慮した移動
	/// </summary>
	/// <param name="pField">フィールドポインタ</param>
	void MoveCollField(ObjectBase* pField);

protected:	// 変数
	int m_modelH;										// モデルハンドル
	int m_topFrameIndex;								// モデルの頂点のフレーム番号
	int m_bottomFrameIndex;								// モデルの底辺のフレーム番号
	
	float m_objSize;									// オブジェクトサイズ
	float m_angle;										// オブジェクトの回転
	float m_moveSpeed;									// 移動速度

	std::vector<int> m_vertexShaderType;				// 頂点シェーダタイプ
	
	ObjectManager* m_pObjectManager;					// オブジェクトマネージャーポインタ
	
	CharacterData::CharacterInfo m_characterInfo;		// キャラクター情報
	
	std::shared_ptr<Model> m_pModel;					// モデルクラスポインタ
	std::shared_ptr<CollisionShape> m_pCollShape;	// 当たり判定用図形のポインタ

private:	 // 関数
	/// <summary>
	/// チェックしたポリゴンが壁ポリゴンか床ポリゴンかを
	/// 判断して保存する
	/// </summary>
	void CheckWallAndFloor();

	/// <summary>
	/// 壁ポリゴンとの当たり判定をチェックして移動させる
	/// </summary>
	void FixPosWithWall();

	/// <summary>
	/// 壁からの押し出し処理をする
	/// </summary>
	void FixPosWithWallInternal();

	/// <summary>
	/// 床ポリゴンとの当たり判定をチェックして移動させる
	/// </summary>
	void FixPosWithFloor();

private:	// 変数
	int m_wallNum;			// 壁と判断されたポリゴンの数
	int m_floorNum;			// 床と判断されたポリゴンの数
	bool m_isMove;			// 移動中かどうか
	bool m_isPolyHit;		// ポリゴンに当たったかどうか
	VECTOR m_prevPos;		// 移動前の座標
	VECTOR m_nextPos;		// 移動後の座標
	
	MV1_COLL_RESULT_POLY_DIM m_hitDin{};								// 当たり判定結果代入用ポリゴン構造体
	MV1_COLL_RESULT_POLY* m_pWallPoly[ColInfo::kMaxColHitPolyNum]{};	// 壁ポリゴンと判断されたポリゴンのアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* m_pFloorPoly[ColInfo::kMaxColHitPolyNum]{};	// 床ポリゴンと判断されたポリゴンのアドレスを保存しておくためのポインタ配列
	MV1_COLL_RESULT_POLY* m_pPoly;										// ポリゴンの構造体にアクセスする為に使用するポインタ
	HITRESULT_LINE m_lineRes{};											// 線分とポリゴンとの当たり判定の結果を代入する構造体
};


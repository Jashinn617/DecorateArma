#pragma once
#include "DxLib.h"
#include <vector>
#include <memory>

/// <summary>
/// モデル制御クラス
/// </summary>
class Model
{
public:	// 関数

	/// <summary>
	/// コンストラクタ
	/// ファイル名を指定してロードを行う
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	Model(const char* fileName);

	/// <summary>
	/// コンストラクタ
	/// 指定したハンドルのモデルをコピーとして生成する
	/// </summary>
	/// <param name="ModelH">モデルハンドル</param>
	Model(int modelH);

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Model();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 位置の設定
	/// </summary>
	/// <param name="pos">座標</param>
	void SetPos(VECTOR pos);

	/// <summary>
	/// サイズの設定
	/// </summary>
	/// <param name="scale">サイズ</param>
	void SetScale(VECTOR scale);

	/// <summary>
	/// 角度の設定
	/// </summary>
	/// <param name="rot">角度</param>
	void SetRot(VECTOR rot);

	/// <summary>
	/// モデルハンドルの取得
	/// </summary>
	/// <returns>モデルハンドル</returns>
	int GetModelHandle()const { return m_modelH; }

	/// <summary>
	/// 当たり判定用のフレームインデックスの取得
	/// </summary>
	/// <returns></returns>
	int GetColFrameIndex() const { return m_colFrameIndex; }


	/// <summary>
	/// 位置の取得
	/// </summary>
	/// <returns>位置</returns>
	VECTOR GetPos()const { return m_pos; }

	/*アニメーション制御*/
	/// <summary>
	/// アニメーションを設定する
	/// (ブレンド率に考慮せずに切り替える)
	/// </summary>
	/// <param name="animNo">変更後のアニメーション番号</param>
	/// <param name="isLoop">アニメーションがループするか</param>
	/// <param name="isForceChange">既にanimNoと同じアニメーションが再生されていても変更するか</param>
	void SetAnim(int animNo, bool isLoop, bool isForceChange);

	/// <summary>
	/// アニメーションを変化させる
	/// (changeFrameかけてアニメーションを切り替える)
	/// </summary>
	/// <param name="animNo">変更後のアニメーション番号</param>
	/// <param name="isLoop">アニメーションがループするか</param>
	/// <param name="isForceChange">既にanimNoと同じアニメーションが再生されていても変更するか</param>
	/// <param name="changeFrameNum">アニメーションを変化させるフレーム数</param>
	void ChangeAnim(int animNo, bool isLoop, bool isForceChange, int changeFrameNum);

	/// <summary>
	/// 現在のアニメーションが終了しているかどうかを取得する
	/// (Loopするアニメーションの場合は取得できない)
	/// </summary>
	/// <returns>アニメーションが終了しているかどうか</returns>
	bool IsAnimEnd();

private:	// 構造体
	// アニメーション情報
	struct AnimData
	{
		int animNo;			// アニメーション番号
		int attachNo;		// アタッチ番号
		float totalTime;	// アニメーションの総再生時間
		bool isLoop;		// アニメーションがループするか
	};

private:	// 関数
	/// <summary>
	/// アニメーション情報のクリア
	/// </summary>
	/// <param name="anim">アニメーション情報</param>
	void ClearAnimData(AnimData& anim);

	/// <summary>
	/// アニメーションの更新
	/// </summary>
	/// <param name="anim">アニメーション情報</param>
	/// <param name="dt">アニメーション速度(デフォルトは1.0f)</param>
	void UpdateAnim(AnimData anim, float animSpeed = 1.0f);

	/// <summary>
	// アニメーションのブレンド率を設定する
	/// </summary>
	void UpdateAnimBlendRate();

private:	// 変数
	int m_modelH;					// モデルハンドル
	int m_animSpeed;				// アニメーション速度
	VECTOR m_pos;					// 座標
	/*アニメーションの切り替え情報*/
	int m_animChangeFrame;			// 現在の切り替えフレーム数
	int m_animChangeFrameTotal;		// 切り替えにかける総フレーム数
	int m_animChangeSpeed;			// アニメーションの変更速度
	int m_colFrameIndex;			// 当たり判定として使用するフレームのインデックス
	bool m_isUseCol;				// 当たり判定の情報を使用するかどうか
	bool m_isUpdateCol;				// 当たり判定の情報を毎フレーム更新するかどうか

	AnimData m_prevAnim;		// 変更前アニメーション
	AnimData m_nextAnim;		// 変更後アニメーション
};
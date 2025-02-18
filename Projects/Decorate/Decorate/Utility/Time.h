#pragma once

/// <summary>
/// 一定タイムが経過しているかを返すクラス
/// </summary>
class Time
{
public:		// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="rimitTime">判定したいタイム</param>
	Time(int rimitTime);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Time();

	/// <summary>
	/// タイムリセット
	/// </summary>
	void Reset();

	/// <summary>
	/// タイム更新
	/// リミット時間を超えていた場合はtrue
	/// </summary>
	/// <returns>リミット時間を超えているかどうか</returns>
	bool Update();

	/// <summary>
	/// 現在の秒数のデバッグ用描画
	/// </summary>
	void DebugDraw();

private:	// 変数
	int m_time;			// 経過時間
	int m_rimitTime;	// リミット時間
};


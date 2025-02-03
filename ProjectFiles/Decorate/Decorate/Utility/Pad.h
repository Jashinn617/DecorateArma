#pragma once

/// <summary>
/// パッド処理を行うnamespace
/// </summary>
namespace Pad
{
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 押し下げ判定
	/// </summary>
	/// <param name="button">押しているボタン</param>
	/// <param name="padNo">コントローラー番号(二人プレイを実装したら使う)</param>
	/// <returns>今ボタンを押しているかどうか</returns>
	bool IsPress(int button, int padNo = 0);

	/// <summary>
	/// トリガー判定
	/// </summary>
	/// <param name="button">押しているボタン</param>
	/// <param name="padNo">コントローラー番号(二人プレイを実装したら使う)</param>
	/// <returns>ボタンを押した瞬間かどうか</returns>
	bool IsTrigger(int button, int padNo = 0);

	/// <summary>
	/// 離した判定
	/// </summary>
	/// <param name="button">押しているボタン</param>
	/// <param name="padNo">コントローラー番号(二人プレイを実装したら使う)</param>
	/// <returns>ボタンを離した瞬間かどうか</returns>
	bool IsRelase(int button, int padNo = 0);

	/// <summary>
	/// ログ記録開始
	/// </summary>
	void StartRecordLog();

	/// <summary>
	/// ログ記録終了
	/// </summary>
	void EndRecordLog();

	/// <summary>
	/// ログ再生開始
	/// </summary>
	void StartPlayLog();

	/// <summary>
	/// ログ再生終了
	/// </summary>
	void EndPlayLog();
}
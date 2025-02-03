#include "DxLib.h"
#include "Pad.h"

#include <vector>

namespace
{
	/*定数*/
	constexpr int kLogNum = 16;	// ログを記録するフレーム数
	constexpr int kMaxPad = 2;	// パッドの最大数

	/*使うパッドのデータ*/
	constexpr int kUsePadData[kMaxPad] =
	{
		DX_INPUT_KEY_PAD1,
		DX_INPUT_PAD2
	};

	/*変数*/
	int padLog[kMaxPad][kLogNum];	// 入力ログ(0が最新の状態)

	/*キーログ*/
	int playLogNo = -1;			// キーログの再生フレーム数
	bool isRecordLog = false;	// ログを記録中かどうか
	std::vector<int> keyLog;	// キーログ
}

namespace Pad
{
	void Update()
	{
		// パッドの数だけ繰り返す
		for (int padNo = 0; padNo < kMaxPad; padNo++)
		{
			// 現在のパッドの状態の取得
			int padState = GetJoypadInputState(kUsePadData[padNo]);

			if ((playLogNo >= 0) && (padNo == 0)) // ログのサイズが0以上でパッドが先頭のパッドだった場合
			{				
				if (keyLog.size() > playLogNo) // ログのサイズよりもログのフレーム数が小さかったら
				{
					// パッドの状態更新
					padState = keyLog[playLogNo];
					// ログを増やす
					playLogNo++;
				}
				else // ログのサイズよりもログのフレーム数が大きかったら
				{
					// ログをリセットする
					playLogNo = -1;
				}
			}

			for (int i = kLogNum - 1; i >= 1; i--)
			{
				// ログの更新
				// 過去の入力ログを一つ後ろにしている
				padLog[padNo][i] = padLog[padNo][i - 1];
			}
			// 最新の状態にする
			padLog[padNo][0] = padState;

			/*キーログ*/
			if (isRecordLog) // ログが記録中だったら
			{
				// パッドが先頭のパッドだったら
				if (padNo == 0)
				{
					// ログの追加
					keyLog.push_back(padState);
				}
			}
		}
	}

	bool IsPress(int button, int padNo)
	{
		// 現在の状態(ビット演算)
		// 現在のフレームで押しているかどうか
		return (padLog[padNo][0] & button);
	}

	bool IsTrigger(int button, int padNo)
	{
		bool isNow = (padLog[padNo][0] & button);	// 現在の状態(ビット演算)
		bool isLast = (padLog[padNo][1] & button);	// 前フレームの状態(ビット演算)

		// 現在のフレームで押していて、前のフレームでは押していないかどうか
		return isNow && !isLast;
	}

	bool IsRelase(int button, int padNo)
	{
		bool isNow = (padLog[padNo][0] & button);	// 現在の状態(ビット演算)
		bool isLast = (padLog[padNo][1] & button);	// 前フレームの状態(ビット演算)

		// 現在のフレームで押していなくて、前のフレームでは押されていたかどうか
		return !isNow && isLast;
	}

	void StartRecordLog()
	{
		// 記録の開始
		isRecordLog = true;
		// ログのクリア
		keyLog.clear();
	}

	void EndRecordLog()
	{
		// 記録の終了
		isRecordLog = false;
	}

	void StartPlayLog()
	{
		// ログの初期化
		playLogNo = 0;
	}

	void EndPlayLog()
	{
		// ログのリセット
		playLogNo = -1;
	}
}
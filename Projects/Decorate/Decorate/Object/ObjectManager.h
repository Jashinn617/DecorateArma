#pragma once

#include "../Utility/Game.h"

#include <memory>
#include <list>

class ObjectBase;
class Collision;
class Player;
class EnemyBase;
class ToonShader;
class ShadowMapShader;
class Spawn;
class Gear;

class ObjectManager
{
public:		// 関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="stageKind">ステージの種類</param>
	/// <param name="pGear">装備ポインタ</param>
	ObjectManager(Game::StageKind stageKind, std::shared_ptr<Gear> pGear);

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~ObjectManager();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// オブジェクトの追加
	/// </summary>
	/// <typeparam name="T">オブジェクトベース</typeparam>
	/// <param name="pObj">追加するオブジェクト</param>
	template <class T>
	void AddObject(T* pObj)
	{
		m_pObject.push_back(pObj);
		m_pObject.back()->SetObjectManager(this);
	}

	/// <summary>
	/// プレイヤーポインタの取得
	/// </summary>
	/// <returns>プレイヤーポインタ</returns>
	Player* const GetPlayer();

	/// <summary>
	/// 装備品ポインタ取得
	/// </summary>
	/// <returns>装備品ポインタ</returns>
	std::shared_ptr<Gear> const GetGear() { return m_pGear; }

	/// <summary>
	/// ロックオンされている敵の設定
	/// </summary>
	void InitLockOnEnemy();

	/// <summary>
	/// ロックオンされている敵の取得
	/// </summary>
	/// <returns>ロックオンされている敵のポインタ</returns>
	const EnemyBase* const GetLockOnEnemy() { return m_pLockOnEnemy; }

	/// <summary>
	/// ゲームクリアしたかどうか
	/// </summary>
	/// <returns>ゲームクリアしたかどうか</returns>
	bool IsGameClear() const { return m_isGameClear; }

	/// <summary>
	/// ゲームオーバーになったかどうか
	/// </summary>
	/// <returns>ゲームオーバーになったかどうか</returns>
	bool IsGameOver() const;

	/// <summary>
	/// ボス戦かどうか
	/// </summary>
	/// <returns>ボス戦かどうか</returns>
	bool IsBossButtle() const { return m_isBossButtle; }
	

private:	// 変数
	bool m_isBossButtle;									// ボス戦フラグ
	bool m_isBossDead;										// ボスの死亡フラグ
	bool m_isGameClear;										// ゲームクリアフラグ

	EnemyBase* m_pLockOnEnemy;								// ロックオンされている敵のポインタ

	std::shared_ptr<Collision> m_pCollision;				// 当たり判定
	std::shared_ptr<Spawn> m_pSpawn;						// オブジェクト出現
	std::shared_ptr<ToonShader> m_pToon;					// トゥーンシェーダ
	std::shared_ptr<Gear> m_pGear;							// 装備品
	std::shared_ptr<ShadowMapShader> m_pShadowMapShader;	// シャドウマップシェーダ

	std::list<ObjectBase*> m_pObject;						// オブジェクト
};
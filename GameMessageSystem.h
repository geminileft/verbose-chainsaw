#pragma once

#include <queue>
#include <set>
#include <map>

enum class GameMessageType
{
	None
	, Action
	, DirectionUp
	, DirectionRight
	, DirectionLeft
	, ActionStop
};

struct GameMessageInfo
{
	GameMessageType mType;

};

class GameMessageSystem
{
public:
	void Broadcast(GameMessageInfo message);
	int CreateSubscription(std::set<GameMessageType> subscriptionFilter);
	std::queue<GameMessageInfo> GetSubscriptionMessages(int subscriptionId);

private:
	std::queue<GameMessageInfo> m_messages;
	std::map<int, std::set<GameMessageType>> m_subscriptionFilters;
	std::map<int, std::queue<GameMessageInfo>> m_subscriptionQueues;
	int m_nextSubcriptionId = 0;

};


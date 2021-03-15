#include "pch.h"
#include "GameMessageSystem.h"

#include <stack>
#include <map>
#include <queue>
#include <iterator>

void GameMessageSystem::Broadcast(GameMessageInfo message)
{
	m_messages.push(message);

	std::map<int, GameMessageType>::iterator filterIter;
	for (auto const& subscriptionFilter : m_subscriptionFilters) {
		for (auto const& typeFilter : subscriptionFilter.second) {
			if (message.mType == typeFilter)
			{
				m_subscriptionQueues[subscriptionFilter.first].push(message);
			}
		}
	}

}

int GameMessageSystem::CreateSubscription(std::set<GameMessageType> subscriptionFilter)
{
	m_nextSubcriptionId += 1;
	std::set<GameMessageType> filter;
	filter.insert(subscriptionFilter.begin(), subscriptionFilter.end());
	m_subscriptionFilters[m_nextSubcriptionId] = filter;
	std::queue<GameMessageInfo> subscriptionQueue;
	m_subscriptionQueues[m_nextSubcriptionId] = subscriptionQueue;
	return m_nextSubcriptionId;
}

std::queue<GameMessageInfo> GameMessageSystem::GetSubscriptionMessages(int subscriptionId)
{
	std::queue<GameMessageInfo> messages;
	std::map<int, std::queue<GameMessageInfo>>::iterator elementIter = m_subscriptionQueues.find(subscriptionId);
	if (elementIter != m_subscriptionQueues.end())
	{
		std::queue<GameMessageInfo>* subscriptionMessages = &elementIter->second;
		while (!subscriptionMessages->empty())
		{
 			messages.push(subscriptionMessages->front());
			subscriptionMessages->pop();
		}
	}
	
	return messages;
}

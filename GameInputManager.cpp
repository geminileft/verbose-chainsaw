#pragma once

#include "pch.h"

#include <profileapi.h>

#include "GameInputManager.h"
#include "GameMessageSystem.h"

using namespace Windows::Foundation;
using namespace Windows::System;

GameInputManager::GameInputManager()
{
    QueryPerformanceFrequency(&m_timerFrequency);
}

GameInputManager* GameInputManager::InitPlatformManager(CoreWindow^ window)
{
    GameInputManager* inputManager = new GameInputManager();
    GameInputManager::PlatformInputForwarder^ inputForwarder = ref new GameInputManager::PlatformInputForwarder(inputManager);
    inputForwarder->Setup(window);
    inputManager->m_inputForwarder = inputForwarder;
    return inputManager;
}

void GameInputManager::Update(DX::StepTimer const& timer)
{
    while (!m_inputQueue.empty()) {
        GameInputEvent event = m_inputQueue.front();
        if (event.etype == InputEventType::KeyDown)
        {
            if (event.value == "<UP>")
            {
                auto msg = "DIR::UP";
                GameMessageInfo info;
                info.mType = GameMessageType::DirectionUp;
                m_messageSystem->Broadcast(info);
            }
            else if (event.value == "<RIGHT>")
            {
                GameMessageInfo info;
                info.mType = GameMessageType::DirectionRight;
                m_messageSystem->Broadcast(info);
            }
            else if (event.value == "<LEFT>")
            {
                GameMessageInfo info;
                info.mType = GameMessageType::DirectionLeft;
                m_messageSystem->Broadcast(info);
            }
            else if (event.value == "<SPACE>")
            {
                auto msg = "OTHER::SPACE";
                GameMessageInfo info;
                info.mType = GameMessageType::ActionStop;
                m_messageSystem->Broadcast(info);
            }
        }
        m_inputQueue.pop();
    }
}

void GameInputManager::SetMessageSystem(GameMessageSystem* messageSystem)
{
    m_messageSystem = messageSystem;
}

void GameInputManager::PlatformInputForwarder::OnKeyDown(CoreWindow^ sender, KeyEventArgs^ args)
{
    std::string keyValue = "";
    auto inputKey = args->VirtualKey;
    switch (inputKey) {
    case VirtualKey::Up:
        keyValue = "<UP>";
        break;
    case VirtualKey::Right:
        keyValue = "<RIGHT>";
        break;
    case VirtualKey::Left:
        keyValue = "<LEFT>";
        break;
    case VirtualKey::Space:
        keyValue = "<SPACE>";
        break;
    }

    // only proceed of we have a value we are interested in
    if (keyValue.empty())
        return;

    LARGE_INTEGER currentTimestamp;
    QueryPerformanceCounter(&currentTimestamp);
    GameInputEvent keyEvent;
    keyEvent.timestamp = currentTimestamp.QuadPart;
    keyEvent.etype = InputEventType::KeyDown;
    keyEvent.value = keyValue;
    m_inputManager->m_inputQueue.push(keyEvent);
}

void GameInputManager::PlatformInputForwarder::OnKeyUp(CoreWindow^ sender, KeyEventArgs^ args)
{
    LARGE_INTEGER currentTimestamp;
    QueryPerformanceCounter(&currentTimestamp);
}

void GameInputManager::PlatformInputForwarder::Setup(CoreWindow^ window)
{
    window->KeyDown +=
        ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &GameInputManager::PlatformInputForwarder::OnKeyDown);

    window->KeyUp +=
        ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &GameInputManager::PlatformInputForwarder::OnKeyUp);
}

GameInputManager::PlatformInputForwarder::PlatformInputForwarder(GameInputManager* inputManager)
{
    m_inputManager = inputManager;
}
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
    inputManager->m_myGamepads = ref new Vector<Gamepad^>();

    for (auto gamepad : Gamepad::Gamepads)
    {
        inputManager->m_myGamepads->Append(gamepad);
    }

    Gamepad::GamepadAdded += ref new EventHandler<Gamepad^>([=](Platform::Object^, Gamepad^ args)
        {
            inputManager->m_myGamepads->Append(args);
        });

    Gamepad::GamepadRemoved += ref new EventHandler<Gamepad^>([=](Platform::Object^, Gamepad^ args)
        {
            unsigned int indexRemoved;

            if (inputManager->m_myGamepads->IndexOf(args, &indexRemoved))
            {
                inputManager->m_myGamepads->RemoveAt(indexRemoved);
            }
        });

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
            else if (event.value == "<DOWN>")
            {
                GameMessageInfo info;
                info.mType = GameMessageType::DirectionDown;
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
            else if (event.value == "<NUMBER1>")
            {
                GameMessageInfo info;
                info.mType = GameMessageType::GameSwitchInputControl;
                m_messageSystem->Broadcast(info);
            }
            else if (event.value == "<A>")
            {
                GameMessageInfo info;
                info.mType = GameMessageType::ControlsStrafeLeft;
                m_messageSystem->Broadcast(info);
            }
            else if (event.value == "<D>")
            {
                GameMessageInfo info;
                info.mType = GameMessageType::ControlsStrafeRight;
                m_messageSystem->Broadcast(info);
            }
            else if (event.value == "<W>")
            {
                GameMessageInfo info;
                info.mType = GameMessageType::ControlsCircleUp;
                m_messageSystem->Broadcast(info);
            }
            else if (event.value == "<S>")
            {
                GameMessageInfo info;
                info.mType = GameMessageType::ControlsCircleDown;
                m_messageSystem->Broadcast(info);
            }
            else if (event.value == "<L>")
            {
                GameMessageInfo info;
                info.mType = GameMessageType::GameLightDirUpdate;
                m_messageSystem->Broadcast(info);
            }
        }
        m_inputQueue.pop();
    }
    Gamepad^ player = GetFirstGamepad();

    if (player != nullptr)
    {
        auto currentReading = player->GetCurrentReading();
        if ((currentReading.Buttons & GamepadButtons::B) == GamepadButtons::B)
        {
            GameMessageInfo info;
            info.mType = GameMessageType::ActionStop;
            m_messageSystem->Broadcast(info);
        }
        else if ((currentReading.Buttons & GamepadButtons::DPadRight) == GamepadButtons::DPadRight)
        {
            GameMessageInfo info;
            info.mType = GameMessageType::DirectionRight;
            m_messageSystem->Broadcast(info);
        }
        else if ((currentReading.Buttons & GamepadButtons::DPadLeft) == GamepadButtons::DPadLeft)
        {
            GameMessageInfo info;
            info.mType = GameMessageType::DirectionLeft;
            m_messageSystem->Broadcast(info);
        }
    }
}

void GameInputManager::SetMessageSystem(GameMessageSystem* messageSystem)
{
    m_messageSystem = messageSystem;
}

Gamepad^ GameInputManager::GetFirstGamepad()
{
    if (m_myGamepads->Size > 0)
    {
        return m_myGamepads->GetAt(0);
    }
    else
    {
        return nullptr;
    }
}

void GameInputManager::PlatformInputForwarder::OnKeyDown(CoreWindow^ sender, KeyEventArgs^ args)
{
    std::string keyValue = "";
    Windows::System::VirtualKey inputKey = args->VirtualKey;
    switch (inputKey) {
    case VirtualKey::Down:
        keyValue = "<DOWN>";
        break;
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
    case VirtualKey::Number1:
        keyValue = "<NUMBER1>";
        break;
    case VirtualKey::A:
        keyValue = "<A>";
        break;
    case VirtualKey::D:
        keyValue = "<D>";
        break;
    case VirtualKey::W:
        keyValue = "<W>";
        break;
    case VirtualKey::S:
        keyValue = "<S>";
        break;
    case VirtualKey::L:
        keyValue = "<L>";
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
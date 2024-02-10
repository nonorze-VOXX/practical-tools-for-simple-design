#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

#include "GiraffeText.hpp"
#include "testObject.hpp"
#include <algorithm>
#include <cmath>
#include <memory>

void App::Start() {
    LOG_TRACE("Start");

    // m_Giraffe->SetDrawable(
    //     std::make_shared<Util::Image>("../assets/sprites/giraffe.png"));
    // m_Giraffe->SetZIndex(5);
    // m_Giraffe->Start();

    // auto gf = std::make_shared<GiraffeText>("../assets/fonts/Inter.ttf", 50);
    // gf->SetZIndex(m_Giraffe->GetZIndex() - 1);
    // gf->Start();
    // m_Giraffe->AddChild(gf);

    m_CurrentState = State::UPDATE;
}

void App::Update() {
    // test->AddChild(m_Giraffe);

    auto cursorPos = Util::Input::GetCursorPosition();
    if (Util::Input::IsLButtonPressed()) {
        LOG_DEBUG("Left button pressed");
        auto f = Factory<Component>::GetInstance();
        auto go = f->Create();
        go->SetPostion(cursorPos);
        LOG_DEBUG("MyGo: {}", f->M_Gos.size());
    }
    if (Util::Input::IsRButtonPressed()) {
        LOG_DEBUG("Right button pressed");
    }
    if (Util::Input::IsMButtonPressed()) {
        LOG_DEBUG("Middle button pressed");
    }
    if (Util::Input::IfScroll()) {
        auto delta = Util::Input::GetScrollDistance();
        LOG_DEBUG("Scrolling: x: {}, y: {}", delta.x, delta.y);
    }
    if (Util::Input::IsMouseMoving()) {
        // LOG_DEBUG("Mouse moving! x:{}, y{}", cursorPos.x, cursorPos.y);
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    if (Util::Input::IsKeyPressed(Util::Keycode::A)) {
        LOG_DEBUG("A");
    }
    if (Util::Input::IsKeyPressed(Util::Keycode::B)) {
        LOG_DEBUG("B");
        Util::Input::SetCursorPosition({0.0F, 0.0F});
    }
    WorldFactory::Update();
    WorldFactory::Draw();

    // m_Giraffe->Update();
}

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}

#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

#include "GiraffeText.hpp"
#include "testObject.hpp"
#include <algorithm>
#include <cmath>
#include <glm/fwd.hpp>
#include <memory>

void App::Start() {
    LOG_TRACE("Start");
    for (int i = -10; i < 10; i++) {
        for (int ii = -10; ii < 10; ii++) {
            auto f = Factory<Converyor>::GetInstance();
            auto go = f->Create();
            go->SetPostion({i * gridWidth, ii * gridWidth});
            go->SetScale(gridWidth / go->GetScaledSize());
        }
    }

    m_CurrentState = State::UPDATE;
}

void App::Update() {
    // test->AddChild(m_Giraffe);

    auto cursorPos = Util::Input::GetCursorPosition();
    if (Util::Input::IsLButtonPressed()) {
        LOG_DEBUG("Left button pressed");
        auto f = Factory<Plate>::GetInstance();
        auto go = f->Create();
        auto t = round(cursorPos / gridWidth) * gridWidth;
        go->SetPostion(t);
        go->SetScale(gridWidth / go->GetScaledSize());
        go->SetZIndex(5);
        LOG_DEBUG("MyGo: {}", f->GetList().size());
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
    ConveryorCarryPlate(Factory<Plate>::GetInstance()->GetList(),
                        Factory<Converyor>::GetInstance()->GetList());
    WorldFactory::Draw();

    // m_Giraffe->Update();
}

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}

void App::ConveryorCarryPlate(
    std::pmr::vector<std::shared_ptr<Plate>> plate,
    std::pmr::vector<std::shared_ptr<Converyor>> converyor) {
    float speed = 1;
    for (auto &p : plate) {
        for (auto &c : converyor) {
            auto platePos = ConvertPositionToGrid(p->GetPostion());
            auto converyorPos = ConvertPositionToGrid(c->GetPostion());
            if (platePos == converyorPos) {
                p->SetPostion(p->GetPostion() + glm::vec2(0, speed));
            }
        }
    }
}

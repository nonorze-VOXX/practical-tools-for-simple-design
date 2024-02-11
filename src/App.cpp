#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

#include "testObject.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <glm/fwd.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <vector>

void App::Start() {
    LOG_TRACE("Start");
    std::pmr::vector<std::vector<int>> map{
        {0, 0, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0},
    };
    // LOG_DEBUG(static_cast<int>(-(map[i].size() - 1) / 2))
    for (int i = 0; i < static_cast<int>(map.size()); i++) {
        for (int ii = 0; ii < static_cast<int>(map[i].size()); ii++) {
            auto x = static_cast<int>(-(map[i].size() - 1) / 2 + ii);
            auto y = static_cast<int>(-(map.size() - 1) / 2 + i);
            if (map[i][ii] == 0)
                continue;
            LOG_DEBUG("x: {}, y: {}", x, y);
            auto f = Factory<Converyor>::GetInstance();
            auto go = f->Create();
            go->SetPostion({x * gridWidth, y * gridWidth});
            go->SetScale(gridWidth / go->GetScaledSize());
            go->SetDirection(Direction::RIGHT);
            LOG_DEBUG("test {} {}", i, ii);
        }
    }

    m_CurrentState = State::UPDATE;
    LOG_DEBUG(DirectionToRotation(Direction::UP));
    LOG_DEBUG(DirectionToRotation(Direction::DOWN));
    LOG_DEBUG(DirectionToRotation(Direction::RIGHT));
    LOG_DEBUG(DirectionToRotation(Direction::LEFT));
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
        auto f = Factory<Arm>::GetInstance();
        auto go = f->Create();
        auto t = round(cursorPos / gridWidth) * gridWidth;
        go->SetPostion(t);
        go->SetScale(gridWidth / go->GetScaledSize());
        go->SetZIndex(5);
        LOG_DEBUG("MyGo: {}", f->GetList().size());
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
    ArmCarryPlate(Factory<Plate>::GetInstance()->GetList(),
                  Factory<Arm>::GetInstance()->GetList());
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
            auto platePos = PositionToGrid(p->GetPostion());
            auto converyorPos = PositionToGrid(c->GetPostion());
            if (platePos == converyorPos) {
                p->SetPostion(p->GetPostion() +
                              DirectionToVec2(c->GetDirection()) * speed);
            }
        }
    }
}

void App::ArmCarryPlate(std::pmr::vector<std::shared_ptr<Plate>> plate,
                        std::pmr::vector<std::shared_ptr<Arm>> arm) {
    for (auto &p : plate) {
        for (auto &c : arm) {
            auto platePos = PositionToGrid(p->GetPostion());
            auto armPos = PositionToGrid(c->GetPostion());
            if (platePos == armPos - glm::vec2(1, 0)) {
                p->SetPostion(GridToPosition(armPos + glm::vec2(1, 0)));
            }
        }
    }
}

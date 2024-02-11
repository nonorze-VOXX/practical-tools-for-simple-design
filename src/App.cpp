#include "App.hpp"

#include "SDL_video.h"
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
enum class MapObjectType { ARM, CONVERYOR, BOX, NONE };
struct MapObject {
    MapObjectType type;
    Direction direction;
};

void App::Start() {
    LOG_TRACE("Start");
    std::pmr::vector<std::vector<MapObject>> map{
        {{
            {
                MapObjectType::CONVERYOR,
                Direction::RIGHT,
            },
            {
                MapObjectType::CONVERYOR,
                Direction::UP,
            },
            {
                MapObjectType::CONVERYOR,
                Direction::LEFT,
            },
            {
                MapObjectType::CONVERYOR,
                Direction::DOWN,
            },
        }},
        {{
            {
                MapObjectType::ARM,
                Direction::RIGHT,
            },
            {
                MapObjectType::ARM,
                Direction::UP,
            },
            {
                MapObjectType::ARM,
                Direction::LEFT,
            },
            {
                MapObjectType::ARM,
                Direction::DOWN,
            },
        }},
        {{
            {
                MapObjectType::CONVERYOR,
                Direction::RIGHT,
            },
            {
                MapObjectType::CONVERYOR,
                Direction::UP,
            },
            {
                MapObjectType::CONVERYOR,
                Direction::LEFT,
            },
            {
                MapObjectType::CONVERYOR,
                Direction::DOWN,
            },
        }},
    };
    // LOG_DEBUG(static_cast<int>(-(map[i].size() - 1) / 2))
    for (int i = 0; i < static_cast<int>(map.size()); i++) {
        for (int ii = 0; ii < static_cast<int>(map[i].size()); ii++) {
            auto x = static_cast<int>(-(map[i].size() - 1) / 2 + ii);
            auto y = static_cast<int>(-(map.size() - 1) / 2 + i);

            switch (map.at(i).at(ii).type) {
            case MapObjectType::NONE:
                continue;
            case MapObjectType::ARM: {
                auto f = Factory<Arm>::GetInstance();
                auto go = f->Create();
                go->SetPostion({x * gridWidth, y * gridWidth});
                go->SetScale(gridWidth / go->GetScaledSize());
                go->SetDirection(map.at(i).at(ii).direction);
            } break;
            case MapObjectType::CONVERYOR: {
                auto f = Factory<Converyor>::GetInstance();
                auto go = f->Create();
                go->SetPostion({x * gridWidth, y * gridWidth});
                go->SetScale(gridWidth / go->GetScaledSize());
                go->SetDirection(map.at(i).at(ii).direction);
            }

            break;
            case MapObjectType::BOX:
                break;
            }
        }
    }

    m_CurrentState = State::UPDATE;
    LOG_DEBUG(DirectionToAngle(Direction::UP));
    LOG_DEBUG(DirectionToAngle(Direction::DOWN));
    LOG_DEBUG(DirectionToAngle(Direction::RIGHT));
    LOG_DEBUG(DirectionToAngle(Direction::LEFT));
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
        // LOG_DEBUG("Mouse moving! x:{}, y{}", cursorPos.x,
        // cursorPos.y);
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
    ArmCarrying(Factory<Arm>::GetInstance()->GetList());
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
        if (p->GetState() == PlateState::CARRYING)
            continue;
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
        if (p->GetState() == PlateState::CARRYING)
            continue;

        for (auto &c : arm) {
            if (c->GetState() == ArmState::CARRYING)
                continue;
            auto platePos = PositionToGrid(p->GetPostion());
            auto armPos = PositionToGrid(c->GetPostion());
            if (platePos == armPos - DirectionToVec2(c->GetDirection())) {
                p->SetState(PlateState::CARRYING);
                c->SetState(ArmState::CARRYING);
                c->CarryUp(p);
                // p->SetPostion(GridToPosition(
                //     armPos + DirectionToVec2(c->GetDirection())));
            }
        }
    }
}
void App::ArmCarrying(std::pmr::vector<std::shared_ptr<Arm>> arm) {
    for (auto &a : arm) {
        if (a->GetState() == ArmState::CARRYING) {
            auto plate = a->GetCarrying();
            std::for_each(plate.begin(), plate.end(), [&](auto &p) {
                p->SetPostion(a->GetHandPosition());
            });
            if (a->IsTimerEnd()) {

                a->SetState(ArmState::IDLE);
                a->ResetTimer();
            }
        }
    }
}

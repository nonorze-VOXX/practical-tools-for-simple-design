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

// gamejam
// TODO gameflow
// start box
//      reset button
//      start button
//      end detection
//          end box count > level need in time
//      delete endbox to end game

// click to delete
// map design

// feature
// TODO conveyor know next and precius
// TODO map editor

enum class MapObjectType { ARM, CONVERYOR, BOX, NONE };
struct MapObject {
    MapObjectType type;
    Direction direction;
};
// clang-format off
std::pmr::vector<std::vector<MapObject>> map1{
{{MapObjectType::BOX},{ MapObjectType::ARM, Direction::RIGHT, }, { MapObjectType::CONVERYOR, Direction::RIGHT, }, { MapObjectType::CONVERYOR,Direction::RIGHT }, { MapObjectType::CONVERYOR, Direction::RIGHT, }},
{ {MapObjectType::NONE},{MapObjectType::NONE}, { MapObjectType::NONE, Direction::DOWN, }, { MapObjectType::ARM, Direction::DOWN, }, { MapObjectType::ARM, Direction::DOWN, }},
{ {MapObjectType::NONE},{MapObjectType::NONE},{ MapObjectType::NONE, Direction::RIGHT, }, { MapObjectType::BOX }, { MapObjectType::BOX } }
};
std::pmr::vector<std::vector<MapObject>> map2{
{ { MapObjectType::CONVERYOR, Direction::RIGHT, }, { MapObjectType::BOX, }, { MapObjectType::CONVERYOR, Direction::LEFT, }, { MapObjectType::CONVERYOR, Direction::LEFT, }, }, 
{ { MapObjectType::ARM, Direction::RIGHT, }, { MapObjectType::ARM, Direction::DOWN, }, { MapObjectType::ARM, Direction::LEFT, }, { MapObjectType::ARM, Direction::DOWN, }, }, 
{ { MapObjectType::CONVERYOR, Direction::RIGHT, }, { MapObjectType::BOX, }, { MapObjectType::CONVERYOR, Direction::LEFT, }, { MapObjectType::CONVERYOR, Direction::DOWN, }, },
{ { MapObjectType::CONVERYOR, Direction::DOWN, }, { MapObjectType::CONVERYOR, Direction::UP, }, { MapObjectType::CONVERYOR, Direction::RIGHT, }, { MapObjectType::CONVERYOR, Direction::DOWN, }, },
};
// clang-format on
void App::GenerateMap() {
    for (int i = 0; i < static_cast<int>(map1.size()); i++) {
        for (int ii = 0; ii < static_cast<int>(map1[i].size()); ii++) {
            auto x = static_cast<int>(-(map1[i].size() - 1) / 2 + ii);
            auto y = -1 * static_cast<int>(-(map1.size() - 1) / 2 + i);

            switch (map1.at(i).at(ii).type) {
            case MapObjectType::NONE:
                continue;
            case MapObjectType::ARM: {
                auto f = Factory<Arm>::GetInstance();
                auto go = f->Create();
                go->SetPostion({x * gridWidth, y * gridWidth});
                go->SetScale(gridWidth / go->GetScaledSize());
                go->SetDirection(map1.at(i).at(ii).direction);
            } break;
            case MapObjectType::CONVERYOR: {
                auto f = Factory<Conveyor>::GetInstance();
                auto go = f->Create();
                go->SetPostion({x * gridWidth, y * gridWidth});
                go->SetScale(gridWidth / go->GetScaledSize());
                go->SetDirection(map1.at(i).at(ii).direction);
            }

            break;
            case MapObjectType::BOX: {
                auto f = Factory<Box>::GetInstance();
                auto go = f->Create();
                go->SetPostion({x * gridWidth, y * gridWidth});
                auto pf = Factory<Plate>::GetInstance();
                for (int iii = 0; iii < 10; iii++) {
                    auto plate = pf->Create();
                    plate->SetPostion(go->GetPostion());
                    plate->SetScale(gridWidth / plate->GetScaledSize());
                    plate->SetState(PlateState::BOXING);
                    plate->SetZIndex(5);
                    go->PutIn(plate);
                }
                go->SetScale(gridWidth / go->GetScaledSize());
            }

            break;
            }
        }
    }
}
void App::Start() {

    GenerateMap();

    m_CurrentState = State::UPDATE;
    StartButton->Start();
    ResetButton->Start();
    StartButton->SetPostion({5 * gridWidth, 5 * gridWidth});
    ResetButton->SetPostion({5 * gridWidth, 4 * gridWidth});
    StartButton->SetScale(gridWidth / StartButton->GetScaledSize());
    ResetButton->SetScale(gridWidth / ResetButton->GetScaledSize());
    StartButton->SetImage(
        std::make_unique<Util::Image>("../assets/sprites/triangle.bmp"));
    ResetButton->SetImage(
        std::make_unique<Util::Image>("../assets/sprites/rotate.bmp"));
}

void App::Update() {
    switch (m_GameFlow) {

    case GameFlow::Prepare:
        StartButton->Update();
        if (StartButton->GetTrigger()) {
            m_GameFlow = GameFlow::PLaying;
            StartButton->SetTrigger(false);
        }
        StartButton->Draw();
        WorldFactory::Draw();
        break;
    case GameFlow::PLaying:
        ResetButton->Update();
        if (ResetButton->GetTrigger()) {
            m_GameFlow = GameFlow::Prepare;
            ResetButton->SetTrigger(false);
        }
        ResetButton->Draw();
        WorldFactory::Update();
        ConveyorCarryPlate(Factory<Plate>::GetInstance()->GetList(),
                           Factory<Conveyor>::GetInstance()->GetList());
        ArmCarryPlate(Factory<Plate>::GetInstance()->GetList(),
                      Factory<Arm>::GetInstance()->GetList(),
                      Factory<Box>::GetInstance()->GetList());
        ArmCarrying(Factory<Arm>::GetInstance()->GetList());
        ArmReturning(Factory<Arm>::GetInstance()->GetList());
        PlateWaitting(Factory<Plate>::GetInstance()->GetList(),
                      Factory<Arm>::GetInstance()->GetList(),
                      Factory<Box>::GetInstance()->GetList());
        PlateMove(Factory<Plate>::GetInstance()->GetList());
        WorldFactory::Draw();
        break;
    case GameFlow::End:
        WorldFactory::Draw();
        break;
    }

#pragma region framework
    auto cursorPos = Util::Input::GetCursorPosition();
    if (Util::Input::IsLButtonPressed()) {
        // LOG_DEBUG("Left button pressed");
        // auto f = Factory<Plate>::GetInstance();
        // auto go = f->Create();
        // auto t = round(cursorPos / gridWidth) * gridWidth;
        // go->SetPostion(t);
        // go->SetScale(gridWidth / go->GetScaledSize());
        // go->SetZIndex(5);
        // LOG_DEBUG("MyGo: {}", f->GetList().size());
    }
    if (Util::Input::IsRButtonPressed()) {
        // auto f = Factory<Arm>::GetInstance();
        // auto go = f->Create();
        // auto t = round(cursorPos / gridWidth) * gridWidth;
        // go->SetPostion(t);
        // go->SetScale(gridWidth / go->GetScaledSize());
        // go->SetZIndex(5);
        // LOG_DEBUG("MyGo: {}", f->GetList().size());
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
#pragma endregion

    // m_Giraffe->Update();
}

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}

void App::ConveyorCarryPlate(
    std::pmr::vector<std::shared_ptr<Plate>> plate,
    std::pmr::vector<std::shared_ptr<Conveyor>> converyor) {
    float speed = 1;
    for (auto &p : plate) {
        if (p->GetState() != PlateState::IDLE)
            continue;
        for (auto &c : converyor) {
            auto platePos = PositionToGrid(p->GetPostion());
            auto converyorPos = PositionToGrid(c->GetPostion());
            if (platePos == converyorPos) {
                p->AddDeltaMove(DirectionToVec2(c->GetDirection()) * speed);
            }
        }
    }
}

void App::ArmCarryPlate(std::pmr::vector<std::shared_ptr<Plate>> plate,
                        std::pmr::vector<std::shared_ptr<Arm>> arm,
                        std::pmr::vector<std::shared_ptr<Box>> box) {
    for (auto &c : arm) {
        if (c->GetState() != ArmState::IDLE)
            continue;
        for (auto &b : box) {
            if (b->GetCarryingCount() == 0) {
                continue;
            }
            auto boxPos = PositionToGrid(b->GetPostion());
            auto armPos = PositionToGrid(c->GetPostion());
            if (boxPos == armPos - DirectionToVec2(c->GetDirection())) {
                auto p = b->PopPlate();
                p->SetState(PlateState::CARRYING);
                c->SetState(ArmState::CARRYING);
                c->CarryUp(p);
                break;
                // p->SetPostion(GridToPosition(
                //     armPos + DirectionToVec2(c->GetDirection())));
            }
        }
        for (auto &p : plate) {
            if (p->GetState() != PlateState::IDLE)
                continue;

            auto platePos = PositionToGrid(p->GetPostion());
            auto armPos = PositionToGrid(c->GetPostion());
            if (platePos == armPos - DirectionToVec2(c->GetDirection())) {
                p->SetState(PlateState::CARRYING);
                c->SetState(ArmState::CARRYING);
                c->CarryUp(p);
                break;
                // p->SetPostion(GridToPosition(
                //     armPos + DirectionToVec2(c->GetDirection())));
            }
        }
    }
}
void App::ArmCarrying(std::pmr::vector<std::shared_ptr<Arm>> arm) {
    for (auto &a : arm) {
        if (a->GetState() != ArmState::CARRYING) {
            continue;
        }
        auto plate = a->GetCarrying();
        std::for_each(plate.begin(), plate.end(),
                      [&](auto &p) { p->SetPostion(a->GetHandPosition()); });
        a->SetRotation(a->GetCarryingAngle());
        if (a->IsTimerEnd()) {
            a->SetState(ArmState::WAITING);
        }
    }
}
void App::ArmReturning(std::pmr::vector<std::shared_ptr<Arm>> arm) {
    for (auto &a : arm) {
        if (a->GetState() != ArmState::RETURNING) {
            continue;
        }
        a->SetRotation(a->GetCarryingAngle());
        if (a->IsReturnEnd()) {
            a->SetState(ArmState::IDLE);
            LOG_DEBUG("go back to idle");
            a->ResetTimer();
        }
    }
}

void App::PlateMove(std::pmr::vector<std::shared_ptr<Plate>> plate) {
    for (auto &p1 : plate) {
        if (p1->GetState() != PlateState::IDLE)
            continue;
        bool collide = false;
        for (auto &p2 : plate) {
            if (p2->GetState() != PlateState::IDLE)
                continue;
            if (p1 == p2)
                continue;

            auto p1Pos = PositionToGrid(p1->GetPostion() + p1->GetDeltaMove());
            auto p2Pos = PositionToGrid(p2->GetPostion());
            if (p1Pos == p2Pos) {
                collide = true;
                break;
            }
        }
        if (!collide) {
            p1->SetPostion(p1->GetPostion() + p1->GetDeltaMove());
        }
        p1->ResetDeltaMove();
    };
}
void App::PlateWaitting(std::pmr::vector<std::shared_ptr<Plate>> plate,
                        std::pmr::vector<std::shared_ptr<Arm>> arm,
                        std::pmr::vector<std::shared_ptr<Box>> box) {
    for (auto &c : arm) {
        if (c->GetState() != ArmState::WAITING)
            continue;
        bool isGroundHaveBox = false;
        for (auto &b : box) {
            auto boxPOs = PositionToGrid(b->GetPostion());
            auto armPos = PositionToGrid(c->GetHandPosition());
            if (boxPOs == armPos) {
                auto plate = c->PopPlate();
                plate->SetState(PlateState::BOXING);
                b->PutIn(plate);
                isGroundHaveBox = true;
                break;
            }
        }
        if (!isGroundHaveBox) {
            bool isGroundHavePlate = false;
            for (auto &p : plate) {
                if (p->GetState() != PlateState::IDLE)
                    continue;
                auto platePos = PositionToGrid(p->GetPostion());
                auto armPos = PositionToGrid(c->GetHandPosition());
                if (platePos == armPos) {
                    isGroundHavePlate = true;
                    break;
                }
            }
            if (!isGroundHavePlate) {
                LOG_DEBUG("No plate");
                auto plate = c->PopPlate();
                plate->SetState(PlateState::IDLE);
            }
        }
        if (c->GetCarryingCount() == 0) {
            c->SetState(ArmState::RETURNING);
            LOG_DEBUG("Return");
        }
    }
}

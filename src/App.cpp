#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

#include "Util/Text.hpp"
#include "testObject.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <glm/fwd.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <vector>

// gamejam
// TODO gameflow

// map design

// feature
// TODO conveyor know next and precius
// TODO map editor

enum class MapObjectType { ARM, CONVERYOR, BOX, NONE, GOAL };
struct MapObject {
    MapObjectType type;
    Direction direction;
};
// clang-format off
std::pmr::vector<std::vector<MapObject>> map1{
{{MapObjectType::BOX},{ MapObjectType::ARM, Direction::RIGHT, }, { MapObjectType::CONVERYOR, Direction::RIGHT, }, { MapObjectType::CONVERYOR,Direction::RIGHT }, { MapObjectType::CONVERYOR, Direction::RIGHT, }},
{ {MapObjectType::NONE},{MapObjectType::NONE}, { MapObjectType::NONE, Direction::DOWN, }, { MapObjectType::ARM, Direction::DOWN, }, { MapObjectType::ARM, Direction::DOWN, }},
{ {MapObjectType::NONE},{MapObjectType::NONE},{ MapObjectType::NONE, Direction::RIGHT, }, { MapObjectType::NONE }, { MapObjectType::GOAL } }
};
std::pmr::vector<std::vector<MapObject>> map2{
{ {MapObjectType::NONE},{MapObjectType::NONE},{MapObjectType::BOX},{MapObjectType::NONE},{MapObjectType::NONE},},
{ {MapObjectType::NONE},{MapObjectType::NONE},{MapObjectType::ARM,Direction::DOWN},{MapObjectType::NONE},{MapObjectType::NONE},},
{ {MapObjectType::BOX},{MapObjectType::ARM,Direction::RIGHT},{MapObjectType::CONVERYOR,Direction::RIGHT},{MapObjectType::CONVERYOR,Direction ::RIGHT},{MapObjectType::CONVERYOR ,Direction::RIGHT},},
{ {MapObjectType::NONE},{MapObjectType::NONE},{MapObjectType::NONE},{MapObjectType::NONE},{MapObjectType::ARM, Direction::DOWN},},
{ {MapObjectType::NONE},{MapObjectType::NONE},{MapObjectType::NONE},{MapObjectType::NONE},{MapObjectType::GOAL},},
};
std::pmr::vector<std::vector<MapObject>> map3{
{ {MapObjectType::BOX},{MapObjectType::NONE},{MapObjectType::CONVERYOR,Direction::RIGHT},{MapObjectType::CONVERYOR,Direction::DOWN},{MapObjectType::BOX},},
{ {MapObjectType::ARM,Direction::DOWN},{MapObjectType::NONE},{MapObjectType::ARM,Direction::UP},{MapObjectType::CONVERYOR,Direction::DOWN},{MapObjectType::ARM,Direction::DOWN},},
{ {MapObjectType::CONVERYOR,Direction::RIGHT},{MapObjectType::CONVERYOR,Direction::RIGHT},{MapObjectType::CONVERYOR,Direction::DOWN},{MapObjectType::CONVERYOR,Direction::LEFT},{MapObjectType::CONVERYOR,Direction::LEFT},},
{ {MapObjectType::NONE},{MapObjectType::NONE},{MapObjectType::ARM,Direction::DOWN},{MapObjectType::NONE},{MapObjectType::NONE},},
{ {MapObjectType::NONE},{MapObjectType::NONE},{MapObjectType::GOAL},{MapObjectType::NONE},{MapObjectType::NONE},},
};
std::pmr::vector<std::vector<MapObject>> map0{
};
// clang-format on
void App::GenerateMap(int level) {
    auto map = map0;
    if (level == 0) {
        map = map1;
    }
    if (level == 1) {
        map = map2;
    }
    if (level == 2) {
        map = map3;
    }

    for (int i = 0; i < static_cast<int>(map.size()); i++) {
        for (int ii = 0; ii < static_cast<int>(map[i].size()); ii++) {
            auto x = static_cast<int>(-(map[i].size() - 1) / 2 + ii);
            auto y = -1 * static_cast<int>(-(map.size() - 1) / 2 + i);

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
                auto f = Factory<Conveyor>::GetInstance();
                auto go = f->Create();
                go->SetPostion({x * gridWidth, y * gridWidth});
                go->SetScale(gridWidth / go->GetScaledSize());
                go->SetDirection(map.at(i).at(ii).direction);
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
                    plate->SetZIndex(7);
                    go->PutIn(plate);
                }
                go->SetScale(gridWidth / go->GetScaledSize());
            }

            break;
            case MapObjectType::GOAL: {
                auto f = Factory<Box>::GetInstance();
                auto go = f->Create();
                go->SetGoal(true);
                go->SetPostion({x * gridWidth, y * gridWidth});
                go->SetScale(gridWidth / go->GetScaledSize());
            }

            break;
            }
        }
    }
}
void App::Start() {
    GenerateMap(level);

    m_CurrentState = State::UPDATE;
    StartButton->Start();
    ResetButton->Start();
    NextLevelButton->Start();
    tutorioText->Start();
    madeText->Start();

    StartButton->SetPostion({5 * gridWidth, 5 * gridWidth});
    ResetButton->SetPostion({5 * gridWidth, 4 * gridWidth});
    NextLevelButton->SetPostion({5 * gridWidth, 3 * gridWidth});
    tutorioText->SetPostion({-2 * gridWidth, 5 * gridWidth});
    madeText->SetPostion({-2 * gridWidth, -5 * gridWidth});

    StartButton->SetScale(gridWidth / StartButton->GetScaledSize());
    ResetButton->SetScale(gridWidth / ResetButton->GetScaledSize());
    NextLevelButton->SetScale(gridWidth / ResetButton->GetScaledSize());
    madeText->SetScale(gridWidth / ResetButton->GetScaledSize());

    StartButton->SetImage(
        std::make_unique<Util::Image>("../assets/sprites/triangle.bmp"));
    ResetButton->SetImage(
        std::make_unique<Util::Image>("../assets/sprites/rotate.bmp"));
    NextLevelButton->SetImage(std::make_unique<Util::Text>(
        "../assets/fonts/Inter.ttf", gridWidth, ">>>"));
    tutorioText->SetImage(std::make_unique<Util::Text>(
        "../assets/fonts/Inter.ttf", gridWidth / 2,
        "LMB to some object to disable,\n one more to enable.\n all giraffe "
        "need to go in red circle.\n"));
    madeText->SetImage(std::make_unique<Util::Text>(
        "../assets/fonts/Inter.ttf", gridWidth / 2,
        "Press Esc To close game.\n"
        "Use PTSD framework By "
        "ntut-open-source-club.\n The Game make By @nonorze-VOXX."));
}

void App::Update() {
    if (Util::Input::IsKeyPressed(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
    auto cursorPos = Util::Input::GetCursorPosition();
    tutorioText->Draw();
    madeText->Draw();
    switch (m_GameFlow) {
    case GameFlow::Prepare: {

        if (Util::Input::IsLButtonPressed()) {
            auto girdPos = round(cursorPos / gridWidth) * gridWidth;
            for (auto &go : Factory<Conveyor>::GetInstance()->GetList()) {
                if ((go->GetPostion()) == (girdPos)) {
                    go->ToggleActive();
                    LOG_DEBUG("Conveyor active toggle");
                }
            }
            for (auto &go : Factory<Arm>::GetInstance()->GetList()) {
                if ((go->GetPostion()) == (girdPos)) {
                    go->ToggleActive();
                    LOG_DEBUG("arm active toggle");
                }
            }
            for (auto &go : Factory<Box>::GetInstance()->GetList()) {
                if ((go->GetPostion()) == (girdPos)) {
                    go->ToggleActive();
                    LOG_DEBUG("box active toggle");
                }
            }
        }
        StartButton->Update();
        if (StartButton->GetTrigger()) {
            m_GameFlow = GameFlow::PLaying;
            StartButton->SetTrigger(false);
        }

        std::pmr::vector<glm::vec2> disablePosList =
            std::pmr::vector<glm::vec2>();
        for (auto component : Factory<Box>::GetInstance()->GetList()) {
            if (!component->GetActive()) {
                disablePosList.push_back(component->GetPostion());
            }
        }
        for (auto component : Factory<Conveyor>::GetInstance()->GetList()) {
            if (!component->GetActive()) {
                disablePosList.push_back(component->GetPostion());
            }
        }
        for (auto component : Factory<Arm>::GetInstance()->GetList()) {
            if (!component->GetActive()) {
                disablePosList.push_back(component->GetPostion());
            }
        }
        for (int i = 0; i < static_cast<int>(disablePosList.size()); i++) {
            auto pos = disablePosList[i];
            if (i >= static_cast<int>(
                         Factory<Disable>::GetInstance()->GetList().size())) {
                auto d = Factory<Disable>::GetInstance()->Create();
                d->Start();
                d->SetScale(gridWidth / d->GetScaledSize());
                d->SetZIndex(6);
            }
            auto dis = Factory<Disable>::GetInstance()->GetList().at(i);
            dis->SetVisible(true);
            dis->SetPostion(pos);
        }
        for (int i = disablePosList.size();
             i < static_cast<int>(
                     Factory<Disable>::GetInstance()->GetList().size());
             i++) {
            auto dis = Factory<Disable>::GetInstance()->GetList().at(i);
            dis->SetVisible(false);
        }
        StartButton->Draw();
        WorldFactory::Draw();
        break;
    }
    case GameFlow::PLaying: {
        ResetButton->Update();
        if (ResetButton->GetTrigger()) {
            m_GameFlow = GameFlow::Prepare;
            ResetButton->SetTrigger(false);
            WorldFactory::Clear();
            Factory<Plate>::GetInstance()->Clear();
            Factory<Conveyor>::GetInstance()->Clear();
            Factory<Arm>::GetInstance()->Clear();
            Factory<Disable>::GetInstance()->Clear();
            Factory<Box>::GetInstance()->Clear();
            GenerateMap(level);
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
        auto totalGoalPlate = 0;
        for (auto goalbox : Factory<Box>::GetInstance()->GetList()) {
            if (goalbox->IsGoal())
                totalGoalPlate += goalbox->GetCarryingCount();
        }
        if ((level == 0 && totalGoalPlate >= 10) ||
            (level == 1 && totalGoalPlate >= 20) ||
            (level == 2 && totalGoalPlate >= 20)) {
            m_GameFlow = GameFlow::End;
            if (level == 2) {
                NextLevelButton->SetImage(std::make_unique<Util::Text>(
                    "../assets/fonts/Inter.ttf", gridWidth * 2,
                    "Thank you for playing. "));
                NextLevelButton->SetPostion({0, 0});
            }
            level++;
        }

        WorldFactory::Draw();
        break;
    }
    case GameFlow::End:
        NextLevelButton->Draw();
        if (level == 3) {
            return;
        }
        NextLevelButton->Update();
        if (NextLevelButton->GetTrigger()) {
            m_GameFlow = GameFlow::Prepare;
            WorldFactory::Clear();
            Factory<Plate>::GetInstance()->Clear();
            Factory<Conveyor>::GetInstance()->Clear();
            Factory<Arm>::GetInstance()->Clear();
            Factory<Disable>::GetInstance()->Clear();
            Factory<Box>::GetInstance()->Clear();
            GenerateMap(level);
            NextLevelButton->SetTrigger(false);
        }

        WorldFactory::Draw();
        break;
    default:
        break;
    }

#pragma region framework
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
        // LOG_DEBUG("Mouse moving! x:{}, y{}", cursorPos.x,
        // cursorPos.y);
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
            if (!c->GetActive()) {
                continue;
            }
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
        if (!c->GetActive()) {
            continue;
        }
        if (c->GetState() != ArmState::IDLE)
            continue;
        for (auto &b : box) {
            if (!b->GetActive()) {
                continue;
            }
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
        if (!a->GetActive()) {
            continue;
        }
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
        if (!a->GetActive()) {
            continue;
        }
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

            auto p1Pos = (p1->GetPostion() + p1->GetDeltaMove());
            auto p2Pos = (p2->GetPostion());
            bool isin = p1Pos.x < p2Pos.x + gridWidth / 2 &&
                        p1Pos.x + gridWidth / 2 > p2Pos.x &&
                        p1Pos.y < p2Pos.y + gridWidth / 2 &&
                        p1Pos.y + gridWidth / 2 > p2Pos.y;
            if (isin) {
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
        if (!c->GetActive()) {
            continue;
        }
        bool isGroundHaveBox = false;
        for (auto &b : box) {
            if (!b->GetActive()) {
                continue;
            }
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

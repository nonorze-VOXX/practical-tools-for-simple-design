
#ifndef TEST_OBJECT_HPP
#define TEST_OBJECT_HPP

#include "Core/Drawable.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include <cmath>
#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <memory_resource>
#include <queue>
#include <set>
#include <vector>

enum class Direction { UP, DOWN, LEFT, RIGHT };
enum class PlateState { IDLE, CARRYING, BOXING };
enum class ArmState { IDLE, CARRYING, WAITING, RETURNING };

inline glm::vec2 DirectionToVec2(Direction direction) {
    switch (direction) {

    case Direction::UP:
        return {0, 1};
    case Direction::DOWN:
        return {0, -1};
    case Direction::LEFT:
        return {-1, 0};
    case Direction::RIGHT:
        return {1, 0};
    }
}
inline float DirectionToAngle(Direction direction) {
    auto vec = DirectionToVec2(direction);
    return vec.y * 90 + vec.x * ((-1) + vec.x) * 90;
}
inline glm::vec2 AngleToVec2(float angle) {
    return {cos(angle / 180 * acos(-1)), sin(angle / 180 * acos(-1))};
}

class Button : public Util::GameObject {
    bool trigger = false;

public:
    void Start() override {
        SetDrawable(
            std::make_unique<Util::Image>("../assets/sprites/circle.bmp"));
    };
    void SetImage(std::shared_ptr<Core::Drawable> image) { SetDrawable(image); }
    void SetPostion(const glm::vec2 &position) {
        m_Transform.translation = position;
    };
    void SetScale(const glm::vec2 &scale) { m_Transform.scale = scale; };
    void SetTrigger(bool b) { trigger = b; }
    bool GetTrigger() { return trigger; }
    void Update(const Util::Transform &transform = Util::Transform()) override {
        if (!m_Visible)
            return;
        auto cursorPos = Util::Input::GetCursorPosition();
        glm::vec2 p1 = {m_Transform.translation.x - GetScaledSize().x / 2,
                        m_Transform.translation.y - GetScaledSize().y / 2};
        glm::vec2 p2 = {m_Transform.translation.x + GetScaledSize().x / 2,
                        m_Transform.translation.y + GetScaledSize().y / 2};
        auto isIn = cursorPos.x > p1.x && cursorPos.x < p2.x &&
                    cursorPos.y > p1.y && cursorPos.y < p2.y;
        if (isIn && Util::Input::IsLButtonPressed()) {
            // OnClick();
            trigger = true;
        }
    }
};
class Component : public Util::GameObject {
    int m_id;
    bool active = true;

public:
    void SetId(int id) { m_id = id; }
    void SetPostion(const glm::vec2 &position);
    void SetRotation(const float &rotation) {
        m_Transform.rotation = rotation * 3.14 / 180;
    };
    const glm::vec2 GetPostion() { return m_Transform.translation; };
    void SetScale(const glm::vec2 &scale);
    void SetActive(bool b) { active = b; }
    bool GetActive() { return active; }
    void ToggleActive() { active = !active; }
    int GetId() { return m_id; }
    void Update(const Util::Transform &transform = Util::Transform()) override {
    }
    void Start() override {
        SetDrawable(
            std::make_unique<Util::Image>("../assets/sprites/giraffe.png"));
    }
};
template <class T>
class Singleton {
private:
    static inline std::shared_ptr<T> m_instance = nullptr;

public:
    static std::shared_ptr<T> GetInstance() {
        if (m_instance == nullptr) {
            m_instance = std::make_shared<T>();
        }
        return m_instance;
    }
};
class WorldFactory {
private:
    static inline std::pmr::vector<std::shared_ptr<Component>> worldObjects =
        std::pmr::vector<std::shared_ptr<Component>>();

    static bool compareMyGoWithId(const std::shared_ptr<Component> &a,
                                  const std::shared_ptr<Component> &b) {
        return a->GetId() < b->GetId();
    }

protected:
    static int GetUnusedId() {
        auto id = 0;
        for (auto &go : worldObjects) {
            if (go->GetId() == id)
                id++;
            else
                break;
        }
        return id;
    }

public:
    static std::pmr::vector<std::shared_ptr<Component>> GetWorldObjects() {
        return worldObjects;
    }
    static void AddWorldObject(std::shared_ptr<Component> go) {
        worldObjects.push_back(go);
        go->SetId(GetUnusedId());
        go->Start();
        std::sort(worldObjects.begin(), worldObjects.end(), compareMyGoWithId);
    }
    static void RemoveWorldObject(std::shared_ptr<Component> go) {
        worldObjects.erase(
            std::remove(worldObjects.begin(), worldObjects.end(), go),
            worldObjects.end());
    }
    static void Draw() {
        for (auto &go : worldObjects) {
            go->Draw();
        }
    }

    static void Update() {
        for (auto &go : worldObjects) {
            go->Update();
        }
    }
};
template <class T = Component>
class Factory : public Singleton<Factory<T>>, public WorldFactory {

    std::pmr::vector<std::shared_ptr<T>> m_components =
        std::pmr::vector<std::shared_ptr<T>>();

public:
    std::pmr::vector<std::shared_ptr<T>> GetList() { return m_components; };

    std::shared_ptr<T> Create() {
        auto go = std::make_shared<T>();
        m_components.push_back(go);
        AddWorldObject(go);

        return go;
    }
};
class Conveyor : public Component {
    Direction m_direction;

public:
    // Converyor(Direction direction)
    //     : m_direction(direction) {}
    void SetDirection(Direction direction) {
        m_direction = direction;
        SetRotation(DirectionToAngle(direction));
    }
    Direction GetDirection() { return m_direction; }
    void Start() override {
        SetDrawable(
            std::make_unique<Util::Image>("../assets/sprites/conveyor.bmp"));
    }
};

class Plate : public Component {
    PlateState m_state = PlateState::IDLE;
    glm::vec2 deltaMove = {0, 0};

public:
    void ResetDeltaMove() { deltaMove = {0, 0}; }
    void AddDeltaMove(const glm::vec2 &delta) { deltaMove += delta; }
    glm::vec2 GetDeltaMove() { return deltaMove; }
    void SetState(PlateState state) { m_state = state; }
    PlateState GetState() { return m_state; }
};
class Arm : public Component {
    Direction m_direction;
    ArmState m_state = ArmState::IDLE;
    std::pmr::vector<std::shared_ptr<Plate>> m_context =
        std::pmr::vector<std::shared_ptr<Plate>>();
    float carryingTime = 1;
    float carryingTimer = 0;

public:
    void Start() override {
        SetDrawable(std::make_unique<Util::Image>("../assets/sprites/arm.bmp"));
    }
    void ResetTimer() { carryingTimer = 0; }
    // float GetTimer() { return carryingTimer; }

    bool IsTimerEnd() { return carryingTimer >= carryingTime; }
    bool IsReturnEnd() { return carryingTimer >= carryingTime * 2; }
    void Update(const Util::Transform &transform = Util::Transform()) override {
        if (m_state == ArmState::CARRYING) {
            if (carryingTimer < carryingTime) {
                carryingTimer += Util::Time::GetDeltaTime();
                if (carryingTimer > carryingTime) {
                    carryingTimer = carryingTime;
                }
            }
        } else if (m_state == ArmState::RETURNING) {
            if (carryingTimer < carryingTime * 2) {
                carryingTimer += Util::Time::GetDeltaTime();
                if (carryingTimer > carryingTime * 2) {
                    carryingTimer = carryingTime * 2;
                }
            }
        }
    }
    void SetState(ArmState state) { m_state = state; }
    ArmState GetState() { return m_state; }
    void CarryUp(std::shared_ptr<Plate> go) { m_context.push_back(go); }
    std::shared_ptr<Plate> PopPlate() {
        auto result = m_context.back();
        m_context.pop_back();
        return result;
    }
    int GetCarryingCount() { return m_context.size(); }

    // void CarryDown(std::shared_ptr<Plate> go) { m_context.erase(go); }
    void SetDirection(Direction direction) {
        m_direction = direction;
        SetRotation(GetCarryingAngle());
    }
    glm::vec2 GetHandPosition() {
        auto p = 50.0F * AngleToVec2(GetCarryingAngle());
        return GetPostion() + p;
    }
    float GetCarryingAngle() {
        return DirectionToAngle(m_direction) + 180 +
               180 * (carryingTimer / carryingTime);
    }

    Direction GetDirection() { return m_direction; }
    std::pmr::vector<std::shared_ptr<Plate>> GetCarrying() { return m_context; }
};
class Box : public Component {
    std::pmr::vector<std::shared_ptr<Plate>> m_context =
        std::pmr::vector<std::shared_ptr<Plate>>();

    bool isGoal = false;

public:
    void Start() override {
        SetDrawable(
            std::make_unique<Util::Image>("../assets/sprites/circle.bmp"));
    }

    void PutIn(std::shared_ptr<Plate> go) { m_context.push_back(go); }
    std::shared_ptr<Plate> PopPlate() {
        auto result = m_context.back();
        m_context.pop_back();
        return result;
    }
    int GetCarryingCount() { return m_context.size(); }
    void SetGoal(bool b) {
        isGoal = b;
        SetDrawable(
            std::make_unique<Util::Image>("../assets/sprites/goal.bmp"));
    }
    bool IsGoal() { return isGoal; }
};
#endif

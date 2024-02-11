
#ifndef TEST_OBJECT_HPP
#define TEST_OBJECT_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <set>
#include <vector>

enum class Direction { UP, DOWN, LEFT, RIGHT };
enum class PlateState { IDLE, CARRYING, BOXING };

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
inline float DirectionToRotation(Direction direction) {
    auto vec = DirectionToVec2(direction);
    return vec.x * -90 + vec.y * ((-1) + vec.y) * 90;
}
// 1 0 -> 270
// -1 0 -> 90
// 0 1 -> 0
// 0 -1 -> 180
class Component : public Util::GameObject {
    int m_id;

public:
    void SetId(int id) { m_id = id; }
    void SetPostion(const glm::vec2 &position);
    void SetRotation(const float &rotation) {
        m_Transform.rotation = rotation * 3.14 / 180;
    };
    const glm::vec2 GetPostion() { return m_Transform.translation; };
    void SetScale(const glm::vec2 &scale);
    int GetId() { return m_id; }
    void Update(const Util::Transform &transform = Util::Transform()) override {
    }
    void Start() override {
        SetDrawable(
            std::make_unique<Util::Image>("../assets/sprites/giraffe.png"));
    }
};
class Initable {
public:
    virtual void Init() = 0;
};
template <class T = Initable>
class Singleton {
private:
    static inline std::shared_ptr<T> m_instance = nullptr;

public:
    static std::shared_ptr<T> GetInstance() {
        if (m_instance == nullptr) {
            m_instance = std::make_shared<T>();
            m_instance->Init();
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
class Factory : public Singleton<Factory<T>>,
                public Initable,
                public WorldFactory {

    std::pmr::vector<std::shared_ptr<T>> m_components;

public:
    std::pmr::vector<std::shared_ptr<T>> GetList() { return m_components; };

    void Init() override {
        m_components = std::pmr::vector<std::shared_ptr<T>>();
    };

    std::shared_ptr<T> Create() {
        auto go = std::make_shared<T>();
        m_components.push_back(go);
        AddWorldObject(go);

        return go;
    }
};
class Converyor : public Component {
    Direction m_direction;

public:
    // Converyor(Direction direction)
    //     : m_direction(direction) {}
    void SetDirection(Direction direction) {
        m_direction = direction;
        SetRotation(DirectionToRotation(direction));
    }
    Direction GetDirection() { return m_direction; }
    void Start() override {
        SetDrawable(
            std::make_unique<Util::Image>("../assets/sprites/conveyor.bmp"));
    }
};

class Plate : public Component {
    PlateState m_state = PlateState::IDLE;

public:
    void SetState(PlateState state) { m_state = state; }
    PlateState GetState() { return m_state; }
};
class Arm : public Component {
    Direction m_direction;
    std::pmr::set<std::shared_ptr<Plate>> m_context =
        std::pmr::set<std::shared_ptr<Plate>>();

public:
    void CarryUp(std::shared_ptr<Plate> go) { m_context.insert(go); }
    void CarryDown(std::shared_ptr<Plate> go) { m_context.erase(go); }
    void SetDirection(Direction direction) {
        m_direction = direction;
        SetRotation(DirectionToRotation(direction));
    }
    Direction GetDirection() { return m_direction; }
};
// class Box : public Component {
//     std::pmr::vector<std::shared_ptr<Component>> m_context =
//         std::pmr::vector<std::shared_ptr<Component>>();

// public:
//     void PutIn(std::shared_ptr<Component> go) { m_context.push_back(go); }
// };
#endif

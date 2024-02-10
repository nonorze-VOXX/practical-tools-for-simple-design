
#ifndef TEST_OBJECT_HPP
#define TEST_OBJECT_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>
class Component : public Util::GameObject {
    int m_id;

public:
    void SetId(int id) { m_id = id; }
    void SetPostion(const glm::vec2 &position);
    void SetScale(const glm::vec2 &scale);
    int GetId() { return m_id; }
    void Update(const Util::Transform &transform = Util::Transform()) override {
        LOG_DEBUG("MyGo: {}", m_id);
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
template <class T>
class Factory : public Singleton<Factory<T>>,
                public Initable,
                public WorldFactory {

public:
    void Init() override { M_Gos = std::pmr::vector<std::shared_ptr<T>>(); };
    std::pmr::vector<std::shared_ptr<T>> M_Gos;

    std::shared_ptr<T> Create() {
        auto go = std::make_shared<T>();
        M_Gos.push_back(go);
        AddWorldObject(go);

        return go;
    }
};
class MyGo1 : public Component {};

#endif

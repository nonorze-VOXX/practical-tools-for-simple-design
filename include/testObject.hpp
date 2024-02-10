
#ifndef TEST_OBJECT_HPP
#define TEST_OBJECT_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>
class TestObject : public Util::GameObject {
public:
    void Start() override;
    void Update(const Util::Transform &transform = Util::Transform()) override;
    void SetPostion(const glm::vec2 &position);
    void SetScale(const glm::vec2 &scale);
};
class MyGo : public Util::GameObject {
    int m_id;

public:
    void SetId(int id) { m_id = id; }
    int GetId() { return m_id; }
    // static std::pmr::vector<std::shared_ptr<MyGo>> M_Gos;

    // static std::shared_ptr<MyGo> CreateMyGo() {
    //     auto go = std::make_shared<MyGo>();
    //     go->id = M_Gos.size();
    //     M_Gos.push_back(go);
    //     return go;
    // }
    void Update(const Util::Transform &transform = Util::Transform()) override {
        LOG_DEBUG("MyGo: {}", m_id);
    }
    void Start() override {}
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
    static inline std::pmr::vector<std::shared_ptr<MyGo>> worldObjects =
        std::pmr::vector<std::shared_ptr<MyGo>>();

    static bool compareMyGoWithId(const std::shared_ptr<MyGo> &a,
                                  const std::shared_ptr<MyGo> &b) {
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
    static std::pmr::vector<std::shared_ptr<MyGo>> GetWorldObjects() {
        return worldObjects;
    }
    static void AddWorldObject(std::shared_ptr<MyGo> go) {
        worldObjects.push_back(go);
        go->SetId(GetUnusedId());
        std::sort(worldObjects.begin(), worldObjects.end(), compareMyGoWithId);
    }
    static void RemoveWorldObject(std::shared_ptr<MyGo> go) {
        worldObjects.erase(
            std::remove(worldObjects.begin(), worldObjects.end(), go),
            worldObjects.end());
    }
};
template <class T = MyGo>
class Factory : public Singleton<Factory<T>>,
                public Initable,
                public WorldFactory {

public:
    void Init() override { M_Gos = std::pmr::vector<std::shared_ptr<T>>(); };
    std::pmr::vector<std::shared_ptr<T>> M_Gos;

    std::shared_ptr<T> CreateMyGo() {
        auto go = std::make_shared<T>();
        M_Gos.push_back(go);
        AddWorldObject(go);

        return go;
    }
};
class MyGo1 : public MyGo {};

#endif

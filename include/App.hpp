#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export

#include "Giraffe.hpp"
#include "testObject.hpp"
#include <memory>
#include <vector>

class App {
public:
    enum class State {
        START,
        UPDATE,
        END,
    };

    State GetCurrentState() const { return m_CurrentState; }

    void Start();
    void Update();
    void End(); // NOLINT(readability-convert-member-functions-to-static)

private:
    State m_CurrentState = State::START;
    float gridWidth = 100;

    // std::shared_ptr<Giraffe> m_Giraffe = std::make_shared<Giraffe>();
    // std::shared_ptr<TestObject> test;
    std::pmr::vector<std::shared_ptr<TestObject>> gridMap =
        std::pmr::vector<std::shared_ptr<TestObject>>();
};

#endif

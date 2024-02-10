#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export

#include "Giraffe.hpp"
#include "testObject.hpp"
#include <cmath>
#include <glm/fwd.hpp>
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
    float gridWidth = 50;

    // std::shared_ptr<Giraffe> m_Giraffe = std::make_shared<Giraffe>();
    // std::shared_ptr<TestObject> test;
    void
    ConveryorCarryPlate(std::pmr::vector<std::shared_ptr<Plate>> plate,
                        std::pmr::vector<std::shared_ptr<Converyor>> converyor);
    glm::vec2 ConvertPositionToGrid(glm::vec2 pos) {
        return round(pos / gridWidth);
    };
};

#endif

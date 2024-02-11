#ifndef APP_HPP
#define APP_HPP

#include "pch.hpp" // IWYU pragma: export

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
    static inline float gridWidth = 50;

private:
    State m_CurrentState = State::START;

    // std::shared_ptr<Giraffe> m_Giraffe = std::make_shared<Giraffe>();
    // std::shared_ptr<TestObject> test;
    void
    ConveyorCarryPlate(std::pmr::vector<std::shared_ptr<Plate>> plate,
                       std::pmr::vector<std::shared_ptr<Conveyor>> converyor);
    void ArmCarryPlate(std::pmr::vector<std::shared_ptr<Plate>> plate,
                       std::pmr::vector<std::shared_ptr<Arm>> converyor);
    glm::vec2 PositionToGrid(glm::vec2 pos) { return round(pos / gridWidth); };
    glm::vec2 GridToPosition(glm::vec2 grid) { return grid * gridWidth; };
    void ArmCarrying(std::pmr::vector<std::shared_ptr<Arm>> arm);
    void ArmReturning(std::pmr::vector<std::shared_ptr<Arm>> arm);
    void PlateWaitting(std::pmr::vector<std::shared_ptr<Plate>> plate,
                       std::pmr::vector<std::shared_ptr<Arm>> arm);
};

#endif

#include "Giraffe.hpp"

#include <cmath>

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"
#include "Util/Text.hpp"
#include "Util/Time.hpp"
#include "Util/Transform.hpp"

#include "config.hpp"

void Giraffe::Start() {}

void Giraffe::Update([[maybe_unused]] const Util::Transform &transform) {
    static glm::vec2 dir = {1, 0.5};

    auto &pos = m_Transform.translation;
    auto &scale = m_Transform.scale;
    auto &rotation = m_Transform.rotation;

    if (pos.y >= static_cast<float>(WINDOW_HEIGHT) / 2 ||
        pos.y + static_cast<float>(WINDOW_HEIGHT) / 2 <= 0) {
        dir.y *= -1;
    }
    if (pos.x >= static_cast<float>(WINDOW_WIDTH) / 2 ||
        pos.x + static_cast<float>(WINDOW_WIDTH) / 2 <= 0) {
        dir.x *= -1;
    }

    auto delta = static_cast<float>(Util::Time::GetDeltaTime());
    Util::Transform deltaTransform{
        dir * delta * 1000.0F, 2 * delta,
        glm::vec2(1, 1) * (std::sin(rotation / 2) + 1.0F) * 100.0F};

    pos += deltaTransform.translation;
    rotation += deltaTransform.rotation;
    // scale = deltaTransform.scale;

    for (auto &child : m_Children) {
        child->Update(m_Transform);
    }

    // LOG_DEBUG("GIRA: x: {}, y: {}", pos.x, pos.y);
}

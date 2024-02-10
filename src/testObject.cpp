
#include "testObject.hpp"
void Component::SetPostion(const glm::vec2 &position) {
    m_Transform.translation = position;
}
void Component::SetScale(const glm::vec2 &scale) {
    m_Transform.scale = scale;
}

// std::pmr::vector<std::shared_ptr<MyGo>> MyGo::M_Gos =
//     std::pmr::vector<std::shared_ptr<MyGo>>();

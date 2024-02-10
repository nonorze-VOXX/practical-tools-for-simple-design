
#include "testObject.hpp"
void TestObject::Start() {}
void TestObject::Update(const Util::Transform &transform) {}
void TestObject::SetPostion(const glm::vec2 &position) {
    m_Transform.translation = position;
}
void TestObject::SetScale(const glm::vec2 &scale) {
    m_Transform.scale = scale;
}


#ifndef TEST_OBJECT_HPP
#define TEST_OBJECT_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <glm/fwd.hpp>
class TestObject : public Util::GameObject {
public:
    void Start() override;
    void Update(const Util::Transform &transform = Util::Transform()) override;
    void SetPostion(const glm::vec2 &position);
    void SetScale(const glm::vec2 &scale);
};

#endif

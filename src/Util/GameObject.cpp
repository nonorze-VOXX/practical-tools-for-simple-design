#include "Util/GameObject.hpp"

namespace Util {

void GameObject::Draw() {
    if (!m_Visible) {
        return;
    }
    m_Drawable->Draw(m_Transform, m_ZIndex);
    for (auto &child : m_Children) {
        child->Draw();
    }
}

} // namespace Util

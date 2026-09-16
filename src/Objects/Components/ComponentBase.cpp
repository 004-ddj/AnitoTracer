#include "ComponentBase.hpp"

#include "HierarchyObject.hpp"

std::string ComponentBase::GetLabel() {
    return GetName() + (m_owner ? (" (" + m_owner.GetPtr()->GetName() + ")") : "");
}
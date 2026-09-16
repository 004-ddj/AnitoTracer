#pragma once

#include ANITO_SERIALIZATION_INCLUDES
#include ANITO_EVENT_INCLUDES

#include <string>
#include <iostream>
#include <typeinfo>

#include "Organization/IInstanceManager.hpp"
#include "AssetPipeline.hpp"


// Forward declaration to avoid circular dependency
class HierarchyObject;

class ComponentBase : public gbe::ISerializable {
public:
    // Initializes the component with a name and an optional owner.
    ComponentBase(const std::string&, gbe::IInstanceManager<HierarchyObject>::Ref owner = {})
        : m_owner(owner) {}

    // A virtual destructor is critical for base classes to ensure 
    // derived class destructors are called correctly.
    virtual ~ComponentBase() = default;

    // Allows a component to hide specific serialized fields from the inspector
    // (e.g. StaticBody hiding its inherited "mass" field). Return the field's
    // m_id (the variable name, unless overridden via GBE_SERIALIZE_FIELD_W_NAME).
    virtual std::vector<std::string> GetHiddenProperties() const { return {}; }

    // Delete copy constructor and assignment operator to prevent object slicing.
    ComponentBase(const ComponentBase&) = delete;
    ComponentBase& operator=(const ComponentBase&) = delete;

    // Allow moving for container compatibility.
    ComponentBase(ComponentBase&&) = default;
    ComponentBase& operator=(ComponentBase&&) = default;

    // The component name is always the concrete C++ type name.
    std::string GetName() const {
        std::string typeName = typeid(*this).name();
        constexpr const char* prefixes[] = {"class ", "struct ", "enum "};
        for (const char* prefix : prefixes) {
            const std::string prefixString(prefix);
            if (typeName.rfind(prefixString, 0) == 0) {
                typeName.erase(0, prefixString.size());
                break;
            }
        }
        return typeName;
    }
    gbe::IInstanceManager<HierarchyObject>::Ref GetOwner() const { return m_owner; }

    // Sets or updates the owning HierarchyObject.
    void SetOwner(gbe::IInstanceManager<HierarchyObject>::Ref owner) { 
        m_owner = owner; 
        if (m_owner.GetPtr() != nullptr) {
            OnOwnerAttached();
        }
    }

protected:
    gbe::IInstanceManager<HierarchyObject>::Ref m_owner;

    virtual inline void GBE_Init() {};
    GBE_GENERATE_SERIALIZER_CONSTRUCTOR(ComponentBase, gbe::ISerializable);

    virtual void OnOwnerAttached() {}
public:
    virtual std::string GetLabel() override;
};

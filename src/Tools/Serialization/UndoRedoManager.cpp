#include "UndoRedoManager.hpp"

#include "ISerializable.hpp"
#include "SceneRegistry.hpp"

namespace gbe {

    SnapshotCommand::SnapshotCommand(GUID targetGuid, SerializedData before, SerializedData after)
        : m_targetGuid(targetGuid), m_before(std::move(before)), m_after(std::move(after)) {}

    void SnapshotCommand::Undo() { Apply(m_before); }
    void SnapshotCommand::Redo() { Apply(m_after); }

    void SnapshotCommand::Apply(const SerializedData& data) {
        if (ISerializable* target = SceneRegistry::GetInstance().Resolve<ISerializable>(m_targetGuid)) {
            SerializedData copy = data;
            target->Deserialize(copy);
        }
    }

    UndoRedoManager& UndoRedoManager::GetInstance() {
        static UndoRedoManager instance;
        return instance;
    }

    void UndoRedoManager::PushCommand(std::unique_ptr<IUndoCommand> command) {
        if (!command) return;

        m_redoStack.clear();
        m_undoStack.push_back(std::move(command));

        if (m_undoStack.size() > kMaxHistory) {
            m_undoStack.erase(m_undoStack.begin());
        }
    }

    void UndoRedoManager::BeginAction(const GUID& targetGuid) {
        if (m_actionDepth == 0) {
            m_hasPendingSnapshot = false;
            if (ISerializable* target = SceneRegistry::GetInstance().Resolve<ISerializable>(targetGuid)) {
                m_pendingGuid = targetGuid;
                m_pendingBefore = target->Serialize();
                m_hasPendingSnapshot = true;
            }
        }
        m_actionDepth++;
    }

    void UndoRedoManager::EndAction() {
        if (m_actionDepth == 0) return;

        m_actionDepth--;
        if (m_actionDepth > 0) return; // Still nested inside an outer action.

        if (!m_hasPendingSnapshot) return;
        m_hasPendingSnapshot = false;

        ISerializable* target = SceneRegistry::GetInstance().Resolve<ISerializable>(m_pendingGuid);
        if (!target) return;

        SerializedData after = target->Serialize();
        if (after.serialized_variables == m_pendingBefore.serialized_variables) return; // No actual change.

        PushCommand(std::make_unique<SnapshotCommand>(m_pendingGuid, std::move(m_pendingBefore), std::move(after)));
    }

    void UndoRedoManager::Undo() {
        if (m_undoStack.empty()) return;

        std::unique_ptr<IUndoCommand> command = std::move(m_undoStack.back());
        m_undoStack.pop_back();

        const GUID selectionGuid = m_selectionGetter ? m_selectionGetter() : GUID::Empty();
        command->Undo();
        if (m_selectionSetter) m_selectionSetter(selectionGuid);

        m_redoStack.push_back(std::move(command));
    }

    void UndoRedoManager::Redo() {
        if (m_redoStack.empty()) return;

        std::unique_ptr<IUndoCommand> command = std::move(m_redoStack.back());
        m_redoStack.pop_back();

        const GUID selectionGuid = m_selectionGetter ? m_selectionGetter() : GUID::Empty();
        command->Redo();
        if (m_selectionSetter) m_selectionSetter(selectionGuid);

        m_undoStack.push_back(std::move(command));
    }

    void UndoRedoManager::SetSelectionHooks(SelectionGetter getter, SelectionSetter setter) {
        m_selectionGetter = std::move(getter);
        m_selectionSetter = std::move(setter);
    }

    void UndoRedoManager::Clear() {
        m_undoStack.clear();
        m_redoStack.clear();
        m_hasPendingSnapshot = false;
        m_actionDepth = 0;
    }

}

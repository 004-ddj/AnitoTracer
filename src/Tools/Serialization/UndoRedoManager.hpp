#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "GUID.hpp"
#include "SerializedData.hpp"

namespace gbe {

    // Base interface for a single undoable/redoable operation.
    struct IUndoCommand {
        virtual ~IUndoCommand() = default;
        virtual void Undo() = 0;
        virtual void Redo() = 0;
    };

    // Restores a previously captured before/after SerializedData snapshot onto the
    // ISerializable identified by targetGuid (resolved through SceneRegistry).
    // Because the snapshot comes from ISerializable::Serialize()/Deserialize(), it
    // naturally covers every GBE_SERIALIZE_FIELD-registered property on the target,
    // including nested subtrees for container fields (e.g. HierarchyManager's
    // m_rootNodes), without any command needing to know the target's concrete type.
    class SnapshotCommand : public IUndoCommand {
    public:
        SnapshotCommand(GUID targetGuid, SerializedData before, SerializedData after);

        void Undo() override;
        void Redo() override;

    private:
        void Apply(const SerializedData& data);

        GUID m_targetGuid;
        SerializedData m_before;
        SerializedData m_after;
    };

    // Central undo/redo stack for editor operations. Structural and field-level
    // edits are recorded as before/after SerializedData snapshots of a single
    // ISerializable target looked up by GUID, so restoring a snapshot simply
    // replays the existing AutoSerializer-driven serialization backend.
    class UndoRedoManager {
    public:
        static UndoRedoManager& GetInstance();

        // Pushes a fully-formed command onto the undo stack, clearing the redo stack.
        void PushCommand(std::unique_ptr<IUndoCommand> command);

        // Begins tracking an edit against targetGuid by capturing its current
        // serialized state. Calls may nest (e.g. a composite object-creation helper
        // calling several lower level mutators); only the outermost Begin/End pair
        // captures the snapshot and decides whether to push a single undo entry.
        void BeginAction(const GUID& targetGuid);
        void EndAction();

        void Undo();
        void Redo();

        bool CanUndo() const { return !m_undoStack.empty(); }
        bool CanRedo() const { return !m_redoStack.empty(); }

        // Lets a UI owner (e.g. a hierarchy/selection panel) keep the active
        // selection stable across Undo()/Redo(), since those calls rebuild the
        // scene tree (GUIDs are the only thing that survive the rebuild).
        // The getter/setter deal purely in GUIDs so this manager stays UI-agnostic.
        using SelectionGetter = std::function<GUID()>;
        using SelectionSetter = std::function<void(const GUID&)>;
        void SetSelectionHooks(SelectionGetter getter, SelectionSetter setter);

        // Drops all recorded history. Call when switching/loading scenes, since
        // GUIDs from a previous scene no longer resolve to anything meaningful.
        void Clear();

    private:
        UndoRedoManager() = default;

        std::vector<std::unique_ptr<IUndoCommand>> m_undoStack;
        std::vector<std::unique_ptr<IUndoCommand>> m_redoStack;

        int m_actionDepth = 0;
        bool m_hasPendingSnapshot = false;
        GUID m_pendingGuid;
        SerializedData m_pendingBefore;

        SelectionGetter m_selectionGetter;
        SelectionSetter m_selectionSetter;

        static constexpr size_t kMaxHistory = 100;
    };

}

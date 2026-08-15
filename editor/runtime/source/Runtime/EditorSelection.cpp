#include <Runtime/EditorSelection.h>

namespace chai
{
    void EditorSelection::select(EditorSelectionItem item)
    {
        selected_ = item;
    }

    const EditorSelectionItem EditorSelection::getSelected() const
    {
        return selected_;
    }
}
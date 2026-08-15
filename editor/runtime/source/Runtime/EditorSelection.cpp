#include <Runtime/EditorSelection.h>

namespace chai
{
    void EditorSelection::select(EditorSelectionItem item)
    {
        selected_.push_back(item);
    }

    const EditorSelectionItem EditorSelection::getSelected() const
    {
        return selected_.front();
    }
}
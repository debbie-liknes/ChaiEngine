#pragma once
#include <TypeInfo.h>
#include <vector>

namespace chai
{
    struct EditorSelectionItem 
    {
        uint64_t id;
        // other stuff? idk
        bool operator==(const EditorSelectionItem&) const = default;
    };

	class EditorSelection
	{
    public:
        void select(EditorSelectionItem item);

        const EditorSelectionItem getSelected() const;

    private:
        //std::vector<EditorSelectionItem> selected_;
        EditorSelectionItem selected_;
	};
}
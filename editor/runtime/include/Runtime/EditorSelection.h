#pragma once
#include <TypeInfo.h>
#include <vector>

namespace chai
{
    struct EditorSelectionItem 
    {
        std::type_index type;
        uint64_t id;
        bool operator==(const EditorSelectionItem&) const = default;
    };

	class EditorSelection
	{
    public:
        void select(EditorSelectionItem item);

        const EditorSelectionItem getSelected() const;

    private:
        std::vector<EditorSelectionItem> selected_;
	};
}
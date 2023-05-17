#include <ImGui.h>

#include "imgui_extras.hpp"
#include "EditorProjectile.hpp"
#include "Localization.hpp"
#include "EditorVisualsBase.hpp"

void EditorProjectile::OnFieldLabelClick(int listIdx, EditorInput* field)
{
	int id = atoi(field->buffer);
	auto& name = field->name;

	if (name == "cancel_addr") {
		m_baseWindow->OpenFormWindow(EditorWindowType_Cancel, id);
	}
	else if (name == "hit_condition_addr") {
		m_baseWindow->OpenFormWindow(EditorWindowType_HitCondition, id);
	}
}

void EditorProjectile::RequestFieldUpdate(EditorWindowType_ winType, int valueChange, int listStart, int listEnd)
{
	
	if (winType & (EditorWindowType_Cancel | EditorWindowType_HitCondition))
{
		std::string fieldName = (winType & EditorWindowType_Cancel) ? "cancel_addr" : "hit_condition_addr";
		EditorInput* field = m_fieldIdentifierMap[fieldName];

		if (field->errored) {
			return;
		}

		int value = atoi(field->buffer);
		if (MUST_SHIFT_ID(value, valueChange, listStart, listEnd)) {
			// Same shifting logic as in ListCreations
			// Might be a good idea to macro it
			sprintf_s(field->buffer, field->bufsize, "%d", value + valueChange);
		}
	}
}
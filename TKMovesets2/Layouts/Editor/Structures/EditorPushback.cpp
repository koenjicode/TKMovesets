#include <ImGui.h>
#include <format>
#include <string>

#include "imgui_extras.hpp"
#include "EditorPushback.hpp"
#include "Localization.hpp"

// -- Public methods -- //

EditorPushback::EditorPushback(std::string windowTitleBase, uint32_t t_id, Editor* editor, EditorWindowBase* baseWindow)
{
	windowType = EditorWindowType_Pushback;
	m_baseWindow = baseWindow;
	InitForm(windowTitleBase, t_id, editor);
}

void EditorPushback::OnFieldLabelClick(int listIdx, EditorInput* field)
{
	std::string& name = field->name;

	if (name == "extradata_addr") {
		int id = atoi(field->buffer);
		auto& amountField = m_fieldIdentifierMap["num_of_loops"];

		if (!amountField->errored) {
			int pushbackExtraAmount = atoi(amountField->buffer);
			m_baseWindow->OpenFormWindow(EditorWindowType_PushbackExtradata, id, pushbackExtraAmount);
		}
	}
}

void EditorPushback::RequestFieldUpdate(std::string fieldName, int valueChange, int listStart, int listEnd)
{
	if (fieldName == "inputs") {
		if (m_fieldIdentifierMap["pushback_extra"]->errored) {
			return;
		}

		int value = atoi(m_fieldIdentifierMap["pushback_extra"]->buffer);

		if (MUST_SHIFT_ID(value, valueChange, listStart, listEnd)) {
			// Same shifting logic as in ListCreations
			sprintf(m_fieldIdentifierMap["pushback_extra"]->buffer, "%d", value + valueChange);
		}
		else if (value >= listStart && value <= (listEnd)) {
			value = atoi(m_fieldIdentifierMap["num_of_loops"]->buffer);
			sprintf(m_fieldIdentifierMap["num_of_loops"]->buffer, "%d", value + valueChange);
		}
	}
}
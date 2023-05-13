#include <ImGui.h>
#include <format>

#include "imgui_extras.hpp"
#include "EditorForm.hpp"
#include "Localization.hpp"

// -- Helpers -- //

namespace EditorFormUtils
{
	void SetFieldDisplayText(EditorInput* field, const std::string& newName)
	{
		field->displayName = newName;
		field->textSizeHalf = ImGui::CalcTextSize(newName.c_str()).x / 2;
	}

	int GetColumnCount()
	{
		float windowWidth = ImGui::GetWindowWidth();

		if (windowWidth > 1200) {
			return 8;
		}
		if (windowWidth > 460) {
			return 4;
		}
		if (windowWidth > 230) {
			return 2;
		}

		return 1;
	}

	std::string GetWindowTypeName(EditorWindowType_ type)
	{
		switch (type)
		{
		case EditorWindowType_Requirement:
			return "requirement";

		case EditorWindowType_Move:
			return "move";
		case EditorWindowType_Voiceclip:
			return "voiceclip";

		case EditorWindowType_Cancel:
			return "cancel";
		case EditorWindowType_CancelExtradata:
			return "cancel_extra";
		case EditorWindowType_GroupedCancel:
			return "grouped_cancel";

		case EditorWindowType_HitCondition:
			return "hit_condition";
		case EditorWindowType_Reactions:
			return "reactions";
		case EditorWindowType_Pushback:
			return "pushback";
		case EditorWindowType_PushbackExtradata:
			return "pushback_extradata";

		case EditorWindowType_Extraproperty:
			return "extraproperty";
		case EditorWindowType_MoveBeginProperty:
			return "move_start_extraprop";
		case EditorWindowType_MoveEndProperty:
			return "move_end_extraprop";

		case EditorWindowType_Input:
			return "input";
		case EditorWindowType_InputSequence:
			return "input_sequence";

		case EditorWindowType_Projectile:
			return "projectile";

		case EditorWindowType_CameraData:
			return "camera_data";

		case EditorWindowType_ThrowCamera:
			return "throw_camera";

		case EditorWindowType_MovelistDisplayable:
			return "mvl_displayable";
		case EditorWindowType_MovelistPlayable:
			return "mvl_playable";
		case EditorWindowType_MovelistInput:
			return "mvl_input";
		}
		return "UNKNOWN";
	}
}

// -- Public methods -- //

EditorForm::EditorForm(const std::string& parentWindowName, EditorWindowType_ t_windowType, uint16_t t_structureId, Editor* editor, EditorWindowBase* baseWindow, int listSize)
{
	windowType = t_windowType;
	m_baseWindow = baseWindow;
	InitForm(parentWindowName, t_structureId, editor);
	// Disable single-structure deletion for now.
	m_isDeletable = false;
}

// -- Private methods -- //

void EditorForm::RenderDiscardButtons()
{
	ImGui::TextUnformatted(_("edition.discard_changes"));

	if (ImGui::Button(_("yes")))
	{
		unsavedChanges = false;
		popen = false;
	}
	ImGui::SameLine();
	if (ImGui::Button(_("no"))) {
		m_requestedClosure = false;
	}
}

void EditorForm::Apply()
{
	if (!IsFormValid()) {
		return;
	}

	m_editor->SaveItem(windowType, structureId, m_fieldIdentifierMap);
	unsavedChanges = false;
	justAppliedChanges = true;
	m_requestedClosure = false;

	OnApply();
}

void EditorForm::RenderInputs(int listIdx, std::vector<EditorInput*>& inputs, int category, int columnCount)
{
	unsigned int colIdx = 0;
	for (auto& field : inputs)
	{
		if (!field->visible) {
			// If not visible: don't render and don't increment colIdx
			continue;
		}

		//Render label
		if (colIdx % columnCount == 0) {
			ImGui::TableNextRow();
		}
		ImGui::TableNextColumn();
		RenderLabel(listIdx, field);

		// Render input field
		if (columnCount == 1) {
			ImGui::TableNextRow();
		}
		ImGui::TableNextColumn();
		RenderInput(listIdx, field);

		++colIdx;
	}
}

void EditorForm::RenderLabel(int listIdx, EditorInput* field)
{
	const char* fieldLabel = _(field->displayName.c_str());

	if (field->flags & EditorInput_Interactable && (!field->errored || field->flags & EditorInput_ClickableAlways)) {
		ImGui::PushStyleColor(ImGuiCol_Header, FORM_LABEL_CLICKABLE);

		if (ImGui::Selectable(fieldLabel, true)) {
			OnFieldLabelClick(listIdx, field);
		}
		ImGui::PopStyleColor();
	} else {
		ImVec2 pos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(pos.x + m_labelWidthHalf - field->textSizeHalf, pos.y + 2));
		ImGui::TextUnformatted(fieldLabel);
		ImGui::SetCursorPos(pos);
	}


	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
		// todo: maybe use this for a full-on description
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(fieldLabel);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}

}

void EditorForm::RenderInput(int listIdx, EditorInput* field)
{
	bool appliedBg = field->errored;
	if (appliedBg) {
		ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(186, 54, 54, 150));
	}
	else if (field->color != 0) {
		ImGui::PushStyleColor(ImGuiCol_FrameBg, field->color);
		appliedBg = true;
	}

	if (field->nextValue.size() != 0) {
		// Have to implement pasting manually
		strcpy_s(field->buffer, field->bufsize, field->nextValue.c_str());
		// Put the focus back on the input as it should
		ImGui::SetKeyboardFocusHere();
	}

	ImGui::PushID(field);
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

	if (ImGui::InputText("##", field->buffer, field->bufsize, field->imguiInputFlags) ||
		field->nextValue.size() != 0)
	{
		field->nextValue.clear();
		if (m_ignoreNextChange) {
			m_ignoreNextChange = false;
		}
		else {
			unsavedChanges = true;
			field->errored = m_editor->ValidateField(windowType, field) == false;
			if (!field->errored) {
				OnUpdate(listIdx, field);
			}
		}
	}
	else if (ImGui::IsItemFocused() && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
	{
		// Have to manually implement copying
		if (ImGui::IsKeyPressed(ImGuiKey_C, false) || ImGui::IsKeyPressed(ImGuiKey_X, false)) {
			ImGui::SetClipboardText(field->buffer);
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_V, false))
		{
			field->nextValue = ImGui::GetClipboardText();
			// Force focus on the next widget
			// This is because ImGUI prevents from writing on a focused input's buffer
			ImGui::SetKeyboardFocusHere();
			ImGui::Dummy(ImVec2(20, 20));
		}
		else if ((field->flags & EditorInput_DataChangeable) && ImGui::IsKeyPressed(ImGuiKey_B, false))
		{
			if (!unsavedChanges) {
				m_ignoreNextChange = true;
			}

			EditorUtils::ChangeFieldDataType(field);
			// Force focus on the next widget
			// This is because ImGUI prevents from writing on a focused input's buffer
			ImGui::SetKeyboardFocusHere();
		}
	}
	ImGui::PopID();
	ImGui::PopItemWidth();

	if (appliedBg) {
		ImGui::PopStyleColor();
	}
}

bool EditorForm::IsFormValid()
{
	for (auto& [category, fields] : m_fieldsCategoryMap) {
		for (auto& field : fields) {
			if (field->errored) {
				return false;
			}
		}
	}
	return true;
}

void EditorForm::ApplyWindowName(bool reapplyWindowProperties)
{
	if (reapplyWindowProperties) return;
	std::string windowName = _(std::format("{}.window_name", m_identifierPrefix).c_str());
	m_windowTitle = std::format("{} {} - {}###{}{}", windowName, structureId, m_windowTitleBase.c_str(), windowName, structureId);
}

// -- Public methods -- //

void EditorForm::InitForm(std::string windowTitleBase, uint32_t t_id, Editor* editor)
{
	structureId = t_id;
	m_editor = editor;

	m_identifierPrefix = "edition." + EditorFormUtils::GetWindowTypeName(windowType);

	VectorSet<std::string> drawOrder;
	m_fieldIdentifierMap = editor->GetFormFields(windowType, t_id, drawOrder);

	// Tries to find a name to show in the window title
	// Also figure out the categories
	for (const std::string& fieldName : drawOrder) {
		EditorInput* field = m_fieldIdentifierMap[fieldName];

		m_categories.insert(field->category);
	}

	// Builds the <category : fields> map
	for (uint8_t category : m_categories)
	{
		std::vector<EditorInput*> inputs;
		for (const auto& fieldName : drawOrder) {
			EditorInput* field = m_fieldIdentifierMap[fieldName];
			EditorFormUtils::SetFieldDisplayText(field, _(field->fullName.c_str()));
			if (field->category == category) {
				inputs.push_back(field);
			}
		}
		m_fieldsCategoryMap[category] = inputs;
		m_categoryStringIdentifiers[category] = std::format("{}.category_{}", m_identifierPrefix, category);
	}

	m_windowTitleBase = windowTitleBase;
	ApplyWindowName(false);
}

void EditorForm::RenderInternal()
{
	currentViewport = ImGui::GetWindowViewport();

	// Responsive form that tries to use big widths to draw up to 4 fields (+ 4 labels) per line
	const int columnCount = EditorFormUtils::GetColumnCount();
	m_labelWidthHalf = m_winInfo.size.x / columnCount / 2;
	for (uint8_t category : m_categories)
	{
		const int headerFlags = ImGuiTreeNodeFlags_Framed | (category & 1 ? 0 : ImGuiTreeNodeFlags_DefaultOpen);
		// todo: compute this std::format() once and not every frame
		if (category != 0 && !ImGui::CollapsingHeader(_(m_categoryStringIdentifiers[category].c_str()), headerFlags)) {
			// Only show titles for category > 0, and if tree is not open: no need to render anything
			continue;
		}

		else if (ImGui::BeginTable(m_windowTitle.c_str(), columnCount))
		{
			std::vector<EditorInput*>& inputs = m_fieldsCategoryMap[category];
			RenderInputs(0, inputs, category, columnCount);
			ImGui::EndTable();
		}

	}
}

void EditorForm::Render()
{
	if (setFocus) {
		ImGui::SetNextWindowFocus();
		setFocus = false;
	}

	if (nextDockId != -1) {
		ImGui::SetNextWindowDockID(nextDockId);
		nextDockId = -1;
	}

	// Setup style
	ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 0));

	ImGui::PushID(structureId);
	if (ImGui::Begin(m_windowTitle.c_str(), &popen, unsavedChanges ? ImGuiWindowFlags_UnsavedDocument : 0))
	{
		m_winInfo.pos = ImGui::GetWindowPos();
		m_winInfo.size = ImGui::GetWindowSize();

		if (m_requestedClosure) {
			RenderDiscardButtons();
		}
		else
		{
			// Render Apply button
			bool enabledBtn = unsavedChanges;
			const float buttonHeight = 25;// ImGui::GetFrameHeightWithSpacing();

			ImVec2 startPos = ImGui::GetCursorPos();

			if (enabledBtn) {
				ImGui::PushStyleColor(ImGuiCol_Button, FORM_SAVE_BTN);
			}
			ImGui::SetCursorPos(ImVec2(0, m_winInfo.size.y - buttonHeight + ImGui::GetScrollY()));
			if (ImGuiExtra::RenderButtonEnabled(_("edition.apply"), enabledBtn, ImVec2(ImGui::GetContentRegionAvail().x, buttonHeight))) {
				Apply();
			}
			if (enabledBtn) {
				ImGui::PopStyleColor();
			}
		
			// Render form content
			ImGui::SetCursorPos(startPos);
			RenderInternal();

			// Render Apply button (again)
			// Yes, it's a bit off to render a button twice, but i didn't find any other reliable way to give priority to the button input
			if (ImGui::GetCursorPosY() + buttonHeight >= m_winInfo.size.y)
			{
				if (enabledBtn) {
					ImGui::PushStyleColor(ImGuiCol_Button, FORM_SAVE_BTN);
				}
				ImGui::Dummy(ImVec2(1, buttonHeight));
				// Render Apply() button at the bottom to create more scrollable space
				ImGui::SetCursorPos(ImVec2(0, m_winInfo.size.y - buttonHeight + ImGui::GetScrollY()));
				if (ImGuiExtra::RenderButtonEnabled(_("edition.apply"), enabledBtn, ImVec2(ImGui::GetContentRegionAvail().x, buttonHeight))) {
					Apply();
				}
				if (enabledBtn) {
					ImGui::PopStyleColor();
				}
			}

			// Open popup on right click
			if (IsWindowRightClicked()) {
				ImGui::OpenPopup("ContextMenuPopup");
			}

			// Context menu popup
			if (ImGui::BeginPopup("ContextMenuPopup"))
			{
				ImVec2 selectableSize(0, ImGui::GetFrameHeightWithSpacing());

				{
					ImGuiExtra::DisableBlockIf __(uniqueType);
					if (ImGui::Selectable(_("edition.form_popup.duplicate_structure"), false, 0, selectableSize))
					{
						auto structureListSize = GetOriginalStructureListSize();
						uint32_t newStructure = m_editor->DuplicateStructure(windowType, structureId, structureListSize);
						m_baseWindow->SetChangesUnsaved();
						m_baseWindow->OpenFormWindow(windowType, newStructure, structureListSize);
						OnDuplication(newStructure, structureListSize);
					}
				}

				{
					ImGuiExtra::DisableBlockIf __(!m_isDeletable || uniqueType || structureId == 0);
					if (ImGui::Selectable(_("edition.form_popup.delete_structure"), false, 0, selectableSize)) {
						ImGui::CloseCurrentPopup();
						m_deletionPopupOpen = true;
					}
				}

				RenderExtraContextMenuItems();
				ImGui::EndPopup();
			}

			if (m_deletionPopupOpen) {
				ImGui::OpenPopup("StructureDeletionPopup");
			}

			if (ImGui::BeginPopupModal("StructureDeletionPopup", &m_deletionPopupOpen))
			{
				ImGui::TextUnformatted(_("edition.form_popup.deletion_are_you_sure"));

				ImGui::PushStyleColor(ImGuiCol_Button, RED_BTN);
				if (ImGui::Button(_("yes"))) {
					DeleteStructure();
					m_baseWindow->SetChangesUnsaved();
					ImGui::CloseCurrentPopup();
				}
				ImGui::PopStyleColor();

				ImGui::SameLine();
				if (ImGui::Button(_("no"))) {
					ImGui::CloseCurrentPopup();
					m_deletionPopupOpen = false;
				}
				ImGui::EndPopup();
			}
		}
	}
	ImGui::End();

	ImGui::PopID();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();

	if (!popen && unsavedChanges) {

		m_requestedClosure = true;
		popen = true;
	}

	PostRender();
}

void EditorForm::DeleteStructure()
{
	m_editor->DeleteStructures(windowType, structureId, 0);
	popen = false;
	unsavedChanges = false;
}

bool EditorForm::IsWindowRightClicked() const
{
	if (!ImGui::GetIO().MouseClicked[1]) return false;

	auto cursorPos = ImGui::GetMousePos();
	auto& boundStart = m_winInfo.pos;

	DEBUG_LOG("Cursor: %f %f \n", cursorPos.x, cursorPos.y);
	DEBUG_LOG("Bound start: %f %f \n", boundStart.x, boundStart.y);

	if (cursorPos.x < boundStart.x) return false;
	if (cursorPos.y < boundStart.y) return false;

	auto boundEnd = m_winInfo.pos + m_winInfo.size;
	DEBUG_LOG("Bound end: %f %f \n", boundEnd.x, boundEnd.y);

	if (cursorPos.x >= boundEnd.x) return false;
	if (cursorPos.y >= boundEnd.y) return false;

	return true;
}

void EditorForm::OnUpdate(int listIdx, EditorInput* field)
{
	m_editor->Live_OnFieldEdit(windowType, structureId + listIdx, field);
}

EditorForm::~EditorForm()
{
	for (auto& [key, fieldPtr] : m_fieldIdentifierMap)
	{
		delete[] fieldPtr->buffer;
		delete fieldPtr;
	}

	m_fieldIdentifierMap.clear();
	m_fieldsCategoryMap.clear();
	m_categories.clear();
	m_categoryStringIdentifiers.clear();
}
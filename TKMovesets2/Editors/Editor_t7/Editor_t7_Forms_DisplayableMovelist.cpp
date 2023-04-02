#include "Editor_t7.hpp"

#include <format>

using namespace EditorUtils;

#define SET_DEFAULT_VAL(fieldName, format, value) sprintf_s(inputMap[fieldName]->buffer, FORM_INPUT_BUFSIZE, format, value)
#define CREATE_STRING_FIELD(a, c, d, g) drawOrder.push_back(a), inputMap[a] = new EditorInput { .category = c, .imguiInputFlags = 0, .flags = EditorInput_String | d }, SET_DEFAULT_VAL(a, "%s", g)
#define CREATE_FIELD(a, c, e, g) drawOrder.push_back(a), inputMap[a] = new EditorInput { .category = c, .imguiInputFlags = GetFieldCharset(e), .flags = e }, SET_DEFAULT_VAL(a, GetFieldFormat(e), g), SetInputfieldColor(inputMap[a])

// Utils

std::string EditorT7::GetMovelistDisplayableText(uint32_t offset)
{
	const char* entryString = (char*)m_mvlHead + offset;

	std::string convertedString;

	size_t maxLen = min(strlen(entryString), FORM_INPUT_BUFSIZE - 1);
	for (int i = 0; i < maxLen;)
	{
		if ((unsigned char)entryString[i] == 0xE3 && (i + 2 < maxLen) && (unsigned char)entryString[i + 1] == 0x80)
		{
			if ((unsigned char)entryString[i + 2] == 0x90) {
				convertedString += "{[}";
				i += 3;
				continue;
			}
			else if ((unsigned char)entryString[i + 2] == 0x91) {
				convertedString += "{]}";
				i += 3;
				continue;
			}
			// Might be korean/japanese characters
		}
		else if ((unsigned char)entryString[i] == 0xEE && (i + 2 < maxLen) && (unsigned char)entryString[i + 1] == 0x80)
		{
			switch ((unsigned char)entryString[i + 2])
			{
			case 0x80:
				convertedString += "{DB}";
				break;
			case 0x81:
				convertedString += "{D}";
				break;
			case 0x82:
				convertedString += "{DF}";
				break;
			case 0x83:
				convertedString += "{B}";
				break;
			case 0x84:
				convertedString += "{F}";
				break;
			case 0x85:
				convertedString += "{UB}";
				break;
			case 0x86:
				convertedString += "{U}";
				break;
			case 0x87:
				convertedString += "{UF}";
				break;

			case 0x88:
				convertedString += "{!DB}";
				break;
			case 0x89:
				convertedString += "{!D}";
				break;
			case 0x8A:
				convertedString += "{!DF}";
				break;
			case 0x8B:
				convertedString += "{!B}";
				break;
			case 0x8C:
				convertedString += "{!F}";
				break;
			case 0x8D:
				convertedString += "{!UB}";
				break;
			case 0x8E:
				convertedString += "{!U}";
				break;
			case 0x8F:
				convertedString += "{!UF}";
				break;

			case 0x95:
				convertedString += "{1}";
				break;
			case 0x96:
				convertedString += "{2}";
				break;
			case 0x97:
				convertedString += "{12}";
				break;
			case 0x98:
				convertedString += "{3}";
				break;
			case 0x99:
				convertedString += "{34}";
				break;
			case 0x9A:
				convertedString += "{23}";
				break;
			case 0x9B:
				convertedString += "{123}";
				break;
			case 0x9C:
				convertedString += "{4}";
				break;
			case 0x9D:
				convertedString += "{14}";
				break;
			case 0x9E:
				convertedString += "{24}";
				break;
			case 0x9F:
				convertedString += "{124}";
				break;
			case 0xA0:
				convertedString += "{34}";
				break;
			case 0xA1:
				convertedString += "{134}";
				break;
			case 0xA2:
				convertedString += "{234}";
				break;
			case 0xA3:
				convertedString += "{1234}";
				break;
			}
			i += 3;
			continue;
		}
		convertedString += entryString[i];
		++i;
	}

	/*
	if (convertedString.size() >= FORM_INPUT_BUFSIZE) {
		// todo: bigger bufsizes
		convertedString.erase(FORM_INPUT_BUFSIZE - 1);
	}
	*/
	return convertedString;
}
// -- Inputs -- //

std::vector<std::map<std::string, EditorInput*>> EditorT7::GetMovelistInputListInputs(uint16_t id, int listSize, VectorSet<std::string>& drawOrder)
{
	std::vector<std::map<std::string, EditorInput*>> inputListMap;

	auto input = m_iterators.mvl_inputs.begin() + id;

	// Set up fields. Draw order is same as declaration order because of macro.
	// Default value is written from the last two arguments, also thanks to the macro
	// (fieldName, category, EditorInputFlag, value)
	// 0 has no category name. Even categories are open by default, odd categories are hidden by default.
	do
	{
		std::map<std::string, EditorInput*> inputMap;

		CREATE_FIELD("directions", 0, EditorInput_H8, input->directions);
		CREATE_FIELD("buttons", 0, EditorInput_H8, input->buttons);
		CREATE_FIELD("frame_duration", 0, EditorInput_U8, input->frame_duration);
		CREATE_FIELD("_unkn_0x2", 0, EditorInput_U8, input->_unkn_0x2);

		WriteFieldFullname(inputMap, "mvl_input");
		inputListMap.push_back(inputMap);
		input++;
	} while (--listSize > 0);

	return inputListMap;
}

void EditorT7::SaveMovelistInput(uint16_t id, std::map<std::string, EditorInput*>& inputs)
{
	auto input = m_iterators.mvl_inputs.begin() + id;

	SetMemberValue(&input->directions, inputs["directions"]);
	SetMemberValue(&input->buttons, inputs["buttons"]);
	SetMemberValue(&input->frame_duration, inputs["frame_duration"]);
	SetMemberValue(&input->_unkn_0x2, inputs["_unkn_0x2"]);
}

// -- Displayables -- //

std::vector<std::map<std::string, EditorInput*>> EditorT7::GetMovelistDisplayablesInputs(uint16_t id, VectorSet<std::string>& drawOrder)
{
	std::vector<std::map<std::string, EditorInput*>> inputListMap;

	// Set up fields. Draw order is same as declaration order because of macro.
	// Default value is written from the last two arguments, also thanks to the macro
	// (fieldName, category, EditorInputFlag, value)
	// 0 has no category name. Even categories are open by default, odd categories are hidden by default.

	for (auto& displayable : m_iterators.mvl_displayables)
	{
		std::map<std::string, EditorInput*> inputMap;

		CREATE_FIELD("type", 0, EditorInput_H32, displayable.type);
		CREATE_FIELD("playable_id", 0, EditorInput_S16 | EditorInput_Interactable, displayable.playable_id);

		for (int i = 0; i < _countof(displayable.title_translation_offsets); ++i) {
			std::string key = "title_translation_" + std::to_string(i);
			std::string value = GetMovelistDisplayableText(displayable.title_translation_offsets[i]);

			CREATE_STRING_FIELD(key, 0, 3, value.c_str());
		}
		for (int i = 0; i < _countof(displayable.translation_offsets); ++i) {
			std::string key = "translation_" + std::to_string(i);
			std::string value = GetMovelistDisplayableText(displayable.translation_offsets[i]);

			CREATE_STRING_FIELD(key, 0, 3, value.c_str());
		}


		CREATE_FIELD("_unk0x40", 3, EditorInput_H32_Changeable, displayable._unk0x40);
		CREATE_FIELD("_unk0x46", 3, EditorInput_H16_Changeable, displayable._unk0x46);

		for (int ofst = 0x4C; ofst <= 0x170; ofst += 4) {
			int value = *(int*)((char*)&displayable + ofst);
			std::string key = std::format("unk_{:x}", ofst);
			CREATE_FIELD(key, 3, EditorInput_H32_Changeable, value);
		}

		WriteFieldFullname(inputMap, "mvl_displayable");
		inputListMap.push_back(inputMap);
	}

	return inputListMap;
}

void EditorT7::SaveMovelistDisplayable(uint16_t id, std::map<std::string, EditorInput*>& inputs)
{
	auto displayable = m_iterators.mvl_displayables[id];

	SetMemberValue(&displayable->type, inputs["type"]);
	SetMemberValue(&displayable->playable_id, inputs["playable_id"]);



	SetMemberValue(&displayable->_unk0x40, inputs["_unk0x40"]);
	SetMemberValue(&displayable->_unk0x46, inputs["_unk0x46"]);

	for (int ofst = 0x4C; ofst <= 0x170; ofst += 4) {
		int* valuePtr = (int*)((char*)displayable + ofst);
		std::string key = std::format("unk_{:x}", ofst);
		SetMemberValue(valuePtr, inputs[key]);
	}
}

bool EditorT7::ValidateMovelistDisplayableField(EditorInput* field)
{
	std::string& name = field->name;

	if (name == "playable_id")
	{
		int listIdx = atoi(field->buffer);
		return -1 <= listIdx && listIdx < (int)m_iterators.mvl_playables.size();
	}

	return true;
}

// -- Playables -- //

std::map<std::string, EditorInput*> EditorT7::GetMovelistPlayableInputs(uint16_t id, VectorSet<std::string>& drawOrder)
{
	std::map<std::string, EditorInput*> inputMap;

	auto playable = m_iterators.mvl_playables[id];

	// Set up fields. Draw order is same as declaration order because of macro.
	// Default value is written from the last two arguments, also thanks to the macro
	// (fieldName, category, EditorInputFlag, value)
	// 0 has no category name. Even categories are open by default, odd categories are hidden by default.

	CREATE_FIELD("_unk0x0", 0, EditorInput_U16_Changeable, playable->_unk0x0);
	CREATE_FIELD("distance", 0, EditorInput_U16, playable->distance);
	CREATE_FIELD("p2_rotation", 0, EditorInput_U16_Changeable, playable->p2_rotation);
	CREATE_FIELD("_unk0x6", 0, EditorInput_U16_Changeable, playable->_unk0x6);
	CREATE_FIELD("_unk0x8", 0, EditorInput_U16_Changeable, playable->_unk0x8);
	CREATE_FIELD("p1_facing_related", 0, EditorInput_U16_Changeable, playable->p1_facing_related);
	CREATE_FIELD("_unk0xc", 0, EditorInput_U16_Changeable, playable->_unk0xc);
	CREATE_FIELD("input_sequence_id", 0, EditorInput_U16 | EditorInput_Interactable, playable->input_sequence_offset);
	CREATE_FIELD("input_count", 0, EditorInput_U16, playable->input_count);
	CREATE_FIELD("has_rage", 0, EditorInput_U16_Changeable, playable->has_rage);
	CREATE_FIELD("_unk0x16", 0, EditorInput_U16_Changeable, playable->_unk0x16);

	WriteFieldFullname(inputMap, "mvl_playable");
	return inputMap;
}

void EditorT7::SaveMovelistPlayable(uint16_t id, std::map<std::string, EditorInput*>& inputs)
{
	auto playable = m_iterators.mvl_playables[id];

	uint32_t playable_addr = m_mvlHead->playables_offset + sizeof(MvlPlayable) * id;
	uint32_t input_sequence_id = (uint32_t)GetFieldValue(inputs["input_sequence_id"]);
	uint32_t input_sequence_addr = input_sequence_id * sizeof(MvlInput) + m_mvlHead->inputs_offset;
	uint32_t input_sequence_offset = input_sequence_addr - playable_addr;

	SetMemberValue(&playable->_unk0x0, inputs["_unk0x0"]);
	SetMemberValue(&playable->distance, inputs["distance"]);
	SetMemberValue(&playable->p2_rotation, inputs["p2_rotation"]);
	SetMemberValue(&playable->_unk0x6, inputs["_unk0x6"]);
	SetMemberValue(&playable->_unk0x8, inputs["_unk0x8"]);
	SetMemberValue(&playable->p1_facing_related, inputs["p1_facing_related"]);
	SetMemberValue(&playable->_unk0xc, inputs["_unk0xc"]);
	SetMemberValue(&playable->input_count, inputs["input_count"]);
	SetMemberValue(&playable->input_sequence_offset, inputs["input_sequence_id"]);
	SetMemberValue(&playable->has_rage, inputs["has_rage"]);
	SetMemberValue(&playable->_unk0x16, inputs["_unk0x16"]);
}

// Utils

std::string EditorT7::GetDisplayableMovelistInputStr(const char* directions, const char* buttons)
{
	std::string retVal;

	unsigned int d = strtol(directions, nullptr, 16);
	unsigned int b = strtol(buttons, nullptr, 16);

	const char* directionStrings[4] = { "/U", "/F", "/D", "/B" };
	for (int i = 0; i < 4; ++i)
	{
		if (d & (0x10 << i)) {
			retVal += directionStrings[i];
		}
	}

	std::string buttons_str;
	const char* buttonStrings[4] = { "+2", "+4", "+3", "+1" };
	for (int i = 0; i < 4; ++i)
	{
		if (b & (0x10 << i)) {
			buttons_str += buttonStrings[i];
		}
	}

	if (retVal.size() != 0) {
		retVal.erase(0, 1);
	}

	if (buttons_str.size() != 0) {
		if (retVal.size() == 0) {
			buttons_str.erase(0, 1);
		}
		retVal += buttons_str;
	}

	if (retVal.size() == 0 && (b + d) != 0) {
		retVal = "???";
	}

	return retVal;
}

int EditorT7::GetDisplayableMovelistEntryColor(EditorInput* field)
{
	MvlDisplayableType type = (uint32_t)GetFieldValue(field);

	switch (type & 0xFFFF)
	{
	case 8:
		return MVL_CATEGORY;
		break;
	case 2:
	case 3:
	case 4:
	case 6:
	case 7:
	case 9:
		return MVL_DISABLED;
		break;
	case 10:
		return MVL_COMBO;
		break;
	default:
		return 0;
		break;
	}
}

bool EditorT7::IsMovelistDisplayableEntryCategory(EditorInput* field)
{
	MvlDisplayableType type = (uint32_t)GetFieldValue(field);
	return type == 8;
}

bool EditorT7::IsMovelistDisplayableEntryCombo(EditorInput* field)
{
	MvlDisplayableType type = (uint32_t)GetFieldValue(field);
	return type == 10;
}
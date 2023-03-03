#include <ImGui.h>
#include <format>

#include "Submenu_Import.hpp"
#include "Localization.hpp"
#include "imgui_extras.hpp"
#include "Helpers.hpp"

void Submenu_Import::Render(GameImport *importerHelper)
{
	ImGuiExtra::RenderTextbox(_("importation.explanation"));
	ImGui::SeparatorText(_("importation.importation"));

	ImGui::TextUnformatted(_("importation.import_to"));

	{
		ImGui::SameLine();
		size_t currentGameId = importerHelper->currentGameId;
		ImGui::PushItemWidth(ImGui::CalcTextSize(_("select_game")).x * 1.5f);
		ImGui::PushID(&importerHelper); // Have to push an ID here because extraction.select_game would cause a conflict
		size_t gameListCount = Games::GetGamesCount();
		if (ImGui::BeginCombo("##", currentGameId == -1 ? _("select_game") : Games::GetGameInfo(currentGameId)->name))
		{
			for (size_t i = 0; i < gameListCount; ++i)
			{
				GameInfo* game = Games::GetGameInfo(i);
				if (game->flags & GameImportable) {
					if (ImGui::Selectable(game->name, currentGameId == i, 0, ImVec2(100.0f, 0))) {
						importerHelper->SetTargetProcess(game->processName, i);
					}
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopID();
	}


	{
		ImGui::SameLine();
		char buf[3] = { '1' + importerHelper->currentPlayerId, 'p', '\0' };
		ImGui::PushItemWidth(150.0f);
		if (ImGui::BeginCombo("##", _(buf)))
		{
			size_t currentPlayerId = importerHelper->currentPlayerId;
			for (int8_t i = 0; i < importerHelper->characterCount; ++i)
			{
				buf[0] = '1' + i;
				if (ImGui::Selectable(_(buf), currentPlayerId == i, 0, ImVec2(100.0f, 0)))
				{
					importerHelper->currentPlayerId = i;
				}
			}
			ImGui::EndCombo();
		}
	}

	// Extraction settings
	ImGui::SameLine();
	ImGui::Checkbox(_("importation.apply_instantly"), &importerHelper->apply_instantly);
	ImGui::SameLine();
	ImGuiExtra::HelpMarker(_("importation.apply_instantly_explanation"));

	// If we can't import, display a warning detailling why
	GameProcess* p = importerHelper->process;

	bool busy = importerHelper->IsBusy();
	bool canImport = p->status == PROC_ATTACHED && !busy && importerHelper->CanStart();

	if (p->status != PROC_ATTACHED)
	{
		switch (p->status)
		{
		case PROC_NOT_ATTACHED:
		case PROC_EXITED:
		case PROC_ATTACHING:
			ImGuiExtra_TextboxWarning(_("process.game_not_attached"));
			break;
		case PROC_NOT_FOUND:
			ImGuiExtra_TextboxWarning(_("process.game_not_running"));
			break;
		case PROC_VERSION_MISMATCH:
			ImGuiExtra_TextboxError(_("process.game_version_mismatch"));
			break;
		case PROC_ATTACH_ERR:
			ImGuiExtra_TextboxError(_("process.game_attach_err"));
			break;
		}
	}
	else if (!importerHelper->CanStart()) {
		ImGuiExtra_TextboxWarning(_("importation.cant_import"));
	}

	ImGui::SeparatorText(_("importation.select_moveset"));
	if (ImGui::BeginTable("##", 6, ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_RowBg
		| ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable))
	{
		ImGui::TableSetupColumn("##", 0, 0.0f);
		ImGui::TableSetupColumn(_("moveset.origin"));
		ImGui::TableSetupColumn(_("moveset.target_character"));
		ImGui::TableSetupColumn(_("moveset.date"));
		ImGui::TableSetupColumn(_("moveset.size"));
		ImGui::TableSetupColumn(_("moveset.import"));
		ImGui::TableHeadersRow();

		// Yes, we don't use an iterator here because the vector might actually change size mid-iteration
		ImGui::PushID(&importerHelper);
		for (size_t i = 0; i < importerHelper->storage->extractedMovesets.size(); ++i)
		{
			// moveset is guaranteed not to be freed until after this loop
			movesetInfo* moveset = importerHelper->storage->extractedMovesets[i];

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::TextUnformatted(moveset->name.c_str());

			ImGui::TableNextColumn();
			if (moveset->origin == "INVALID") {
				// Badly formatted file. Display it, but mention it is invalid
				ImGui::TextUnformatted(_("moveset.invalid"));
			}
			else {
				ImGui::TextUnformatted(moveset->origin.c_str());

				ImGui::TableNextColumn();
				ImGui::TextUnformatted(moveset->target_character.c_str());

				ImGui::TableNextColumn();
				ImGui::TextUnformatted(Helpers::currentDateTime(moveset->date).c_str());

				ImGui::TableNextColumn();
				std::string sizeString = std::format("{:.2f} {}", moveset->size, _("moveset.size_mb"));
				ImGui::TextUnformatted(sizeString.c_str());

				ImGui::TableNextColumn();
				ImGui::PushID(moveset->filename.c_str());
				if (ImGuiExtra::RenderButtonEnabled(_("moveset.import"), canImport))
				{
					importerHelper->QueueCharacterImportation(moveset->filename);
				}
				ImGui::PopID();
			}
		}
		ImGui::PopID();

		// Don't de-allocate moveset infos until we're done iterating on it
		importerHelper->storage->CleanupUnusedMovesetInfos();

		ImGui::EndTable();
	}
}

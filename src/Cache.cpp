#include "Cache.h"

namespace Cache
{
	EditorID* EditorID::GetSingleton()
	{
		static EditorID singleton;
		return std::addressof(singleton);
	}

	void EditorID::CacheEditorID(const RE::TESForm* a_form, const char* a_editorID)
	{
		Locker locker(_lock);
		_formIDToEditorIDMap.emplace(a_form->GetFormID(), a_editorID);
	}

	const std::string& EditorID::GetEditorID(RE::FormID a_formID)
	{
		const auto it = _formIDToEditorIDMap.find(a_formID);
		if (it != _formIDToEditorIDMap.end()) {
			return it->second;
		}

		static std::string emptyStr;
		return emptyStr;
	}

	const std::string& EditorID::GetEditorID(const RE::TESForm* a_form)
	{
		return GetEditorID(a_form->GetFormID());
	}
}

extern "C" DLLEXPORT const char* GetFormEditorID(std::uint32_t a_formID)
{
	return Cache::EditorID::GetSingleton()->GetEditorID(a_formID).c_str();
}

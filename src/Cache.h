#pragma once

namespace Cache
{
	class EditorID : public ISingleton<EditorID>
	{
	public:
		void CacheEditorID(const RE::TESForm* a_form, const char* a_editorID);

		const std::string& GetEditorID(RE::FormID a_formID);
		const std::string& GetEditorID(const RE::TESForm* a_form);

	private:
		using Lock = std::mutex;
		using Locker = std::scoped_lock<Lock>;

		mutable Lock                                          _lock;
		ankerl::unordered_dense::map<RE::FormID, std::string> _formIDToEditorIDMap;
	};
}

#pragma once

namespace Cache
{
	class EditorID
	{
	public:
		static EditorID* GetSingleton();

		void CacheEditorID(const RE::TESForm* a_form, const char* a_editorID);

		const std::string& GetEditorID(RE::FormID a_formID);
		const std::string& GetEditorID(const RE::TESForm* a_form);

	protected:
		using Lock = std::mutex;
		using Locker = std::scoped_lock<Lock>;

		EditorID() = default;
		EditorID(const EditorID&) = delete;
		EditorID(EditorID&&) = delete;
		~EditorID() = default;

		EditorID& operator=(const EditorID&) = delete;
		EditorID& operator=(EditorID&&) = delete;

	private:
		mutable Lock _lock;
		robin_hood::unordered_flat_map<RE::FormID, std::string> _formIDToEditorIDMap;
	};
}

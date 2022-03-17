#pragma once

namespace Cache
{
	class EditorID
	{
	public:
		static EditorID* GetSingleton()
		{
			static EditorID singleton;
			return std::addressof(singleton);
		}

		void CacheEditorID(const RE::TESForm* a_form, const char* a_editorID)
		{
			Locker locker(_lock);
			_formIDToEditorIDMap.emplace(a_form->GetFormID(), a_editorID);
		}

		std::string GetEditorID(const RE::TESForm* a_form)
		{
			const auto it = _formIDToEditorIDMap.find(a_form->GetFormID());
			return it != _formIDToEditorIDMap.end() ? it->second : std::string();
		}

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

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

		const std::string& GetEditorID(RE::FormID a_formID)
		{
			const auto it = _formIDToEditorIDMap.find(a_formID);
			if (it != _formIDToEditorIDMap.end()) {
				return it->second;
			}

			static std::string emptyStr;
			return emptyStr;
		}

		const std::string& GetEditorID(const RE::TESForm* a_form)
		{
			return GetEditorID(a_form->GetFormID());
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

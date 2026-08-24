#pragma once

namespace Cache
{
	class EditorID : public REX::TSingleton<EditorID>
	{
	public:
		void CacheEditorID(RE::FormID a_formID, const char* a_editorID);
		void CacheEditorID(const RE::TESForm* a_form, const char* a_editorID);

		const std::string& GetEditorID(RE::FormID a_formID);
		const std::string& GetEditorID(const RE::TESForm* a_form);

	private:
		using Lock = std::mutex;
		using Locker = std::scoped_lock<Lock>;

		mutable Lock                     _lock;
		FlatMap<RE::FormID, std::string> _formIDToEditorIDMap;
	};

	const std::string& GetEditorID(RE::FormID a_formID);
	const std::string& GetEditorID(const RE::TESForm* a_form);

	template <class T>
	struct SetFormEditorID
	{
		static bool thunk(T* a_this, const char* a_str)
		{
			if (!REX::STR::IS_EMPTY(a_str) && !a_this->IsDynamicForm()) {
				const auto& [map, lock] = RE::TESForm::GetAllFormsByEditorID();
				const RE::BSWriteLockGuard locker{ lock };
				if (map) {
					map->emplace(a_str, a_this);
				}
				EditorID::GetSingleton()->CacheEditorID(a_this, a_str);
			}
			return func(a_this, a_str);
		}
		static inline REL::Relocation<decltype(thunk)> func;
		static constexpr std::size_t                   idx{ 0x33 };
	};

	template <class T, std::size_t offset = 0>
	void write_editorID_vfunc()
	{
		stl::write_vfunc<T, offset, SetFormEditorID<T>>();
	}
}

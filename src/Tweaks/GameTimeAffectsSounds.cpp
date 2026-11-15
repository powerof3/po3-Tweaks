#include "Tweaks.h"

//shift pitch with time mult
namespace Tweaks::GameTimeAffectsSounds
{
	inline RE::BGSSoundCategory*                masterCategory;
	inline std::array<RE::BGSSoundCategory*, 2> excludedCategories;

	struct SetFrequency
	{
		static void thunk(RE::BSSoundCategoryHandle* a_handle, float a_frequency)
		{
			if (masterCategory && a_handle->category == masterCategory) {
				masterCategory->SetCategoryFrequency(a_frequency);
				// noise cancellation
				for (const auto category : excludedCategories) {
					if (category) {
						const auto parent = category->parentCategory;
						category->SetCategoryFrequency(1.0f / std::max(parent ? parent->GetCategoryFrequency() : 1.0f, 0.01f));
					}
				}
			} else {
				func(a_handle, a_frequency);
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(34304, 35125), OFFSET(0x74, 0x69) };  //TESAudio::UpdateTimeSensitiveAudio
		stl::write_thunk_call<SetFrequency>(target.address());

		REX::INFO("\t\tInstalled sound - time sync tweak (hook)"sv);
	}

	void InstallOnDataLoad()
	{
		masterCategory = RE::TESForm::LookupByID<RE::BGSSoundCategory>(0x000EB803);
		if (masterCategory) {
			excludedCategories[0] = RE::TESForm::LookupByID<RE::BGSSoundCategory>(0x00071E64);  // AudioCategoryMUS
			excludedCategories[1] = RE::TESForm::LookupByID<RE::BGSSoundCategory>(0x00064451);  // AudioCategoryUI

			const auto dobjMgr = RE::BGSDefaultObjectManager::GetSingleton();
			dobjMgr->objects[RE::DEFAULT_OBJECT::kTimeSensitiveSoundCategory] = masterCategory;
			dobjMgr->objectInit[RE::DEFAULT_OBJECT::kTimeSensitiveSoundCategory] = true;

			REX::INFO("\t\tInstalled sound - time sync tweak (category)"sv);
		}
	}
}

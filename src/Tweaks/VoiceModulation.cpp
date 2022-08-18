#include "Settings.h"
#include "Tweaks.h"

//voice distortion while wearing a helmet
namespace Tweaks::VoiceModulation
{
	struct SetObjectToFollow
	{
		static void thunk(RE::BSSoundHandle& a_handle, RE::NiAVObject* a_node)
		{
			func(a_handle, a_node);

			const auto user = a_node ? a_node->GetUserData() : nullptr;
			const auto biped = user ? user->GetBiped() : nullptr;

			if (biped && biped->objects[RE::BIPED_OBJECT::kHead].partClone.get()) {
				static auto frequency = Settings::GetSingleton()->tweaks.voiceModulationValue;
				a_handle.SetFrequency(frequency);
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL_ID(36541, 37542), OFFSET_3(0x6F3, 0x7A2, 0x6E6) };
		stl::write_thunk_call<SetObjectToFollow>(target.address());

		logger::info("Installed voice modulation tweak"sv);
	}
}

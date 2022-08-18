#include "Tweaks.h"

//disable poison confirmation messagebox
namespace Tweaks::NoPoisonPrompt
{
	struct ShowPoisonConfirmationPrompt
	{
		static void thunk(char*, void (*PoisonWeapon)(std::uint8_t a_result), std::uint8_t a_result, std::uint32_t, std::int32_t, char*, char*)
		{
			PoisonWeapon(a_result);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	struct ShowPoisonInformationPrompt
	{
		static void thunk(char* a_message, void (*PoisonWeapon)(std::uint8_t a_result), std::uint8_t a_result, std::uint32_t, std::int32_t, char*, char*)
		{
			RE::DebugNotification(a_message, "UIMenuCancel");
			PoisonWeapon(a_result);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install(std::uint32_t a_type)
	{
		REL::Relocation<std::uintptr_t> target{ REL_ID(39406, 40481) };

		switch (a_type) {
		case 1:
			stl::write_thunk_call<ShowPoisonConfirmationPrompt>(target.address() + 0x10B);
			break;
		case 2:
			stl::write_thunk_call<ShowPoisonInformationPrompt>(target.address() + 0x143);
			break;
		case 3:
			{
				stl::write_thunk_call<ShowPoisonConfirmationPrompt>(target.address() + 0x10B);
				stl::write_thunk_call<ShowPoisonInformationPrompt>(target.address() + 0x143);
			}
			break;
		default:
			break;
		}

		logger::info("Installed no poison message tweak"sv);
	}
}

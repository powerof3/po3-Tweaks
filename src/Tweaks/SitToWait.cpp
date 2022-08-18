#include "Settings.h"
#include "Tweaks.h"

//you can only wait while sitting down
namespace Tweaks::SitToWait
{
	struct detail
	{
		static bool ProcessMenu(const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, bool a_unk03)
		{
			using func_t = decltype(&ProcessMenu);
			REL::Relocation<func_t> func{ REL_ID(80077, 82180) };
			return func(a_menuName, a_type, a_unk03);
		}

		static bool CanSleepWait(RE::PlayerCharacter* a_player, RE::TESObjectREFR* a_furniture)
		{
			using func_t = decltype(&CanSleepWait);
			REL::Relocation<func_t> func{ REL_ID(39371, 40443) };
			return func(a_player, a_furniture);
		}

		static bool CanWait(RE::PlayerCharacter* a_player, RE::TESObjectREFR* a_furniture)
		{
			const auto result = CanSleepWait(a_player, a_furniture);
			if (result && a_player->GetSitSleepState() != RE::SIT_SLEEP_STATE::kIsSitting) {
				static auto& message = Settings::GetSingleton()->tweaks.sitToWait.message;
				RE::DebugNotification(message.c_str(), "UIMenuCancel");
				return false;
			}
			return result;
		}
	};

	struct HandleWaitRequest  //no way to determine menu type with just CanWait
	{
		static void thunk(bool a_sleep)
		{
			const auto intfc = RE::InterfaceStrings::GetSingleton();

			if (RE::UI::GetSingleton()->IsMenuOpen(intfc->sleepWaitMenu)) {
				RE::UIMessageQueue::GetSingleton()->AddMessage(intfc->sleepWaitMenu, RE::UI_MESSAGE_TYPE::kHide, nullptr);
			} else if (detail::CanWait(RE::PlayerCharacter::GetSingleton(), nullptr)) {
				detail::ProcessMenu(intfc->sleepWaitMenu, RE::UI_MESSAGE_TYPE::kShow, a_sleep);
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL_ID(51400, 52249), OFFSET_3(0x394, 0x379, 0x681) };
		stl::write_thunk_call<HandleWaitRequest>(target.address());

		logger::info("Installed sit to wait tweak"sv);
	}
}

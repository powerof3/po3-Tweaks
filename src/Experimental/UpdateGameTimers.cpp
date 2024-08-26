#include "Experimental.h"

//update timers when incrementing game hour through SetValue
namespace Experimental::UpdateGameTimers
{
	struct detail
	{
		static void UpdateTimers(RE::PlayerCharacter* a_player)
		{
			using func_t = decltype(&UpdateTimers);
			static REL::Relocation<func_t> func{ REL_ID(39410, 40485) };
			return func(a_player);
		}

		static bool& GetSleeping()
		{
			REL::Relocation<bool*> sleeping{ REL_ID(509271, 381534) };
			return *sleeping;
		}

		static inline constexpr RE::FormID gameHour{ 0x38 };
		static inline constexpr RE::FormID gameDay{ 0x37 };
	};

	struct SetGlobal
	{
		static void func(RE::BSScript::IVirtualMachine* a_vm, RE::VMStackID a_stackID, RE::TESGlobal* a_global, float a_value)
		{
			if ((a_global->formFlags & RE::TESForm::RecordFlags::kGlobalConstant) != 0) {
				a_vm->TraceStack("Cannot set the value of a constant GlobalVariable", a_stackID, RE::BSScript::ErrorLogger::Severity::kError);
			} else {
				a_global->value = a_value;

				if (a_global->GetFormID() == detail::gameHour || a_global->GetFormID() == detail::gameDay) {
					detail::GetSleeping() = false;
					detail::UpdateTimers(RE::PlayerCharacter::GetSingleton());
				}
			}
		}
		static inline constexpr std::size_t size = 0x29;
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> func{ REL_ID(55352, 55923) };
		stl::asm_replace<SetGlobal>(func.address());

		logger::info("\t\tInstalled game hour timer fix"sv);
	}
}

#include "Papyrus.h"
#include "Settings.h"

namespace Papyrus
{
	bool IsTweakInstalled(VM*, StackID, RE::StaticFunctionTag*, RE::BSFixedString a_tweak)
	{
		return Settings::GetSingleton()->IsTweakInstalled(a_tweak);
	}

	bool Bind(VM* a_vm)
	{
		if (!a_vm) {
			REX::CRITICAL("couldn't get VM State"sv);
			return false;
		}

		REX::INFO("{:*^50}", "FUNCTIONS"sv);

		a_vm->RegisterFunction("IsTweakInstalled"sv, "po3_Tweaks"sv, IsTweakInstalled, true);

		REX::INFO("Registered IsTweakInstalled"sv);

		return true;
	}
}

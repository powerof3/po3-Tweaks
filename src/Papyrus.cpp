#include "Papyrus.h"
#include "Settings.h"

namespace Papyrus
{
	bool IsTweakInstalled(VM*, StackID, RE::StaticFunctionTag*, RE::BSFixedString a_tweak);
	{
		return Settings::GetSingleton()->IsTweakInstalled(a_tweak);
	}

	bool Bind(VM* a_vm)
	{
		if (!a_vm) {
			logger::critical("couldn't get VM State"sv);
			return false;
		}

		logger::info("{:*^30}", "FUNCTIONS"sv);

		a_vm.RegisterFunction("IsTweakInstalled"sv, "po3_Tweaks"sv, IsTweakInstalled, true);

		logger::info("Registered functions"sv);

		return true;
	}
}

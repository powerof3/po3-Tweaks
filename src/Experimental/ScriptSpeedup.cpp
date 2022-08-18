#include "Experimental.h"
#include "Settings.h"

//script speedup, make function non latent
namespace Experimental::ScriptSpeedup
{
	bool Speedup(RE::BSScript::Internal::VirtualMachine* a_vm)
	{
		if (!a_vm) {
			return false;
		}

		auto experimental = Settings::GetSingleton()->experimental;

		if (experimental.fastRandomInt) {
			a_vm->SetCallableFromTasklets("Utility", "RandomInt", true);
			logger::info("patched Utility.RandomInt"sv);
		}
		if (experimental.fastRandomFloat) {
			a_vm->SetCallableFromTasklets("Utility", "RandomFloat", true);
			logger::info("patched Utility.RandomFloat"sv);
		}

		return true;
	}

	void Install()
	{
		const auto papyrus = SKSE::GetPapyrusInterface();
		papyrus->Register(Speedup);
	}
}

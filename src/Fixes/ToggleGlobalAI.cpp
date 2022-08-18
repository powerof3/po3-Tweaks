#include "Fixes.h"

namespace Fixes::ToggleGlobalAI
{
	struct detail
	{
		static void ClearPath(RE::Actor* a_actor)
		{
			using func_t = decltype(&ClearPath);
			REL::Relocation<func_t> func{ REL_ID(36802, 37818) };
			return func(a_actor);
		}

		static void UpdateHighProcess(RE::ProcessLists* a_processLists, bool a_enable)
		{
			for (auto& actorHandle : a_processLists->highActorHandles) {
				if (const auto actor = actorHandle.get()) {
					if (!a_enable) {
						ClearPath(actor.get());
					}
					actor->EnableAI(a_enable);
				}
			}
		}
	};

	struct ToggleAI
	{
		static void func(RE::ProcessLists* a_processLists)
		{
			const bool runSchedules = !a_processLists->runSchedules;
			a_processLists->runSchedules = runSchedules;

			detail::UpdateHighProcess(a_processLists, runSchedules);
		}

		static inline constexpr std::size_t size{ OFFSET(0x19, 0xAC) };
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> func{ REL_ID(40317, 41327) };
		stl::asm_replace<ToggleAI>(func.address());
	}
}

#include "Fixes.h"

//multiply jump height by 1% of JumpBonus AV
//credit to KernalsEgg for implementation
namespace Fixes::RestoreJumpingBonus
{
	struct SetJumpHeightPatch
	{
		static void Install()
		{
			REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(36271, 37257), OFFSET(0x1BB, 0x1AA) };

			struct Patch : Xbyak::CodeGenerator
			{
				Patch(std::uintptr_t a_func)
				{
					Xbyak::Label f;

					mov(r8, rdi);
					jmp(ptr[rip + f]);

					L(f);
					dq(a_func);
				}
			};

			Patch patch{ reinterpret_cast<std::uintptr_t>(SetJumpHeight) };
			patch.ready();

			auto& trampoline = SKSE::GetTrampoline();
			_SetJumpHeight = trampoline.write_call<5>(target.address(), trampoline.allocate(patch));
		}

	private:
		static void SetJumpHeight(RE::bhkCharacterController* a_controller, float a_jumpHeight, RE::Actor* a_actor)
		{
			const auto jumpingBonus = 1.0f + (a_actor->GetActorValue(RE::ActorValue::kJumpingBonus) / 100.0f);
			const auto newJumpHeight = a_jumpHeight * jumpingBonus;

			return _SetJumpHeight(a_controller, newJumpHeight);
		}
		static inline REL::Relocation<void(RE::bhkCharacterController*, float)> _SetJumpHeight;
	};

	void Install()
	{
		SetJumpHeightPatch::Install();

		logger::info("\t\tInstalled jumping bonus fix"sv);
	}
}

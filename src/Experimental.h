#pragma once

#include "Settings.h"

namespace Experimental
{
	void Install();
}

//script speedup, make function non latent
namespace Script
{
	inline bool Speedup(RE::BSScript::Internal::VirtualMachine* a_vm)
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
}

//update timers when incrementing game hour through SetValue
namespace GameTimers
{
	struct detail
	{
		static void UpdateTimers(RE::PlayerCharacter* a_player)
		{
			using func_t = decltype(&UpdateTimers);
			REL::Relocation<func_t> func{ REL_ID(39410, 40485) };
			return func(a_player);
		}

		static bool& get_sleeping()
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
				if (stl::is_in(a_global->GetFormID(), detail::gameHour, detail::gameDay)) {
					detail::get_sleeping() = false;
					detail::UpdateTimers(RE::PlayerCharacter::GetSingleton());
				}
			}
		}
		static inline constexpr std::size_t size = 0x29;
	};

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> func{ REL_ID(55352, 55923) };
		stl::asm_replace<SetGlobal>(func.address());

		logger::info("Installed game hour timer fix"sv);
	}
}

//cleanup orphan active effects when character has missing ability perks that are referenced in the save
namespace CleanupOrphanedActiveEffects
{
	struct detail
	{
		static void init_ability_perk_map(std::map<RE::SpellItem*, std::set<RE::BGSPerk*>>& a_map)
		{
			if (const auto dataHandler = RE::TESDataHandler::GetSingleton(); dataHandler) {
				for (const auto& perk : dataHandler->GetFormArray<RE::BGSPerk>()) {
					for (const auto& entry : perk->perkEntries) {
						if (entry && entry->GetType() == RE::PERK_ENTRY_TYPE::kAbility) {
							if (const auto abilityEntry = static_cast<RE::BGSAbilityPerkEntry*>(entry); abilityEntry && abilityEntry->ability) {
								a_map[abilityEntry->ability].insert(perk);
							}
						}
					}
				}
			}
		}
	};

	struct ReadFromSaveGame
	{
		static void thunk(RE::Character* a_this, std::uintptr_t a_buf)
		{
			func(a_this, a_buf);

			if (a_this && !a_this->IsPlayerRef()) {
				static std::map<RE::SpellItem*, std::set<RE::BGSPerk*>> abilityPerkMap;
				if (abilityPerkMap.empty()) {
					detail::init_ability_perk_map(abilityPerkMap);
				}

				auto& addedSpells = a_this->addedSpells;
				for (auto it = addedSpells.begin(); it != addedSpells.end();) {
					bool result = false;
					if (auto spell = *it; spell && spell->GetSpellType() == RE::MagicSystem::SpellType::kAbility) {
						if (abilityPerkMap.contains(spell)) {
							const auto base = a_this->GetActorBase();
							if (base && !std::ranges::any_of(abilityPerkMap[spell], [&](const auto& perk) {
									return base->GetPerkIndex(perk).has_value();
								})) {
								result = true;
							}
						}
					}
					if (result) {
						it = addedSpells.erase(it);
					} else {
						++it;
					}
				}
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;

		static inline constexpr std::size_t idx{ 0x0F };
	};

	inline void Install()
	{
		stl::write_vfunc<RE::Character, ReadFromSaveGame>();

		logger::info("Installed orphan AE cleanup fix"sv);
	}
}

//modify timeout check for Suspended Stack flushing
namespace ModifySuspendedStackFlushTimeout
{
	namespace ModifyLimit
	{
		inline void Install(double a_milliseconds)
		{
			static REL::Relocation<std::uintptr_t> target{ REL_ID(53209, 54020), OFFSET(0x3D, 0x10D) };

			struct StackDumpTimeout_Code : Xbyak::CodeGenerator
			{
				StackDumpTimeout_Code(std::uintptr_t a_address, double a_val)
				{
					static auto VAL = static_cast<float>(a_val);

					push(rax);

					mov(rax, stl::unrestricted_cast<std::uintptr_t>(std::addressof(VAL)));
					mulss(xmm0, ptr[rax]);

					pop(rax);

					jmp(ptr[rip]);
					dq(a_address + 0x8);
				}
			};

			StackDumpTimeout_Code code(target.address(), a_milliseconds);
			code.ready();

			auto& trampoline = SKSE::GetTrampoline();
			SKSE::AllocTrampoline(38);

			trampoline.write_branch<6>(
				target.address(),
				trampoline.allocate(code));

			logger::info("set timeout on suspended stack flush to {} seconds"sv, a_milliseconds / 1000.0);
		}
	}

	namespace NoLimit
	{
		inline void Install()
		{
			static REL::Relocation<std::uintptr_t> target{ REL_ID(53209, 54020), OFFSET(0x8B, 0x152) };
			REL::safe_write(target.address(), static_cast<std::uint8_t>(0xEB));  // swap jle 0x7e for jmp 0xeb

			logger::info("Removed timeout check on suspended stack flush"sv);
		}
	}

	inline void Install()
	{
		if (const auto timeoutSeconds = Settings::GetSingleton()->experimental.stackDumpTimeoutModifier; timeoutSeconds != 30.0) {
			if (timeoutSeconds == 0.0) {
				NoLimit::Install();
			} else {
				ModifyLimit::Install(timeoutSeconds * 1000.0);
			}
		}
	}
}

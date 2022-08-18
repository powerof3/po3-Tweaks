#include "Fixes.h"

//patches IsFurnitureAnimType to work on furniture references
namespace Fixes::IsFurnitureAnimTypeForFurniture
{
	struct IsFurnitureAnimType
	{
		struct detail
		{
			static std::uint32_t GetEquippedFurnitureType(RE::Actor* a_actor)
			{
				using func_t = decltype(&GetEquippedFurnitureType);
				REL::Relocation<func_t> func{ REL_ID(36720, 37732) };
				return func(a_actor);
			}

			static std::uint32_t GetFurnitureType(const RE::TESFurniture* a_furniture)
			{
				using FLAGS = RE::TESFurniture::ActiveMarker;

				const auto flags = a_furniture->furnFlags;
				if (flags.any(FLAGS::kCanSit)) {
					return 1;
				}
				if (flags.any(FLAGS::kCanSleep)) {
					return 2;
				}
				if (flags.any(FLAGS::kCanLean)) {
					return 4;
				}
				return 0;
			}
		};

		static bool func(RE::TESObjectREFR* a_this, std::uint32_t a_type, void*, double& a_result)
		{
			a_result = 0.0;
			if (!a_this) {
				return true;
			}

			if (const auto actor = a_this->As<RE::Actor>(); actor) {
				if (detail::GetEquippedFurnitureType(actor) == a_type) {
					a_result = 1.0;
				}
			} else {
				const auto base = a_this->GetBaseObject();
				if (const auto furniture = base ? base->As<RE::TESFurniture>() : nullptr; furniture) {
					if (detail::GetFurnitureType(furniture) == a_type) {
						a_result = 1.0;
					}
				} else {
					return true;
				}
			}

			if (const auto log = RE::ConsoleLog::GetSingleton(); log && RE::ConsoleLog::IsConsoleMode()) {
				log->Print("IsFurnitureAnimType >> %0.2f", a_result);
			}

			return true;
		}
		static inline constexpr std::size_t size{ 0x87 };
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> func{ REL_ID(21211, 21668) };
		stl::asm_replace<IsFurnitureAnimType>(func.address());

		logger::info("Installed IsFurnitureAnimType fix"sv);
	}
}

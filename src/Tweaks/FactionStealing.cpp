#include "Tweaks.h"
#include "Cache.h"

//removes steal tag if all faction members have appropriate relationship rank
namespace Tweaks::FactionStealing
{
	struct detail
	{
		static std::int32_t GetRelationshipFavorPointsValue(RE::TESNPC* a_player, RE::TESNPC* a_owner)
		{
			using func_t = decltype(&GetRelationshipFavorPointsValue);
			static REL::Relocation<func_t> func{ REL_ID(23626, 24078) };
			return func(a_player, a_owner);
		}

		static bool CanTake(RE::TESNPC* a_playerBase, RE::TESNPC* a_npc, std::int32_t a_cost)
		{
			const auto favorPoints = GetRelationshipFavorPointsValue(a_playerBase, a_npc);
			return favorPoints > 1 ?
			           a_cost <= favorPoints :
			           false;
		}
	};

	struct IsFriendAnOwner
	{
		static bool func(const RE::PlayerCharacter* a_player, RE::TESForm* a_owner, std::int32_t a_cost)
		{
			if (!a_owner) {
				return false;
			}

			const auto playerBase = RE::PlayerCharacter::GetSingleton()->GetActorBase();
			if (a_owner == playerBase) {
				return true;
			}

			if (const auto npc = a_owner->As<RE::TESNPC>(); npc) {
				return detail::CanTake(playerBase, npc, a_cost);
			}

			if (const auto faction = a_owner->As<RE::TESFaction>(); faction) {
				if (a_player->IsInFaction(faction)) {
					return true;
				}

				static Map<RE::TESFaction*, std::vector<RE::TESNPC*>> factionNPCMap{};
				if (factionNPCMap.empty()) {
					for (auto& npc : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::TESNPC>()) {
						if (npc && !npc->IsDeleted()) {
							for (auto& factionRank : npc->factions) {
								if (factionRank.faction) {
									factionNPCMap[factionRank.faction].push_back(npc);
								}
							}
						}
					}
				}

				if (auto it = factionNPCMap.find(faction); it != factionNPCMap.end()) {
					return std::ranges::all_of(it->second, [&](const auto& npc) {
						return detail::CanTake(playerBase, npc, a_cost);
					});
				}
			}

			return false;
		}
#ifdef SKYRIM_AE
		//extra inlining
		static inline constexpr std::size_t size{ 0x163 };
#else
		static inline constexpr std::size_t size{ 0xAC };
#endif
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> func{ REL_ID(39584, 40670) };
		stl::asm_replace<IsFriendAnOwner>(func.address());

		logger::info("\t\tInstalled faction stealing tweak"sv);
	}
}

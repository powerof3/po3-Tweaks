#include "Fixes.h"
#include "Settings.h"

namespace RE
{
	struct PermanentMagicFunctor
	{
		PermanentMagicFunctor(MagicCaster* a_caster, Actor* a_actor) :
			caster(a_caster),
			actor(a_actor)
		{
			flags = (flags & 0xF9) | 1;
		}

		MagicCaster*  caster;
		Actor*        actor;
		std::uint8_t  isSpellType{ 0xFF };
		std::uint8_t  isNotSpellType{ 0xA };
		std::uint8_t  flags{ 0 };
		std::uint8_t  pad13{ 0 };
		std::uint32_t pad14{ 0 };

		BSContainer::ForEachResult operator()(MagicItem* a_spell)
		{
			using func_t = decltype(&PermanentMagicFunctor::operator());
			REL::Relocation<func_t> func{ REL_ID(33684, 34464) };
			return func(this, a_spell);
		}
	};
	static_assert(sizeof(PermanentMagicFunctor) == 0x18);
}

//fixes added spells not being reapplied on actor load
//fixes no death dispel spells from not being reapplied on actor load
namespace Fixes::ReapplySpellsOnLoad
{
	// Reapply added spells on 3D reload
	namespace Added
	{
		namespace Apply
		{
			struct GetAliasInstanceArray
			{
				static RE::ExtraAliasInstanceArray* thunk(RE::ExtraDataList* a_list)
				{
					auto* actor = stl::adjust_pointer<RE::Character>(a_list, -0x70);

					if (auto* caster = actor && !actor->IsPlayerRef() && !actor->addedSpells.empty() ?
                                           actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant) :
                                           nullptr) {
						RE::PermanentMagicFunctor applier{ caster, actor };

						for (const auto& spell : actor->addedSpells) {
							if (spell && applier(spell) == RE::BSContainer::ForEachResult::kStop) {
								break;
							}
						}
					}

					return func(a_list);
				}
				static inline REL::Relocation<decltype(thunk)> func;
			};

			void Install()
			{
				REL::Relocation<std::uintptr_t> target{ REL_ID(37804, 38753), 0x115 };
				stl::write_thunk_call<GetAliasInstanceArray>(target.address());
			}
		}

		namespace Dispel
		{
			struct GetAliasInstanceArray
			{
				static RE::ExtraAliasInstanceArray* thunk(RE::ExtraDataList* a_list)
				{
					auto* actor = stl::adjust_pointer<RE::Character>(a_list, -0x70);

					if (actor && !actor->IsPlayerRef() && !actor->addedSpells.empty()) {
						auto handle = RE::ActorHandle{};

						for (const auto& spell : actor->addedSpells) {
							if (spell && spell->IsValid()) {
								actor->GetMagicTarget()->DispelEffect(spell, handle);
							}
						}
					}

					return func(a_list);
				}
				static inline REL::Relocation<decltype(thunk)> func;
			};

			void Install()
			{
				REL::Relocation<std::uintptr_t> target{ REL_ID(37805, 38754), 0x131 };
				stl::write_thunk_call<GetAliasInstanceArray>(target.address());
			}
		}

		void Install()
		{
			Apply::Install();
			Dispel::Install();
		}
	}

	// Reapply no-death-dispel spells after death on 3D reload
	namespace OnDeath
	{
		struct Load3D
		{
			static RE::NiAVObject* thunk(RE::Actor* a_actor, bool a_backgroundLoading)
			{
				auto* node = func(a_actor, a_backgroundLoading);

				if (auto* caster = node && a_actor->IsDead() && !a_actor->IsPlayerRef() ?
                                       a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant) :
                                       nullptr) {
					RE::PermanentMagicFunctor applier{ caster, a_actor };

					const auto npc = a_actor->GetActorBase();
					const auto actorEffects = npc ? npc->actorEffects : nullptr;

					if (actorEffects && actorEffects->spells) {
						const std::span span(actorEffects->spells, actorEffects->numSpells);
						for (const auto& spell : span) {
							if (spell && has_no_death_dispel(*spell) && applier(spell) == RE::BSContainer::ForEachResult::kStop) {
								break;
							}
						}
					}

					if (Settings::GetSingleton()->GetFixes().addedSpell) {
						for (const auto& spell : a_actor->addedSpells) {
							if (spell && has_no_death_dispel(*spell) && applier(spell) == RE::BSContainer::ForEachResult::kStop) {
								break;
							}
						}
					}
				}

				return node;
			}
			static inline REL::Relocation<decltype(thunk)> func;

		private:
			static bool has_no_death_dispel(const RE::SpellItem& a_spell)
			{
				return std::ranges::any_of(a_spell.effects, [&](const auto& effect) {
					const auto mgef = effect ? effect->baseEffect : nullptr;
					return mgef && mgef->data.flags.any(RE::EffectSetting::EffectSettingData::Flag::kNoDeathDispel);
				});
			}
		};

		void Install()
		{
			REL::Relocation<std::uintptr_t> target{ REL_ID(36198, 37177), OFFSET(0x12, 0xD) };
			stl::write_thunk_call<Load3D>(target.address());

			logger::info("Installed no death dispel spell reapply fix"sv);
		}
	}
}

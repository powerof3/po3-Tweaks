#include "Fixes.h"
#include "Settings.h"

//fixes added spells not being reapplied on actor load
//fixes no death dispel spells from not being reapplied on actor load
namespace Fixes::ReapplySpellsOnLoad
{
	struct PermanentMagicFunctor
	{
		RE::MagicCaster* caster{ nullptr };
		RE::Actor* actor{ nullptr };
		std::uint8_t isSpellType{ 0xFF };
		std::uint8_t isNotSpellType{ 0xAF };
		std::uint8_t flags{ 0 };
		std::uint8_t pad13{ 0 };
		std::uint32_t pad14{ 0 };

		static bool Apply(PermanentMagicFunctor& a_applier, RE::SpellItem* a_spell)
		{
			using func_t = decltype(&Apply);
			REL::Relocation<func_t> func{ REL_ID(33684, 34464) };
			return func(a_applier, a_spell);
		}
	};
	static_assert(sizeof(PermanentMagicFunctor) == 0x18);

	namespace Added::Apply
	{
		struct GetAliasInstanceArray
		{
			static RE::ExtraAliasInstanceArray* thunk(RE::ExtraDataList* a_list)
			{
				const auto actor = stl::adjust_pointer<RE::Character>(a_list, -0x70);
				const auto caster = actor && !actor->IsPlayerRef() && !actor->addedSpells.empty() ?
				                        actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant) :
                                        nullptr;
				if (caster) {
					PermanentMagicFunctor applier{ caster, actor };
					applier.flags = (applier.flags & 0xF9) | 1;
					for (const auto& spell : actor->addedSpells) {
						PermanentMagicFunctor::Apply(applier, spell);
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

	namespace Added::Dispel
	{
		struct GetAliasInstanceArray
		{
			static RE::ExtraAliasInstanceArray* thunk(RE::ExtraDataList* a_list)
			{
				const auto actor = stl::adjust_pointer<RE::Character>(a_list, -0x70);
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

	namespace Added
	{
		void Install()
		{
			Apply::Install();
			Dispel::Install();
		}
	}

	namespace OnDeath
	{
		struct Load3D
		{
			static RE::NiAVObject* thunk(RE::Actor* a_actor, bool a_backgroundLoading)
			{
				const auto node = func(a_actor, a_backgroundLoading);
				const auto caster = node && a_actor->IsDead() && !a_actor->IsPlayerRef() ?
				                        a_actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant) :
                                        nullptr;
				if (caster) {
					PermanentMagicFunctor applier{ caster, a_actor };
					applier.flags = (applier.flags & 0xF9) | 1;

					const auto has_no_dispel_flag = [&](const RE::SpellItem& a_spell) {
						return std::ranges::any_of(a_spell.effects, [&](const auto& effect) {
							const auto effectSetting = effect ? effect->baseEffect : nullptr;
							return effectSetting && effectSetting->data.flags.all(RE::EffectSetting::EffectSettingData::Flag::kNoDeathDispel);
						});
					};

					const auto npc = a_actor->GetActorBase();
					const auto actorEffects = npc ? npc->actorEffects : nullptr;
					if (actorEffects && actorEffects->spells) {
						const std::span span(actorEffects->spells, actorEffects->numSpells);
						for (const auto& spell : span) {
							if (spell && has_no_dispel_flag(*spell)) {
								PermanentMagicFunctor::Apply(applier, spell);
							}
						}
					}

					if (Settings::GetSingleton()->fixes.addedSpell) {
						for (const auto& spell : a_actor->addedSpells) {
							if (spell && has_no_dispel_flag(*spell)) {
								PermanentMagicFunctor::Apply(applier, spell);
							}
						}
					}
				}
				return node;
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		void Install()
		{
			REL::Relocation<std::uintptr_t> target{ REL_ID(36198, 37177), OFFSET(0x12, 0xD) };
			stl::write_thunk_call<Load3D>(target.address());

			logger::info("Installed no death dispel spell reapply fix"sv);
		}
	}
}

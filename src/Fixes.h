#pragma once

#include "Settings.h"

namespace Fixes
{
	void Install();
}

//nullptr crash re: QueuedReference
namespace QueuedRefCrash
{
	struct SetFadeNode
	{
		static void func([[maybe_unused]] RE::TESObjectCELL* a_cell, const RE::TESObjectREFR* a_ref)
		{
			const auto root = a_ref->Get3D();
			const auto fadeNode = root ? root->AsFadeNode() : nullptr;

			if (fadeNode) {
				fadeNode->unk144 = 0;
			}
		}
		static inline constexpr std::size_t size = 0x2D;
	};

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> func{ REL::ID(18642) };
		stl::asm_replace<SetFadeNode>(func.address());

		logger::info("Installed queued ref crash fix"sv);
	}
}

//fixes not being able to place markers near POI when fast travel is disabled
namespace MapMarker
{
	struct IsFastTravelEnabled
	{
		static bool thunk(RE::PlayerCharacter* a_this, bool a_hideNotification)
		{
            const auto enabled = func(a_this, a_hideNotification);
			if (!enabled) {
                const auto UI = RE::UI::GetSingleton();
				auto mapMenu = UI ? UI->GetMenu<RE::MapMenu>() : nullptr;

				if (mapMenu) {
					mapMenu->PlaceMarker();
				}
			}
			return enabled;
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL::ID(52208), 0x2C5 };
		stl::write_thunk_call<IsFastTravelEnabled>(target.address());

		logger::info("Installed map marker placement fix"sv);
	}
}

//restores DontTake flag functionality
namespace CantTakeBook
{
	namespace Button
	{
		struct ShowTakeButton
		{
			static std::int32_t thunk(RE::GFxMovieView* a_movie, const char* a_text, RE::FxResponseArgs<2>& a_args)
			{
				const auto ref = RE::BookMenu::GetTargetReference();
				const auto book = ref ? RE::BookMenu::GetTargetForm() : nullptr;

				if (book && !book->CanBeTaken()) {
					RE::GFxValue* params = nullptr;  //param[0] = ??, param[1] = book ref exists, param[2] = stealing
					a_args.GetValues(&params);

					params[1].SetBoolean(false);
				}

				return func(a_movie, a_text, a_args);
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		inline void Install()
		{
			REL::Relocation<std::uintptr_t> target{ REL::ID(50126), 0x634 };
			stl::write_thunk_call<ShowTakeButton>(target.address());
		}
	}

    namespace Prompt
	{
		struct ProcessMessage
		{
			static RE::UI_MESSAGE_RESULTS thunk(RE::BookMenu* a_this, RE::UIMessage& a_message)
			{
				if (a_this->book3D && a_this->unk96 == 1) {
					const auto ref = RE::BookMenu::GetTargetReference();  //is not in inventory
					const auto data = ref ? static_cast<RE::BSUIMessageData*>(a_message.data) : nullptr;

					if (data && data->fixedStr.data() == RE::UserEvents::GetSingleton()->accept.data()) {  //direct BSFixedString compare causes crashes?
						if (const auto book = RE::BookMenu::GetTargetForm(); book && !book->CanBeTaken()) {
							return RE::UI_MESSAGE_RESULTS::kIgnore;
						}
					}
				}

				return func(a_this, a_message);
			}
			static inline REL::Relocation<decltype(thunk)> func;

			static inline constexpr std::size_t size = 0x04;
		};

		inline void Install()
		{
			stl::write_vfunc<RE::BookMenu, ProcessMessage>();
		}
	}

    inline void Install()
	{
		Button::Install();
		Prompt::Install();

		logger::info("Installed 'Can't Be Taken' book flag fix"sv);
	}
}

//adjusts range of projectile fired while moving
namespace ProjectileRange
{
	struct UpdateCombatThreat
	{
		static void thunk(/*RE::CombatManager::CombatThreats*/ std::uintptr_t a_threats, RE::Projectile* a_projectile)
		{
			using Type = RE::FormType;

			if (a_projectile && (a_projectile->Is(Type::ProjectileMissile) || a_projectile->Is(Type::ProjectileCone))) {
				const auto base = a_projectile->GetBaseObject();
				const auto projectileBase = base ? base->As<RE::BGSProjectile>() : nullptr;
				const auto baseSpeed = projectileBase ? projectileBase->data.speed : 0.0f;
				if (baseSpeed > 0.0f) {
					const auto velocity = a_projectile->linearVelocity;
					a_projectile->range *= velocity.Length() / baseSpeed;
				}
			}

			func(a_threats, a_projectile);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL::ID(43030), 0x3CB };
		stl::write_thunk_call<UpdateCombatThreat>(target.address());

		logger::info("Installed projectile range fix"sv);
	}
}

//fixes combat dialogue
namespace CombatDialogue
{
	struct SayCombatDialogue
	{
		static bool thunk(std::uintptr_t a_combatDialogueManager, RE::Actor* a_speaker, RE::Actor* a_target, RE::DIALOGUE_TYPE a_type, RE::DIALOGUE_DATA::Subtype a_subtype, bool a_ignoreSpeakingDone, RE::CombatController* a_combatController)
		{
			if (a_subtype == RE::DIALOGUE_DATA::Subtype::kLostToNormal && a_target && a_target->IsDead()) {
				const auto combatGroup = a_speaker ? a_speaker->GetCombatGroup() : nullptr;
				if (combatGroup && combatGroup->searchState == 0) {
					a_subtype = RE::DIALOGUE_DATA::Subtype::kCombatToNormal;
				}
			}
			return func(a_combatDialogueManager, a_speaker, a_target, a_type, a_subtype, a_ignoreSpeakingDone, a_combatController);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL::ID(43571), 0x135 };
		stl::write_thunk_call<SayCombatDialogue>(target.address());

		logger::info("Installed combat dialogue fix"sv);
	}
}

//TBD - update timers when incrementing game hour through SetValue
namespace GameHour
{
	struct detail
	{
		static void UpdateTimers(RE::PlayerCharacter* a_player)
		{
			using func_t = decltype(&UpdateTimers);
			REL::Relocation<func_t> func{ REL::ID(39410) };
			return func(a_player);
		}

		static bool& get_sleeping()
		{
			REL::Relocation<bool*> sleeping{ REL::ID(509271) };
			return *sleeping;
		}

		static inline constexpr RE::FormID gameHour{ 0x38 };
	};

	struct SetGlobal
	{
		static void func(RE::BSScript::IVirtualMachine* a_vm, RE::VMStackID a_stackID, RE::TESGlobal* a_global, float a_value)
		{
			if ((a_global->formFlags & RE::TESForm::RecordFlags::kGlobalConstant) != 0) {
				a_vm->TraceStack("Cannot set the value of a constant GlobalVariable", a_stackID, RE::BSScript::ErrorLogger::Severity::kError);
			} else {
				a_global->value = a_value;
				if (a_global->GetFormID() == detail::gameHour) {
					detail::get_sleeping() = false;
					detail::UpdateTimers(RE::PlayerCharacter::GetSingleton());
				}
			}
		}
		static inline constexpr std::size_t size = 0x29;
	};

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> func{ REL::ID(55352) };
		stl::asm_replace<SetGlobal>(func.address());
	}
}

//fixes added spells not being reapplied on actor load
//fixes no death dispel spells from not being reapplied on actor load
namespace Spells
{
	namespace detail
	{
		struct SpellApplyStruct
		{
			RE::MagicCaster* caster{ nullptr };
			RE::Actor* actor{ nullptr };
			std::uint8_t isSpellType{ 0xFF };
			std::uint8_t isNotSpellType{ 0xAF };
			std::uint8_t flags{ 0 };
			std::uint8_t pad13{ 0 };
			std::uint32_t pad14{ 0 };
		};
		static_assert(sizeof(SpellApplyStruct) == 0x18);

		inline bool ApplySpell(SpellApplyStruct& a_applier, RE::SpellItem* a_spell)
		{
			using func_t = decltype(&ApplySpell);
			REL::Relocation<func_t> func{ REL::ID(33684) };
			return func(a_applier, a_spell);
		}
	}

	namespace ReapplyAdded
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
					detail::SpellApplyStruct applier{ caster, actor };
					applier.flags = applier.flags & 0xF9 | 1;
					for (const auto& spell : actor->addedSpells) {
						ApplySpell(applier, spell);
					}
				}
				return func(a_list);
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		inline void Install()
		{
			REL::Relocation<std::uintptr_t> target{ REL::ID(37804), 0x115 };
			stl::write_thunk_call<GetAliasInstanceArray>(target.address());
		}
	}

	namespace DispelAdded
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

		inline void Install()
		{
			REL::Relocation<std::uintptr_t> target{ REL::ID(37805), 0x131 };
			stl::write_thunk_call<GetAliasInstanceArray>(target.address());
		}
	}

	namespace ReapplyOnDeath
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
					detail::SpellApplyStruct applier{ caster, a_actor };
					applier.flags = applier.flags & 0xF9 | 1;

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
                                ApplySpell(applier, spell);
							}
						}
					}

					if (Settings::GetSingleton()->fixes.addedSpell) {
						for (const auto& spell : a_actor->addedSpells) {
							if (spell && has_no_dispel_flag(*spell)) {
								ApplySpell(applier, spell);
							}
						}
					}
				}
				return node;
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		inline void Install()
		{
			REL::Relocation<std::uintptr_t> target{ REL::ID(36198), 0x12 };
			stl::write_thunk_call<Load3D>(target.address());

			logger::info("Installed no death dispel spell reapply fix"sv);
		}
	}
}

//patches IsFurnitureAnimType to work on furniture references
namespace IsFurnitureAnimTypeFix
{
	struct IsFurnitureAnimType
	{
		struct detail
		{
			static std::uint32_t GetEquippedFurnitureType(RE::Actor* a_actor)
			{
				using func_t = decltype(&GetEquippedFurnitureType);
				REL::Relocation<func_t> func{ REL::ID(36720) };
				return func(a_actor);
			}

			static std::uint32_t GetFurnitureType(const RE::TESFurniture* a_furniture)
			{
				using FLAGS = RE::TESFurniture::ActiveMarker;

				const auto flags = a_furniture->furnFlags;
				if (flags.all(FLAGS::kCanSit)) {
					return 1;
				}
				if (flags.all(FLAGS::kCanSleep)) {
					return 2;
				}
				if (flags.all(FLAGS::kCanLean)) {
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
				const auto furniture = base ? base->As<RE::TESFurniture>() : nullptr;
				if (furniture) {
					if (detail::GetFurnitureType(furniture) == a_type) {
						a_result = 1.0;
					}
				} else {
					return true;
				}
			}

			const auto log = RE::ConsoleLog::GetSingleton();
			if (log && log->IsConsoleMode()) {
				log->Print("IsFurnitureAnimType >> %0.2f", a_result);
			}

			return true;
		}
		static inline constexpr std::size_t size = 0x87;
	};

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> func{ REL::ID(21211) };
		stl::asm_replace<IsFurnitureAnimType>(func.address());

		logger::info("Installed IsFurnitureAnimType fix"sv);
	}
}

//nullptr crash re: AttachLightHitEffectVisitor
namespace AttachLightCrash
{
	struct AttachLightHitEffectVisitor
	{
		static std::uint32_t func(RE::AttachLightHitEffectVisitor* a_this, RE::ReferenceEffect* a_hitEffect)
		{
			if (a_hitEffect->Unk_3D()) {
				auto root = a_hitEffect->GetTargetRoot();
				const auto attachLightObj = root ?
                                                root->GetObjectByName(RE::FixedStrings::GetSingleton()->attachLight) :  //crash here because no null check
                                                nullptr;
				if (attachLightObj) {
					root = attachLightObj;
				}
				if (root && root != a_this->actorRoot) {
					a_this->attachLightNode = root;
				}
				if (a_this->attachLightNode) {
					return 0;
				}
			} else {
				a_this->unk18 = false;
			}
			return 1;
		}
		static inline constexpr std::size_t size = 0x86;
	};

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> func{ REL::ID(33610) };
		stl::asm_replace<AttachLightHitEffectVisitor>(func.address());

		logger::info("Installed light attach crash fix"sv);
	}
}

//conjuration spell no absorb
namespace SpellNoAbsorb
{
	inline void Install()
	{
		using Archetype = RE::EffectArchetypes::ArchetypeID;
		using SpellFlag = RE::SpellItem::SpellFlag;

		const auto dataHandler = RE::TESDataHandler::GetSingleton();
		if (dataHandler) {
			const auto settings = Settings::GetSingleton();
			const auto noConj = settings->fixes.noConjurationAbsorb;
			const auto noHostile = settings->tweaks.noHostileAbsorb;

			if (!noConj && !noHostile) {
				return;
			}

			constexpr auto is_conjuration = [](const auto& effect) {
				const auto baseEffect = effect ? effect->baseEffect : nullptr;
				return baseEffect && baseEffect->HasArchetype(Archetype::kSummonCreature);
			};

			constexpr auto is_non_hostile = [](const auto& effect) {
				const auto baseEffect = effect ? effect->baseEffect : nullptr;
				return baseEffect && !baseEffect->IsHostile() && !baseEffect->IsDetrimental();
			};

			for (const auto& spell : dataHandler->GetFormArray<RE::SpellItem>()) {
				if (spell && spell->data.flags.none(SpellFlag::kNoAbsorb)) {
					if (noConj && std::ranges::any_of(spell->effects, is_conjuration) || noHostile && std::ranges::all_of(spell->effects, is_non_hostile)) {
						spell->data.flags.set(SpellFlag::kNoAbsorb);
					}
				}
			}
		}
	}
}

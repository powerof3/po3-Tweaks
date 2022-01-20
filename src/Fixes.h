#pragma once

#include "Cache.h"
#include "Settings.h"

namespace Fixes
{
	void Install(std::uint32_t skse_version);
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
#ifndef SKYRIMVR
				fadeNode->unk144 = 0;
#else
				fadeNode->unk16C = 0;
#endif
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
				const auto mapMenu = UI ? UI->GetMenu<RE::MapMenu>() : nullptr;

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
		REL::Relocation<std::uintptr_t> target{ REL::ID(52208),
#ifndef SKYRIMVR
			0x2C5
#else
			0x358
#endif
		};
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
			REL::Relocation<std::uintptr_t> target{ REL::ID(50126),
#ifndef SKYRIMVR
				0x634
#else
				0x64a
#endif
			};
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
		REL::Relocation<std::uintptr_t> target{ REL::ID(43030),
#ifndef SKYRIMVR
			0x3CB
#else
			0x3A8
#endif
		};
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

//fixes added spells not being reapplied on actor load
//fixes no death dispel spells from not being reapplied on actor load
namespace Spells
{
	namespace detail
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
		};
		static_assert(sizeof(PermanentMagicFunctor) == 0x18);

		inline bool Apply(PermanentMagicFunctor& a_applier, RE::SpellItem* a_spell)
		{
			using func_t = decltype(&Apply);
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
					detail::PermanentMagicFunctor applier{ caster, actor };
					applier.flags = applier.flags & 0xF9 | 1;
					for (const auto& spell : actor->addedSpells) {
						Apply(applier, spell);
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
					detail::PermanentMagicFunctor applier{ caster, a_actor };
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
								Apply(applier, spell);
							}
						}
					}

					if (Settings::GetSingleton()->fixes.addedSpell.value) {
						for (const auto& spell : a_actor->addedSpells) {
							if (spell && has_no_dispel_flag(*spell)) {
								Apply(applier, spell);
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

            if (const auto log = RE::ConsoleLog::GetSingleton(); log && RE::ConsoleLog::IsConsoleMode()) {
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
			if (a_hitEffect->IsModelAttached()) {
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
			const auto noConj = settings->fixes.noConjurationAbsorb.value;
			const auto noHostile = settings->tweaks.noHostileAbsorb.value;

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

//fixes z buffer flag for non-detect life shaders
namespace EffectShaderZBufferFix
{
	inline void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL::ID(501401), 0x1C };

		constexpr std::uint8_t zeroes[] = { 0x0, 0x0, 0x0, 0x0 };
		REL::safe_write(target.address(), zeroes, 4);

		logger::info("Installed effect shader z buffer fix"sv);
	}
}

//nullptr crash re: QueuedReference
namespace ToggleCollisionFix
{
	constexpr auto no_collision_flag = static_cast<std::uint32_t>(RE::CFilter::Flag::kNoCollision);

	struct ToggleCollision
	{
		struct detail
		{
			static void ToggleGlobalCollision()
			{
				using func_t = decltype(&ToggleGlobalCollision);
				REL::Relocation<func_t> func{ REL::ID(13224) };
				return func();
			}

			static bool& get_collision_state()
			{
				REL::Relocation<bool*> collision_state{ REL::ID(514184) };
				return *collision_state;
			}
		};

		static bool func(void*, void*, RE::TESObjectREFR* a_ref)
		{
			if (a_ref) {
				bool hasCollision = a_ref->HasCollision();
				bool isActor = a_ref->Is(RE::FormType::ActorCharacter);

				if (!isActor) {
					const auto root = a_ref->Get3D();
					if (root) {
						const auto cell = a_ref->GetParentCell();
						const auto world = cell ? cell->GetbhkWorld() : nullptr;

						if (world) {
							RE::BSWriteLockGuard locker(world->worldLock);

							RE::BSVisit::TraverseScenegraphCollision(root, [&](RE::bhkNiCollisionObject* a_col) -> RE::BSVisit::BSVisitControl {
								auto& body = a_col->body;
								auto hkpBody = body ? static_cast<RE::hkpWorldObject*>(body->referencedObject.get()) : nullptr;
								if (hkpBody) {
									auto& filter = hkpBody->collidable.broadPhaseHandle.collisionFilterInfo;
									if (hasCollision) {
										filter |= no_collision_flag;
									} else {
										filter &= ~no_collision_flag;
									}
								}
								return RE::BSVisit::BSVisitControl::kContinue;
							});
						}
					}
				}

				a_ref->SetCollision(!hasCollision);

				const auto log = RE::ConsoleLog::GetSingleton();
				if (log && log->IsConsoleMode()) {
					const char* result = hasCollision ? "off" : "on";
					log->Print("%s collision %s", a_ref->GetName(), result);
				}
			} else {
				detail::ToggleGlobalCollision();

				const auto log = RE::ConsoleLog::GetSingleton();
				if (log && log->IsConsoleMode()) {
					const char* result = detail::get_collision_state() ? "Off" : "On";
					log->Print("Collision -> %s", result);
				}
			}

			return true;
		}
		static inline constexpr std::size_t size = 0x83;
	};

	struct ApplyMovementDelta
	{
		struct detail
		{
			static bool should_disable_collision(RE::Actor* a_actor, float a_delta)
			{
				const auto controller = a_actor->GetCharController();
				if (!controller) {
					return false;
				}

				const auto& collisionObj = controller->bumpedCharCollisionObject;
				if (!collisionObj) {
					return false;
				}

				auto& filter = collisionObj->collidable.broadPhaseHandle.collisionFilterInfo;
				if (filter & no_collision_flag) {
					return false;
				}

				const auto colRef = RE::TESHavokUtilities::FindCollidableRef(collisionObj->collidable);
				if (colRef && colRef->HasCollision()) {
					return false;
				}

				filter |= no_collision_flag;

				func(a_actor, a_delta);

				filter &= ~no_collision_flag;

				return true;
			}
		};

		static void thunk(RE::Actor* a_actor, float a_delta)
		{
			if (!detail::should_disable_collision(a_actor, a_delta)) {
				return func(a_actor, a_delta);
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL::ID(36359), 0xF0 };
		stl::write_thunk_call<ApplyMovementDelta>(target.address());

		REL::Relocation<std::uintptr_t> func{ REL::ID(22350) };
		stl::asm_replace<ToggleCollision>(func.address());

		logger::info("Installed toggle collision fix"sv);
	}
}

namespace SkinnedDecalDeleteFix
{
	struct RemoveItem
	{
		static RE::NiPointer<RE::BSTempEffect>*& thunk(RE::BSTArray<RE::NiPointer<RE::BSTempEffect>>& a_this, RE::NiPointer<RE::BSTempEffect>*& a_return, RE::NiPointer<RE::BSTempEffect>*& a_item)
		{
			auto& result = func(a_this, a_return, a_item);

			if (a_item) {
				auto decal = (*a_item)->As<RE::BSTempEffectGeometryDecal>();
				auto decalNode = decal ? decal->decalNode : nullptr;
				if (decalNode && decalNode->parent) {
					decalNode->parent->DetachChild(decalNode.get());
					auto& count = RE::BGSDecalManager::GetSingleton()->skinDecalCount;
					if (count > 0) {
						--count;
					}
				}
			}

			return result;
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	inline void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL::ID(15118), 0x12B };
		stl::write_thunk_call<RemoveItem>(target.address());

		logger::info("Installed skinned decal delete fix"sv);
	}
}

namespace LoadFormEditorIDs
{
	struct detail
	{
		static void add_to_game_map(RE::TESForm* a_form, const char* a_str)
		{
			const auto& [map, lock] = RE::TESForm::GetAllFormsByEditorID();
			const RE::BSWriteLockGuard locker{ lock };
			if (map) {
				map->emplace(a_str, a_form);
			}
		}

		static void cache_editorID(RE::TESForm* a_form, const char* a_str)
		{
			Cache::EditorID::GetSingleton()->CacheEditorID(a_form, a_str);
		}
	};

	struct SetFormEditorID
	{
		static bool thunk(RE::TESForm* a_this, const char* a_str)
		{
			if (!a_this->IsDynamicForm() && !string::is_empty(a_str)) {
				detail::add_to_game_map(a_this, a_str);
			}
			return func(a_this, a_str);
		}
		static inline REL::Relocation<decltype(thunk)> func;

		static inline size_t size = 0x33;
	};

	struct SetFormEditorID_Cache
	{
		static bool thunk(RE::TESForm* a_this, const char* a_str)
		{
			if (!a_this->IsDynamicForm() && !string::is_empty(a_str)) {
				detail::add_to_game_map(a_this, a_str);
				detail::cache_editorID(a_this, a_str);
			}
			return func(a_this, a_str);
		}
		static inline REL::Relocation<decltype(thunk)> func;

		static inline size_t size = 0x33;
	};

	inline void Install()
	{
		//stl::write_vfunc<RE::TESForm, SetFormEditorID>();
		//stl::write_vfunc<RE::BGSKeyword, SetFormEditorID>();
		//stl::write_vfunc<RE::BGSLocationRefType, SetFormEditorID>();
		//stl::write_vfunc<RE::BGSAction, SetFormEditorID>();
		stl::write_vfunc<RE::BGSTextureSet, SetFormEditorID>();
		//stl::write_vfunc<RE::BGSMenuIcon, SetFormEditorID>();
		//stl::write_vfunc<RE::TESGlobal, SetFormEditorID>();
		stl::write_vfunc<RE::TESClass, SetFormEditorID>();
		stl::write_vfunc<RE::TESFaction, SetFormEditorID>();
		//stl::write_vfunc<RE::BGSHeadPart, SetFormEditorID>();
		stl::write_vfunc<RE::TESEyes, SetFormEditorID>();
		//stl::write_vfunc<RE::TESRace, SetFormEditorID>();
		//stl::write_vfunc<RE::TESSound, SetFormEditorID>();
		stl::write_vfunc<RE::BGSAcousticSpace, SetFormEditorID>();
		stl::write_vfunc<RE::EffectSetting, SetFormEditorID>();
		//stl::write_vfunc<RE::Script, SetFormEditorID>();
		stl::write_vfunc<RE::TESLandTexture, SetFormEditorID>();
		stl::write_vfunc<RE::EnchantmentItem, SetFormEditorID>();
		stl::write_vfunc<RE::SpellItem, SetFormEditorID>();
		stl::write_vfunc<RE::ScrollItem, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectACTI, SetFormEditorID>();
		stl::write_vfunc<RE::BGSTalkingActivator, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectARMO, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectBOOK, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectCONT, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectDOOR, SetFormEditorID>();
		stl::write_vfunc<RE::IngredientItem, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectLIGH, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectMISC, SetFormEditorID>();
		stl::write_vfunc<RE::BGSApparatus, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectSTAT, SetFormEditorID>();
		stl::write_vfunc<RE::BGSStaticCollection, SetFormEditorID>();

		//does not directly inherit from TESForm for some godforsaken reason
		stl::write_vfunc<RE::BGSMovableStatic, 2, SetFormEditorID>();

		stl::write_vfunc<RE::TESGrass, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectTREE, SetFormEditorID>();
		stl::write_vfunc<RE::TESFlora, SetFormEditorID>();
		stl::write_vfunc<RE::TESFurniture, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectWEAP, SetFormEditorID>();
		stl::write_vfunc<RE::TESAmmo, SetFormEditorID>();
		stl::write_vfunc<RE::TESNPC, SetFormEditorID>();
		stl::write_vfunc<RE::TESLevCharacter, SetFormEditorID>();
		stl::write_vfunc<RE::TESKey, SetFormEditorID>();
		stl::write_vfunc<RE::AlchemyItem, SetFormEditorID>();
		stl::write_vfunc<RE::BGSIdleMarker, SetFormEditorID>();
		stl::write_vfunc<RE::BGSNote, SetFormEditorID>();
		stl::write_vfunc<RE::BGSConstructibleObject, SetFormEditorID>();
		stl::write_vfunc<RE::BGSProjectile, SetFormEditorID>();
		stl::write_vfunc<RE::BGSHazard, SetFormEditorID>();
		stl::write_vfunc<RE::TESSoulGem, SetFormEditorID>();
		stl::write_vfunc<RE::TESLevItem, SetFormEditorID>();
		stl::write_vfunc<RE::TESWeather, SetFormEditorID>();
		stl::write_vfunc<RE::TESClimate, SetFormEditorID>();
		stl::write_vfunc<RE::BGSShaderParticleGeometryData, SetFormEditorID>();
		stl::write_vfunc<RE::BGSReferenceEffect, SetFormEditorID>();
		stl::write_vfunc<RE::TESRegion, SetFormEditorID>();
		//stl::write_vfunc<RE::NavMeshInfoMap, SetFormEditorID>();
		//stl::write_vfunc<RE::TESObjectCELL, SetFormEditorID>();

		stl::write_vfunc<RE::TESObjectREFR, SetFormEditorID>();
		stl::write_vfunc<RE::Actor, SetFormEditorID>();
		stl::write_vfunc<RE::Character, SetFormEditorID>();
		stl::write_vfunc<RE::PlayerCharacter, SetFormEditorID>();
		stl::write_vfunc<RE::MissileProjectile, SetFormEditorID>();
		stl::write_vfunc<RE::ArrowProjectile, SetFormEditorID>();
		stl::write_vfunc<RE::GrenadeProjectile, SetFormEditorID>();
		stl::write_vfunc<RE::BeamProjectile, SetFormEditorID>();
		stl::write_vfunc<RE::FlameProjectile, SetFormEditorID>();
		stl::write_vfunc<RE::ConeProjectile, SetFormEditorID>();
		stl::write_vfunc<RE::BarrierProjectile, SetFormEditorID>();
		stl::write_vfunc<RE::Hazard, SetFormEditorID>();

		//stl::write_vfunc<RE::TESWorldSpace, SetFormEditorID>();
		//stl::write_vfunc<RE::TESObjectLAND, SetFormEditorID>();
		//stl::write_vfunc<RE::NavMesh, SetFormEditorID>();
		//stl::write_vfunc<RE::TESTopic, SetFormEditorID>();
		stl::write_vfunc<RE::TESTopicInfo, SetFormEditorID>();
		//stl::write_vfunc<RE::TESQuest, SetFormEditorID>();
		//stl::write_vfunc<RE::TESIdleForm, SetFormEditorID>();
		stl::write_vfunc<RE::TESPackage, SetFormEditorID>();
		stl::write_vfunc<RE::DialoguePackage, SetFormEditorID>();
		stl::write_vfunc<RE::TESCombatStyle, SetFormEditorID>();
		stl::write_vfunc<RE::TESLoadScreen, SetFormEditorID>();
		stl::write_vfunc<RE::TESLevSpell, SetFormEditorID>();
		//stl::write_vfunc<RE::TESObjectANIO, SetFormEditorID>();
		stl::write_vfunc<RE::TESWaterForm, SetFormEditorID>();
		stl::write_vfunc<RE::TESEffectShader, SetFormEditorID>();
		stl::write_vfunc<RE::BGSExplosion, SetFormEditorID>();
		stl::write_vfunc<RE::BGSDebris, SetFormEditorID>();
		stl::write_vfunc<RE::TESImageSpace, SetFormEditorID>();
		//stl::write_vfunc<RE::TESImageSpaceModifier, SetFormEditorID>();
		stl::write_vfunc<RE::BGSListForm, SetFormEditorID>();
		stl::write_vfunc<RE::BGSPerk, SetFormEditorID>();
		stl::write_vfunc<RE::BGSBodyPartData, SetFormEditorID>();
		stl::write_vfunc<RE::BGSAddonNode, SetFormEditorID>();
		stl::write_vfunc<RE::ActorValueInfo, SetFormEditorID>();
		stl::write_vfunc<RE::BGSCameraShot, SetFormEditorID>();
		stl::write_vfunc<RE::BGSCameraPath, SetFormEditorID>();
		//stl::write_vfunc<RE::BGSVoiceType, SetFormEditorID>();
		stl::write_vfunc<RE::BGSMaterialType, SetFormEditorID>();
		stl::write_vfunc<RE::BGSImpactData, SetFormEditorID>();
		stl::write_vfunc<RE::BGSImpactDataSet, SetFormEditorID>();
		stl::write_vfunc<RE::TESObjectARMA, SetFormEditorID>();
		stl::write_vfunc<RE::BGSEncounterZone, SetFormEditorID>();
		stl::write_vfunc<RE::BGSLocation, SetFormEditorID>();
		stl::write_vfunc<RE::BGSMessage, SetFormEditorID>();
		//stl::write_vfunc<RE::BGSRagdoll, SetFormEditorID>();
		//stl::write_vfunc<RE::BGSDefaultObjectManager, SetFormEditorID>();
		stl::write_vfunc<RE::BGSLightingTemplate, SetFormEditorID>();
		//stl::write_vfunc<RE::BGSMusicType, SetFormEditorID>();
		stl::write_vfunc<RE::BGSFootstep, SetFormEditorID>();
		stl::write_vfunc<RE::BGSFootstepSet, SetFormEditorID>();
		//stl::write_vfunc<RE::BGSStoryManagerBranchNode, SetFormEditorID>();
		//stl::write_vfunc<RE::BGSStoryManagerQuestNode, SetFormEditorID>();
		//stl::write_vfunc<RE::BGSStoryManagerEventNode, SetFormEditorID>();
		stl::write_vfunc<RE::BGSDialogueBranch, SetFormEditorID>();
		stl::write_vfunc<RE::BGSMusicTrackFormWrapper, SetFormEditorID>();
		stl::write_vfunc<RE::TESWordOfPower, SetFormEditorID>();
		stl::write_vfunc<RE::TESShout, SetFormEditorID>();
		stl::write_vfunc<RE::BGSEquipSlot, SetFormEditorID>();
		stl::write_vfunc<RE::BGSRelationship, SetFormEditorID>();
		stl::write_vfunc<RE::BGSScene, SetFormEditorID>();
		stl::write_vfunc<RE::BGSAssociationType, SetFormEditorID>();
		stl::write_vfunc<RE::BGSOutfit, SetFormEditorID>();
		stl::write_vfunc<RE::BGSArtObject, SetFormEditorID>();

		stl::write_vfunc<RE::BGSMaterialObject, SetFormEditorID_Cache>();

		stl::write_vfunc<RE::BGSMovementType, SetFormEditorID>();
		//stl::write_vfunc<RE::BGSSoundDescriptorForm, SetFormEditorID>();
		stl::write_vfunc<RE::BGSDualCastData, SetFormEditorID>();
		stl::write_vfunc<RE::BGSSoundCategory, SetFormEditorID>();
		stl::write_vfunc<RE::BGSSoundOutput, SetFormEditorID>();
		stl::write_vfunc<RE::BGSCollisionLayer, SetFormEditorID>();
		stl::write_vfunc<RE::BGSColorForm, SetFormEditorID>();
		stl::write_vfunc<RE::BGSReverbParameters, SetFormEditorID>();
		stl::write_vfunc<RE::BGSLensFlare, SetFormEditorID>();

		logger::info("Installed editorID cache"sv);
	}
}

#ifdef SKYRIMVR
//fixes VR CrosshairRefEvent and GetCurrentCrosshairRef to also take the hand selection
//thanks to @adamhynek for help with offsets and fixing stupid bugs
namespace FixCrosshairRefEvent
{
	struct LookupByHandle
	{
		static bool thunk(RefHandle& a_refHandle, NiPointer<TESObjectREFR>& a_refrOut)
		{
			bool result = func(a_refHandle, a_refrOut);
			if (!result)
				return result;
			if (patchSKSE && a_refrOut && a_refrOut->AsReference())
				REL::safe_write<std::uintptr_t>((std::uintptr_t)(sksevr_base + 0x15D9F0), (std::uint64_t)a_refrOut->AsReference());
			const SKSE::CrosshairRefEvent event{ a_refrOut };
			RE::BSTEventSource<SKSE::CrosshairRefEvent>* source = SKSE::GetCrosshairRefEventSource();
			if (source) {
				source->SendEvent(std::addressof(event));
			}
			return result;
		}
		static inline REL::Relocation<decltype(thunk)> func;
		static inline std::uintptr_t sksevr_base;
		static inline bool patchSKSE = false;
	};

	inline void Install(std::uint32_t skse_version)
	{
		LookupByHandle::sksevr_base = reinterpret_cast<uintptr_t>(GetModuleHandleA("sksevr_1_4_15"));
		if (skse_version == 33554624) {  //2.0.12
			LookupByHandle::patchSKSE = true;
			logger::info("VR CrosshairRefEvent: Found patchable sksevr_1_4_15.dll version {} with base {}", skse_version, LookupByHandle::sksevr_base);
		} else
			logger::info("VR CrosshairRefEvent: Found uknown sksevr_1_4_15.dll version {} with base {}; not patching", skse_version, LookupByHandle::sksevr_base);
		REL::Relocation<std::uintptr_t> target{ REL::Offset(0x6D2F82) };
		stl::write_thunk_call<LookupByHandle>(target.address());

		logger::info("Installed VR CrosshairRefEvent fix"sv);
	}

}
#endif

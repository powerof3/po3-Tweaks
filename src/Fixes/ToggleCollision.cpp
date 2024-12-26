#include "Fixes.h"

//adds selective collision toggle on console reference
namespace Fixes::ToggleCollision
{
	constexpr auto no_collision_flag = static_cast<std::uint32_t>(RE::CFilter::Flag::kNoCollision);

	struct ToggleCollision
	{
		struct detail
		{
			static void ToggleGlobalCollision()
			{
				using func_t = decltype(&ToggleGlobalCollision);
				static REL::Relocation<func_t> func{ RELOCATION_ID(13224, 13375) };
				func();
			}

			static bool GetCollisionState()
			{
				REL::Relocation<bool*> collision_state{ RELOCATION_ID(514184, 400334) };
				return *collision_state;
			}

			static void ToggleRefCollision(RE::TESObjectREFR* a_ref, bool a_disable)
			{
				if (const auto root = a_ref->Get3D(); root) {
					const auto cell = a_ref->GetParentCell();

					if (const auto world = cell ? cell->GetbhkWorld() : nullptr) {
						RE::BSWriteLockGuard locker(world->worldLock);

						RE::BSVisit::TraverseScenegraphCollision(root, [&](RE::bhkNiCollisionObject* a_col) -> RE::BSVisit::BSVisitControl {
							if (auto hkpBody = a_col->body ? static_cast<RE::hkpWorldObject*>(a_col->body->referencedObject.get()) : nullptr; hkpBody) {
								auto& filter = hkpBody->collidable.broadPhaseHandle.collisionFilterInfo;
								if (a_disable) {
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
		};

		static bool func(void*, void*, RE::TESObjectREFR* a_ref)
		{
			if (a_ref) {
				const bool hasCollision = a_ref->HasCollision();
				detail::ToggleRefCollision(a_ref, hasCollision);
				a_ref->SetCollision(!hasCollision);

				if (const auto log = RE::ConsoleLog::GetSingleton(); log && RE::ConsoleLog::IsConsoleMode()) {
					log->Print("%s collision -> %s", a_ref->GetDisplayFullName(), !a_ref->HasCollision() ? "Off" : "On");
				}
			} else {
				detail::ToggleGlobalCollision();

				if (const auto log = RE::ConsoleLog::GetSingleton(); log && RE::ConsoleLog::IsConsoleMode()) {
					log->Print("Collision -> %s", detail::GetCollisionState() ? "Off" : "On");
				}
			}

			return true;
		}
		static constexpr std::size_t size{ 0x83 };
	};

	struct Actor_UpdateMovement_Primary
	{
		struct detail
		{
			static bool should_disable_collision(RE::Actor* a_actor, float a_delta)
			{
				const auto charController = a_actor->GetCharController();
				if (!charController) {
					return false;
				}

				const auto& bumpedColObj = charController->bumpedCharCollisionObject;
				if (!bumpedColObj) {
					return false;
				}

				auto& filter = bumpedColObj->collidable.broadPhaseHandle.collisionFilterInfo;
				if (filter & no_collision_flag) {
					return false;
				}

				if (const auto bumpedColRef = RE::TESHavokUtilities::FindCollidableRef(bumpedColObj->collidable); bumpedColRef && bumpedColRef->HasCollision()) {
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
				func(a_actor, a_delta);
			}
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(36359, 37350), OFFSET(0xF0, 0xFB) };  // Actor::UpdateMovement
		stl::write_thunk_call<Actor_UpdateMovement_Primary>(target.address());

		REL::Relocation<std::uintptr_t> func{ RELOCATION_ID(22350, 22825) };
		stl::asm_replace<ToggleCollision>(func.address());

		logger::info("\t\tInstalled toggle collision fix"sv);
	}
}

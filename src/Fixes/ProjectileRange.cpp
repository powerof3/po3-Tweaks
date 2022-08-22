#include "Fixes.h"

//adjusts range of projectile fired while moving
namespace Fixes::ProjectileRange
{
	struct UpdateCombatThreat
	{
		static void thunk(RE::CombatThreatMap* a_threatMap, RE::Projectile* a_projectile)
		{
			if (a_projectile && a_projectile->Is(RE::FormType::ProjectileMissile, RE::FormType::ProjectileCone)) {
				const auto base = a_projectile->GetBaseObject();
				const auto projectileBase = base ? base->As<RE::BGSProjectile>() : nullptr;
				const auto baseSpeed = projectileBase ? projectileBase->data.speed : 0.0f;

				if (baseSpeed > 0.0f) {
					a_projectile->range *= a_projectile->linearVelocity.Length() / baseSpeed;
				}
			}

			func(a_threatMap, a_projectile);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL_ID(43030, 44222), OFFSET_3(0x3CB, 0x79D, 0x3A8) };
		stl::write_thunk_call<UpdateCombatThreat>(target.address());

		logger::info("Installed projectile range fix"sv);
	}
}

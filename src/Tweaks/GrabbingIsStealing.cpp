#include "Tweaks.h"

//send steal alarm when grabbing owned items
namespace Tweaks::GrabbingIsStealing
{
	class GrabReleaseHandler final :
		public REX::Singleton<GrabReleaseHandler>,
		public RE::BSTEventSink<RE::TESGrabReleaseEvent>
	{
	public:
		using EventResult = RE::BSEventNotifyControl;

		EventResult ProcessEvent(const RE::TESGrabReleaseEvent* a_event, RE::BSTEventSource<RE::TESGrabReleaseEvent>*) override
		{
			const auto ref = a_event && a_event->grabbed ?
			                     a_event->ref :
			                     RE::TESObjectREFRPtr();

			const auto player = RE::PlayerCharacter::GetSingleton();

			if (ref && !ref->IsAnOwner(player, true, false)) {
				if (const auto base = ref->GetObjectReference(); base && base->IsInventoryObject()) {
					const auto numItems = ref->extraList.GetCount();
					const auto owner = ref->GetOwner();

					player->StealAlarm(ref.get(), base, numItems, 0, owner, true);
				}
			}

			return EventResult::kContinue;
		}
	};

	void Install()
	{
		if (auto scripts = RE::ScriptEventSourceHolder::GetSingleton()) {
			scripts->AddEventSink(GrabReleaseHandler::GetSingleton());
			logger::info("\t\tInstalled Grabbing Is Stealing tweak"sv);
		}
	}
}

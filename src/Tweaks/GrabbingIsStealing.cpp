#include "Tweaks.h"

//send steal alarm when grabbing owned items
namespace Tweaks::GrabbingIsStealing
{
	class GrabReleaseHandler final : public RE::BSTEventSink<RE::TESGrabReleaseEvent>
	{
	public:
		[[nodiscard]] static GrabReleaseHandler* GetSingleton()
		{
			static GrabReleaseHandler singleton;
			return std::addressof(singleton);
		}

	protected:
		using EventResult = RE::BSEventNotifyControl;

		EventResult ProcessEvent(const RE::TESGrabReleaseEvent* a_event, RE::BSTEventSource<RE::TESGrabReleaseEvent>*) override
		{
			const auto ref = a_event && a_event->grabbed ?
                                 a_event->ref :
                                 RE::TESObjectREFRPtr();

			const auto player = RE::PlayerCharacter::GetSingleton();

			if (ref && !ref->IsAnOwner(player, true, false)) {
				if (const auto base = ref->GetObjectReference(); base && base->IsNot(RE::FormType::Door)) {
					const auto numItems = ref->extraList.GetCount();
					const auto owner = ref->GetOwner();

					player->StealAlarm(ref.get(), base, numItems, 0, owner, true);
				}
			}

			return EventResult::kContinue;
		}

	private:
		GrabReleaseHandler() = default;
		GrabReleaseHandler(const GrabReleaseHandler&) = delete;
		GrabReleaseHandler(GrabReleaseHandler&&) = delete;

		~GrabReleaseHandler() override = default;

		GrabReleaseHandler& operator=(const GrabReleaseHandler&) = delete;
		GrabReleaseHandler& operator=(GrabReleaseHandler&&) = delete;
	};

	void Install()
	{
		if (auto scripts = RE::ScriptEventSourceHolder::GetSingleton()) {
			scripts->AddEventSink(GrabReleaseHandler::GetSingleton());
			logger::info("Installed Grabbing Is Stealing tweak"sv);
		}
	}
}

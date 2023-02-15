#include "Fixes.h"

//update breathing sounds during cell detach/attach
namespace Fixes::BreathingSounds
{
	class CellAttachDetachHandler final : public RE::BSTEventSink<RE::TESCellAttachDetachEvent>
	{
	public:
		[[nodiscard]] static CellAttachDetachHandler* GetSingleton()
		{
			static CellAttachDetachHandler singleton;
			return std::addressof(singleton);
		}

	protected:
		using EventResult = RE::BSEventNotifyControl;

		EventResult ProcessEvent(const RE::TESCellAttachDetachEvent* a_event, RE::BSTEventSource<RE::TESCellAttachDetachEvent>*) override
		{
			const auto actor = a_event && a_event->reference ?
			                     a_event->reference->As<RE::Actor>() :
			                     nullptr;

		    if (actor) {
				if (const auto awakeSound = actor->extraList.GetByType<RE::ExtraCreatureAwakeSound>()) {
					if (awakeSound->handle.IsValid()) {
						const bool isPlaying = awakeSound->handle.IsPlaying();
						if (!a_event->attached) {
							if (isPlaying) {
								awakeSound->handle.Stop();
							}
						} else {
							//if root exists, manually update awake sound
							// otherwise Actor::Load3D does this for us
						    if (const auto root = actor->Get3D(); root && !isPlaying) {
								actor->UpdateAwakeSound(root);
							}
						}
					}
				}
			}

			return EventResult::kContinue;
		}

	private:
		CellAttachDetachHandler() = default;
		CellAttachDetachHandler(const CellAttachDetachHandler&) = delete;
		CellAttachDetachHandler(CellAttachDetachHandler&&) = delete;

		~CellAttachDetachHandler() override = default;

		CellAttachDetachHandler& operator=(const CellAttachDetachHandler&) = delete;
		CellAttachDetachHandler& operator=(CellAttachDetachHandler&&) = delete;
	};

	void Install()
	{
		if (auto scripts = RE::ScriptEventSourceHolder::GetSingleton()) {
			scripts->AddEventSink(CellAttachDetachHandler::GetSingleton());
			logger::info("Installed Breathing Sound Fix"sv);
		}
	}
}

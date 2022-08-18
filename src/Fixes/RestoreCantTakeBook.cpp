#include "Fixes.h"

//restores DontTake flag functionality
namespace Fixes::RestoreCantTakeBook
{
	namespace BlockButton
	{
		struct ShowTakeButton
		{
			static std::int32_t thunk(RE::GFxMovieView* a_movie, const char* a_text, RE::FxResponseArgs<2>& a_args)
			{
				const auto ref = RE::BookMenu::GetTargetReference();
				const auto book = ref ? RE::BookMenu::GetTargetForm() : nullptr;

				if (book && !book->CanBeTaken()) {
					RE::GFxValue* params{ nullptr };  //param[0] = ??, param[1] = book ref exists, param[2] = stealing
					a_args.GetValues(&params);

					params[1].SetBoolean(false);
				}

				return func(a_movie, a_text, a_args);
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		void Install()
		{
			REL::Relocation<std::uintptr_t> target{ REL_ID(50126, 51057), OFFSET_3(0x634, 0x636, 0x64A) };
			stl::write_thunk_call<ShowTakeButton>(target.address());
		}
	}

	namespace BlockPrompt
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

			static inline constexpr std::size_t idx{ 0x04 };
		};

		void Install()
		{
			stl::write_vfunc<RE::BookMenu, ProcessMessage>();
		}
	}

	void Install()
	{
		BlockButton::Install();
		BlockPrompt::Install();

		logger::info("Installed 'Can't Be Taken' book flag fix"sv);
	}
}

#include "TestUE4_01UI.h"
#include "TestUE4_01Character.h"
#include "Editor/EditorStyle/Public/EditorStyleSet.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SSpinBox.h"

class STestUE4_01UI : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(STestUE4_01UI)
	{
	}
	SLATE_END_ARGS()

		STestUE4_01UI()
	{
	}

	void Construct(const FArguments& Args)
	{
		ChildSlot
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				GenerateProjectileRatio()
			]
		];
	}

	TSharedRef<SWidget> GenerateProjectileRatio() const
	{
		const float PercentMin = 0.0f;
		const float PercentMax = 100.0f;

		TSharedRef<SWidget> WidgetRatioBox = SNew(SBox)
			.HAlign(HAlign_Right)
			[
				SNew(SBox)
				.Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
				.WidthOverride(100.0f)
				[
					SNew(SSpinBox<int32>)
					.Font(FEditorStyle::GetFontStyle(TEXT("MenuItem.Font")))
					.MinValue(PercentMin)
					.MaxValue(PercentMax)
					.Value(this, &STestUE4_01UI::GetProgressSkill_2)
				]
			];
		return WidgetRatioBox;
	}

	int GetProgressSkill_2() const
	{
		return ATestUE4_01Character::GetProgressSkill_2();
	}
};

void FTestUE4_01UI::InitTestUI()
{
	GEngine->GameViewport->AddViewportWidgetContent(SNew(STestUE4_01UI));
}

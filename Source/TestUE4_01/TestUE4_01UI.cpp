#include "TestUE4_01UI.h"
#include "TestUE4_01Character.h"
#include "TestProjectile01.h"
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

	FSlateFontInfo FontStyle = FEditorStyle::GetFontStyle(TEXT("MenuItem.Font"));
	FSlateFontInfo FontStyleText = FEditorStyle::GetFontStyle(TEXT("MenuItem.Font"));
	FTextBlockStyle BlockStyle;

	void Construct(const FArguments& Args)
	{
		FontStyle.Size = 50.0f;
		FontStyleText.Size = 20.0f;
		BlockStyle.SetFont(FontStyle);
		BlockStyle.SetHighlightColor(FLinearColor::White);
		ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(FMargin(10.0f, 10.0f, 10.0f, 10.0f))
			[
				GenerateProjectileText()
			]
			+ SVerticalBox::Slot()
			.Padding(FMargin(10.0f, 10.0f, 10.0f, 10.0f))
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				GenerateProjectileRatio()
			]
			+ SVerticalBox::Slot()
			.Padding(FMargin(10.0f, 10.0f, 10.0f, 10.0f))
			[
				GenerateProjectileCount()
			]
			+ SVerticalBox::Slot()
			.Padding(FMargin(10.0f, 10.0f, 10.0f, 10.0f))
			[
				GenerateDestroyBtn()
			]
		];
	}

	TSharedRef<SWidget> GenerateProjectileText() const
	{
		const float PercentMin = 0.0f;
		const float PercentMax = 100.0f;

		FText TextHelp = FText::FromString(TEXT("Move : ( left, right )\nJump : ( Space )\nSkill : ( Q, Q delay, Q+W, A )"));

		TSharedRef<SWidget> WidgetTextBlock = SNew(SBox)
			.WidthOverride(400)
			.HeightOverride(100)
			.HAlign(HAlign_Center)
			[
				SNew(SBox)
				.Padding(FMargin(4.0f, 10.0f, 0.0f, 0.0f))
				.WidthOverride(400)
				[
					SNew(STextBlock)
					.Font(FontStyleText)
					.Text(TextHelp)
				]
			];
		return WidgetTextBlock;
	}

	TSharedRef<SWidget> GenerateProjectileRatio() const
	{
		const float PercentMin = 0.0f;
		const float PercentMax = 100.0f;

		TSharedRef<SWidget> WidgetRatioBox = SNew(SBox)
			.WidthOverride(400)
			.HeightOverride(100)
			.HAlign(HAlign_Right)
			[
				SNew(SBox)
				.Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
				.WidthOverride(400)
				[
					SNew(SSpinBox<int32>)
					.Font(FontStyle)
					.MinValue(PercentMin)
					.MaxValue(PercentMax)
					.Value(this, &STestUE4_01UI::GetProgressSkill_2)
				]
			];
		return WidgetRatioBox;
	}

	TSharedRef<SWidget> GenerateProjectileCount() const
	{
		const float PercentMin = 0.0f;
		const float PercentMax = 100.0f;

		TSharedRef<SWidget> WidgetTextBlock = SNew(SBox)
			.WidthOverride(400)
			.HeightOverride(100)
			.HAlign(HAlign_Right)
			[
				SNew(SBox)
				.Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
				.WidthOverride(400)
				[
					SNew(STextBlock)
					.Font(FontStyle)
					.Text(this, &STestUE4_01UI::GetProjectileCount)
				]
			];
		return WidgetTextBlock;
	}

	TSharedRef<SWidget> GenerateDestroyBtn() const
	{
		TSharedRef<SWidget> WidgetTextBlock = SNew(SBox)
			.WidthOverride(400)
			.HeightOverride(50)
			.HAlign(HAlign_Right)
			[
				SNew(SBox)
				.Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
				.WidthOverride(400)
				.HeightOverride(50)
				.HAlign(HAlign_Center)
				[
					SNew(SButton)
					.TextStyle(&BlockStyle)
					.Text(FText::FromString(TEXT("Reset(R)")))
					.OnClicked(this, &STestUE4_01UI::OnClickDestroyProjectileAll)
				]
			];
		return WidgetTextBlock;
	}

	int GetProgressSkill_2() const
	{
		return ATestUE4_01Character::GetProgressSkill_2();
	}

	FText GetProjectileCount() const
	{
		FText TextCount;
		FString CountString = FString::Printf(TEXT("Count:%d"), ATestProjectile01::Projectiles.Num());
		TextCount = FText::FromString(CountString);
		return TextCount;
	}

	FReply OnClickDestroyProjectileAll() const
	{
		ATestProjectile01::DestroyRequestAll();
		return FReply::Handled();
	}
};

void FTestUE4_01UI::InitTestUI()
{
	GEngine->GameViewport->AddViewportWidgetContent(SNew(STestUE4_01UI));
}

#include "UI/Popup.hpp"
#include "ObjectModel/Axis.hpp"

#include "General/SimpleMath.h"

// Create a standard popup window with a title and a close button at the top right
StandardPopupWindow::StandardPopupWindow(PixelNumber ph, PixelNumber pw, Colour pb, Colour pBorder, Colour textColour, Colour imageBackColour, const char * null title, PixelNumber topMargin)
	: PopupWindow(ph, pw, pb, pBorder), titleField(nullptr)
{
	DisplayField::SetDefaultColours(textColour, pb);
	if (title != nullptr)
	{
		AddField(titleField = new StaticTextField(topMargin + labelRowAdjust, popupSideMargin + closeButtonWidth + popupFieldSpacing,
							pw - 2 * (popupSideMargin + closeButtonWidth + popupFieldSpacing), TextAlignment::Centre, title));
	}
	else
	{
		titleField = nullptr;
	}
	DisplayField::SetDefaultColours(textColour, imageBackColour);
	AddField(closeButton = new IconButton(popupTopMargin, pw - (closeButtonWidth + popupSideMargin), closeButtonWidth, IconCancel, evCancel));
}


void AlertPopup::Set(const char *title, const char *text, int32_t mode, uint32_t controls)
{
	alertTitle.copy(title);

	// Split the alert text into 3 lines
	size_t splitPoint = MessageLog::FindSplitPoint(text, alertText1.Capacity(), (PixelNumber)(GetWidth() - 2 * popupSideMargin));
	alertText1.copy(text);
	alertText1.Truncate(splitPoint);
	text += splitPoint;
	splitPoint = MessageLog::FindSplitPoint(text, alertText2.Capacity(), GetWidth() - 2 * popupSideMargin);
	alertText2.copy(text);
	alertText2.Truncate(splitPoint);
	text += splitPoint;
	alertText3.copy(text);

	closeButton->Show(mode == 1);

	okButton->Show(mode >= 2);
	cancelButton->Show(mode == 3);

	// show controls
	bool selected = false;
	for (size_t i = 0; i < ARRAY_SIZE(axisMap); i++)
	{
		TextButton *axis = axisMap[i];

		bool show = controls & (1u << i);

		assert(axis);
		OM::Axis *omAxis = OM::GetAxis(i);

		if (!omAxis)
		{
			axis->Show(false);
			continue;
		}

		axis->SetText(omAxis->letter);
		axis->Show(show);

		if (show && !selected)
		{
			selected = true;
			ChangeLetter(i);
		}
	}

	for (size_t i = 0; i < ARRAY_SIZE(dirMap); i++)
	{
		struct DirMap *dir = &dirMap[i];

		assert(dir->button);
		dir->button->Show(controls ? true : false);
	}
}

void AlertPopup::ChangeLetter(const size_t index)
{
	if (index >= ARRAY_SIZE(axisMap))
	{
		return;
	}

	OM::Axis *axis = OM::GetAxis(index);

	if (!axis)
	{
		return;
	}

	for (size_t i = 0; i < ARRAY_SIZE(dirMap); i++)
	{
		assert(dirMap[i].button);
		dirMap[i].button->SetAxisLetter(axis->letter[0]);
	}
	
}

AlertPopup::AlertPopup(const ColourScheme& colours)
	: StandardPopupWindow(
			alertPopupHeight, alertPopupWidth, colours.alertPopupBackColour, colours.popupBorderColour,
			colours.alertPopupTextColour, colours.buttonImageBackColour, "", popupTopMargin)		// title is present, but empty for now
{
	DisplayField::SetDefaultColours(colours.alertPopupTextColour, colours.alertPopupBackColour);
	titleField->SetValue(alertTitle.c_str(), true);
	AddField(new StaticTextField(popupTopMargin + 2 * rowTextHeight, popupSideMargin, GetWidth() - 2 * popupSideMargin, TextAlignment::Centre, alertText1.c_str()));
	AddField(new StaticTextField(popupTopMargin + 3 * rowTextHeight, popupSideMargin, GetWidth() - 2 * popupSideMargin, TextAlignment::Centre, alertText2.c_str()));
	AddField(new StaticTextField(popupTopMargin + 4 * rowTextHeight, popupSideMargin, GetWidth() - 2 * popupSideMargin, TextAlignment::Centre, alertText3.c_str()));

	// Calculate the button positions
	constexpr unsigned int numButtons = 6;
	constexpr PixelNumber buttonWidthUnits = 5;
	constexpr PixelNumber buttonSpacingUnits = 1;
	constexpr PixelNumber totalUnits = (numButtons * buttonWidthUnits) + ((numButtons - 1) * buttonSpacingUnits);
	constexpr PixelNumber unitWidth = (alertPopupWidth - 2 * popupSideMargin)/totalUnits;
	constexpr PixelNumber buttonWidth = buttonWidthUnits * unitWidth;
	constexpr PixelNumber buttonAxis = 72;
	constexpr PixelNumber buttonAxisWidth = 52;
	constexpr PixelNumber buttonStep = (buttonWidthUnits + buttonSpacingUnits) * unitWidth;
	constexpr PixelNumber hOffset = popupSideMargin + (alertPopupWidth - 2 * popupSideMargin - totalUnits * unitWidth)/2;

	DisplayField::SetDefaultColours(colours.buttonTextColour, colours.buttonTextBackColour);

	for (size_t i = 0; i < ARRAY_SIZE(axisMap); i++)
	{
		TextButton *button = new TextButton(
				popupTopMargin + 5 * rowTextHeight,
				hOffset + i * buttonAxis, buttonAxisWidth,
				"none", evMoveSelectAxis, i);
		assert(button);

		AddField(button);
		axisMap[i] = button;
	}

	for (size_t i = 0; i < ARRAY_SIZE(dirMap); i++)
	{
		struct DirMap *dir = &dirMap[i];

		TextButtonForAxis *button = new TextButtonForAxis(
				popupTopMargin + 5 * rowTextHeight + buttonHeight + moveButtonRowSpacing ,
				hOffset + i * buttonStep, buttonWidth,
				dir->text, evMoveAxis, dir->param);

		assert(button);
		AddField(button);
		dir->button = button;
	}

	AddField(okButton =          new TextButton(popupTopMargin + 7 * rowTextHeight + buttonHeight + moveButtonRowSpacing, hOffset + buttonStep,     buttonWidth + buttonStep, "OK", evCloseAlert, "M292 P0"));
	AddField(cancelButton =      new TextButton(popupTopMargin + 7 * rowTextHeight + buttonHeight + moveButtonRowSpacing, hOffset + 3 * buttonStep, buttonWidth + buttonStep, "Cancel", evCloseAlert, "M292 P1"));
}


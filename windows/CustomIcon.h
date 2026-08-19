#pragma once

#include <wx/bitmap.h>
#include <wx/string.h>

class wxColour;
class wxDC;

class CustomIcon
{
public:
    static wxBitmap CreateReviewCountBitmap(const wxString& text, int size);

private:
    static void DrawRoundedRectangle(wxDC& dc, const wxColour& colour, int size);
    static void MixImages(int size, const wxImage& colourImage, const wxImage& coverageImage, wxImage& resultImage);
};

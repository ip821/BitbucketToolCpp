#include "CustomIcon.h"

#include <algorithm>
#include <wx/dcmemory.h>
#include <wx/image.h>
#include <wx/wx.h>

void CustomIcon::DrawRoundedRectangle(wxDC& dc, const wxColour& colour, const int size)
{
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(colour));
    dc.DrawRoundedRectangle(0, 0, size, size, size / 4);
}

wxBitmap CustomIcon::CreateReviewCountBitmap(const wxString& text, const int size)
{
    constexpr auto renderScale = 4;
    const auto renderSize = size * renderScale;
    wxBitmap colourBitmap(renderSize, renderSize, 24);
    wxBitmap coverageBitmap(renderSize, renderSize, 24);

    {
        wxMemoryDC dc(colourBitmap);
        dc.SetBackground(*wxBLACK_BRUSH);
        dc.Clear();

        const wxColour badgeColour = text == wxS("0")
                                         ? wxColour(95, 95, 95)
                                         : wxColour(0, 82, 204);

        DrawRoundedRectangle(dc, badgeColour, renderSize);

        wxCoord textWidth{};
        wxCoord textHeight{};
        wxCoord textDescent{};
        constexpr auto minimumFontPixelSize = 4 * renderScale;
        for (auto fontPixelSize = renderSize; fontPixelSize >= minimumFontPixelSize; --fontPixelSize)
        {
            const wxFont font(wxFontInfo(wxSize(0, fontPixelSize))
                                  .Family(wxFONTFAMILY_DEFAULT) //wxFONTFAMILY_SWISS
                                  .Bold());
            dc.SetFont(font);
            dc.GetTextExtent(text, &textWidth, &textHeight, &textDescent);
            if (textWidth <= renderSize - 2 * renderScale && textHeight <= renderSize - 2 * renderScale)
                break;
        }

        dc.SetTextForeground(*wxWHITE);
        dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
        dc.DrawText(
            text,
            (renderSize - textWidth) / 2,
            (renderSize - textHeight) / 2);
    }

    {
        wxMemoryDC dc(coverageBitmap);
        dc.SetBackground(*wxBLACK_BRUSH);
        dc.Clear();
        DrawRoundedRectangle(dc, *wxWHITE, renderSize);
    }

    const auto colourImage = colourBitmap.ConvertToImage().Scale(
        size,
        size,
        wxIMAGE_QUALITY_HIGH);
    const auto coverageImage = coverageBitmap.ConvertToImage().Scale(
        size,
        size,
        wxIMAGE_QUALITY_HIGH);

    wxImage result(size, size, true);
    result.InitAlpha();
    auto *resultColour = result.GetData();
    auto *resultAlpha = result.GetAlpha();
    const auto *sourceColour = colourImage.GetData();
    const auto *sourceCoverage = coverageImage.GetData();

    for (auto pixel = 0; pixel < size * size; ++pixel)
    {
        const auto alpha = sourceCoverage[pixel * 3];
        resultAlpha[pixel] = alpha;

        for (auto channel = 0; channel < 3; ++channel)
        {
            const auto colour = sourceColour[pixel * 3 + channel];
            resultColour[pixel * 3 + channel] = alpha == 0
                                                    ? 0
                                                    : static_cast<unsigned char>(
                                                        std::min(255, (colour * 255 + alpha / 2) / alpha));
        }
    }

    return wxBitmap(result);
}

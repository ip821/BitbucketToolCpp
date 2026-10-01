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

// That loop combines the two downscaled images into the final transparent icon:
// - colourImage contains the blue/gray badge and white number rendered over black.
// - coverageImage contains a white badge over black. After downscaling, its gray edge pixels represent partial coverage for antialiasing.
void CustomIcon::MixImages(const int size, const wxImage& colourImage, const wxImage& coverageImage, wxImage& resultImage)
{
    auto *resultColour = resultImage.GetData();
    auto *resultAlpha = resultImage.GetAlpha();
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
}

wxBitmap CustomIcon::CreateReviewCountBitmap(const wxString& text, const int size, const bool hasAlert, const bool enlargeText)
{
    constexpr auto renderScale = 4;
    const auto renderSize = size * renderScale;
    wxBitmap colourBitmap(renderSize, renderSize, 24);
    wxBitmap coverageBitmap(renderSize, renderSize, 24);

    {
        wxMemoryDC dc(colourBitmap);
        dc.SetBackground(*wxBLACK_BRUSH);
        dc.Clear();

        const wxColour badgeColour = hasAlert
                                         ? wxColour(222, 53, 11)
                                         : text == wxS("0")
                                             ? wxColour(95, 95, 95)
                                             : wxColour(0, 82, 204);

        DrawRoundedRectangle(dc, badgeColour, renderSize);

        dc.SetTextForeground(*wxWHITE);
        dc.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

        constexpr auto minimumFontPixelSize = 4 * renderScale;
        constexpr auto pointsPerInch = 72.0;
        const auto makeFont = [&](const int pixelSize)
        {
            const auto pointSize = pixelSize * pointsPerInch / std::max(1, dc.GetPPI().y);
            return wxFont(wxFontInfo(pointSize).Family(wxFONTFAMILY_DEFAULT).Bold());
        };

        if (enlargeText && !text.IsEmpty())
        {
            // wxDC::GetTextExtent includes font leading. Fit the visible ink instead,
            // since AppIndicator scales this bitmap down to the panel icon size.
            // Keep the count no taller than the GNOME panel text beside it.
            const auto maximumInkHeight = renderSize * 5 / 8;
            const auto measureInk = [&](const wxFont& font)
            {
                dc.SetFont(font);
                wxCoord width{}, height{};
                dc.GetTextExtent(text, &width, &height);
                constexpr auto padding = 2 * renderScale;
                const auto bitmapWidth = std::max(1, static_cast<int>(width) + 2 * padding);
                const auto bitmapHeight = std::max(1, static_cast<int>(height) + 2 * padding);
                wxBitmap bitmap(bitmapWidth, bitmapHeight, 24);
                {
                    wxMemoryDC textDC(bitmap);
                    textDC.SetBackground(*wxBLACK_BRUSH);
                    textDC.Clear();
                    textDC.SetFont(font);
                    textDC.SetTextForeground(*wxWHITE);
                    textDC.SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
                    textDC.DrawText(text, padding, padding);
                }

                const auto image = bitmap.ConvertToImage();
                const auto* pixels = image.GetData();
                auto minX = bitmapWidth;
                auto minY = bitmapHeight;
                auto maxX = -1;
                auto maxY = -1;
                for (auto y = 0; y < bitmapHeight; ++y)
                {
                    for (auto x = 0; x < bitmapWidth; ++x)
                    {
                        if (pixels[(y * bitmapWidth + x) * 3] <= 16)
                            continue;
                        minX = std::min(minX, x);
                        minY = std::min(minY, y);
                        maxX = std::max(maxX, x);
                        maxY = std::max(maxY, y);
                    }
                }
                return maxX < 0
                    ? wxRect()
                    : wxRect(minX - padding, minY - padding, maxX - minX + 1, maxY - minY + 1);
            };

            auto smallest = minimumFontPixelSize;
            auto largest = renderSize * 2;
            auto bestSize = minimumFontPixelSize;
            wxRect ink;
            while (smallest <= largest)
            {
                const auto candidate = smallest + (largest - smallest) / 2;
                const auto candidateInk = measureInk(makeFont(candidate));
                if (candidateInk.width <= renderSize - 4 * renderScale &&
                    candidateInk.height <= maximumInkHeight)
                {
                    bestSize = candidate;
                    ink = candidateInk;
                    smallest = candidate + 1;
                }
                else
                {
                    largest = candidate - 1;
                }
            }
            if (ink.IsEmpty())
                ink = measureInk(makeFont(bestSize));

            dc.SetFont(makeFont(bestSize));
            dc.DrawText(text, (renderSize - ink.width) / 2 - ink.x,
                        (renderSize - ink.height) / 2 - ink.y);
        }
        else
        {
            wxCoord textWidth{}, textHeight{};
            for (auto fontPixelSize = renderSize; fontPixelSize >= minimumFontPixelSize; --fontPixelSize)
            {
                dc.SetFont(makeFont(fontPixelSize));
                dc.GetTextExtent(text, &textWidth, &textHeight);
                if (textWidth <= renderSize - 2 * renderScale && textHeight <= renderSize - 2 * renderScale)
                    break;
            }
            dc.DrawText(text, (renderSize - textWidth) / 2, (renderSize - textHeight) / 2);
        }
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

    wxImage resultImage(size, size, true);
    resultImage.InitAlpha();

    MixImages(size, colourImage, coverageImage, resultImage);

    return wxBitmap(resultImage);
}

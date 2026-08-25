#include "StatusImage.h"

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/mstream.h>

#include "generated/EmbeddedStatusImage.h"

wxBitmap LoadEmbeddedStatusBitmap()
{
    wxMemoryInputStream stream(EmbeddedStatusImage::Png, sizeof(EmbeddedStatusImage::Png));
    return wxBitmap(wxImage(stream, wxBITMAP_TYPE_PNG));
}

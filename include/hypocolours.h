//
//  hypocololurs.h
//  HypoModOSX
//
//  Created by Duncan MacGregor on 16/09/2026.
//

#ifndef HYPOCOLOURS_H
#define HYPOCOLOURS_H




namespace HypoColours
{
    inline wxColour Panel()
    {
        return wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    }

    inline wxColour Canvas()
    {
        return *wxWHITE;
    }
}



#endif // HYPOCOLOURS_H

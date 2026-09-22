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
        //return wxColour(242, 242, 242);
        //return wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
        return wxColour(236, 236, 236);
    }

    inline wxColour Group()
    {
        return wxColour(235, 235, 235);
    }

    inline wxColour Canvas()
    {
        return *wxWHITE;
    }
}



#endif // HYPOCOLOURS_H

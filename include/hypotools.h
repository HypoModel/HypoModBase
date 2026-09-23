

#ifndef HYPOTOOLS_H
#define HYPOTOOLS_H



#include "wx/wx.h"
#include "hypocontrols.h"



class GridBox;
class Mod;


class DiagBox: public ToolBox
{
public:
	wxTextCtrl *textbox;

	void Write(wxString);
	DiagBox(MainFrame *main, const wxString& title, const wxPoint& pos, const wxSize& size);
};





#endif

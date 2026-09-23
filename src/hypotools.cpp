


#include "wx/wx.h"
#include "hypotools.h"
#include "hypopanels.h"
#include "hypodef.h"
#include "hypodata.h"



DiagBox::DiagBox(MainFrame *mainwin, const wxString& title, const wxPoint& pos, const wxSize& size)
	: ToolBox(mainwin, "DiagBox", title, pos, size)
{
	textbox = new wxTextCtrl(panel, -1, "", wxDefaultPosition, wxSize(300, 400), wxTE_MULTILINE);

	//Layout();
	mainbox->Add(textbox, 1, wxEXPAND);
	panel->Layout();
	//Centre();
};


void DiagBox::Write(wxString text)
{
	textbox->AppendText(text);	
}



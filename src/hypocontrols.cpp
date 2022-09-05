
//#include "hypomodel.h"
#include "hypocontrols.h"
//#include "hypomain.h"
#include "hypotools.h"



ToolButton::ToolButton(wxWindow *par, wxWindowID id, wxString label, const wxPoint& pos, const wxSize& size, DiagBox *db)
	: wxButton(par, id, label, pos, size)
{
	//toolbox = tbox;
	//pinmode = 0;
	//toolbox = NULL;
	//mainwin = main;
	diagbox = db;
	parent = par;
	ID = id;
	linkID = 0;

	Connect(wxEVT_LEFT_UP, wxMouseEventHandler(ToolButton::OnLeftUp));
	Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(ToolButton::OnLeftDClick));
	//Connect(wxEVT_RIGHT_DCLICK, wxMouseEventHandler(ToolPanel::OnRightDClick));
	//Connect(wxEVT_MOTION, wxMouseEventHandler(ToolPanel::OnMouseMove));
}


void ToolButton::OnLeftDClick(wxMouseEvent& event)
{
	//diagbox->Write("tool button double click\n");
	event.Skip();
}


void ToolButton::OnLeftUp(wxMouseEvent& event)
{
	wxCommandEvent linkpress(wxEVT_COMMAND_BUTTON_CLICKED, linkID);

	if(linkID) {
		linkpress.SetInt(1);
		AddPendingEvent(linkpress);
		diagbox->Write("ToolButton linkpress\n");
	}

	event.Skip();
}


void ToolButton::Press()
{
	wxCommandEvent press(wxEVT_COMMAND_BUTTON_CLICKED, ID);
	AddPendingEvent(press);
}



// ParamText and ParamNum have been replaced by a generalised version of ParamCon, but still used in System panel and ScaleBox


ParamText::ParamText(wxPanel *panel, wxString pname, wxString labelname, wxString initval, int labelwid, int textwid)
{
	wxControl::Create(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	name = pname;
	labelwidth = labelwid;
	textwidth = textwid;
	buttonwidth = 0;
	ostype = GetSystem();
    textfont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
	//textfont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
    if(ostype == Mac) textfont = wxFont(wxFontInfo(11).FaceName("Tahoma"));
    
	confont = textfont;

	sizer = new wxBoxSizer(wxHORIZONTAL);
	label = new wxStaticText(this, wxID_STATIC, labelname, wxDefaultPosition, wxSize(labelwidth, -1), wxALIGN_CENTRE);
	textbox = new wxTextCtrl(this, wxID_ANY, initval, wxDefaultPosition, wxSize(textwidth, -1), wxTE_PROCESS_ENTER);

	label->SetFont(textfont);
	textbox->SetFont(textfont);

	SetInitialSize(wxDefaultSize);
	Move(wxDefaultPosition);

	sizer->Add(label, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 2);
	sizer->Add(textbox, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 2);
	SetSizer(sizer);
	Layout();
}


void ParamText::AddButton(wxString label, int id, int width)
{
	buttonwidth = width;
	button = new wxButton(this, id, label, wxDefaultPosition, wxSize(buttonwidth, 25));
	button->SetFont(confont);
	sizer->Add(button, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxTOP|wxBOTTOM, 2);
	Layout();
}


wxSize ParamText::DoGetBestSize() const
{ 
	//wxSize sizetext = textbox->GetSize();
	//wxSize sizelabel = label->GetSize();

	//return wxSize(109, 25);
	//return wxSize(sizetext.x + sizelabel.x + 4, 25);
	return wxSize(labelwidth + textwidth + buttonwidth, 25);
}


wxString ParamText::GetValue()
{
	return textbox->GetValue();
}


ParamNum::ParamNum(wxPanel *panel, wxString pname, wxString labelname, double initval, int places)
{
    int numheight = -1;
    
	wxControl::Create(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	name = pname;
	decimals = places;
	ostype = GetSystem();
    textfont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
    //textfont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
    
	//textfont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, wxT("Tahoma"));
    if(ostype == Mac) {
        textfont = wxFont(wxFontInfo(11).FaceName("Tahoma"));
        //textfont = wxFont(11, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, wxT("Tahoma"));
        numheight = 20;
    }

	sizer = new wxBoxSizer(wxHORIZONTAL);
	snum = numstring(initval, places);
	label = new wxStaticText(this, wxID_STATIC, labelname, wxDefaultPosition, wxSize(65, -1), wxALIGN_CENTRE);
	numbox = new wxTextCtrl(this, wxID_ANY, snum, wxDefaultPosition, wxSize(40, numheight), wxTE_PROCESS_ENTER);

	label->SetFont(textfont);
	numbox->SetFont(textfont);

	SetInitialSize(wxDefaultSize);
	Move(wxDefaultPosition);

	sizer->Add(label, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 2);
	sizer->Add(numbox, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 2);
	SetSizer(sizer);
	Layout();
}


wxSize ParamNum::DoGetBestSize() const
{ 
	wxSize sizenum = numbox->GetSize();
	wxSize sizelabel = label->GetSize();

	return wxSize(109, 25);
	//return wxSize(sizenum.x + sizelabel.x + 4, sizenum.y + 2);
}


wxString ParamNum::GetValue()
{
	return numbox->GetValue();
}


void ParamNum::SetValue(double val)
{
	snum = numstring(val, decimals);
	numbox->SetValue(snum);
}



ParamCon::ParamCon(ToolPanel *pan, wxString pname, wxString labelname, wxString initval, int labelwid, int numwid)               // Stripped down string only version for text entry
{
	ostype = GetSystem();
	wxControl::Create(pan, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	panel = pan;
	name = pname;
	plabel = labelname;
	type = textcon;
	labelwidth = labelwid;
	numwidth = numwid;
    numheight = -1;
	pad = panel->controlborder;
	if(ostype == Mac) pad = 0;
	
	mainwin = panel->mainwin;
	
	//textfont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
    textfont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
	
	if(ostype == Mac) {
        textfont = wxFont(wxFontInfo(11).FaceName("Tahoma"));
        smalltextfont = wxFont(wxFontInfo(9).FaceName("Tahoma"));
		//textfont = wxFont(11, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
		//smalltextfont = wxFont(9, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
        numheight = 12;
	}

	min = 0;
	max = 1000;                    // Text version needs min and max set for use with GetParams()

	sizer = new wxBoxSizer(wxHORIZONTAL);
	label = new ToolText(this, panel->toolbox, name, labelname, wxDefaultPosition, wxSize(labelwidth, -1), wxALIGN_CENTRE);
	numbox = new wxTextCtrl(this, wxID_ANY, initval, wxDefaultPosition, wxSize(numwidth, numheight), wxTE_PROCESS_ENTER);

	label->SetFont(textfont);
	if(ostype == Mac && labelwidth < 40) label->SetFont(smalltextfont);
	numbox->SetFont(textfont);

	SetInitialSize(wxDefaultSize);
	Move(wxDefaultPosition);

	sizer->Add(label, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, pad);
	sizer->Add(numbox, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, pad);
	
	SetSizer(sizer);
	Layout();
}


ParamCon::ParamCon(ToolPanel *pan, int tp, wxString pname, wxString labelname, double initval, double step, int places, int labelwid, int numwid)
{
	//mainwin = main;
	//wxControl::Create(panel, wxID_ANY, wxDefaultPosition, wxSize(250, 30), wxBORDER_NONE, wxDefaultValidator, name);
	ostype = GetSystem();
	wxControl::Create(pan, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	numstep = step;
	name = pname;
	plabel = labelname;
	decimals = places;
	type = tp;
	labelwidth = labelwid;
	numwidth = numwid;
	panel = pan;
	pad = panel->controlborder;
	//pad = 0;
	if(ostype == Mac) pad = 0;
	mainwin = panel->mainwin;
	//wxFont textfont(10, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, wxT("Tahoma"));
    textfont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
	//textfont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	//if(ostype == Mac) textfont = wxFont(10, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	if(ostype == Mac) {
        textfont = wxFont(wxFontInfo(11).FaceName("Tahoma"));
        smalltextfont = wxFont(wxFontInfo(9).FaceName("Tahoma"));
		//textfont = wxFont(11, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
		//smalltextfont = wxFont(9, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	}
	//boxfont.New(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, wxT("Tahoma"));
	//wxFont textfont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

	min = 0;
	max = 1000000;

	if(initval < 0) min = -1000000;
	if(initval < min) min = initval * 10;
	if(initval > max) max = initval * 100;
	oldvalue = initval;

	sizer = new wxBoxSizer(wxHORIZONTAL);
	snum.Printf("%.1f", initval);
	snum = numstring(initval, places);

	if(labelname == "") {
		label = NULL;
		labelwidth = 0;
	}
	else {
		label = new ToolText(this, panel->toolbox, name, labelname, wxDefaultPosition, wxSize(labelwidth, -1), wxALIGN_CENTRE);
		label->SetFont(textfont);
		if(ostype == Mac && labelwidth < 40) label->SetFont(smalltextfont);
		sizer->Add(label, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, pad);
	}

	numbox = new wxTextCtrl(this, wxID_ANY, snum, wxDefaultPosition, wxSize(numwidth, -1), wxTE_PROCESS_ENTER);
	numbox->SetFont(textfont);
	sizer->Add(numbox, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, pad);

	if(type == spincon) {
		spin = new wxSpinButton(this, wxID_ANY, wxDefaultPosition, wxSize(17, 23), wxSP_VERTICAL|wxSP_ARROW_KEYS);  // 21
		spin->SetRange(-1000000, 1000000);
	}	

	SetInitialSize(wxDefaultSize);
	Move(wxDefaultPosition);

	if(type == spincon) sizer->Add(spin, 0, wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(sizer);
	Layout();

	Connect(wxEVT_SPIN_UP, wxSpinEventHandler(ParamCon::OnSpinUp));
	Connect(wxEVT_SPIN_DOWN, wxSpinEventHandler(ParamCon::OnSpinDown));
	Connect(wxEVT_SPIN, wxSpinEventHandler(ParamCon::OnSpin));
	Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(ParamCon::OnEnter));
}


void ParamCon::Select()
{
	panel->toolbox->TextClick(name);
}


double ParamCon::GetValue()
{
	if(type == textcon) return 0;
	numbox->GetValue().ToDouble(&value);
	return value;
}


wxString ParamCon::GetString()
{
	return numbox->GetValue();
}


void ParamCon::SetPen(wxColour pen)
{
	numbox->SetForegroundColour(pen);
}


void ParamCon::SetValue(double val)
{
	snum = numstring(val, decimals);
	numbox->SetValue(snum);
}


void ParamCon::SetValue(wxString text)
{
	numbox->SetValue(text);
}


void ParamCon::SetMinMax(double newmin, double newmax, bool newcycle)
{
	min = newmin;
	max = newmax;
	cycle = newcycle;
	//if(type == spincon) spin->SetRange(min, max);
}


wxSize ParamCon::DoGetBestSize() const
{
	//if(type == spincon) wxSize spinsize  = spin->GetSize();
	//wxSize sizenum = numbox->GetSize();
	//wxSize sizelabel = label->GetSize();

	//return wxSize(spinsize.x + sizenum.x + sizelabel.x + 4, sizenum.y + 4);
	//return wxSize(spinsize.x + sizenum.x, sizenum.y);
	if(ostype == Mac) {
		if(type == spincon) return wxSize(numwidth + labelwidth + pad*2 + 17, 23);
		else return wxSize(numwidth + labelwidth + pad*2, 20);
	}
	//}return wxSize(120, 20);
	//if(type == spincon) return wxSize(141, 25);
	//if(type == spincon) return wxSize(numwidth + labelwidth + 20, 25);
	if(type == spincon) return wxSize(numwidth + labelwidth + 17, 25);
	else //return wxSize(110, 25);                 // 109
		return wxSize(numwidth + labelwidth + pad*2, 21 + pad*2);
}


void ParamCon::DoMoveWindow(int x, int y, int width, int height)
{
	wxControl::DoMoveWindow(x, y, width, height);

	// position the subcontrols inside the client area
	//wxSize spinsize = spin->GetSize();

	//wxCoord numwidth = width - spinsize.x;
	//numbox->SetSize(x, y, 80, height);
	//spin->SetSize(x + numwidth + 2, y, wxDefaultCoord, height);
}


void ParamCon::OnSpin(wxSpinEvent& event)
{
	wxString text;

	//int spindir = event.GetPosition();
	//if(mainwin && mainwin->diagnostic) mainwin->diagbox->textbox->AppendText(text.Format("\nspin click %d\n", spindir));

	if(panel->toolbox) {
		//panel->toolbox->diagbox->Write("tool spin click\n");
		panel->toolbox->SpinClick(name);
	}

	event.Skip();
}


void ParamCon::OnEnter(wxCommandEvent& event)
{
	wxString text;

	//if(mainwin && mainwin->diagnostic) mainwin->diagbox->textbox->AppendText(text.Format("\nspin click %d\n", spindir));

	if(panel->toolbox) {
		//panel->toolbox->diagbox->Write("tool box enter\n");
		panel->toolbox->BoxEnter(name);
	}

	event.Skip();
}


void ParamCon::OnSpinUp(wxSpinEvent& WXUNUSED(event))
{
	double value, newvalue;

	numbox->GetValue().ToDouble(&value);
	newvalue = value + numstep;
	if(newvalue > max) {
		if(cycle) newvalue = min + (newvalue - max) - 1;
		else return;
	}
	snum = numstring(newvalue, decimals);
	numbox->SetValue(snum);
}


void ParamCon::OnSpinDown(wxSpinEvent& WXUNUSED(event))
{
	double value, newvalue;

	//if(mainwin && mainwin->diagnostic) mainwin->diagbox->textbox->AppendText("spin down\n");

	numbox->GetValue().ToDouble(&value);
	newvalue = value - numstep;
	if(newvalue < min) {
		if(cycle) newvalue = max + (newvalue - min) + 1;
		else return;
	}
	snum = numstring(newvalue, decimals);
	numbox->SetValue(snum);
}


ParamSet::ParamSet()
{
	//panel = pan;
	numparams = 0;
	currlay = 0;
	//paramstore = new ParamStore();

	// Default field widths
	num_labelwidth = 65;
	num_numwidth = 40;
	con_labelwidth = 60;
	con_numwidth = 60;
	text_labelwidth = 60;
	text_textwidth = 150;
}


ParamSet::~ParamSet()
{
	//delete paramstore;
}

//paramset.AddCon("runtime", "Run Time", 2000, 1, 0);


void ParamSet::SetMinMax(wxString tag, double min, double max)
{
	int id = GetID(tag);

	con[id]->min = min;
	con[id]->max = max;
}


int ParamSet::GetID(wxString tag)
{
	wxString text;
	int index = ref[tag];
	if(index < 0) panel->mainwin->diagbox->Write(text.Format("ParamSet bad tag index %d, tag %s\n", index, tag));
	return index;
}


ParamCon *ParamSet::GetCon(wxString tag)
{
	if(!tagstore.Check(tag)) {
		panel->mainwin->diagbox->Write("ParamSet GetCon " + tag + " not found\n");
		return NULL;
	}
	
	else return con[(int)ref[tag]];
}


ParamCon *ParamSet::AddCon(wxString name, wxString labelname, double initval, double step, int places, int labelwidth, int numwidth) 
{
	if(labelwidth < 0) labelwidth = con_labelwidth;
	if(numwidth < 0) numwidth = con_numwidth;

	con[numparams] = new ParamCon(panel, spincon, name, labelname, initval, step, places, labelwidth, numwidth);           // number + spin
	ref[name] = numparams;
	tagstore.Add(name);     // new December 2020

	return con[numparams++];
}


ParamCon *ParamSet::AddNum(wxString name, wxString labelname, double initval, int places, int labelwidth, int numwidth)
{
	if(labelwidth < 0) labelwidth = num_labelwidth;
	if(numwidth < 0) numwidth = num_numwidth;

	con[numparams] = new ParamCon(panel, numcon, name, labelname, initval, 0, places, labelwidth, numwidth);              // number
	ref[name] = numparams;
	tagstore.Add(name);
	
	return con[numparams++];
}


ParamCon *ParamSet::AddText(wxString name, wxString labelname, wxString initval, int labelwidth, int textwidth)
{
	if(labelwidth < 0) labelwidth = text_labelwidth;
	if(textwidth < 0) textwidth = text_textwidth;

	con[numparams] = new ParamCon(panel, name, labelname, initval, labelwidth, textwidth);								   // text
	ref[name] = numparams;
	tagstore.Add(name);

	return con[numparams++];
}


void ParamSet::SetValue(wxString tag, double value)
{
	int id = GetID(tag);
	con[id]->SetValue(value);
}


double ParamSet::GetValue(wxString tag)
{
	int id;
	double value;

	id = GetID(tag);
	if(id < 0) return 0;
	con[id]->numbox->GetValue().ToDouble(&value);
	return value;	
}

wxString ParamSet::GetText(wxString tag)
{
	int id;
	wxString text;

	id = GetID(tag);
	text = con[id]->GetString();
	return text;	
}


ParamStore *ParamSet::GetParams()
{ 
	int i;
	//if(pstore == NULL) pstore = modparams;
	for(i=0; i<numparams; i++)
		con[i]->numbox->GetValue().ToDouble(&(paramstore[con[i]->name]));
	return &paramstore;
}


ParamStore *ParamSet::GetParamsNew(BoxOut *boxout)
{ 
	int i;
	double value;
	ParamCon *pcon;
	wxString text;

	//if(boxout->diagbox) boxout->diagbox->textbox->AppendText(text.Format("%s get params\n", boxout->name));

	for(i=0; i<numparams; i++) {
		pcon = con[i];
		value = pcon->GetValue();
		//boxout->diagbox->Write(text.Format("param %s type %d value %.4f\n", con[i]->name, con[i]->type, value));
		if(value < pcon->min) {
			value = pcon->oldvalue;
			pcon->SetValue(value);
			//SetStatus(text.Format("Parameter \'%s\' out of range", con->label->GetLabel()));
			//mainwin->diagbox->Write(text.Format("Parameter \'%s\' out of range, min %.2f max %.2f\n", con->label->GetLabel(), con->min, con->max));
		}
		if(value > pcon->max) {
			value = pcon->oldvalue;
			pcon->SetValue(value);
			//SetStatus(text.Format("Parameter %s out of range", con->label->GetLabel()));
		}
		//boxout->diagbox->Write(text.Format("param %s type %d value %.4f\n", con[i]->name, con[i]->type, value));
		paramstore[con[i]->name] = value;
		pcon->oldvalue = value;
	}
	return &paramstore;
}


TextBox::TextBox(wxWindow *parent, wxWindowID id, wxString value, wxPoint pos, wxSize size, long style)
	: wxTextCtrl(parent, id, value, pos, size, style)
{
	//int i;
}


ToolText::ToolText(wxWindow *parent, ToolBox *tbox, wxString ptag, wxString label, const wxPoint& pos, const wxSize& size, long style)
	: wxStaticText(parent, wxID_ANY, label, pos, size, style)
{
	toolbox = tbox;
	tag = ptag;

	Connect(wxEVT_LEFT_UP, wxMouseEventHandler(ToolText::OnLeftClick));
	Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(ToolText::OnLeftDClick));
	Connect(wxEVT_RIGHT_DCLICK, wxMouseEventHandler(ToolText::OnRightDClick));
}


void ToolText::OnLeftDClick(wxMouseEvent& event)
{
	if(toolbox) {
		toolbox->pinmode = 1 - toolbox->pinmode;
		toolbox->diagbox->Write(text.Format("LDClick pin %d\n", toolbox->pinmode));	
	}
}


void ToolText::OnRightDClick(wxMouseEvent& event)
{
	if(toolbox) {
		toolbox->pinmode = 1 - toolbox->pinmode;
		toolbox->diagbox->Write(text.Format("RDClick pin %d\n", toolbox->pinmode));
	}
}


void ToolText::OnLeftClick(wxMouseEvent& event)
{
	if(toolbox) {
		toolbox->diagbox->Write("text click\n");
		toolbox->activepanel->OnClick(event.GetPosition());
		toolbox->TextClick(tag);
	}
}


void ToolText::OnMouseMove(wxMouseEvent &event)
{
	wxPoint pos = event.GetPosition();
	//snum.Printf("ToolMove X %d Y %d", pos.x, pos.y);
	//if(mainwin) toolbox->mainwin->SetStatusText(snum);
}


ToolPanel::ToolPanel(ToolBox *tbox, wxWindow *parent)
	: wxPanel(parent, wxID_ANY)
{
	toolbox = tbox;
	mainwin = toolbox->mainwin;
	Init();
}


ToolPanel::ToolPanel(wxNotebook *book, const wxPoint& pos, const wxSize& size)
	: wxPanel(book, wxID_ANY, pos, size)
{
	toolbox = NULL;
	mainwin = NULL;
	Init();
}


ToolPanel::ToolPanel(wxDialog *box, const wxPoint& pos, const wxSize& size)
	: wxPanel(box, wxID_ANY, pos, size)
{
	toolbox = NULL;
	mainwin = NULL;
	Init();
}


ToolPanel::ToolPanel(ToolBox *tbox, const wxPoint& pos, const wxSize& size)
	: wxPanel(tbox, wxID_ANY, pos, size)
{
	toolbox = tbox;
	mainwin = toolbox->mainwin;
	Init();
}


ToolPanel::ToolPanel(MainFrame *main, const wxPoint& pos, const wxSize& size, long style)
	: wxPanel(main, wxID_ANY, pos, size)
{
	toolbox = NULL;
	mainwin = main;
	Init();
}


void ToolPanel::Init() {
	controlborder = 2;

	if(GetSystem() == Mac) {
		buttonheight = 25;
		boxfont = wxFont(wxFontInfo(12).FaceName("Tahoma"));
		confont = wxFont(wxFontInfo(10).FaceName("Tahoma"));
	}
	else {
		buttonheight = 23;
		boxfont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
		confont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
	}

	Connect(wxEVT_LEFT_UP, wxMouseEventHandler(ToolPanel::OnLeftClick));
	Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(ToolPanel::OnLeftDClick));
	Connect(wxEVT_RIGHT_DCLICK, wxMouseEventHandler(ToolPanel::OnRightDClick));
	//Connect(wxEVT_MOTION, wxMouseEventHandler(ToolPanel::OnMouseMove));
}


void ToolPanel::OnClick(wxPoint pos)
{
	wxString text;
	if(mainwin) mainwin->SetStatusText(text.Format("toolpanel click x %d y %d", pos.x, pos.y));
}


//wxToggleButton *ToolPanel::ToggleButton(int id, wxString label, int width, wxBoxSizer *sizer, int point, wxPanel *pan)
wxToggleButton *ToolPanel::ToggleButton(int id, wxString label, int width, wxBoxSizer *sizer, int point, ToolBox *box)
{
	//if(!pan) pan = this;
	confont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
	if(GetSystem() == Mac) confont = wxFont(wxFontInfo(point).FaceName("Tahoma"));
	wxToggleButton *button = new wxToggleButton(this, id, label, wxDefaultPosition, wxSize(width, buttonheight), 0);
	button->SetFont(confont);
	sizer->Add(button, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxTOP|wxBOTTOM, 1);
	if(box) box->Connect(id, wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler(ToolBox::OnToggle));
	else Connect(id, wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler(ToolPanel::OnToggle));
	return button;
}


void ToolPanel::OnLeftDClick(wxMouseEvent& event)
{
	if(toolbox) {
		toolbox->pinmode = 1 - toolbox->pinmode;
		snum.Printf("LDClick pin %d", toolbox->pinmode);
		mainwin->SetStatusText(snum);
	}
}


void ToolPanel::OnRightDClick(wxMouseEvent& event)
{
	if(toolbox) {
		toolbox->pinmode = 1 - toolbox->pinmode;
		snum.Printf("RDClick pin %d", toolbox->pinmode);
		mainwin->SetStatusText(snum);
	}
}


void ToolPanel::OnLeftClick(wxMouseEvent& event)
{
	//pinmode = 1 - pinmode;
	//snum.Printf("Tool LClick");
	if(mainwin) mainwin->SetStatusText("Tool Click");

	OnClick(event.GetPosition());

	//mainwin->SetStatusText("Tool Panel Click");
}


void ToolPanel::OnMouseMove(wxMouseEvent &event)
{
	wxPoint pos = event.GetPosition();
	snum.Printf("ToolMove X %d Y %d", pos.x, pos.y);
	if(mainwin) toolbox->mainwin->SetStatusText(snum);
}


/*ToolBox::ToolBox(MainFrame *main, const wxString& title, const wxPoint& pos, const wxSize& size, int type, bool serve, bool chi)
	: wxFrame(main, -1, title, pos, size,
	//wxFRAME_FLOAT_ON_PARENT | wxFRAME_TOOL_WINDOW | wxCAPTION | wxRESIZE_BORDER)
	wxFRAME_FLOAT_ON_PARENT | wxFRAME_TOOL_WINDOW | wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX)
{
	mainwin = main;
	mpos = pos;
	boxsize = size;
	boxtype = type;
	servant = serve;
	child = chi;
	boxlabel = title;
	canclose = true;
	vdu = NULL;

	//main->diagbox->Write("ToolBox init\n");

	Init();
}*/


ToolBox::ToolBox(MainFrame *main, wxString tag, const wxString& title, const wxPoint& pos, const wxSize& size, int type, bool close)
	: wxFrame(main, -1, title, pos, size,
		//wxFRAME_FLOAT_ON_PARENT | wxFRAME_TOOL_WINDOW | wxCAPTION | wxRESIZE_BORDER)
		wxFRAME_FLOAT_ON_PARENT | wxFRAME_TOOL_WINDOW | wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX)
{
	mainwin = main;
	mpos = pos;
	boxsize = size;
	boxtype = type;
	//servant = serve;
	//child = chi;
	boxlabel = title;
	canclose = close;
	boxtag = tag;
	vdu = NULL;

	//main->diagbox->Write("ToolBox init\n");

	Init();
}


ToolBox::ToolBox(MainFrame *main, wxString tag, const wxString& title, const wxPoint& pos, const wxSize& size, bool close)
	: wxFrame(main, -1, title, pos, size, wxFRAME_FLOAT_ON_PARENT | wxFRAME_TOOL_WINDOW | wxCAPTION | wxRESIZE_BORDER)
	//wxFRAME_FLOAT_ON_PARENT | wxFRAME_TOOL_WINDOW | wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX)
{
	mainwin = main;
	mpos = pos;
	boxsize = size;
	boxtype = 0;
	boxlabel = title;
	canclose = close;
	boxtag = tag;
	vdu = NULL;
	
	Init();
}


ToolBox::~ToolBox() {
	if(boxtype == 1) {
		winman->UnInit(); 
		delete winman;
	}
	//if(selfstore) Store();   // doesn't work here because of already deleted class data
	//delete paramset;
	//delete toolparams;
} 
	

/*
ToolBox::ToolBox(MainFrame *main, const wxString& title, const wxPoint& pos, const wxSize& size, bool close)
	: wxFrame(main, -1, title, pos, size, wxFRAME_FLOAT_ON_PARENT | wxFRAME_TOOL_WINDOW | wxCAPTION | wxRESIZE_BORDER)
	//wxFRAME_FLOAT_ON_PARENT | wxFRAME_TOOL_WINDOW | wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX)
{
	mainwin = main;
	//mod = NULL;
	mpos = pos;
	boxsize = size;
	SetPosition();
	pinmode = 1;	
	ostype = GetSystem();
	visible = true;
	boxtag = title;
	canclose = close;
	boxtype = 0;

	buttonheight = 23;
	boxfont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	confont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	if(ostype == Mac) {
		buttonheight = 25;
		boxfont = wxFont(12, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
		//confont = wxFont(11, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Lucida Grande");
		confont = wxFont(10, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	}

	if(boxtype == 1) {
		winman = new wxAuiManager;
		winman->SetManagedWindow(this);
		panel = new ToolPanel(this, wxDefaultPosition, wxDefaultSize);                // Base panel for default param file combobox
		panel->SetFont(boxfont);
		//mainbox = new wxBoxSizer(wxVERTICAL);
		//panel->SetSizer(mainbox);
		//tabpanel = new wxNotebook(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);
	}
	else {
		panel = new ToolPanel(this, wxDefaultPosition, wxDefaultSize);
		panel->SetFont(boxfont);
		mainbox = new wxBoxSizer(wxVERTICAL);
		panel->SetSizer(mainbox);
	}

	activepanel = panel;

	Connect(wxEVT_MOVE, wxMoveEventHandler(ToolBox::OnMove));
	Connect(wxEVT_SIZE, wxSizeEventHandler(ToolBox::OnSize));
	Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(ToolBox::OnClose));
	//Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(ToolBox::OnDClick));
	//Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(ToolBox::OnDClick));
	//panel->Connect(wxEVT_MOTION, wxMouseEventHandler(ToolBox::OnMouseMove));
	//panel->Connect(wxEVT_LEFT_UP, wxMouseEventHandler(ToolBox::OnLeftClick));
}*/


void ToolBox::Init()
{
	SetPosition();
	pinmode = 1;	
	ostype = GetSystem();
	diagbox = mainwin->diagbox;
	toolpath = mainwin->toolpath;
	
	visible = true;
	status = NULL;

	buttonheight = 23;
    boxfont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
    confont = wxFont(wxFontInfo(8).FaceName("Tahoma"));
	//boxfont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	//confont = wxFont(8, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	if(ostype == Mac) {
		buttonheight = 25;
        boxfont = wxFont(wxFontInfo(12).FaceName("Tahoma"));
        confont = wxFont(wxFontInfo(10).FaceName("Tahoma"));
		//boxfont = wxFont(12, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
		//confont = wxFont(10, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	}

	if(boxtype == 1) {
		winman = new wxAuiManager;
		winman->SetManagedWindow(this);
		panel = new ToolPanel(this, wxDefaultPosition, wxDefaultSize);                // Base panel for default param file combobox
		panel->SetFont(boxfont);
		//mainbox = new wxBoxSizer(wxVERTICAL);
		//panel->SetSizer(mainbox);
		//tabpanel = new wxNotebook(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);
	}
	else {
		panel = new ToolPanel(this, wxDefaultPosition, wxDefaultSize);
		panel->SetFont(boxfont);
		mainbox = new wxBoxSizer(wxVERTICAL);
		panel->SetSizer(mainbox);
	}

	selfstore = false;
	activepanel = panel;
	//paramset = new ParamSet(activepanel);
	paramset.panel = activepanel;
	//toolparams = new ParamStore();

	//mod->diagbox->Write("ToolBox init\n");

	Connect(wxEVT_MOVE, wxMoveEventHandler(ToolBox::OnMove));
	Connect(wxEVT_SIZE, wxSizeEventHandler(ToolBox::OnSize));
	Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(ToolBox::OnClose));
}


void ToolBox::Store()
{
	wxString filename;
	wxString outline;

	wxString text;
	TextFile toolfile;

	if(!selfstore) return;

	mainwin->diagbox->Write(text.Format("tool store %s\n", boxtag));
	
	if(!wxDirExists(toolpath)) wxMkdir(toolpath);
	filename = toolpath + "/" + boxtag + ".dat";

	mainwin->diagbox->Write(text.Format("tool store path %s file %s params %d\n", toolpath, filename, paramset.numparams));
	
	toolfile.New(filename);

	for(i=0; i<paramset.numparams; i++) {
		if(paramset.con[i]->type != textcon) {
			paramset.con[i]->numbox->GetValue().ToDouble(&(toolparams[paramset.con[i]->name]));
			outline.Printf("%.8f", toolparams[paramset.con[i]->name]);
		}
		else outline = paramset.con[i]->GetString();
		toolfile.WriteLine(paramset.con[i]->name + " " + outline);
	}
	
	toolfile.Close();
	//mainwin->diagbox->Write("Tool File OK\n");
}


void ToolBox::Load()
{
	int id;
	double datval;
	bool diagnostic;
	wxString filename;
	wxString readline, datname;
	TextFile toolfile;
	wxString text;

	diagnostic = true;
	diagbox = mainwin->diagbox;

	// Tool data file
	//filepath = mainwin->toolpath;
	filename = toolpath + "/" + boxtag + ".dat";

	diagbox->Write("ToolLoad " + filename + "\n");

	if(!toolfile.Exists(filename)) {
		diagbox->Write("ToolLoad file not found\n");
		return;
	}

	toolfile.Open(filename);
	readline = toolfile.ReadLine();

	while(!readline.IsEmpty()) {
		datname = readline.BeforeFirst(' ');
		readline = readline.AfterFirst(' ');
		readline.Trim();
		if(paramset.ref.check(datname)) {
			id = paramset.ref[datname];
			if(paramset.con[id]->type != textcon) {
				readline.ToDouble(&datval);
				//paramset.con[id]->SetValue("");
				paramset.con[id]->SetValue(datval);
			}
			else paramset.con[id]->SetValue(readline);
		}
		if(diagnostic) mainwin->diagbox->Write(text.Format("Model Param ID %d, Name %s, Value %.4f\n", id, datname, datval)); 
		if(toolfile.End()) return;
		readline = toolfile.ReadLine();	
	}
	
    toolfile.Close();
}



wxStaticText *ToolBox::StatusBar()
{
	//wxStaticText *text = new(_NORMAL_BLOCK, __FILE__, __LINE__) wxStaticText(activepanel, wxID_ANY, "", wxDefaultPosition,wxDefaultSize, 
	//	wxALIGN_CENTRE|wxBORDER_DOUBLE|wxST_NO_AUTORESIZE);
	wxStaticText *text = new wxStaticText(activepanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 
		wxALIGN_CENTRE|wxBORDER_DOUBLE|wxST_NO_AUTORESIZE);
	//wxALIGN_CENTRE_HORIZONTAL);
	text->SetFont(confont);
	return text;
}


wxStaticText *ToolBox::TextLabel(wxString label)
{
    if(ostype == Mac) confont = wxFont(wxFontInfo(11).FaceName("Tahoma"));
        
    //confont = wxFont(11, wxFONTFAMILY_SWISS, wxNORMAL, wxNORMAL, false, "Tahoma");
	//wxStaticText *text = new (_NORMAL_BLOCK, __FILE__, __LINE__)wxStaticText(activepanel, wxID_ANY, label);
	wxStaticText *text = new wxStaticText(activepanel, wxID_ANY, label);
	text->SetFont(confont);
	return text;
}


TagBox *ToolBox::TextInputCombo(int width, int height, wxString label, wxString name, wxString path)
{
	//wxComboBox *text = new (_NORMAL_BLOCK, __FILE__, __LINE__) wxComboBox(activepanel, wxID_ANY, label, wxDefaultPosition, wxSize(width, height));
	TagBox *text = new TagBox(mainwin, activepanel, wxID_ANY, label, wxDefaultPosition, wxSize(width, height), name, path);
	text->diagbox = mainwin->diagbox;
	text->SetFont(confont);
	return text;
}


wxTextCtrl *ToolBox::TextInput(int width, int height, wxString label)
{
	//wxTextCtrl *text = new (_NORMAL_BLOCK, __FILE__, __LINE__) wxTextCtrl(activepanel, wxID_ANY, label, wxDefaultPosition, wxSize(width, height));
	wxTextCtrl *text = new wxTextCtrl(activepanel, wxID_ANY, label, wxDefaultPosition, wxSize(width, height));
	text->SetFont(confont);
	return text;
}


wxStaticText *ToolBox::NumPanel(int width, int align, wxString label)
{
	//wxStaticText *text = new wxStaticText(panel, -1, inittext, wxDefaultPosition, wxSize(width, -1), wxALIGN_RIGHT|wxBORDER_RAISED|wxST_NO_AUTORESIZE); 
	//wxStaticText *text = new (_NORMAL_BLOCK, __FILE__, __LINE__) wxStaticText(activepanel, -1, label, wxDefaultPosition, wxSize(width, -1), align|wxBORDER_RAISED|wxST_NO_AUTORESIZE);
	wxStaticText *text = new wxStaticText(activepanel, -1, label, wxDefaultPosition, wxSize(width, -1), align|wxBORDER_RAISED|wxST_NO_AUTORESIZE);
	text->SetFont(confont);
	return text;
}


void ToolBox::AddButton(int id, wxString label, int width, wxBoxSizer *box, int pad, int height, wxPanel *pan)
{
	if(pan == NULL) pan = activepanel;
	if(height == 0) height = buttonheight;
	//wxButton *button = new (_NORMAL_BLOCK, __FILE__, __LINE__) wxButton(pan, id, label, wxDefaultPosition, wxSize(width, height));
	ToolButton *button = new ToolButton(pan, id, label, wxDefaultPosition, wxSize(width, height), mainwin->diagbox);
	button->SetFont(confont);
	box->Add(button, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxTOP|wxBOTTOM, pad);
}

/*
void ScaleBox::AddButton(int id, wxString label, int width, wxBoxSizer *box, int pad, int height)
{
    if(height == 0) height = buttonheight;
    wxButton *button = new wxButton(panel, id, label, wxDefaultPosition, wxSize(width, height));
    button->SetFont(confont);
    box->Add(button, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxTOP|wxBOTTOM, pad);
}
 */


void ToolBox::TextClick(wxString tag)
{
	diagbox->Write("toolbox textclick " + tag + "\n");
}


void ToolBox::SpinClick(wxString tag)
{
	diagbox->Write("toolbox spinclick  " + tag + "\n");
}


void ToolBox::BoxEnter(wxString tag)
{
	diagbox->Write("toolbox boxenter " + tag + "\n");
}



ToolBox *ToolSet::GetBox(wxString tag) {
	int index = GetIndex(tag);
	if(index < 0) return NULL;
	else return box[index];
}


wxStaticText *ToolBox::GridLabel(int width, wxString label)
{
	wxStaticText *text;
	//text = new (_NORMAL_BLOCK, __FILE__, __LINE__) wxStaticText(activepanel, wxID_ANY, label, wxDefaultPosition, wxSize(width, -1), wxALIGN_CENTRE);
	text = new wxStaticText(activepanel, wxID_ANY, label, wxDefaultPosition, wxSize(width, -1), wxALIGN_CENTRE);
	text->SetFont(confont);
	return text;
}


void ToolBox::ReSize()
{
	wxFrame::SetSize(boxsize);
	//Show(visible);
}


wxPoint ToolBox::SetPosition()
{
	wxPoint mainpos; 
	wxSize mainsize; 

	if(mainwin) {
		mainpos = mainwin->GetPosition();
		mainsize = mainwin->GetSize();
	}
	else { 
		mainpos = wxPoint(0, 0);
		mainsize = wxSize(0, 0);
	}

	Move(mainpos.x + mainsize.x + mpos.x, mainpos.y + mpos.y + 5);
	oldpos = GetPosition();
	return wxPoint(mainpos.x + mainsize.x + mpos.x, mainpos.y + mpos.y + 5);
}


void ToolBox::OnMove(wxMoveEvent& event)
{
	wxPoint shift;
	//mainwin->SetStatusText("Box Move Off");
	if(IsActive() && pinmode) {
		//if(HasFocus() && pinmode) {
		wxPoint newpos = GetPosition();
		wxSize newsize = GetSize();

		shift.x = newpos.x - oldpos.x;
		shift.y = newpos.y - oldpos.y;
		mpos.x = mpos.x + shift.x;
		mpos.y = mpos.y + shift.y;
		oldpos = newpos;
		//snum.Printf("Box Move x %d y %d", shift.x, shift.y);
		snum.Printf("Box x %d y %d  x %d  y %d", mpos.x, mpos.y, newpos.x, newpos.y);
		mainwin->SetStatusText(snum);
	}
	else {
		snum.Printf("Box x %d y %d", mpos.x, mpos.y);
		//mainwin->SetStatusText(snum);
	}
}


void ToolBox::OnSize(wxSizeEvent& event)
{	
	wxSize newsize = GetSize();
	wxPoint pos = GetPosition();
	snum.Printf("Box Size X %d Y %d", newsize.x, newsize.y);
	mainwin->SetStatusText(snum);
	//panel->Layout();
	boxsize = newsize;
	wxFrame::OnSize(event);
}


void ToolBox::OnClose(wxCloseEvent& event)
{
	TextFile ofp;
	wxString text;
	ofp.Open("boxdiag.txt");

	if(servant || child) {
		Show(false);
		ofp.WriteLine(text.Format("hide box %d, child %d", boxindex, child));
        //diagbox->Write(text.Format("hide box %d, child %d", boxindex, child));
	}
	else if(canclose) {
		ofp.WriteLine(text.Format("close box %d, mod boxes %d", boxindex, toolset->numtools));
        //diagbox->Write(text.Format("close box %d, mod boxes %d", boxindex, toolset->numtools));
		//if(mod != NULL) mod->modtools.RemoveBox(boxindex);
		//mainwin->toolset.RemoveBox(boxindex);
		toolset->RemoveBox(boxindex);
		Destroy();
	}

	ofp.Close();
}


void TagSet::AddTag(wxString boxtag, TagBox *newbox) 
{
    if(!newbox) return;
	tagdata[numtags].tag = boxtag;
	tagdata[numtags].box = newbox;
	numtags++;
    //box[numtags++] = newbox;
}


TagBox *TagSet::GetBox(wxString tag) 
{
	int i;

	for(i=0; i<numtags; i++)
		if(tagdata[i].tag == tag) return tagdata[i].box;
	return NULL;
}


void TagSet::UpdatePath() {
    int i;
    
    for(i=0; i<numtags; i++) tagdata[i].box->PathUpdate();
}



TagBox::TagBox(MainFrame *main, ToolPanel *panel, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size, wxString boxtag, wxString path)
	: wxComboBox(panel, id, label, wxDefaultPosition, size)
{
    mainwin = main;
    boxpath = path;
	redtag = "";
	diagnostic = false;
    
    mainwin->tagset->AddTag(boxtag, this);
    
    //if(mainwin->modpath == "") tagpath = mainwin->mainpath + modpath + "/Tags/";
    //else tagpath = mainwin->modpath + modpath + "/Tags/";

	PathUpdate();

	/*
	if(boxpath.IsEmpty()) {
		if(mainwin->modpath.IsEmpty()) tagpath = "Tags";
		else tagpath = mainwin->modpath + "/Tags";
	}
	else {
		if(mainwin->modpath.IsEmpty()) tagpath = boxpath + "/Tags";
		else tagpath = mainwin->modpath + "/" + boxpath + "/Tags";
	}*/

	/*
	if(mainwin->modpath == "") {
		if(mainwin->mainpath == "") tagpath = "Tags";
		else tagpath = mainwin->mainpath + "/Tags";
	}
	else {
		//if(mainwin->ostype == Mac) tagpath = mainwin->modpath + modpath + "/Tags/";
		//else tagpath = mainwin->modpath + "/" + modpath + "/Tags/";
		tagpath = mainwin->modpath + "/" + modpath + "/Tags";
	}
	*/
    
    if(diagnostic) mainwin->diagbox->Write(text.Format("TagBox tagpath %s boxpath %s\n", tagpath, boxpath));
	
	name = boxtag;
	diagbox = NULL;
	labelset = false;
	bool check;
	TextFile opfile, tagfile;
	wxString readline;
	wxString opfilepath, tagfilepath;

	//tagfilename = "";

	if(!wxDirExists(tagpath)) wxMkdir(tagpath);

	check = opfile.Open(tagpath + "/" + name + "op.ini");
	if(!check) {
		if(diagbox) diagbox->Write("No tagpath found, setting default\n");
		tagfilename = name + "tags.ini";
	}
	else {
		readline = opfile.ReadLine();
		if(readline.IsEmpty()) tagfilename = name + "tags.ini";
		else tagfilename = readline;
		opfile.Close();
	    if(!tagfile.Exists(tagpath + tagfilename)) tagfilename = name + "tags.ini";    // mostly used for update from old version full path opfile
	}

	mainwin->diagbox->Write("\nTagBox init " + name + "\n");
	HistLoad();

	//Connect(id, wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler(TagBox::OnDClick));
	Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(TagBox::OnDClick));
	Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(TagBox::OnRClick));
}

// LoadTag() checks for existing file using current tag with specified directory and suffix, if found updates tag list and returns file path
wxString TagBox::LoadTag(wxString dir, wxString suffix)
{
	wxString filetag, filepath;
	int tagpos;

	filetag = GetValue();
	//if(!dir.IsEmpty()) filepath = tagpath + filetag;       // placeholder 6/1/21
	filepath = dir + "/" + filetag + suffix;

	if(diagnostic) mainwin->diagbox->Write(text.Format("TagBox Load filepath %s\n", filepath));

	if(!wxFileExists(filepath)) {
		SetValue("Not found");
		return "";
	}

	tagpos = FindString(filetag);
	if(tagpos != wxNOT_FOUND) Delete(tagpos);
	Insert(filetag, 0);
	redtag = "";
	SetForegroundColour(mainwin->blackpen);
	SetValue("");
	SetValue(filetag);

	return filepath;
}

// StoreTag() checks for existing file using current tag with specified directory and suffix, if found overwrite warns with tag in red, and on repeat call updates tag list and returns file path
wxString TagBox::StoreTag(wxString dir, wxString suffix)
{
	wxString filetag, filepath;
	int tagpos;

	filetag = GetValue();
	filepath = dir + "/" + filetag + suffix;

	if(diagnostic) mainwin->diagbox->Write(text.Format("TagBox filepath %s\n", filepath));

	tagpos = FindString(filetag);
	if(tagpos != wxNOT_FOUND) Delete(tagpos);
	Insert(filetag, 0);

	if(wxFileExists(filepath) && redtag != filetag) {
		SetForegroundColour(mainwin->redpen);
		SetValue("");
		SetValue(filetag);
		redtag = filetag;
		return "";
	}

	redtag = "";
	SetForegroundColour(mainwin->blackpen);
	SetValue("");
	SetValue(filetag);

	return filepath;
}


void TagBox::PathUpdate()
{
    //if(mainwin->modpath == "") tagpath = mainwin->mainpath + modpath + "/Tags/";
    //else tagpath = mainwin->modpath + modpath + "/Tags/";   
    //if(!wxDirExists(tagpath)) wxMkdir(tagpath);

	if(boxpath.IsEmpty()) {
		if(mainwin->modpath.IsEmpty()) tagpath = "Tags";
		else tagpath = mainwin->modpath + "/Tags";
	}
	else {
		if(mainwin->modpath.IsEmpty()) tagpath = boxpath + "/Tags";
		else tagpath = mainwin->modpath + "/" + boxpath + "/Tags";
	}

	//if(diagnostic)
    mainwin->diagbox->Write(text.Format("TagBox PathUpdate() tagpath %s\n", tagpath));
}


TagBox::~TagBox()
{
	HistStore();
}


void TagBox::HistStore()
{
	int i;
	wxString filename, filepath, outline, text;
	TextFile opfile, tagfile;

	if(!wxDirExists(tagpath)) wxMkdir(tagpath);

	// Tag history
	if(tagfilename == "") return;
	tagfile.New(tagpath + "/" + tagfilename);
	for(i=GetCount()-1; i>=0; i--) {
		outline.Printf("tag %s", GetString(i));
		tagfile.WriteLine(outline);
	}
	tagfile.Close();

	// Fixed location option file, directs to selectable tagfile location
	opfile.New(tagpath + "/" + name + "op.ini");
	opfile.WriteLine(tagfilename);
	opfile.Close();
}


void TagBox::HistLoad()
{
	wxString filename, filepath;
	wxString readline, tag, text;
	TextFile tagfile;
	bool check;

	tag = "";

	// tag history load
	if(tagpath == "") {
		mainwin->diagbox->Write("Tag file not set\n");
		return;
	}
	check = tagfile.Open(tagpath + "/" + tagfilename);
	if(!check) {
		mainwin->diagbox->Write("No tag history\n");
		return;
	}

	if(diagnostic) mainwin->diagbox->Write("Reading tag history " + filename + "\n");

	readline = tagfile.ReadLine();

	while(!readline.IsEmpty()) {
		//diagbox->Write("Readline " + readline + "\n");
		readline = readline.AfterFirst(' ');
		readline.Trim();
		tag = readline;
		Insert(tag, 0);
		//diagbox->Write("Insert " + tag + "\n");
		readline = tagfile.ReadLine();
	}

	tagfile.Close();	
	SetLabel(tag);
	if(diagnostic) mainwin->diagbox->Write(name + " " + tag + "\n");
	if(tag != "") labelset = true;
}


void TagBox::OnDClick(wxMouseEvent& event)
{
	if(diagbox) diagbox->Write("\ntag dclick\n");
}


void TagBox::OnRClick(wxMouseEvent& event)
{
	if(diagbox) diagbox->Write("\ntag rclick\n");
	ChooseFile();
}


void TagBox::ChooseFile()
{
	mainwin->diagbox->Write("TagBox ChooseFile\n");

	wxFileDialog *filebox = new wxFileDialog(this, "Choose tag file", tagpath, tagfilename, "INI files (*.ini)|*.ini", wxFD_OVERWRITE_PROMPT);

	//wxFileDialog *filebox = new wxFileDialog(this, "Choose a directory", path, 0, wxDefaultPosition);
	if(filebox->ShowModal() == wxID_OK) {
		tagfilepath = filebox->GetPath(); 
		mainwin->diagbox->Write("Selected file path:" + tagfilepath + "\n");
		if(mainwin->ostype = Windows) tagfilename = tagfilepath.AfterLast('\\');
		else tagfilename = tagfilepath.AfterLast('/');
		mainwin->diagbox->Write("Selected:" + tagfilename + "\n");
		SetFile(tagfilename);
	}

	//path = filebox->GetPath();
	mainwin->diagbox->Write("TagBox tagpath " + tagpath);
}


void TagBox::SetFile(wxString newfilename)
{
	tagfilename = newfilename;
	Clear();
	HistLoad();
}


void TagBox::SetLabel(wxString label)
{
	wxComboBox::SetLabel(label);
	mainwin->diagbox->Write("TagBox SetLabel " + label + "\n");

	int tagpos = FindString(label);
	if(tagpos != wxNOT_FOUND) Delete(tagpos);
	Insert(label, 0);
}


wxBoxSizer *TagBox::TagCon(ToolBox *box, int storeid, int loadid, int orient)
{
	wxBoxSizer *storesizer = new wxBoxSizer(orient);
	wxBoxSizer *buttons = new wxBoxSizer(wxHORIZONTAL);

	if(box->ostype == Mac) {
		box->AddButton(storeid, "Store", 40, buttons);
		box->AddButton(loadid, "Load", 40, buttons);
	}
	else {
		box->AddButton(storeid, "Store", 40, buttons);
		buttons->AddSpacer(2);
		box->AddButton(loadid, "Load", 40, buttons);
	}
	storesizer->Add(this, 0, wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTRE_VERTICAL | wxALL, 2);
	storesizer->Add(buttons, 0, wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTRE_VERTICAL | wxALL, 2);	

	return storesizer;
}



//ToolStore::ToolStore(ToolBox *toolbox, wxString storetag, wxString path, int width, ToolPanel *toolpanel)

ToolStore::ToolStore(ToolBox *toolbox, TagBox *storetagbox, int storeid, int loadid)
{
	box = toolbox;
	tagbox = storetagbox;
	panel = box->activepanel;

	wxBoxSizer *storesizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *buttons = new wxBoxSizer(wxHORIZONTAL);

	if(box->ostype == Mac) {
		box->AddButton(storeid, "Store", 40, buttons);
		box->AddButton(loadid, "Load", 40, buttons);
	}
	else {
		box->AddButton(storeid, "Store", 40, buttons);
		buttons->AddSpacer(2);
		box->AddButton(loadid, "Load", 40, buttons);
	}
	storesizer->Add(tagbox, 0, wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTRE_VERTICAL | wxALL, 2);
	storesizer->Add(buttons, 0, wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTRE_VERTICAL | wxALL, 2);	
}


void ToolSet::AddBox(ToolBox *newbox, bool serve, bool child) {

	TextFile ofp;
	wxString text;

	//if(mod == NULL) ofp.WriteLine(text.Format("no mod"));
	//else ofp.WriteLine(text.Format("mod address %p", mod));


	// 'serve' and 'child' are old flags that define how toolbox movement is linked to other windows,
	// for most boxes serve = true and child = false
	// better to replace with more specific switches    1/6/21

	if(!newbox) return;
	newbox->toolset = this;

	if(serve && !newbox->servant) newbox->servant = true;
	//ofp.WriteLine(text.Format("box %s, child %d", newbox->boxtag, child));
	newbox->child = child;


	for(i=0; i<numtools; i++)             // Allow adding boxes after removal (not yet implemented)
		if(box[i] == NULL) {
			box[i] = newbox;
			newbox->boxindex = i;
			tagindex[newbox->boxtag] = i;
			return;
		}

	tagindex[newbox->boxtag] = numtools;
	tags[numtools] = newbox->boxtag;
	newbox->boxindex = numtools;
	box[numtools] = newbox;
	numtools++;
};


int ToolSet::GetIndex(wxString tag) {
	if(!tagindex.check(tag)) return -1;
	else return tagindex[tag];
}


wxString ToolSet::GetTag(int index) {	
	return tags[index];
}


/*
void ToolBox::OnLeftDClick(wxMouseEvent& event)
{
pinmode = 1 - pinmode;
snum.Printf("LDClick pin %d", pinmode);
mainwin->SetStatusText(snum);
}


void ToolBox::OnRightDClick(wxMouseEvent& event)
{
pinmode = 1 - pinmode;
snum.Printf("RDClick pin %d", pinmode);
mainwin->SetStatusText(snum);
}
*/


//void ToolBox::OnLeftClick(wxMouseEvent& event)
//{
//	pinmode = 1 - pinmode;
//	//snum.Printf("Tool LClick");
//	//mainwin->SetStatusText("Tool Click");
//
//}
//
//
//void ToolBox::OnMouseMove(wxMouseEvent &event)
//{
//	wxPoint pos = event.GetPosition();
//	snum.Printf("ToolMove X %d Y %d", pos.x, pos.y);
//	mainwin->SetStatusText(snum);
//}

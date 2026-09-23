

#include "wx/wx.h"
#include "hypogrid.h"
#include "hypomain.h"
#include "wx/grid.h"

#include <string>
#include <fstream>

using std::string;
using std::ifstream;
using std::ofstream;

SpikePanel::SpikePanel(NeuroBox *box)
	: ToolPanel(box, box->auitabpanel)
{
	int datwidth, labelwidth, buttspace;
    ostype = GetSystem();

	neurobox = box;

	selectcount = 2;
	neuroindex = 0;
	neurocount = 0;
	cellmode = 0;

	ratetag = "";
	graphwin = NULL;

	selectspikes[0].resize(100000);
	selectspikes[1].resize(100000);

	for(i=0; i<100000; i++) {
		selectspikes[0][i] = 0;
		selectspikes[1][i] = 0;
	}

	this->SetFont(neurobox->boxfont);
	wxBoxSizer *mainbox = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(mainbox);
	neurobox->activepanel = this;
	neurobox->paramset.panel = this;

	// Multi Cell Plots
	//neuropop.popdat->PlotSetBasic(box->mod->graphbase, "MultiCell ", green, 1, "multicell");
	//GraphSet *graphset = box->mod->graphbase->NewSet("MultiCell Intervals", "multiintervals");
	//graphset->IntervalSetBasic("multicell", false, false);


	/////////////////////////////////////////////////////////
	// Neuron selection
	//
	datwidth = 50;
	labelwidth = 70;
	label = neurobox->NumPanel(labelwidth, wxALIGN_CENTRE);
	spikes = neurobox->NumPanel(datwidth, wxALIGN_RIGHT);
	freq = neurobox->NumPanel(datwidth, wxALIGN_RIGHT);
	selectspikecount = neurobox->NumPanel(datwidth, wxALIGN_RIGHT);
	selectfreq = neurobox->NumPanel(datwidth, wxALIGN_RIGHT);

	wxGridSizer *datagrid = new wxGridSizer(2, 5, 5);
	datagrid->Add(new wxStaticText(this, -1, "Name"), 0, wxALIGN_CENTRE);
	datagrid->Add(label);
	datagrid->Add(new wxStaticText(this, -1, "Spikes"), 0, wxALIGN_CENTRE);
	datagrid->Add(spikes);
	datagrid->Add(new wxStaticText(this, -1, "Freq"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	datagrid->Add(freq);
	datagrid->Add(new wxStaticText(this, -1, "Select Spikes"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	datagrid->Add(selectspikecount);
	datagrid->Add(new wxStaticText(this, -1, "Select Freq"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	datagrid->Add(selectfreq);

	// need to fix this for multiple use
	filtercheck = neurobox->SetBoxCheck(ID_filtercheck, "cellfilter", "Filter", false);
	
	datneuron = new wxTextCtrl(this, ID_Neuron, "---", wxDefaultPosition, wxSize(50, -1), wxALIGN_LEFT|wxBORDER_SUNKEN|wxST_NO_AUTORESIZE|wxTE_PROCESS_ENTER);
	if(neurobox->ostype == Mac) 
		datspin = new wxSpinButton(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxSP_HORIZONTAL|wxSP_ARROW_KEYS);
	else 
		datspin = new wxSpinButton(this, wxID_ANY, wxDefaultPosition, wxSize(40, 17), wxSP_HORIZONTAL|wxSP_ARROW_KEYS);
	datspin->SetRange(-1000000, 1000000);

	wxBoxSizer *datbox = new wxBoxSizer(wxHORIZONTAL);
	datbox->Add(datspin, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	datbox->AddSpacer(5);

	wxBoxSizer *cellbox = new wxBoxSizer(wxHORIZONTAL);
	cellbox->Add(new wxStaticText(this, wxID_ANY, "Neuron"), 1, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	cellbox->Add(datneuron, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

	wxStaticBoxSizer *databox = new wxStaticBoxSizer(wxVERTICAL, this, "");
	databox->AddSpacer(2);
	databox->Add(cellbox, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL| wxALL, 5);
	databox->AddSpacer(5);
	databox->Add(datbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL| wxALL, 0);
	databox->AddSpacer(5);
	databox->Add(datagrid, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);
	databox->AddSpacer(5);
	databox->Add(filtercheck, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);

	/////////////////////////////////////////////////////////
	// Spike selection
	//
	fromcon = neurobox->paramset.AddNum("from", "From", 0, 0, 30); 
	tocon = neurobox->paramset.AddNum("to", "To", 100, 0, 20); 
	selectcon[0] = new wxStaticBoxSizer(wxHORIZONTAL, this, "Selection 1");
	selectcon[1] = new wxStaticBoxSizer(wxHORIZONTAL, this, "Selection 2");
	
	wxBoxSizer *fromtobox = new wxBoxSizer(wxHORIZONTAL);
	fromtobox->Add(fromcon, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
	fromtobox->Add(tocon, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);

	buttspace = 20;
	for(i=0; i<selectcount; i++) {
		addbutton[i] = ToggleButton(100 + i, "Add", 40, selectcon[i]);	
		selectcon[i]->AddSpacer(buttspace);
		subbutton[i] = ToggleButton(200 + i, "Sub", 40, selectcon[i]);	
		selectcon[i]->AddSpacer(buttspace);
		neurobox->AddButton(300 + i, "Clear", 40, selectcon[i]);
		selectcon[i]->AddSpacer(buttspace);
		neurobox->AddButton(400 + i, "Invert", 40, selectcon[i]);
		selectmode[i] = 1;
	}

	currselect = 0;
	addbutton[currselect]->SetValue(true);

	wxBoxSizer *selectconbox = new wxBoxSizer(wxVERTICAL);
	selectconbox->Add(fromtobox, 0, wxALIGN_CENTRE_HORIZONTAL);
	selectconbox->AddSpacer(10);
	selectconbox->Add(selectcon[0], 0);
	selectconbox->AddSpacer(10);
	selectconbox->Add(selectcon[1], 0);

	wxBoxSizer *columnbox = new wxBoxSizer(wxHORIZONTAL); 
	columnbox->AddStretchSpacer();
	columnbox->Add(databox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	columnbox->AddSpacer(20);
	//colbox2->AddStretchSpacer();
	columnbox->Add(selectconbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//colbox2->AddStretchSpacer();

	// Multi Cell Data
	multicount = neurobox->NumPanel(datwidth, wxALIGN_RIGHT);
	multifreq = neurobox->NumPanel(datwidth, wxALIGN_RIGHT);
	wxBoxSizer *multibox = new wxStaticBoxSizer(wxHORIZONTAL, this, "Multi");
	wxGridSizer *multigrid = new wxGridSizer(2, 5, 5);
	multigrid->Add(new wxStaticText(this, -1, "Count"), 0, wxALIGN_CENTRE);
	multigrid->Add(multicount);
	multigrid->Add(new wxStaticText(this, -1, "Freq"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
	multigrid->Add(multifreq);
	multibox->Add(multigrid, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

	//wxBoxSizer *colbox2box = new wxBoxSizer(wxVERTICAL);
	selectconbox->AddSpacer(20);
	selectconbox->Add(multibox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);


	mainbox->AddStretchSpacer();
	mainbox->Add(columnbox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	mainbox->AddStretchSpacer();
	this->Layout();

	Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(SpikePanel::OnEnter));
    if(ostype == Mac) {
        Connect(wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler(SpikePanel::OnNext));
        Connect(wxEVT_SCROLL_LINEUP, wxSpinEventHandler(SpikePanel::OnPrev));
    }
    else {
        Connect(wxEVT_SCROLL_LINEUP, wxSpinEventHandler(SpikePanel::OnNext));
        Connect(wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler(SpikePanel::OnPrev));
    }
	Connect(300, 305, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SpikePanel::OnClear));
	Connect(400, 405, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SpikePanel::OnInvert));
}


void SpikePanel::SetData(SpikeDat *dataneuron, std::vector<NeuroDat>*dataneurons)
{
	currneuron = dataneuron;
	neurons = dataneurons;

	currneuron->SelectInit();
	currneuron->dispmodemax = 2;
	currneuron->diagbox = neurobox->diagbox;

	neuropop.neurons = neurons;
}


void SpikePanel::PanelData(NeuroDat *data)
{
	if(!data) data = &(*neurons)[neuroindex];

	if(!data) {
		toolbox->diagbox->Write("SpikePanel data error\n");
		return;
	}

	toolbox->diagbox->Write("SpikePanel data OK\n");

	if((*neurons)[neuroindex].netflag) snum = "sum";
	else snum = numstring(neuroindex, 0);
	datneuron->SetLabel(snum);

	label->SetLabel("panel test");

	label->SetLabel((*neurons)[neuroindex].name);
	spikes->SetLabel(snum.Format("%d", data->spikecount));
	freq->SetLabel(snum.Format("%.2f", data->freq));
	selectspikecount->SetLabel(snum.Format("%d", currneuron->selectdata->intraspikes));
	selectfreq->SetLabel(snum.Format("%.2f", currneuron->selectdata->freq));
}


void SpikePanel::OnPrev(wxSpinEvent& WXUNUSED(event))
{
    neurobox->diagbox->Write("OnPrev\n");
    PrevCell();
}


void SpikePanel::PrevCell()
{
	if(!neurocount) return;
	if(neuroindex > 0) neuroindex--;
	else neuroindex = neurocount-1;

	// Store rate X-axis position
	graphwin = neurobox->mod->GetGraphWin(ratetag);
	if(graphwin) currneuron->neurodata->xscrollpos = graphwin->xscrollpos;

	// Store select grids
	for(int i=0; i<selectcount; i++) {
		currneuron->selectdata->spikes = selectspikes[i].data();
		currneuron->SelectScan(i);  // store current cell's select grid to NeuroDat
	}
	NeuroData();
}


void SpikePanel::OnNext(wxSpinEvent& WXUNUSED(event))
{
    neurobox->diagbox->Write("OnNext\n");
	NextCell();
}


void SpikePanel::NextCell()
{
	if(!neurocount) return;
	if(neuroindex < neurocount-1) neuroindex++;
	else neuroindex = 0;

	//neurobox->mainwin->graphwin[0]->SetScroll(0); 

	// Store rate X-axis position
	graphwin = neurobox->mod->GetGraphWin(ratetag);
	if(graphwin) currneuron->neurodata->xscrollpos = graphwin->xscrollpos;

	// Store select grids
	for(int i=0; i<selectcount; i++) {
		currneuron->selectdata->spikes = selectspikes[i].data();
		currneuron->SelectScan(i);  // store current cell's select grid to NeuroDat
	}

	NeuroData();
}


void SpikePanel::OnEnter(wxCommandEvent& event)
{
	int id = event.GetId();
	long data;

	// Enter pressed for neuron selection
	if(id == ID_Neuron) {
		datneuron->GetValue().ToLong(&data);
		if(data >= 0 && data < neurocount) {
			neuroindex = data;
			NeuroData();
		}
		return;
	}
	else NeuroData();
}


void SpikePanel::NeuroData(bool dispupdate)
{	
	ParamStore *calcparams = neurobox->GetParams();

	//if((*neurons)[neuroindex].spikecount >= currneuron->times.size()) currneuron->ReSize((*neurons)[neuroindex].spikecount);

	currneuron->normscale = (*calcparams)["normscale"];
	currneuron->binoffset = (*calcparams)["binoffset"];

	currneuron->neurocalc(&(*neurons)[neuroindex]);
	currneuron->id = neuroindex;
	currneuron->name = (*neurons)[neuroindex].name;

#ifdef HYPOSOUND
	if(mainwin->soundbox) mainwin->soundbox->DataLink(currneuron);
#endif

	for(i=0; i<selectcount; i++) {
		currneuron->selectdata->spikes = selectspikes[i].data();
		currneuron->SelectSpikes(i);  // store current cell's select grid to NeuroDat
	}
	currneuron->selectdata->spikes = selectspikes[currselect].data();
	AnalyseSelection();

	if(cellmode) {
		if(neurobox->burstbox) {
			neurobox->diagbox->Write(text.Format("SpikePanel NeuroData() numselects %d\n", currneuron->neurodata->numselects[currselect]));
			neurobox->burstbox->ExpDataScan(currneuron);
			//burstbox->SetExpGrid();
		}
		neurobox->mod->SpikeDataSwitch(currneuron);   // Switch evo fitting data source
	}

	// Restore rate plot scroll position
	if(graphwin) graphwin->ScrollX(currneuron->neurodata->xscrollpos);

	if(dispupdate) {
		PanelData();
		mainwin->scalebox->GraphUpdate();	
	}

	neurobox->SetCheck(filtercheck, (*neurons)[neuroindex].filter);

	neurobox->diagbox->Write(text.Format("NeuroData cell %d gridcol %d\n", neuroindex, (*neurons)[neuroindex].gridcol));
	if(cellmode) {
		neurobox->gridbox->textgrid[0]->SelectCol((*neurons)[neuroindex].gridcol);
		neurobox->gridbox->textgrid[0]->MakeCellVisible(0, (*neurons)[neuroindex].gridcol);
	}

	//mod->DataOutput();
}


// OnToggle, from ToolBox, used here for Add/Sub toggle buttons
void SpikePanel::OnToggle(wxCommandEvent& event)      
{
	int sel, type, i;
	wxString text;
	int id = event.GetId();

	type = id / 100;
	sel = id % 100;

	AddSubToggle(sel, type);
	SelectUpdate();
}


// AddSubToggle clears the Add and Sub toggle buttons for each select and sets the specified button
void SpikePanel::AddSubToggle(int sel, int type)      
{
	int i;
	wxString text;

	for(i=0; i<selectcount; i++) {
		addbutton[i]->SetValue(false);
		subbutton[i]->SetValue(false);
	}

	if(type == 1) addbutton[sel]->SetValue(true);
	if(type == 2) subbutton[sel]->SetValue(true);

	selectmode[sel] = type;	
	currselect = sel;

	neurobox->diagbox->Write(text.Format("\nSelect Button type %d  selection %d\n", type, sel));
}


// OnClick, from ToolBox, used here for detecting clicks in select control panels to switch between selects
void SpikePanel::OnClick(wxPoint pos)
{
	wxString text;
	bool select = false;

	wxRect selrect1 = wxRect(selectcon[0]->GetPosition(), selectcon[0]->GetSize());
	wxRect selrect2 = wxRect(selectcon[1]->GetPosition(), selectcon[1]->GetSize());

	if(selrect1.Contains(pos) || selrect2.Contains(pos)) select = true;
	if(selrect1.Contains(pos)) currselect = 0;
	if(selrect2.Contains(pos)) currselect = 1;

	AddSubToggle(currselect, 1);
	SelectUpdate();      

	if(select) neurobox->diagbox->Write(text.Format("Neuro Box Click x %d y %d  Select %d\n", pos.x, pos.y, currselect));
	mainwin->scalebox->BurstDisp(1);
}


void SpikePanel::SetSelectRange(double from, double to)
{
	fromcon->SetValue(from);
	tocon->SetValue(to);

	if(selectmode[currselect] == 1) SelectAdd();
	if(selectmode[currselect] == 2) SelectSub();
}


void SpikePanel::SelectAdd()
{
	int sfrom, sto;
	wxString text;

	if(diagnostic) neurobox->diagbox->Write(text.Format("Add %d\n", currselect));

	//ParamStore *selectparams = neurobox->GetParams();
	//sfrom = (*selectparams)["from"] * 1000;         // Convert from s to ms
	//sto = (*selectparams)["to"] * 1000;
	sfrom = fromcon->GetValue() * 1000;         // Convert from s to ms
	sto = tocon->GetValue() * 1000;

	//diagbox->Write(text.Format("spike count %d\n", numspikes));

	for(i=0; i<currneuron->spikecount; i++) {
		if(currneuron->times[i] > sfrom && currneuron->times[i] < sto) {               // should this be <= and >= ?														
			selectspikes[currselect][i] = currselect + 1;		
		}	
	}

	SelectUpdate();
}


void SpikePanel::SelectSub()
{
	int sfrom, sto;

	//ParamStore *selectparams = neurobox->GetParams();
	//sfrom = (int)(*selectparams)["from"] * 1000;
	//sto = (int)(*selectparams)["to"] * 1000;
	sfrom = fromcon->GetValue() * 1000;         // Convert from s to ms
	sto = tocon->GetValue() * 1000;
	
	for(i=0; i<currneuron->spikecount; i++) {
		if(currneuron->times[i] > sfrom && currneuron->times[i] < sto) {
			selectspikes[currselect][i] = 0;
		}
	}

	SelectUpdate();
}


void SpikePanel::SelectUpdate()
{
	if(!currneuron->spikecount) return;      // use spikecount to check for spike data

	currneuron->selectdata->spikes = selectspikes[currselect].data();
	if(!currneuron->colourdata) {
		currneuron->ColourSwitch(2);
		mainwin->scalebox->ratedata = 2;
		mainwin->scalebox->databutton->SetLabel("Select");
	}

	/*
	// Switch to select plots if not in burst mode
	if(!(*mainwin->scalebox->gflags)["burstmode"]) {
	(*mainwin->scalebox->gflags)["selectmode"] = 1;
	mainwin->scalebox->GraphSwitch();
	}
	*/

	//diagbox->textbox->AppendText(text.Format("sub%d from %d to %d\n", currselect, sfrom, sto));

	AnalyseSelection();
	if(cellmode && neurobox->burstbox) neurobox->burstbox->ExpDataScan(currneuron);
	mainwin->scalebox->GraphUpdate();
}


/*
// OnAdd, out of use since switch to toggle button
void NeuroBox::OnAdd(wxCommandEvent& event)
{
	currselect = event.GetId() - 100;
	SelectAdd();

	diagbox->Write(text.Format("\nAdd Button selection %d\n", currselect));
}


// OnSub, out of use since switch to toggle button
void NeuroBox::OnSub(wxCommandEvent& event)
{
	currselect = event.GetId() - 200;
	SelectSub();

	diagbox->Write(text.Format("\nSub Button selection %d\n", currselect));
}
*/


void SpikePanel::OnInvert(wxCommandEvent& event)
{
	int sel = event.GetId() - 400;

	for(i=0; i<currneuron->spikecount; i++) selectspikes[sel][i] = (sel + 1) - selectspikes[sel][i];

	currselect = sel;
	AddSubToggle(currselect, 1);
	SelectUpdate();
}


void SpikePanel::OnClear(wxCommandEvent& event)
{
	int sel = event.GetId() - 300;

	for(i=0; i<currneuron->spikecount; i++) selectspikes[sel][i] = 0;

	currselect = sel;
	AddSubToggle(currselect, 1);
	SelectUpdate();
}


void SpikePanel::AnalyseSelection()
{
	int i, selspike;
	int numspikes, scount;
	int sfrom, sto;
	double isi, timepoint;
	double intracount, intratime;
	wxString text;

	if(!currneuron->selectdata) currneuron->selectdata = new BurstDat();
	currneuron->selectdata->times = currneuron->times.data();

	//currneuron->SelectScan();
	currneuron->SelectScan(currselect);

	currneuron->selectdata->IntraBurstAnalysis();
	if(currneuron->neurodata->Selected()) {
		currneuron->SelectFitAnalysis();
		currneuron->IoDfit = currneuron->selectdata->IoDdata.data.data();
		if(diagnostic) neurobox->diagbox->Write("NeuroBox select fit mode\n");
	}
	else {
		currneuron->IoDfit = currneuron->IoDdata.data.data();
		if(diagnostic) neurobox->diagbox->Write("NeuroBox basic fit mode\n");
	}

	//mod->SelectBurst(selectdata[currselect]);

	PanelData();

	if(diagnostic) neurobox->diagbox->Write(text.Format("\nSelect analyse %d spikes %.2fHz time %.2f test %d\n", 
		currneuron->selectdata->intraspikes, currneuron->selectdata->freq, currneuron->selectdata->intratime, currneuron->selectdata->test));

	//if(currselect == 0) mod->burstbox->BurstDataDisp(mod->spikedata, mod->burstbox->modburst);
	//if(currselect == 1) mod->burstbox->BurstDataDisp(mod->spikedata, mod->burstbox->evoburst);
}


void SpikePanel::MultiCellAnalysis()
{
	neuropop.SpikeAnalysis();
	multicount->SetLabel(snum.Format("%d", neuropop.numneurons));
	multifreq->SetLabel(snum.Format("%.2f", neuropop.popfreq));
}


NeuroDat *SpikePanel::GetCell(wxString name) 
{
	for(i=0; i<neurocount; i++) if((*neurons)[i].name == name) return &(*neurons)[i];
	return NULL;
}


void SpikePanel::SelectStore()
{
	int cellindex, select;
	wxString filepath;
	wxString filedir, filesuffix;
	wxString text;
	TextFile selectfile;
	NeuroDat *cell;

	if(currneuron->spikecount) {
		for(select=0; select<selectcount; select++) {
			currneuron->selectdata->spikes = selectspikes[select].data();
			currneuron->SelectScan(select); 
		}
	}

	filedir = neurobox->mod->GetPath() + "/Tools";
	if(!wxDirExists(filedir)) wxMkdir(filedir);
	filesuffix = "-select.dat";

	// Select data file
	filepath = neurobox->selectstoretag->StoreTag(filedir, filesuffix);
	if(filepath.IsEmpty()) return;

	selectfile.New(filepath);
	for(cellindex=0; cellindex<neurocount; cellindex++) {
		cell = &(*neurons)[cellindex];
		for(select=0; select<selectcount; select++) {
			if(diagnostic) neurobox->diagbox->Write(text.Format("SelectStore cell %d select %d numselects %d\n", cellindex, select, cell->numselects[select]));
			for(i=1; i<=cell->numselects[select]; i++) {
				text.Printf("dat %s  cel %d  sel %d  index %d  sta %d  end %d", 
					cell->name, cellindex, select, i, cell->selectstore[select][i].start, cell->selectstore[select][i].end);	
				selectfile.WriteLine(text);
			}
		}
	}
	selectfile.Close();
}


void SpikePanel::SelectLoad()
{
	wxString filedir, filepath;
	wxString filesuffix;
	TextFile selectfile;
	wxString readline;
	int index, start, end;
	int cellindex, select;
	wxString name;
	bool diagnostic = true;
	NeuroDat *cell;

	if(currneuron->selectdata) currneuron->selectdata->spikes = selectspikes[currselect].data();

	for(i=0; i<neurocount; i++) {
		for(select=0; select<selectcount; select++) (*neurons)[i].numselects[select] = 0;
	}

	filedir = neurobox->mod->GetPath() + "/Tools";
	filesuffix = "-select.dat";
	filepath = neurobox->selectstoretag->LoadTag(filedir, filesuffix);
	if(filepath.IsEmpty()) return;

	if(diagnostic) neurobox->diagbox->Write("SelectLoad " + filepath + "\n");

	selectfile.Open(filepath);
	readline = selectfile.ReadLine();

	// Updated to reference by data tag string instead of index - October 2020

	while(!readline.IsEmpty()) {
		name = "";
		if(readline.GetChar(0) == 'd') name = ParseString(&readline, 't');
		cellindex = ParseLong(&readline, 'l');
		if(diagnostic) neurobox->diagbox->Write(text.Format("SelectLoad name %s  index %d\n", name, cellindex));
		if(!name.IsEmpty()) cell = GetCell(name);   // new file format
		else cell = &(*neurons)[cellindex];           // old file format or blank name
		if(!cell) {
			if(diagnostic) neurobox->diagbox->Write("NULL cell\n");
			readline = selectfile.ReadLine();	
			continue;
		}
		if(diagnostic) neurobox->diagbox->Write(text.Format("index check readline %s check %d\n", readline, ParseCheck(&readline, "sel")));
		if(ParseCheck(&readline, "sel")) select = ParseLong(&readline, 'l');
		else select = currselect;
		index = ParseLong(&readline, 'x');
		cell->numselects[select]++;
		cell->selectstore[select][index].start = ParseLong(&readline, 'a');
		cell->selectstore[select][index].end = ParseLong(&readline, 'd');
		if(diagnostic) neurobox->diagbox->Write(text.Format("SelectLoad  cell %d  index %d  start %d  end %d\n", 
			cellindex, index, cell->selectstore[select][index].start, cell->selectstore[select][index].end)); 
		if(selectfile.End()) break;
		readline = selectfile.ReadLine();	
	}

	if(currneuron->spikecount) {
		for(i=0; i<selectcount; i++) {
			currneuron->selectdata->spikes = selectspikes[i].data();
			currneuron->SelectSpikes(i);  // store current cell's select grid to NeuroDat
		}
		currneuron->selectdata->spikes = selectspikes[currselect].data();
		AnalyseSelection();
		if(!currneuron->colourdata) {
			currneuron->ColourSwitch(2);
			mainwin->scalebox->ratedata = 2;
			mainwin->scalebox->databutton->SetLabel("Select");
		}
	}

	mainwin->scalebox->GraphUpdate();
	selectfile.Close();	
}



NeuroBox::NeuroBox(Mod *modarg, const wxString& title, const wxPoint& pos, const wxSize& size)
	: ParamBox(modarg, title, pos, size, "cellbox", 1, false)
	//ParamBox(model, title, pos, size, "outbox", 0, 1)
{
	int datwidth, labelwidth;

	mod = modarg;
	diagbox = mod->diagbox;
	//cellcount = 0;
	paramindex = 0;
	textgrid = NULL;
	burstbox = NULL;
	cellpanel = NULL;
	modpanel = NULL;

	selfstore = true;

	//selectcount = 2;
	//neuroindex = 0;
	
	//selectspikes[0].resize(100000);
	//selectspikes[1].resize(100000);
	spikeselectLink = false;

	//for(i=0; i<100000; i++) {
	//	selectspikes[0][i] = 0;
	//	selectspikes[1][i] = 0;
	//}

	long notestyle = wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS;
	//wxAuiNotebook *tabpanel = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, notestyle);
	 auitabpanel = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, notestyle);

	 // Analysis parameters

	ToolPanel *analysispanel = new ToolPanel(this, auitabpanel);
	//ToolPanel *analysispanel = new ToolPanel(tabpanel, this, wxPoint(-1, -1), wxSize(-1, -1));
	analysispanel->SetFont(boxfont);
	wxBoxSizer *analysisbox = new wxBoxSizer(wxVERTICAL);
	analysispanel->SetSizer(analysisbox);
	activepanel = analysispanel;
	paramset.panel = analysispanel;

	wxBoxSizer *histparambox = new wxBoxSizer(wxVERTICAL);
	paramset.AddNum("normscale", "Norm Scale", 10000, 0, 70, 50);
	paramset.AddNum("histrange", "Hist Range", 1000, 0, 70, 50);
	paramset.AddNum("filterthresh", "ISI Filter", 5, 0, 70, 50);
	paramset.AddNum("binoffset", "30s Bin Offset", 0, 2, 70, 50);
	//paramset.AddNum("binsize", "Bin Size", 5, 0, 70, 50);
	PanelParamLayout(histparambox);

	wxBoxSizer *colbox = new wxBoxSizer(wxHORIZONTAL); 
	//colbox->AddStretchSpacer();
	//colbox->Add(databox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//colbox->AddSpacer(10);
	colbox->Add(histparambox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//colbox->AddStretchSpacer();

	analysisbox->AddStretchSpacer();
	analysisbox->Add(colbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	analysisbox->AddStretchSpacer();
	analysispanel->Layout();


	// Spike data panel (including selection)

	cellpanel = new SpikePanel(this);
	cellpanel->cellmode = true;
	cellpanel->ratetag = "cellspikes";

	//modpanel = new SpikePanel(this);

	// Data Loading - currently for batch loading 

	ToolPanel *loadpanel = new ToolPanel(this, auitabpanel);
	loadpanel->SetFont(boxfont);
	wxBoxSizer *loadbox = new wxBoxSizer(wxVERTICAL);
	loadpanel->SetSizer(loadbox);
	activepanel = loadpanel;
	paramset.panel = loadpanel;

	wxBoxSizer *datapathbox = new wxBoxSizer(wxHORIZONTAL);
	paramset.AddText("datapath", "Data Path", "", 60, 250);
	datapathbox->Add(paramset.GetCon("datapath"), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxRIGHT|wxLEFT, 5);
	AddButton(ID_PathBrowse, "Browse", 60, datapathbox);

	wxBoxSizer *datatagbox = new wxBoxSizer(wxHORIZONTAL);
	neurodatatag = new TagBox(mainwin, activepanel, ID_Select, "", wxDefaultPosition, wxSize(150, -1), "neurodatatag", mod->path);
	datatagbox->Add(neurodatatag, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 2);
	if(ostype == Mac) {
		AddButton(ID_Load, "Load", 60, datatagbox);
		AddButton(ID_FileBrowse, "Browse", 60, datatagbox);
	}
	else {
		AddButton(ID_Load, "Load", 40, datatagbox);
		datatagbox->AddSpacer(2);
		AddButton(ID_FileBrowse, "Browse", 50, datatagbox);
	}

	loadbox->AddStretchSpacer();
	loadbox->Add(datapathbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	loadbox->AddSpacer(10);
	loadbox->Add(datatagbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	loadbox->AddStretchSpacer();
	loadpanel->Layout();


	//////////////////////////////////////////////////
	// Main Structure

	auitabpanel->Freeze();
	auitabpanel->AddPage(cellpanel, "Cell Select" , true);
	//if(modpanel) auitabpanel->AddPage(modpanel, "Mod Select" , false);
	auitabpanel->AddPage(analysispanel, "Analysis" , false);
	auitabpanel->AddPage(loadpanel, "Loading" , false);
	auitabpanel->Thaw();

	ToolPanel *storepanel = new ToolPanel(this, wxDefaultPosition, wxDefaultSize);
	wxBoxSizer *storebox = new wxBoxSizer(wxVERTICAL);
	storepanel->SetSizer(storebox);

	activepanel = storepanel;
	selectstoretag = new TagBox(mainwin, activepanel, ID_Select, "", wxDefaultPosition, wxSize(150, -1), "selectstoretag", mod->path);
	wxBoxSizer *selectstorebox = selectstoretag->TagCon(this, ID_selectstore, ID_selectload);

	//selectstorebox->Add(selectstoretag, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//wxBoxSizer *selectbuttons = new wxBoxSizer(wxHORIZONTAL);
	//AddButton(ID_selectstore, "Store", 40, selectbuttons, 2);
	//AddButton(ID_selectload, "Load", 40, selectbuttons, 2);
	//selectstorebox->Add(selectbuttons, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);

	//wxBoxSizer *filterbox = new wxBoxSizer(wxHORIZONTAL);
	selectstorebox->AddSpacer(20);
	AddButton(ID_filter, "Grid Remove", 70, selectstorebox);
	selectstorebox->AddSpacer(5);
	AddButton(ID_keep, "Grid Keep", 60, selectstorebox);

	//filterbox->AddSpacer(20);
	//filterbox->Add(selectstorebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	//filterbox->Add(selectstorebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
	
	storebox->Add(selectstorebox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);	
	storepanel->Layout();

	winman->AddPane(auitabpanel, wxAuiPaneInfo().Name("tabpane").CentrePane().PaneBorder(false));
	winman->AddPane(storepanel, wxAuiPaneInfo().Name("storepane").Bottom().CaptionVisible(false).BestSize(-1, 30).PaneBorder(false));
	winman->Update();

	if(selfstore) Load();   // load self-stored tool parameter values

	
	Connect(ID_Load, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NeuroBox::OnLoadData));

	//Connect(100, 105, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NeuroBox::OnAdd));        // old code from before Add and Sub made toggle buttons
	//Connect(200, 205, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NeuroBox::OnSub));

	Connect(ID_filter, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NeuroBox::OnGridFilter));
	Connect(ID_keep, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NeuroBox::OnGridFilter));

	Connect(ID_PathBrowse, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NeuroBox::OnBrowse));
	Connect(ID_FileBrowse, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NeuroBox::OnBrowse));

	Connect(ID_selectstore, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NeuroBox::OnSelectStore));
	Connect(ID_selectload, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NeuroBox::OnSelectLoad));
}


void NeuroBox::AddModSpikePanel(SpikeDat *spikedata, std::vector<NeuroDat>*dataneurons)
{
	modpanel = new SpikePanel(this);
	modpanel->SetData(spikedata, dataneurons);

	auitabpanel->Freeze();
	auitabpanel->InsertPage(1, modpanel, "Mod Select" , false);
	auitabpanel->Thaw();
	winman->Update();
}


void NeuroBox::OnSelectStore(wxCommandEvent& event)
{
	cellpanel->SelectStore();
}


void NeuroBox::OnSelectLoad(wxCommandEvent& event)
{
	cellpanel->SelectLoad();
}


void NeuroBox::OnBoxCheck(wxCommandEvent &event)
{
	int id = event.GetId();
	wxString checktag = checkrefs->GetRef(id);

	if((*modflags)[checktag] == 0) (*modflags)[checktag] = 1;
	else (*modflags)[checktag] = 0;

	diagbox->Write(text.Format("NeuroBox OnBoxCheck id %d\n", id));

	// Set cell for filtering
	if(checktag == "cellfilter") {
		(*cellpanel->neurons)[cellpanel->neuroindex].filter = (*modflags)[checktag];
	}
}


void NeuroBox::OnGridFilter(wxCommandEvent &event)
{
	int filtermode = 0;

	diagbox->Write("Grid filter clicked\n");

	if(event.GetId() == ID_keep) filtermode = 1;
	gridbox->NeuroGridFilter(filtermode);
	gridbox->NeuroScan();
}


void NeuroBox::Analysis()           // Specific to generating histogram and hazard grid output - currently out of use
{
	int i, j;
	int binsize = 5;
	int timerange = 1000;
	int gridmax = 500;
	int histcount;
	wxString text;

	ParamStore *calcparams = GetParams();
	cellpanel->currneuron->normscale = (*calcparams)["normscale"];
	timerange = (*calcparams)["histrange"];
	//binsize = (*calcparams)["binsize"];
	if(timerange < 0) timerange = 0;
	histcount = timerange / binsize;
	if(histcount > gridmax) histcount = gridmax;

	/*
	for(j=0; j<histcount; j++) {
		gridbox->histgrid->SetCell(j + 1, 0, text.Format("%d", j * 5));
		gridbox->hazgrid->SetCell(j + 1, 0, text.Format("%d", j * 5));
	}

	for(i=0; i<cellcount; i++) {
		//diagbox->Write(text.Format("Scanning cell %d %s ", i, celldata[i].name));
		databox->histgrid->SetCell(0, i, celldata[i].name);
		databox->hazgrid->SetCell(0, i, celldata[i].name);
		//diagbox->Write("Calling neurocalc\n");
		viewcell[0].neurocalc(&(celldata[i]));
		//diagbox->Write("OK\n"); 
		for(j=0; j<histcount; j++) {
			databox->histgrid->SetCell(j + 1, i + 1, text.Format("%.4f", viewcell[0].hist5norm[j]));
			databox->hazgrid->SetCell(j + 1, i + 1, text.Format("%.4f", viewcell[0].haz5[j]));
		}
	}
	*/
	//databox->notebook->SetPageText(1, text.Format("%dms Histograms", binsize));
	//databox->notebook->SetPageText(2, text.Format("%dms Hazards", binsize));
}


void NeuroBox::OnBrowse(wxCommandEvent& event)
{
	if(event.GetId() == ID_PathBrowse) {
		wxDirDialog *dirdialog = new wxDirDialog(this, "Choose a directory", paramset.GetCon("datapath")->GetLabel(), 0, wxDefaultPosition);
		if(dirdialog->ShowModal() == wxID_OK) paramset.GetCon("datapath")->SetValue(dirdialog->GetPath()); 
	}

	if(event.GetId() == ID_OutputBrowse) {
		wxDirDialog *dirdialog = new wxDirDialog(this, "Choose a directory", paramset.GetCon("outpath")->GetLabel(), 0, wxDefaultPosition);
		if(dirdialog->ShowModal() == wxID_OK) paramset.GetCon("outpath")->SetValue(dirdialog->GetPath()); 
	}

	if(event.GetId() == ID_FileBrowse) {
		wxFileDialog *filedialog = new wxFileDialog(this, "Choose a file", neurodatatag->GetLabel(), "", "", 0, wxDefaultPosition);
		if(filedialog->ShowModal() == wxID_OK) neurodatatag->SetValue(filedialog->GetFilename()); 
	}
}


void NeuroBox::OnLoadData(wxCommandEvent& event)
{
	wxString filetag, filepath, text;
	short tagpos;
	FileDat *file;

	filetag = neurodatatag->GetValue();
	filepath = paramset.GetCon("datapath")->GetString();
	//filepath = "C:/Data/VMN";
	FileDat newfile = FileDat(filetag, filepath);

	tagpos = neurodatatag->FindString(filetag);

	file = mainwin->filebase->Add(FileDat(filetag, filepath));

	if(tagpos != wxNOT_FOUND) neurodatatag->Delete(tagpos);

	//file = new FileDat(filetag, filepath);
	//mainwin->diagbox->Write(text.Format("Entry name %s path %s\n", filetag, filepath));
	//file = new FileDat(filetag, filepath);
	//mainwin->diagbox->Write(text.Format("FileDat name %s path %s\n", file->name, file->path));

	neurodatatag->Insert(filetag, 0, file);
	neurodatatag->SetValue(filetag);

	//if(mainwin->diagbox) mainwin->diagbox->Write("filetag " + filetag);
	//if(mainwin->diagbox) mainwin->diagbox->Write("\nOn Load\n");
	//LoadData(file);
	LoadDataList(file);

	//mod->filebase->newentry = false;
}

/*
void NeuroBox::PanelData(NeuroDat *data)
{
	if(!data) data = &(*cells)[neuroindex];
	if(data->netflag) snum = "sum";
	else snum = numstring(neuroindex, 0);
	datneuron->SetLabel(snum);

	label->SetLabel(data->name);
	spikes->SetLabel(snum.Format("%d", data->spikecount));
	freq->SetLabel(snum.Format("%.2f", data->freq));
	//mean->SetLabel(snum.Format("%.1f", data->meanisi));
	//sd->SetLabel(snum.Format("%.2f", data->isivar));
	selspikes->SetLabel(snum.Format("%d", currcell->selectdata->intraspikes));
	selfreq->SetLabel(snum.Format("%.2f", currcell->selectdata->freq));
}
*/



/*
void NeuroBox::OnPrev(wxSpinEvent& WXUNUSED(event))
{
	if(!cellcount) return;
	if(neuroindex > 0) neuroindex--;
	else neuroindex = cellcount-1;

	for(i=0; i<selectcount; i++) {
		currcell->selectdata->spikes = selectspikes[i].data();
		currcell->SelectScan(i);  // store current cell's select grid to NeuroDat
	}
	NeuroData();
}


void NeuroBox::OnNext(wxSpinEvent& WXUNUSED(event))
{
	if(!cellcount) return;
	if(neuroindex < cellcount-1) neuroindex++;
	else neuroindex = 0;

	for(i=0; i<selectcount; i++) {
		currcell->selectdata->spikes = selectspikes[i].data();
		currcell->SelectScan(i);  // store current cell's select grid to NeuroDat
	}
	NeuroData();
}


void NeuroBox::OnEnter(wxCommandEvent& event)
{
	int id = event.GetId();
	long data;

	// Enter pressed for neuron selection
	if(id == ID_Neuron) {
		datneuron->GetValue().ToLong(&data);
		if(data >= 0 && data < cellcount) {
			neuroindex = data;
			NeuroData();
		}
		return;
	}
	else NeuroData();
}
*/

// LoadDataList - batch loading of spike time data files into the grid

void NeuroBox::LoadDataList(FileDat *file)
{
	int i, row, col;
	int start, filecount;
	wxString filepath;
	wxString datstring, readline, text;
	string line, filename;
	long timeval;
	double dataval;

	if(!file) {
		mainwin->diagbox->Write("Bad file\n");
		return;
	}
	if(file->path == "") {
		mainwin->diagbox->Write("No file path\n");
		return;
	}
	if(file->name == "") {
		mainwin->diagbox->Write("No file name\n");
		return;
	}

	mainwin->diagbox->Write(text.Format("FileDat path %s name %s\n", file->path, file->name));

	row = 0;
	col = 0;
	filecount = 0;
	filename = file->String().ToStdString();

	ifstream infile(filename.c_str());
	if(infile.is_open()) {
		while(getline(infile, line)) {
			wxString readline(line);
			mainwin->diagbox->Write(readline + "\n");
			LoadNeuroData(FileDat(readline, file->path), filecount++);
			//textgrid->ParseLine(row++, col, line);
			//if(i%100000 == 0) mainwin->diagbox->Write(text.Format("Line %d\n", i)); 
		}
		//mainwin->diagbox->Write(line + "\n");
		infile.close();
	}
	else {
		mainwin->diagbox->Write("\nUnable to open file\n"); 
		mainwin->diagbox->Write(text.Format("filename %s\n", filename));
	}
	mainwin->diagbox->Write("\nFile OK\n"); 
}


// LoadNeuroData - load data from spike time data file into the grid

void NeuroBox::LoadNeuroData(FileDat file, int col)
{
	int row;
	int start;
	wxString filepath;
	wxString datstring, readline, text;
	string line, filename;
	long timeval;
	double dataval;
	int linecount = 0;
	int datacount = 0;

	if(file.path == "") {
		mainwin->diagbox->Write("No file path\n");
		return;
	}
	if(file.name == "") {
		mainwin->diagbox->Write("No file name\n");
		return;
	}

	mainwin->diagbox->Write(text.Format("FileDat path %s name %s\n", file.path, file.name));

	row = 0;
	//col = 0;
	filename = file.String().ToStdString();

	ifstream infile(filename.c_str());

	if(!infile.is_open()) {
		filename = filename + ".txt";
		infile.open(filename.c_str());
	}

	/*
	readline = datfile->GetFirstLine();
	datfiletag->SetValue(readline);
	if(readline.GetChar(0) == '\'') {
		readline = readline.AfterFirst('\'');
		datname = readline.BeforeFirst('\'');
	}
	else if(readline.GetChar(0) == '\"') {
		readline = datfile->GetNextLine();
		readline = readline.AfterFirst('\"');
		datname = readline.BeforeFirst('\"');
	}
	datfiletag->SetValue(datname);

	if(readline.ToDouble(&datval)) rawdata[count++] = datval * 1000;        // rawdata in ms
	readline = datfile->GetNextLine();

	while(readline.IsEmpty() || readline.GetChar(0) == '\"' || readline.GetChar(0) == ':') 
		readline = datfile->GetNextLine();

	while(!readline.IsEmpty() && readline.GetChar(0) != '\"' && readline.GetChar(0) != ':') {
		readline.Trim();
		readline.ToDouble(&datval);
		rawdata[count++] = datval * units;
		readline = datfile->GetNextLine();
		if(count == stretchdata) {
			stretchdata = stretchdata + 1000;
			rawdata.setsize(stretchdata);
			loaddata->ReSize(stretchdata);
		}
	}*/

	if(infile.is_open()) {
		while(getline(infile, line)) {
			linecount++;
			//wxString readline(line);
			//mainwin->diagbox->Write(readline + "\n");
			//LoadNeuroData(FileDat(readline, file->path));
			if(line == ":") {
				mainwin->diagbox->Write("Dumping :\n");
				continue;
			}
			if(line.front() == '\'') {
				mainwin->diagbox->Write("Label detect\n");
				textgrid->ParseLabel(row++, col, line);
				continue;
			}
			if(line.front() == '\"') {
				if(linecount == 2) {
					mainwin->diagbox->Write("Label detect\n");
					textgrid->ParseLabel(row++, col, line);
				}
				continue;
			}
			if(line.empty()) {
				if(datacount) break;
				else continue;
			}
			textgrid->ParseLine(row++, col, line);
			datacount++;
			//if(i%100000 == 0) mainwin->diagbox->Write(text.Format("Line %d\n", i)); 
		}
		//mainwin->diagbox->Write(line + "\n");
		infile.close();
	}
	else {
		mainwin->diagbox->Write("\nUnable to open file\n"); 
		mainwin->diagbox->Write(text.Format("filename %s\n", filename));
	}
	mainwin->diagbox->Write(text.Format("\nFile OK, datacount = %d\n", datacount)); 
}


/*
void NeuroBox::SetCell(int cellindex, GraphDat* graph)
{
	neuroindex = cellindex;
	//(*graph).gname.Printf("n%d", cellindex);
	(*graph).gname = (*cells)[cellindex].name;
	NeuroData(false);
}
*/


int NeuroBox::GetCellIndex()
{
	return cellpanel->neuroindex;
}


void NeuroBox::DataSelect(double from, double to)
{
	cellpanel->SetSelectRange(from, to);
}


void NeuroBox::SetGraph(GraphWindow3 *graphwin)
{
	int i, type;
	wxString tag;
	double pval;
	GraphDat *graph;

	//if(newgraphwin) graphwin = newgraphwin;            // default newgraphwin=NULL for updating panel without changing graph window
	graph = graphwin->dispset[0]->plot[0];

	if(graph->gname.Contains("Cell")) {
		auitabpanel->SetSelection(0);
	}
	if(graph->gname.Contains("Model")) {	
		auitabpanel->SetSelection(1);
	}
}





void SpikePanel::SetCount(int count)
{
	neurocount = count;
	neuropop.numneurons = count;
}



/*NeuroBox::NeuroBox(Model *mod, const wxString& title, const wxPoint& pos, const wxSize& size)
: ParamBox(mod, title, pos, size, "cellbox")
{
int datwidth;

diagbox = mod->diagbox;
cellcount = 0;
paramindex = 0;


notebook = new wxNotebook(panel, -1, wxPoint(-1,-1), wxSize(-1, 400), wxNB_TOP);


ToolPanel *analysispanel = new ToolPanel(mainwin, notebook);
analysispanel->SetFont(boxfont);
wxBoxSizer *analysisbox = new wxBoxSizer(wxVERTICAL);
analysispanel->SetSizer(analysisbox);
activepanel = analysispanel;
paramset.panel = analysispanel;


notebook->AddPage(analysispanel, "Analysis");

//chromesizer->AddStretchSpacer();
//chromesizer->Add(chromestatbox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 0);
//chromesizer->AddStretchSpacer();


wxBoxSizer *histparambox = new wxBoxSizer(wxVERTICAL);
paramset.AddNum("normscale", "Norm Scale", 10000, 0, 70, 50);
paramset.AddNum("histrange", "Hist Range", 1000, 0, 70, 50);
//paramset.AddNum("binsize", "Bin Size", 5, 0, 70, 50);
PanelParamLayout(histparambox);


datwidth = 50;
spikes = NumPanel(datwidth, wxALIGN_RIGHT);
mean = NumPanel(datwidth, wxALIGN_RIGHT);
freq = NumPanel(datwidth, wxALIGN_RIGHT);
sd = NumPanel(datwidth, wxALIGN_RIGHT);

wxGridSizer *datagrid = new wxGridSizer(2, 5, 5);
datagrid->Add(new wxStaticText(activepanel, -1, "Spikes"), 0, wxALIGN_CENTRE);
datagrid->Add(spikes);
datagrid->Add(new wxStaticText(activepanel, -1, "Freq"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
datagrid->Add(freq);
datagrid->Add(new wxStaticText(activepanel, -1, "Mean"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
datagrid->Add(mean);
datagrid->Add(new wxStaticText(activepanel, -1, "Std Dev"), 0, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
datagrid->Add(sd);

datneuron = new wxTextCtrl(activepanel, ID_Neuron, "---", wxDefaultPosition, wxSize(50, -1), wxALIGN_LEFT|wxBORDER_SUNKEN|wxST_NO_AUTORESIZE|wxTE_PROCESS_ENTER);
datspin = new wxSpinButton(activepanel, wxID_ANY, wxDefaultPosition, wxSize(40, 17), wxSP_HORIZONTAL|wxSP_ARROW_KEYS);
wxBoxSizer *datbox = new wxBoxSizer(wxHORIZONTAL);
datbox->Add(datspin, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
datbox->AddSpacer(5);

wxBoxSizer *neurobox = new wxBoxSizer(wxHORIZONTAL);
neurobox->Add(new wxStaticText(activepanel, wxID_ANY, "Neuron"), 1, wxALIGN_CENTRE|wxST_NO_AUTORESIZE);
neurobox->Add(datneuron, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

wxStaticBoxSizer *databox = new wxStaticBoxSizer(wxVERTICAL, activepanel, "");
databox->AddSpacer(2);
databox->Add(neurobox, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL| wxALL, 5);
databox->AddSpacer(5);
databox->Add(datbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL| wxALL, 0);
databox->AddSpacer(5);
databox->Add(datagrid, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxALL, 5);

wxBoxSizer *colbox = new wxBoxSizer(wxHORIZONTAL); 
//colbox->AddStretchSpacer();
colbox->Add(databox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
colbox->AddSpacer(10);
colbox->Add(histparambox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
//colbox->AddStretchSpacer();



analysisbox->AddStretchSpacer();
analysisbox->Add(colbox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
analysisbox->AddStretchSpacer();

analysispanel->Layout();

mainbox->Add(notebook, 1, wxEXPAND);

//panel->Layout();


Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(NeuroBox::OnEnter));
Connect(wxEVT_SCROLL_LINEUP, wxSpinEventHandler(NeuroBox::OnNext));
Connect(wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler(NeuroBox::OnPrev));
}*/

#pragma once


#include "hypocontrols.h"
#include "hypotools.h"
#include "hypodat.h"
#include "hypomod.h"
#include "hypopanels.h"
#include "hyponeuro.h"



class SpikePanel: public ToolPanel
{
public:
	NeuroBox *neurobox;
	SpikeDat *currneuron;
	std::vector<NeuroDat>*neurons;

	NeuroPop neuropop;

	int i;
	wxString text;
	bool diagnostic = true;

	int neuroindex;
	int neurocount;
	int currselect;
	int selectcount;
	int cellmode;

	int selectmode[2];
	std::vector<int>selectspikes[2];

	wxStaticBoxSizer *selectcon[2];
	wxToggleButton *addbutton[2];
	wxToggleButton *subbutton[2];

	wxTextCtrl *datneuron;
	wxSpinButton *datspin;
	ParamCon *fromcon; 
	ParamCon *tocon;

	wxStaticText *label;
	wxStaticText *spikes;
	wxStaticText *freq;
	wxStaticText *selectspikecount;
	wxStaticText *selectfreq;
	wxCheckBox *filtercheck;

	// Multi Panel
	wxStaticText *multicount;
	wxStaticText *multifreq;

	wxString ratetag;
	GraphWindow3 *graphwin;  // used to link the spike rate graph panel

	SpikePanel(NeuroBox *);

	// Data processing and display
	void PanelData(NeuroDat *data = NULL);
	void NeuroData(bool dispupdate = true);
	void AnalyseSelection();
	void MultiCellAnalysis();

	// Button and panel commands
	void OnNext(wxSpinEvent& event);
	void OnPrev(wxSpinEvent& event);
	void OnEnter(wxCommandEvent& event);
	void OnAdd(wxCommandEvent& event);
	void OnSub(wxCommandEvent& event);
	void OnClear(wxCommandEvent& event);
	void OnInvert(wxCommandEvent& event);
	void OnClick(wxPoint);
	void OnToggle(wxCommandEvent& event);

	// External commands
	void SetSelectRange(double, double);
	void SetData(SpikeDat *, std::vector<NeuroDat>*);
	void SetCount(int);

	// Selection
	void SelectAdd();
	void SelectSub();
	void SelectUpdate();
	void AddSubToggle(int sel, int type);
	void SelectStore();
	void SelectLoad();
	NeuroDat *GetCell(wxString name);  
	void NextCell();
    void PrevCell();
};


class NeuroBox: public ParamBox
{
public:
	Mod *mod;
	DiagBox *diagbox;

	//int neuroindex;
	//int cellcount;

	//SpikeDat *currcell;
	//std::vector<NeuroDat>*cells;
	//SpikeDat *currmodcell;
	//std::vector<NeuroDat>*modcells;

	GridBox *gridbox;
	BurstBox *burstbox;

	//wxTextCtrl *datneuron;
	//wxSpinButton *datspin;
	TagBox *neurodatatag;
	TagBox *selectstoretag;
	TextGrid *textgrid;

	SpikePanel *cellpanel, *modpanel;

	//wxStaticBoxSizer *selectbox1, *selectbox2;  //, *filterbox;
	//wxStaticBoxSizer *selectbox3, *selectbox4;
	//wxToggleButton *addbutton[5];
	//wxToggleButton *subbutton[5];
	//int selectmode[5];
	//int selectcount;
	//int currselect;
	//int currselectmod;
	
	//std::vector<int>selectspikes[4];
	bool spikeselectLink;

	wxCheckBox *filtercheck;


	NeuroBox(Mod *mod, const wxString& title, const wxPoint& pos, const wxSize& size);
	//~NeuroBox();
	//void NeuroData(bool dispupdate = true);
	//void NeuroAnalysis();
	//void PanelData(NeuroDat *data = NULL);
	//void OnNext(wxSpinEvent& event);
	//void OnPrev(wxSpinEvent& event);
	//void OnEnter(wxCommandEvent& event);
	void OnLoadData(wxCommandEvent& event);
	void OnBrowse(wxCommandEvent& event);
	void LoadDataList(FileDat *);
	void LoadNeuroData(FileDat file, int col);

	void OnGridFilter(wxCommandEvent& event);
	void OnSelectStore(wxCommandEvent& event);
	void OnSelectLoad(wxCommandEvent& event);
	void OnBoxCheck(wxCommandEvent& event);


	// Functions ported from PlotModel
	void DataSelect(double, double);
	//void SetCell(int, GraphDat*);
	int GetCellIndex();
	void Analysis();

	void AddModSpikePanel(SpikeDat *, std::vector<NeuroDat>*);
	void SetGraph(GraphWindow3 *newgraphwin=NULL);
};





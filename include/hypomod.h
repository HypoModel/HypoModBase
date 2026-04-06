
#ifndef HYPOMOD_H
#define HYPOMOD_H


#include "wx/wx.h"
#include "hypodat.h"
#include "hypotools.h"


class HypoMain;
class EvoFitBox;
class EvoChrome;
class ParamBox;
class ModThread;


wxDECLARE_EVENT(EVT_MODTHREAD_COMPLETED, wxThreadEvent);
wxDECLARE_EVENT(EVT_DIAG_WRITE, wxThreadEvent);



class Mod : public wxEvtHandler
{
public:
	short modtype;
	short ostype;
	short gsmode;
	short gcount;
	bool graphload;
	short gsync;
	int storesize;
	double xmin;
	int xscaletoggle;
	bool oldhist;
	int basicmode;
	bool evoflag;
	int burstmode;
	int projmode;
    
    // Threads
    wxMutex *runmute;
    bool runflag;

	// Prefs
	int numdraw;

	// Utils
	wxString text;
    
	wxString modname;
	wxString modtag;
	wxString initparams;
	wxString initgraph;
	wxString initdata;
	wxString path;

	ParamBox *modbox;
	DiagBox *diagbox;
	HypoMain *mainwin;
	GraphBase *graphbase;
	ModThread *modthread;
	ParamStore *modeflags;
	ParamStore *toolflags;
	ParamStore prefstore;
	ToolSet modtools;
	wxString gcodes[10], gtags[10];
	int gmax;
	ScaleBox *scalebox;
	GraphWindow3 **graphwin;

	DatStore *recdata;
	GridBox *gridbox;
	PlotBox *plotbox;

	NeuroBox *neurobox;
	SoundBox *soundbox;
	EvoFitBox *fitbox;
	EvoChrome *fitchrome;

	// Data Storage
	SpikeDat *expdata;
	datdouble testhist[5], testhistx[5];
	datdouble datahist[10], datahistx[10];
	datdouble gridplot[20], gridplotx[20], gridploterr[20];


	Mod(int, wxString, HypoMain *);
	virtual ~Mod();

	long ReadNextData(wxString *);
	wxString GetPath();
    void DiagWrite(wxString);
	virtual void RunModel();
	virtual void Output() {};
	virtual void GSwitch(GraphDisp *gpos, ParamStore *gflags, int command=0);
	virtual void ScaleSwitch(double xscale = 0) {};
	virtual int ModeSum(ParamStore *gflags);
#ifdef HYPOSOUND
	virtual int SoundLink(SoundBox *);
	virtual void SoundOn() {}; 
#endif
	virtual void ModStore();
	virtual void ModLoad();
	virtual void DataSelect(wxString, double, double) {};
	virtual void GHistStore();
	virtual void GHistLoad(wxComboBox *);
	virtual void ModClose() {}
	virtual void EvoRun() {}
	virtual void ScaleConsoleAbove(ScaleBox *scalebox, int condex) {};
	virtual void ScaleConsoleBelow(ScaleBox *scalebox, int condex) {};
	virtual void SetCell(int cellindex, GraphDat *) {};
	virtual int GetCellIndex();
	virtual void ParamScan() {}
	virtual void SpikeDataSwitch(SpikeDat *);
	virtual void BurstUpdate();
	virtual void GridColumn(int col);
	virtual void GridRow(int row) {}
    virtual void DataCopy(wxString oldpath, wxString newpath);
    virtual void OnModThreadCompletion(wxThreadEvent&);
	virtual void DataOutput() {}
	virtual void GridOutput() {}

    void OnDiagWrite(wxThreadEvent&);
	GraphWindow3 *GetGraphWin(wxString settag);
};



class ModThread : public wxThread
{
public:
    ParamBox *modbox = nullptr;
    HypoMain *mainwin = nullptr;
    wxString snum;
    bool diag = false;

    ModThread(ParamBox *box, HypoMain *main, wxThreadKind kind = wxTHREAD_DETACHED)
        : wxThread(kind), modbox(box), mainwin(main) {}

    virtual ~ModThread() = default;
};



class TextGridMod : public TextGrid
{
public:
    Mod *mod;

    TextGridMod(Mod *mod, wxWindow *parent, wxSize size);
};


#endif

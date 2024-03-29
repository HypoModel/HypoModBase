

#ifndef HYPOGRAPH_H
#define HYPOGRAPH_H


#include "wx/wx.h"
#include "wx/tglbtn.h"
#include "wx/overlay.h"
//#include "hypobase.h"
#include "hypodat.h"
#include "hypomods.h"
#include "wx/clrpicker.h"



// Graph switch command codes
enum {
	NONE = 0,
	XSYNCH = 1          // synch new plot's x-axis with previous plot
};



class GraphPanel: public wxScrolledWindow             // For testing, not in Use
{
public:
	bool usebuffer;
	
	GraphPanel(wxFrame *parent);

	void OnPaint(wxPaintEvent &event);
	void OnEraseBackground(wxEraseEvent& event);
	void PaintBackground(wxDC& dc);
	void Draw(wxDC &dc);
};


class GraphWindow3: public wxPanel
{
public:
	HypoMain *mainwin;
	wxFrame *drawframe;
	//wxBufferedPaintDC dc;
	ScaleBox *scalebox;
	wxScrollBar *scrollbar;
	//GraphDisp *gpos;
	GraphDat *graph;
	GraphBase *graphbase;
	Model *mod;
	int graphindex;
	wxString snum;
	int yplot, xplot;
	BurstDat *burstdata;
	int xbase, ybase;
	int ostype;
	int gsynch;
	int xstretch;
	int xscrollpos;
	wxPoint mousedown;
	//wxOverlay *overlay;
	wxFont textfont, smallfont;
	wxColour *colourpen;
	//wxString colourstring[10];
	wxBufferedPaintDC *dc;
	wxBitmap radio_on, radio_off;

	//wxMenu *menuPlot;

	// Selection Overlay
	bool selectband;
	wxPoint anchorpos, currentpos;
	wxOverlay overlay;

	wxTextCtrl *yf;
	wxTextCtrl *yt;
	wxTextCtrl *xf;
	wxTextCtrl *xt;
	wxBitmapButton *yzoomout;
	wxBitmapButton *yzoomin;
	wxBitmapButton *xzoomout;
	wxBitmapButton *xzoomin;
	int ypos;
	int xpos;
	GraphDisp *dispset[5];
	int numdisps, maxdisps;
	int currentgraph;
	int spikedisp;

	void AddGraph(GraphDisp *);
	void SetGraph(int, GraphDisp *);
	void FrontGraph(GraphDisp *);

	GraphWindow3(HypoMain *main, wxFrame *parent, Model *, wxPoint pos, wxSize size, GraphDisp *gdisp, int index);
	~GraphWindow3();

	void OnPaint(wxPaintEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	void PaintBackground(wxDC& dc);
	void OnScroll(wxScrollEvent& event);
	void UpdateScroll(int pos=-1);
	void ReSize(int, int);
	void OnPaintGC(wxPaintEvent& event);
	void OnPaintGCdemo(wxPaintEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnLeftUp(wxMouseEvent& event);
	void OnLeftDown(wxMouseEvent& event);
	void OnRightClickOld(wxMouseEvent& event);
	void OnRightClick(wxMouseEvent& event);
	void OnGraphSelectSet(wxCommandEvent& event);
	void OnGraphSelectPlot(wxCommandEvent& event);
	void OnGraphRemove(wxCommandEvent& event);
	//void OnGraphPrint(wxCommandEvent& event);
	void OnGraphEPS(wxCommandEvent& event);
	void OnMultiEPS(wxCommandEvent& event);
	void OnMultiCell(wxCommandEvent& event);
	void OnScale(wxCommandEvent& event);
	void OnUnZoom(wxCommandEvent& event);
	void OnDrawTest(wxCommandEvent& event);
	void OnGridOutput(wxCommandEvent& event);

	void DrawLine(wxDC& dc, wxGraphicsContext *gc, int xfrom, int yfrom, int xto, int yto);
	void PrintEPS(double xbase = -1, double ybase = -1, TextFile *ofp = NULL);
	void MultiEPS();
	void MultiCell();
	void XYSynch();
	wxRealPoint GraphPos(wxPoint);
	void Highlight(double xpos, double width=0);              // new 28/5/20
	void HighlightTime(double start, double stop);
	void SetScroll(int xpos);
	void ScrollX(int scrollpos);
};


class GraphBox : public wxDialog
{
public:
	GraphWindow3 *graphwin;
	ToolPanel *panel;
	DiagBox *diagbox;

	int ostype;
	int buttonheight;
	int column;
	wxFont boxfont, confont;
	bool autosynch;

	wxBoxSizer *mainbox;
	wxBoxSizer *parambox;
	wxBoxSizer *vbox[5];
	ParamSet paramset;
	GraphDat *graph;
	BoxOut *boxout;
	wxStaticText *status;
	wxColourPickerCtrl *strokepicker, *fillpicker;
	wxChoice *typechoice, *fontchoice;
	TypeSet typeset, *fontset;
	wxRadioButton *xtickrad[3], *ytickrad[3];
	wxRadioButton *xlabrad[5], *ylabrad[5];
	wxRadioButton *xscalerad[2], *yscalerad[2];
	wxRadioButton *xaxisrad[2], *yaxisrad[2];
	wxCheckBox *clipcheck, *scattercheck, *linecheck;
	wxCheckBox *fillcheck, *fillstrokecheck;

	wxRadioButton *symbolrad[4];

	ParamNum *numdrawcon;
	ParamNum *viewheightcon;
	ParamNum *ylabelcon;
	ParamNum *datsamplecon;
	ParamText *datapathcon;
	ParamText *outpathcon;
	ParamText *parampathcon;
	ParamText *modpathcon;
	GraphBox(GraphWindow3 *, const wxString&);

	void OnOK(wxCommandEvent& event);
	void OnPrint(wxCommandEvent& event);
	void OnRadio(wxCommandEvent& event);
	void OnSynch(wxCommandEvent& event);
	void OnChoice(wxCommandEvent& event);
	void OnCheck(wxCommandEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnSpin(wxSpinEvent& event);
	wxBoxSizer *ParamLayout(int columns=1);
	void OnClose(wxCloseEvent& event);
	void SetGraph(GraphWindow3 *newgraphwin=NULL);
	void SetControls();
	void SetParams(GraphDat *setgraph=NULL);
	void SetParamsCopy(GraphDat *setgraph);
	void SetParamsCopyAxes(GraphDat *setgraph);
	void SynchLayers();
	void SynchPlotSize();
};


class ScaleBox: public ToolPanel
{
public:
	ToolPanel *panel;
	wxColour backgroundcolour;
	MainFrame *mainwin;
	//wxFrame *drawframe;
	int buttonheight;
	int numgraphs, startgraph, modesum;
	int maxgraphs;
	int hazmode1, hazmode2, binrestog1, binrestog2;
	int overtog, overtog2;
	int timeres;
	int rateres;
	int vmhflag, nettog;
	int vasoflag, cortflag;
	int vasonetflag, vasosecflag;
	int burstmode, filtermode;
	int expdatflag;
	int inputflag;
	int dispmod;
	int profmode, proftype;
	int secmode, sectype;
	int expmode;
	int ostype;
	int gsynch;
	int dendmode;
	int ratedata;
	int internflag;
	int normtog;
	int boxtype;
	int synchcon;
	//int overlayindex[10][2];

	int xmin, xmax, ymin, ymax;
	int overpan1, overpan2, overpan3, overpan4;
	OverSet overset;
	int zbwidth, zbheight;  // zoom button sizes
	wxBitmap rightarrow, leftarrow;
	wxBitmap downarrow, uparrow;

	wxString snum, redtag, text;
	wxFont boxfont, confont;
	GraphDisp *gpos;
	//GraphDat *greg;
	std::vector <GraphDat> greg;
	GraphBase *gbase;
	GraphDat *graph;
	GraphWindow3 **graphwin;
	Model *gmod;
	Model *mod;

	ParamStore *gflags;
	RefStore *gflagrefs;
	int *gflagmax;

	//wxComboBox *gstag;
	TagBox *gstag;
	wxToggleButton *syncbutton;
	ToolButton *databutton;
	ToolButton *burstbutton;
	ToolButton *selectbutton;
	//ToolButton *spikesbutton;
	//ToolButton *ratebutton;

	wxCheckBox *gsync[10];
	wxBoxSizer *consolebox[10];
	wxBoxSizer *vconbox;
	//wxBoxSizer *psetbox;

	//ScaleBox(HypoMain *main, wxFrame *draw, const wxSize& size, int numgraphs, GraphDisp *gpos, Model *model, GraphWindow3 **graphwin, int startgraph=0, int boxtype=0);
	ScaleBox(HypoMain *main, const wxSize& size, int numgraphs, GraphDisp *gpos, Model *model, GraphWindow3 **graphwin, int boxtype=0);
	~ScaleBox();

	//wxTextCtrl *AddScaleParam(wxString name, double initval, wxBoxSizer *sizer);
	TextBox *AddScaleParam(wxString name, double initval, wxBoxSizer *sizer, int);
	ToolButton *ScaleButton(int id, wxString label, int width, wxBoxSizer *box, int point = 10);
	ToolButton *GraphButton(wxString tag, int initval, int id, wxString label, int width, wxBoxSizer *box, int type = 1, int point = 10);
	wxToggleButton *ToggleButton(int id, wxString label, int width, wxBoxSizer *box, int point = 10);
	void OnOK(wxCommandEvent& event);
	void OnSync(wxCommandEvent& event);
	void OnHistHaz1(wxCommandEvent& event);
	void OnHistHaz2(wxCommandEvent& event);
	void OnBinRes1(wxCommandEvent& event);
	void OnBinRes2(wxCommandEvent& event);
	void OnNetMode(wxCommandEvent& event);
	//void OnNorm(wxCommandEvent& event);
	//void OnAllBurst(wxCommandEvent& event);
	void OnProfMode(wxCommandEvent& event);
	void OnExpMode(wxCommandEvent& event);
	void OnSecMode(wxCommandEvent& event);
	void OnDendMode(wxCommandEvent& event);
	void OnOverlay(wxCommandEvent& event);
	void OnPosition(wxCommandEvent& event);
	//void OnSpikes(wxCommandEvent& event);
	void OnData(wxCommandEvent& event);
	void OnIntern(wxCommandEvent& event);
	//void OnRateRes(wxCommandEvent& event);
	void OnYZoomIn(wxCommandEvent& event);
	void OnYZoomOut(wxCommandEvent& event);
	void OnXZoomIn(wxCommandEvent& event);
	void OnXZoomOut(wxCommandEvent& event);
	void OnGStore(wxCommandEvent& event);
	void OnGLoad(wxCommandEvent& event);
	void OnXScale(wxCommandEvent& event);

	void OnGraphButton(wxCommandEvent& event);   // New general button system
	void GraphCommand(int id);

	//void OnClose(wxCloseEvent& event);
	//void GSwitch(int mod = 0);

	void GLoad(wxString tag = "");
	void GSwitch1();
	void GSwitch2();
	void VMHswitch(int);
	void VasoSwitch(int);
	void CortSwitch(int);
	void VasoSecSwitch(int);
	void VasoNetSwitch(int);
	void NetSwitch(int);
	void BurstDisp(int);
	void PanelUpdate();
	void ScaleUpdate();
	void ScrollUpdate(int index, int pos=-1);
	void SynchScale();
	void XSynch(int pos=-1);
	void XSlide(int graph, int xpos);
	void OnPaint(wxPaintEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	void PaintBackground(wxDC& dc);
	void GraphUpdate(int pos=-1);
	void SetMod(Model *gmod);
	//void GraphSwitch(int disp=1);
	void GraphSwitch(int command=0);
	wxBoxSizer *GSBox(wxString, wxPanel *storepanel=NULL);
	void AddButton(int id, wxString label, int width, wxBoxSizer *box, int pad = 1, int height = 0);
	ParamStore *GetFlags();
	void OnConFocus(wxFocusEvent& event);

	int CheckValue();
	void AddGraphConsole(int index, GraphWindow3 *graphwin);
	void RemoveGraphConsole(int index);
	void ConsoleButtons(int panel);  // Add mod type dependent panel buttons, now being replaced by Model class version (boxtype = -1)

};


class DispWin: public wxFrame
{
public:
	HypoMain *mainwin;

	int i;
	int startpos, numdraw;
	wxString snum;
	GraphDisp *gpos;
	ScaleBox *scalebox;
	GraphWindow3 *graphwin[10];
	GraphPanel *graphpanel[10];

	DispWin(HypoMain *main, const wxString& title, const wxPoint& pos, const wxSize& size, int start, int numgraphs);
	void GraphUpdate();
};


#endif

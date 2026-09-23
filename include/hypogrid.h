//
//  hypogrid.h
//  HypoModOSX
//
//  Created by Duncan MacGregor on 23/09/2026.
//

#ifndef hypogrid_h
#define hypogrid_h


#include "wx/wx.h"
#include "hypopanels.h"


class GridBox;


class TextGrid : public wxGrid
{
public:
    wxMenu *rightmenu;
    int ostype;
    DiagBox *diagbox;
    Mod *mod;

    int selectcol, selectrow;
    int numrows, numcols;

    wxGridStringTable *undogrid;
    wxTextCtrl *vdu;
    GridBox *gridbox;
    wxGauge *gauge;
    wxString text;

    TextGrid(wxWindow *parent, wxSize size);
    ~TextGrid();

    void OnRightClick(wxGridEvent& event);
    void OnLeftClick(wxGridEvent& event);
    void OnLabelClick(wxGridEvent& event);
    void OnKey(wxKeyEvent& event);
    void OnTypeKey(wxKeyEvent &event);
    void OnSelectAll(wxCommandEvent& event);
    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void Undo();
    void OnPaste(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnBold(wxCommandEvent& event);
    void CopyUndo();
    void Copy();
    void Paste(int mode=0);  // mode 0 normal, mode 1 transpose
    void Cut();
    void Delete();
    void SetBold();
    void ClearCol(int);
    void ParseLine(int row, int col, wxString line);
    void ParseLabel(int row, int col, wxString line);
    void SetCell(int row, int col, wxString data);
    double ReadDouble(int row, int col);
    bool CheckDouble(int row, int col, double *data);
    wxString GetCell(int row, int col);
    void CopyColumn(int source, int dest);
    void InsertColumn(int currcol);
    void OnInsertColumn(wxCommandEvent& event);
};


class GridNumDat
{
public:
    int row;
    int col;
    double data;
};


class GridTextDat
{
public:
    int row;
    int col;
    wxString data;
};


class DataGrid
{
public:
    int count;
    int grow;      // expansion step for increasing storage
    int rowmax;
    int colmax;

    DataGrid() {
        count = 0;
        rowmax = 0;
        colmax = 0;
    };

    void Clear() {
        count = 0;
        rowmax = 0;
        colmax = 0;
    }
};



class TextGridMod : public TextGrid
{
public:
    Mod *mod;

    TextGridMod(Mod *mod, wxWindow *parent, wxSize size);
};


class GridBox: public ParamBox
{
public:
    Mod *mod;
    wxTextCtrl *textbox;
    TextGrid *textgrid[10];   // grid store array
    TagStore gridindex;
    DiagBox *diagbox;
    wxNotebook *notebook;
    //PlotBox *plotbox;
    bool bookmode, vdumode;
    bool undomode;
    bool startshift;
    int colflag[1000];
    int numgrids;
    

    // NeuroBox links
    NeuroBox *neurobox;
    std::vector<NeuroDat>* celldata;

    // PlotBox link
    PlotBox *plotbox;

    // Grid data vectors for large grids, currently not in use
    std::vector<GridNumDat> numdata;
    DataGrid numdatagrid;
    std::vector<GridTextDat> textdata;
    DataGrid textdatagrid;

    // Standard grid links
    TextGrid *currgrid;   // pointer to selected grid, textgrid[0] by default
    TextGrid *datagrid;
    TextGrid *outputgrid;
    TextGrid *paramgrid;
    TextGrid *layoutgrid;

    GridBox(Mod *mod, const wxString& title, const wxPoint& pos, const wxSize& size, int rows=100, int cols=20, bool bookmode=true, bool vdumode=true);

    virtual void GridDefault();
    virtual void TestGrid();
    void GridStore();
    void GridStoreAll();
    void GridLoad();
    void GridLoadAll();
    void HistLoad();
    void HistStore();

    void OnGridStore(wxCommandEvent& event);
    void OnGridLoad(wxCommandEvent& event);
    void OnRightClick(wxMouseEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnButton(wxCommandEvent& event);

    int ColumnData(int, datdouble *);
    int ColumnDataXY(int xcol, int ycol, datdouble *xdata, datdouble *ydata);
    TextGrid *AddGrid(wxString label, wxSize size);
    void SetCurrentGrid();
    void ParamButton();
    void NeuroButton();
    void PlotButton();

    void OnParamMode(wxCommandEvent& event);
    void OnParamScan(wxCommandEvent& event);
    void OnNeuroScan(wxCommandEvent& event);
    virtual void OnPlot(wxCommandEvent& event);
    void OnGridSelect(wxBookCtrlEvent& event);

    void NeuroGridFilter(int mode=0);
    void NeuroScan();

    virtual void OnCellChange(wxGridEvent& event);
    virtual void ColumnSelect(int);
    virtual void RowSelect(int);

    void SetNumCell(int row, int col, double data);
    void SetTextCell(int row, int col, wxString data);
};





class GridLoadThread : public wxThread
{
public:
    GridBox *gridbox;

    GridLoadThread(GridBox *gridbox);
    virtual void *Entry();
};






#endif /* hypogrid_h */

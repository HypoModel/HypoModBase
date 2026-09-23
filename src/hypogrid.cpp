//
//  hypogrid.cpp
//  HypoModOSX
//
//  Created by Duncan MacGregor on 23/09/2026.
//

#include "hypogrid.h"
#include "hypomain.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <wx/numformatter.h>

using std::string;
using std::ifstream;
using std::ofstream;



GridBox::GridBox(Mod *modarg, const wxString& title, const wxPoint& pos, const wxSize& size, int rows, int cols, bool bmode, bool vmode)
    : ParamBox(modarg, title, pos, size, "gridbox", 0, 1)
{
    int gridrows, gridcols;
    wxBoxSizer *vdubox;
    mod = modarg;
    numgrids = 0;
    
    undomode = true;

    redtag = "";
    gridrows = rows;
    gridcols = cols;
    bookmode = bmode;
    vdumode = vmode;
    delete parambox;

    startshift = false;   // true;
    notebook = NULL;
    vdu = NULL;
    gauge = NULL;

    plotbox = NULL;

    textdata.resize(1000);
    //textdatagrid.max = 1000;
    textdatagrid.grow = 10;

    numdata.resize(10000);
    //numdatagrid.max = 10000;
    numdatagrid.grow = 100;

    datagrid = NULL;
    outputgrid = NULL;
    paramgrid = NULL;
    layoutgrid = NULL;

    //InitMenu();
    //SetModFlag(ID_FileIO, "ioflag", "IO Mode", 0);

    diagbox = mod->diagbox;
    vdubox = new wxBoxSizer(wxVERTICAL);

    if(vdumode) {
        vdu = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(-1, -1), wxBORDER_RAISED|wxTE_MULTILINE);
        vdu->SetFont(confont);
        vdu->SetForegroundColour(wxColour(0,255,0)); // set text color
        vdu->SetBackgroundColour(wxColour(0,0,0)); // set text back color
        gauge = new wxGauge(panel, wxID_ANY, 10);
        vdubox->Add(vdu, 1, wxEXPAND);
    }

    if(bookmode) {
        notebook = new wxNotebook(panel, -1, wxPoint(-1,-1), wxSize(-1, 400), wxNB_TOP);
        datagrid = AddGrid("Data", wxSize(gridrows, gridcols));
        outputgrid = AddGrid("Output", wxSize(gridrows, gridcols));
        paramgrid = AddGrid("Params", wxSize(20, 20));
        layoutgrid = AddGrid("Layout", wxSize(20, 20));
    }
    else AddGrid("", wxSize(gridrows, gridcols));

    currgrid = textgrid[0];
    //textgrid[0]->diagbox = diagbox;

    //for(i=0; i<gridrows; i++) textgrid->SetRowSize(i, 25);
    //for(i=0; i<gridcols; i++) textgrid->SetColSize(i, 60);
    //textgrid[0]->SetDefaultRowSize(20, true);
    //textgrid[0]->SetDefaultColSize(60, true);
    //textgrid->SetRowLabelSize(80);
    //textgrid[0]->SetRowLabelSize(50);
    //textgrid[0]->vdu = NULL;
    //textgrid[0]->gauge = NULL;

    wxBoxSizer *controlbox = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *storebox = StoreBox();

    buttonbox = new wxBoxSizer(wxHORIZONTAL);
    buttonbox->Add(storebox, 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
    AddButton(ID_Undo, "Undo", 40, buttonbox);
    //buttonbox->AddSpacer(2);
    //AddButton(ID_Copy, "Copy", 40, buttonbox);

    wxBoxSizer *leftbox = new wxBoxSizer(wxVERTICAL);
    leftbox->Add(buttonbox, 0);
    if(vdumode) leftbox->Add(gauge, 0, wxEXPAND);

    //controlbox->Add(storebox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
    controlbox->AddSpacer(10);
    //controlbox->Add(buttonbox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
    controlbox->Add(leftbox, 1, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
    controlbox->AddSpacer(10);
    //controlbox->Add(vdu, 100, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
    //controlbox->Add(displaybox, 100, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);
    if(vdumode) controlbox->Add(vdubox, 100, wxEXPAND);
    controlbox->AddSpacer(10);

    if(bookmode) mainbox->Add(notebook, 1, wxEXPAND);
    else mainbox->Add(textgrid[0], 1, wxEXPAND);
    mainbox->Add(controlbox, 0);
    mainbox->AddSpacer(2);

    //GridDefault();

    //histmode = 1;
    //HistLoad();
    //paramstoretag->SetLabel(initparams);

    panel->Layout();

    //textgrid[0]->vdu = vdu;
    //textgrid[0]->gauge = gauge;
    //textgrid[0]->gridbox = this;

    Connect(ID_paramstore, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GridBox::OnGridStore));
    Connect(ID_paramload, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GridBox::OnGridLoad));
    Connect(ID_Undo, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GridBox::OnUndo));
    Connect(ID_Copy, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GridBox::OnCopy));
    Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(GridBox::OnRightClick));
    Connect(wxEVT_GRID_CELL_CHANGED, wxGridEventHandler(GridBox::OnCellChange));
    Connect(wxEVT_NOTEBOOK_PAGE_CHANGED, wxBookCtrlEventHandler(GridBox::OnGridSelect));
};


void GridBox::OnGridSelect(wxBookCtrlEvent& event) {
    wxString text;
    int newpage;

    newpage = event.GetSelection();
    //diagbox->Write(text.Format("OnGridSelect %d\n", newpage));
    currgrid = textgrid[newpage];
}


// AddGrid() in (now default) notebook mode adds a new TextGrid and wxNotebook page
// initialises grid and links to output controls

TextGrid *GridBox::AddGrid(wxString label, wxSize size) {

    // Initialise
    if(notebook) {
        textgrid[numgrids] = new TextGrid(notebook, size);
        notebook->AddPage(textgrid[numgrids], label);
        gridindex.Add(label);
    }
    else textgrid[numgrids] = new TextGrid(panel, size);

    // Set Links
    textgrid[numgrids]->diagbox = diagbox;
    textgrid[numgrids]->vdu = vdu;
    textgrid[numgrids]->gauge = gauge;
    textgrid[numgrids]->gridbox = this;

    // Format
    textgrid[numgrids]->SetDefaultRowSize(20, true);
    textgrid[numgrids]->SetDefaultColSize(60, true);
    textgrid[numgrids]->SetRowLabelSize(50);

    return textgrid[numgrids++];
}


void GridBox::PlotButton()
{
    buttonbox->AddSpacer(2);
    AddButton(ID_Plot, "Plot", 40, buttonbox);
    Connect(ID_Plot, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GridBox::OnPlot));
}


void GridBox::ParamButton()
{
    buttonbox->AddSpacer(2);
    AddButton(ID_ParamScan, "Param", 40, buttonbox);
    Connect(ID_ParamScan, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GridBox::OnParamScan));
}


void GridBox::NeuroButton()
{
    int buttwidth;
    if(ostype == Mac) buttwidth = 45;
    else buttwidth = 40;
    buttonbox->AddSpacer(2);
    AddButton(ID_Neuron, "Neuro", buttwidth, buttonbox);
    Connect(ID_Neuron, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GridBox::OnNeuroScan));
}


void GridBox::OnParamMode(wxCommandEvent& event)
{
    WriteVDU("param mode");
    buttonbox->AddSpacer(2);
    AddButton(ID_ParamScan, "Params", 40, buttonbox);
    Connect(ID_ParamScan, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(GridBox::OnParamScan));
}


void GridBox::OnParamScan(wxCommandEvent& event)
{
    int paramindex;

    paramindex = gridindex.GetIndex("Params");
    if(textgrid[paramindex]->selectrow == 0) {
        textgrid[paramindex]->selectrow = 2;
        textgrid[paramindex]->GoToCell(2, 0);
    }


    // On first press if Params grid is hidden bring to front
    if(bookmode && notebook->GetSelection() != paramindex) {
        notebook->SetSelection(gridindex.GetIndex("Params"));
        textgrid[paramindex]->SelectRow(textgrid[paramindex]->selectrow);
        return;
    }

    if(!textgrid[paramindex]->IsSelection()) textgrid[paramindex]->SelectRow(textgrid[paramindex]->selectrow);

    WriteVDU("Param Scan\n");
    mod->ParamScan();
}


void GridBox::OnPlot(wxCommandEvent& event)
{
    //mod->RangePlot(textgrid);
    WriteVDU("Plot\n");
    //diagbox->Write("param scan\n");

    if(!plotbox) mainwin->PlotModule(mod);
}


void GridBox::OnUndo(wxCommandEvent& event)
{
    currgrid->Undo();
}


void GridBox::OnButton(wxCommandEvent& event)
{
    wxString text;

    mainwin->diagbox->Write("Button: Neuro data scan\n");
}


void GridBox::OnCopy(wxCommandEvent& event)
{
    currgrid->Copy();
}


void GridBox::OnCellChange(wxGridEvent& event)
{
    int col = event.GetCol();

    mainwin->diagbox->Write(text.Format("grid cell change col %d\n", col));
}


void GridBox::OnRightClick(wxMouseEvent& event)
{
    //int id = event.GetId();
    //wxWindow *pos = FindWindowById(id, toolpanel);
    //wxPoint point = this->GetPosition();
    currgrid->SetCellValue(10, 0, "right");

    wxPoint pos = event.GetPosition();
    //wxSize size = this->GetSize();
    //menuPlot->Check(1000 + gpos->GetFront()->gindex, true);
    //PopupMenu(rightmenu, pos.x + 20, pos.y);
}


void GridBox::HistLoad()
{
    wxString filename, readline;
    TextFile infile;

    // Output Grid History
    filename =  "outgridhist.ini";
    if(infile.Open(filename)) {
        readline = infile.ReadLine();
        //tofp.WriteLine(readline);
        while(!readline.IsEmpty()) {
            readline = readline.AfterFirst(' ');
            readline.Trim();
            initparams = readline;
            paramstoretag->Insert(initparams, 0);
            readline = infile.ReadLine();
            //tofp.WriteLine(readline);
        }
        infile.Close();
    }
    paramstoretag->SetValue(initparams);
}


void GridBox::TestGrid()
{
    int i;
    wxString text;

    for(i=0; i<1000; i++) currgrid->SetCellValue(i, 0, text.Format("%.1f", i*0.1 + 10));
}


void GridBox::GridDefault()
{
    currgrid->SetCellValue(0, 0, "date");
    currgrid->SetCellValue(1, 0, "breath rhy");
    currgrid->SetCellValue(2, 0, "odour");
    currgrid->SetCellValue(3, 0, "bedding");
    currgrid->SetCellValue(4, 0, "val");
    currgrid->SetCellValue(5, 0, "hex");
    currgrid->SetCellValue(6, 0, "air");
    currgrid->SetCellValue(7, 0, "other");

    currgrid->SetCellValue(9, 0, "phasic");
    currgrid->SetCellValue(10, 0, "other");
    currgrid->SetCellValue(11, 0, "vas");

    currgrid->SetCellValue(13, 0, "data");
    currgrid->SetCellValue(14, 0, "spikes");
    currgrid->SetCellValue(15, 0, "freq");
    currgrid->SetCellValue(16, 0, "mean isi");
    currgrid->SetCellValue(17, 0, "isi SD");
}


void GridBox::OnGridStore(wxCommandEvent& event)
{
    //GridStore();
    GridStoreAll();
}


void GridBox::OnGridLoad(wxCommandEvent& event)
{
    if(undomode) currgrid->CopyUndo();
    //GridLoad();
    //int ioflag = (*modflags)["ioflag"];

    /*
    GridLoad *gridload = new GridLoad(this);
    gridload->Create();
    gridload->Run();
    gridload->Wait();
    delete gridload;
    */

    //int ioflag = true;
    //if(ioflag) GridLoadFast();
    //else GridLoad();
    GridLoadAll();

    //textgrid->AutoSizeColumns(false);
}


void GridBox::ColumnSelect(int col)
{
    wxString text;

    WriteVDU(text.Format("Column Select %d\n", col));

    mod->GridColumn(col);
    if(plotbox) plotbox->SetColumn(col);
}


void GridBox::RowSelect(int row)
{
    wxString text;

    WriteVDU(text.Format("Row Select %d\n", row));

    mod->GridRow(row);
}


int GridBox::ColumnData(int col, datdouble *data)
{
    int row, count;
    double value, max;
    wxString celltext;

    count = 0;
    max = 0;
    for(row=0; row<currgrid->GetNumberRows(); row++) {
        celltext = currgrid->GetCellValue(row, col);
        if(celltext != "") {
            celltext.ToDouble(&value);
            count++;
        }
        else value = 0;
        (*data)[row] = value;
        if(value > max) max = value;
    }
    data->max = max;
    return count;
}


int GridBox::ColumnDataXY(int xcol, int ycol, datdouble *dataX, datdouble *dataY)
{
    int row, count;
    double valueX, valueY, maxX, maxY;
    wxString celltextX, celltextY;

    count = 0;
    maxX = 0;
    maxY = 0;

    for(row=0; row<currgrid->GetNumberRows(); row++) {
        celltextX = currgrid->GetCellValue(row, xcol);
        celltextY = currgrid->GetCellValue(row, ycol);
        if(celltextX != "" && celltextY != "") {
            celltextX.ToDouble(&valueX);
            celltextY.ToDouble(&valueY);
            (*dataX)[count] = valueX;
            if(valueX > maxX) maxX = valueX;
            (*dataY)[count] = valueY;
            if(valueY > maxY) maxY = valueY;
            diagbox->Write(text.Format("Row %d X %.2f Y %.2f\n", row, valueX, valueY));
            count++;
        }
    }
    dataX->max = maxX;
    dataY->max = maxY;
    return count;
}


void GridBox::GridStoreAll()
{
    TextFile ofp;
    int gridindex, row, col;
    wxString celltext, text, filename, filetag, filepath;
    wxColour redpen("#dd0000"), blackpen("#000000");
    std::string line, sfilename;
    std::vector <Index> columnindex;  // stores list of columns (by index) containing data
    int storeversion = 1;   //  initial multi grid store 14/12/20

    columnindex.resize(numgrids);

    filepath = mod->GetPath() + "/Grids";
    if(!wxDirExists(filepath)) wxMkdir(filepath);

    filetag = paramstoretag->GetValue();
    filename = filepath + "/" + filetag + "-grid.txt";

    short tagpos = paramstoretag->FindString(filetag);
    if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
    paramstoretag->Insert(filetag, 0);

    if(ofp.Exists(filename) && redtag != filetag) {
        paramstoretag->SetForegroundColour(redpen);
        paramstoretag->SetValue("");
        paramstoretag->SetValue(filetag);
        redtag = filetag;
        return;
    }

    redtag = "";
    paramstoretag->SetForegroundColour(blackpen);
    paramstoretag->SetValue("");
    paramstoretag->SetValue(filetag);

    sfilename = filename.ToStdString();
    std::ofstream outfile(sfilename.c_str());

    if(!outfile.is_open()) {
        paramstoretag->SetValue("File error");
        return;
    }

    WriteVDU("Writing file...");

    text.Printf("gsv %d\n", storeversion);
    outfile << text.ToStdString();
    text.Printf("num %d\n", numgrids);
    outfile << text.ToStdString();

    for(gridindex=0; gridindex<numgrids; gridindex++) {
        text.Printf("g %d r %d c %d\n", gridindex, textgrid[gridindex]->GetNumberRows(), textgrid[gridindex]->GetNumberCols());
        outfile << text.ToStdString();
    }

    for(gridindex=0; gridindex<numgrids; gridindex++) {
        for(row=0; row<textgrid[gridindex]->GetNumberRows(); row++) {
            if(gauge) gauge->SetValue(100 * (row + 1) / textgrid[gridindex]->GetNumberRows());
            for(col=0; col<textgrid[gridindex]->GetNumberCols(); col++) {
                celltext = textgrid[gridindex]->GetCellValue(row, col);
                celltext.Trim();
                if(!celltext.IsEmpty()) {
                    text.Printf("%d %d %d %s\n", gridindex, row, col, celltext);
                    columnindex[gridindex].Add(col);
                    outfile << text.ToStdString();
                }
            }
        }
    }

    outfile << "\n";
    outfile.close();
    if(gauge) gauge->SetValue(0);
    WriteVDU("OK\n");

    filename = filepath + "/" + filetag + "-gridsize.txt";
    ofp.New(filename);
    for(gridindex=0; gridindex<numgrids; gridindex++) {
        for(i=0; i<columnindex[gridindex].count; i++) {
            col = columnindex[gridindex].list[i];
            ofp.WriteLine(text.Format("grid %d col %d %d", gridindex, col, textgrid[gridindex]->GetColSize(col)));
        }
    }
    ofp.Close();
}


void GridBox::GridStore()
{
    TextFile ofp;
    int row, col;
    wxString celltext, text, filename, filetag, filepath;
    wxColour redpen("#dd0000"), blackpen("#000000");
    string line, sfilename;
    Index columnindex;

    /*
    filetag = gstag->GetValue();
    filename = "graph-" + filetag + ".dat";

    // Tag history
    short tagpos = gstag->FindString(filetag);
    if(tagpos != wxNOT_FOUND) gstag->Delete(tagpos);
    gstag->Insert(filetag, 0);

    // Check and warn existing file
    wxTextFile checkfile(filepath + "/" + filename);*/

    filepath = mod->GetPath() + "/Grids";
    if(!wxDirExists(filepath)) wxMkdir(filepath);

    filetag = paramstoretag->GetValue();
    filename = filepath + "/" + filetag + "-grid.txt";

    short tagpos = paramstoretag->FindString(filetag);
    if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
    paramstoretag->Insert(filetag, 0);

    if(ofp.Exists(filename) && redtag != filetag) {
        paramstoretag->SetForegroundColour(redpen);
        paramstoretag->SetValue("");
        paramstoretag->SetValue(filetag);
        redtag = filetag;
        return;
    }

    redtag = "";
    paramstoretag->SetForegroundColour(blackpen);
    paramstoretag->SetValue("");
    paramstoretag->SetValue(filetag);

    //ofp.New(filename);
    sfilename = filename.ToStdString();
    ofstream outfile(sfilename.c_str());

    if(!outfile.is_open()) {
        paramstoretag->SetValue("File error");
        return;
    }

    WriteVDU("Writing file...");

    for(row=0; row<currgrid->GetNumberRows(); row++) {
        if(gauge) gauge->SetValue(100 * (row + 1) / currgrid->GetNumberRows());
        for(col=0; col<currgrid->GetNumberCols(); col++) {
            celltext = currgrid->GetCellValue(row, col);
            celltext.Trim();                                                                     // Fixes odd line endings in pasted data  23/4/15
            //if(!celltext.IsEmpty()) ofp.WriteLine(text.Format("%d %d %s", row, col, celltext));
            if(!celltext.IsEmpty()) {
                text.Printf("%d %d %s\n", row, col, celltext);
                columnindex.Add(col);
                outfile << text.ToStdString();
            }
        }
    }

    outfile.close();
    if(gauge) gauge->SetValue(0);
    WriteVDU("OK\n");

    filename = filepath + "/" + filetag + "-gridsize.txt";
    ofp.New(filename);
    for(i=0; i<columnindex.count; i++) {
        col = columnindex.list[i];
        ofp.WriteLine(text.Format("col %d %d", col, currgrid->GetColSize(col)));
    }
    ofp.Close();
}


void GridBox::GridLoadAll()
{
    TextFile ifp;
    int row, col, width;
    long numdat = 0;
    double cellnum;
    wxString text, filetag, filepath, filename;
    wxString vertag, celldata;
    wxString datstring, readline;
    wxColour redpen("#dd0000"), blackpen("#000000");
    std::string line, sfilename;
    int numlines, linecount, cellcount;
    int numrows, numcols;
    int newnumgrids;
    int storeversion = 0;
    int gindex;  // grid index
    bool readdiag = false;
    
    WriteVDU("Grid Load\n");
    wxYieldIfNeeded();

    
    filepath = mod->GetPath() + "/Grids";
    filetag = paramstoretag->GetValue();
    filename = filepath + "/" + filetag + "-grid.txt";

    if(!ifp.Exists(filename)) {
        paramstoretag->SetValue("Not found");
        return;
    }
    
    sfilename = filename.ToStdString();
    
    // New ChatGPT code 23/9/26
    ifstream readfile(sfilename, std::ios::binary);

    if(!readfile) {
        WriteVDU("Unable to open grid file\n");
        return;
    }
    
    // Param file history
    short tagpos = paramstoretag->FindString(filetag);
    if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
    paramstoretag->Insert(filetag, 0);

    redtag = "";
    paramstoretag->SetForegroundColour(blackpen);
    paramstoretag->SetValue("");
    paramstoretag->SetValue(filetag);

    readfile.seekg(0, std::ios::end);
    std::streampos filesize = readfile.tellg();

    if(filesize <= 0) {
        WriteVDU("File empty\n");
        return;
    }
    
    WriteVDU("Reading file...");
    wxYieldIfNeeded();

    // Read file to stream
    std::string contents;
    contents.resize(static_cast<size_t>(filesize));

    readfile.seekg(0, std::ios::beg);
    readfile.read(contents.data(),
                  static_cast<std::streamsize>(contents.size()));

    if(!readfile) {
        WriteVDU("Error reading grid file\n");
        return;
    }
    readfile.close();

    // Count lines from the in-memory copy,
    // rather than making a separate pass through the file.
    numlines = std::count(contents.begin(), contents.end(), '\n');

    if(!numlines) {
        WriteVDU("File empty\n");
        return;
    }

    std::istringstream infile(contents);
    // end new code
    
    linecount = 0;
    

    // Read and check file version
    getline(infile, line);
    readline = StringConvert(line);
    vertag = readline.BeforeFirst(' ');
    if(vertag != "gsv") {
        diagbox->Write("GridLoadAll file version not found, trying old GridLoad\n");
        GridLoad();
        return;
    }
    storeversion = ParseLong(&readline, 'v');

    // Read and set number of grids
    getline(infile, line);
    readline = StringConvert(line);
    newnumgrids = ParseLong(&readline, 'm');
    if(newnumgrids > numgrids) {
        // code to add new grids
        numgrids = newnumgrids;
    }
    if(readdiag) diagbox->Write(text.Format("GridLoadAll  numgrids %d  newnumgrids %d\n", numgrids, newnumgrids));

    // Read and set grid sizes
    for(i=0; i<newnumgrids; i++) {                 // use newnumgrids to match number of grids to load from file
        getline(infile, line);
        readline = StringConvert(line);
        gindex = ParseLong(&readline, 'g');
        
        if(gindex < 0 || gindex >= numgrids || textgrid[gindex] == nullptr) {
            diagbox->Write(text.Format("GridLoadAll Invalid grid index %d reading grid sizes\n", gindex));
            return;
        }
        
        numrows = ParseLong(&readline, 'r');
        numcols = ParseLong(&readline, 'c');
        if(numrows > textgrid[gindex]->GetNumberRows()) textgrid[gindex]->AppendRows(numrows - textgrid[gindex]->GetNumberRows());
        if(numcols > textgrid[gindex]->GetNumberCols()) textgrid[gindex]->AppendCols(numcols - textgrid[gindex]->GetNumberCols());
        textgrid[gindex]->ClearGrid();
        textgrid[gindex]->numcols = numcols;
        textgrid[gindex]->numrows = numrows;
        
    }

    WriteVDU("OK\n");

    // Read cells
    cellcount = 0;
    int lastprogress = -1;
    int progress;
    WriteVDU("Reading data...");

    while(getline(infile, line)) {
        if(readdiag) diagbox->Write(text.Format(" line length %d first %d\n", (int)line.length(), (char)line[0]));
        //wxString readline(line);
        readline = StringConvert(line);
        if(readdiag) diagbox->Write("readline: " + readline + "\n");
        
        readline.Trim(true);
        readline.Trim(false); // Thorough Mac and Windows blank line clean up
        if(readline.IsEmpty()) break;
        
        long parsed;

        datstring = readline.BeforeFirst(' ');
        if(!datstring.ToLong(&parsed)) {
            diagbox->Write("GridLoadAll Bad grid index: " + readline + "\n");
            break;
        }
        gindex = (int)parsed;
        readline = readline.AfterFirst(' ');

        datstring = readline.BeforeFirst(' ');
        if(!datstring.ToLong(&parsed)) {
            diagbox->Write("GridLoadAll Bad row: " + readline + "\n");
            break;
        }
        row = (int)parsed;
        readline = readline.AfterFirst(' ');

        datstring = readline.BeforeFirst(' ');
        if(!datstring.ToLong(&parsed)) {
            diagbox->Write("GridLoadAll Bad col: " + readline + "\n");
            break;
        }
        col = (int)parsed;
        readline = readline.AfterFirst(' ');
        
        if(gindex < 0 || gindex >= numgrids) {
            diagbox->Write(text.Format(
                "GridLoadAll Invalid grid index %d at cell %d\n",
                gindex, cellcount));
            break;
        }

        readline.Trim();
        celldata = readline;
        if(readdiag) diagbox->Write(text.Format("row %d col %d data: %s end\n", row, col, celldata));
        textgrid[gindex]->SetCell(row, col, celldata);
        cellcount++;
        linecount++;
        
        //if(gauge) gauge->SetValue(100 * linecount / numlines);
        if(cellcount % 2000 == 0) {
            if(gauge) {
                progress = 100 * linecount / numlines;
                if(progress != lastprogress) {
                    gauge->SetValue(progress);
                    gauge->Update();
                    lastprogress = progress;
                }
            }
            wxYieldIfNeeded();
        }
    }

    WriteVDU("OK\n");
    if(gauge) gauge->SetValue(0);

    // Read column sizes file
    filename = filepath + "/" + filetag + "-gridsize.txt";
    if(!ifp.Open(filename)) return;

    readline = ifp.ReadLine();
    while(!readline.IsEmpty()) {
        gindex = ParseLong(&readline, 'd');
        col = ParseLong(&readline, 'l');
        width = ParseLong(&readline);
        //WriteVDU(text.Format("gindex %d col %d %d\n", gindex, col, width));
        textgrid[gindex]->SetColSize(col, width);
        if(ifp.End()) break;
        readline = ifp.ReadLine();
    }

    ifp.Close();
}

/*
void GridBox::GridLoad()            // Replaced by GridLoadFast()
{
    TextFile ifp;
    int i, row, col;
    long numdat;
    double cellnum;
    wxString text, filetag, filename, filepath, celldata;
    wxString datstring;
    wxColour redpen("#dd0000"), blackpen("#000000");
    string line, sfilename;
    int numlines, linecount, cellcount;

    filepath = mod->GetPath() + "/Grids";
    filetag = paramstoretag->GetValue();
    filename = filepath + "/" + filetag + "-grid.txt";

    

    sfilename = filename.ToStdString();
    ifstream infile(sfilename.c_str());
    if(!infile.is_open()) {
        paramstoretag->SetValue("Not found");
        return;
    }

    // Param file history
    short tagpos = paramstoretag->FindString(filetag);
    if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
    paramstoretag->Insert(filetag, 0);

    redtag = "";
    paramstoretag->SetForegroundColour(blackpen);
    paramstoretag->SetValue("");
    paramstoretag->SetValue(filetag);

    currgrid->ClearGrid();

    WriteVDU("Reading file...");


    numlines = count(istreambuf_iterator<char>(infile), istreambuf_iterator<char>(), '\n');
    infile.clear();
    infile.seekg(0, ios::beg);
    linecount = 0;
    cellcount = 0;

    //readline = ifp.ReadLine();
    while(getline(infile, line)) {
        wxString readline(line);
        //diagbox->Write(readline + "\n");
        datstring = readline.BeforeFirst(' ');
        datstring.ToLong(&numdat);
        row = numdat;
        readline = readline.AfterFirst(' ');

        datstring = readline.BeforeFirst(' ');
        datstring.ToLong(&numdat);
        col = numdat;
        readline = readline.AfterFirst(' ');

        readline.Trim();
        celldata = readline;

        currgrid->SetCell(row, col, celldata);
        cellcount++;
        //diagbox->Write(text.Format("Load R %d C %d String %s\n", row, col, cell));
        //readline = ifp.ReadLine();
        //diagbox->Write("Read " + readline + "\n");
        linecount++;
        if(gauge) gauge->SetValue(100 * linecount / numlines);
    }
    infile.close();

    diagbox->Write("OK\n");
    //WriteVDU("OK\n");
    WriteVDU(text.Format("OK, %d cells\n", cellcount));
    if(gauge) gauge->SetValue(0);
}
*/


void GridBox::SetNumCell(int row, int col, double data)
{
    if(numdatagrid.count == numdata.size()) numdata.resize(numdata.size() + numdatagrid.grow);
    if(row > numdatagrid.rowmax) numdatagrid.rowmax = row;
    if(col > numdatagrid.colmax) numdatagrid.colmax = col;

    numdata[numdatagrid.count].row = row;
    numdata[numdatagrid.count].col = col;
    numdata[numdatagrid.count].data = data;

    numdatagrid.count++;
}


void GridBox::SetTextCell(int row, int col, wxString data)
{
    if(textdatagrid.count == textdata.size()) textdata.resize(textdata.size() + textdatagrid.grow);
    if(row > textdatagrid.rowmax) textdatagrid.rowmax = row;
    if(col > textdatagrid.colmax) textdatagrid.colmax = col;

    textdata[textdatagrid.count].row = row;
    textdata[textdatagrid.count].col = col;
    textdata[textdatagrid.count].data = data;

    textdatagrid.count++;
}




// Experimental file access thread for improving GUI performance during load     December 2020 - not currently in use

GridLoadThread::GridLoadThread(GridBox *gbox)
    : wxThread(wxTHREAD_JOINABLE)
{
    gridbox = gbox;
}


void *GridLoadThread::Entry()
{
    wxString text;
    TextFile ifp;
    int i;
    int row, col, width;
    long numdat;
    double cellnum;
    wxString filetag, filepath, filename, celldata;
    wxString datstring, readline;
    wxColour redpen("#dd0000"), blackpen("#000000");
    string line, sfilename;
    int numlines, linecount, cellcount;


    filepath = gridbox->mod->GetPath() + "/Grids";
    filetag = gridbox->paramstoretag->GetValue();
    filename = filepath + "/" + filetag + "-grid.txt";

    if(!ifp.Exists(filename)) {
        filename = filetag + "-grid.txt";            // Backwards compatibility, try old grid file location
        if(!ifp.Exists(filename)) {
            gridbox->paramstoretag->SetValue("Not found");
            return NULL;
        }
    }
    sfilename = filename.ToStdString();
    ifstream readfile(sfilename.c_str());

    // Param file history
    short tagpos = gridbox->paramstoretag->FindString(filetag);
    if(tagpos != wxNOT_FOUND) gridbox->paramstoretag->Delete(tagpos);
    gridbox->paramstoretag->Insert(filetag, 0);

    gridbox->redtag = "";
    gridbox->paramstoretag->SetForegroundColour(blackpen);
    gridbox->paramstoretag->SetValue("");
    gridbox->paramstoretag->SetValue(filetag);

    gridbox->currgrid->ClearGrid();
    gridbox->textdatagrid.Clear();
    gridbox->numdatagrid.Clear();

    gridbox->WriteVDU("Reading file...");

    numlines = count(std::istreambuf_iterator<char>(readfile), std::istreambuf_iterator<char>(), '\n');
    if(!numlines) {
        gridbox->WriteVDU("File empty\n");
        return NULL;
    }
    readfile.clear();
    //readfile.seekg(0, ios::beg);
    linecount = 0;

    //string filetext = (ReadFile(filename.c_str()));
    //istringstream filetext(ReadFile(filename.c_str()));

    //ifstream infile(filename, std::ios::in | std::ios::binary);

    /*
    string contents;
    readfile.seekg(0, ios::end);
    contents.resize(readfile.tellg());
    readfile.seekg(0, ios::beg);
    readfile.read(&contents[0], contents.size());
    readfile.close();
    istringstream infile(contents);
    */

    string contents;
    readfile.seekg(0, readfile.end);
    contents.resize(readfile.tellg());
    readfile.seekg(0, readfile.beg);
    readfile.read(&contents[0], contents.size());
    readfile.close();
    std::istringstream infile(contents);

    /*
    diagbox->Write("Contents codes:\n");
    for(i=0; i<contents.length(); i++) {
    diagbox->Write(text.Format("%d ", contents[i]));
    if(contents[i] == '\n') diagbox->Write("\n");
    }
    diagbox->Write("EOF Contents\n");

    diagbox->Write(text.Format("File length %d size %d numlines %d :\n%s\n", (int)contents.length(), (int)contents.size(), numlines, contents));
    */

    cellcount = 0;

    //readline = ifp.ReadLine();
    while(getline(infile, line)) {
        //diagbox->Write(text.Format(" line length %d first %d\n", (int)line.length(), (char)line[0]));
        wxString readline(line);
        /*
        diagbox->Write("Line codes:");
        for(i=0; i<readline.Len(); i++) diagbox->Write(text.Format("%d ", readline[i]));
        diagbox->Write("\n");
        */
        //readline.Trim(false);
        //readline.Trim();
        //diagbox->Write("readline " + readline);
        if(readline.IsEmpty() || !readline[0]) break;
        //else diagbox->Write(text.Format(" length %d ", (int)readline.Len()));
        //diagbox->Write(readline + "\n");
        datstring = readline.BeforeFirst(' ');
        datstring.ToLong(&numdat);
        row = numdat;
        readline = readline.AfterFirst(' ');

        datstring = readline.BeforeFirst(' ');
        datstring.ToLong(&numdat);
        col = numdat;
        readline = readline.AfterFirst(' ');

        readline.Trim();
        celldata = readline;

        if(celldata.ToDouble(&cellnum)) gridbox->SetNumCell(row, col, cellnum);
        else gridbox->SetTextCell(row, col, celldata);

        //currgrid->SetCell(row, col, celldata);

        //diagbox->Write(text.Format(" setcell %d %d %s\n", row, col, cell));
        cellcount++;
        //diagbox->Write(text.Format("Load R %d C %d String %s\n", row, col, cell));
        //readline = ifp.ReadLine();
        //diagbox->Write("Read " + readline + "\n");
        linecount++;
        //if(gauge && (linecount % (numlines / 10)) == 0) {
        //    //diagbox->Write(text.Format("Gauge %d%%\n", 100 * linecount / numlines));
        //    gauge->SetValue(100 * linecount / numlines);
        //}
        if(gridbox->gauge) gridbox->gauge->SetValue(100 * linecount / numlines);
    }


    // Transfer cell data to display grid


    int numrows, rowmax;
    int numcols, colmax;

    rowmax = gridbox->numdatagrid.rowmax;
    if(gridbox->textdatagrid.rowmax > rowmax) rowmax = gridbox->textdatagrid.rowmax;
    colmax = gridbox->numdatagrid.colmax;
    if(gridbox->textdatagrid.colmax > colmax) colmax = gridbox->textdatagrid.colmax;

    //if(gauge) gauge->SetValue(100);
    //diagbox->Write(text.Format("\nlinecount %d  numlines %d\n", linecount, numlines));

    numrows = gridbox->currgrid->GetNumberRows();
    numcols = gridbox->currgrid->GetNumberCols();
    if(rowmax > numrows) gridbox->currgrid->AppendRows(rowmax - numrows);
    if(colmax > numcols) gridbox->currgrid->AppendCols(colmax - numcols);

    int numcount, textcount;
    numcount = gridbox->numdatagrid.count;
    textcount =    gridbox->textdatagrid.count;

    for(i=0; i<numcount; i++) gridbox->currgrid->SetCell(gridbox->numdata[i].row, gridbox->numdata[i].col, text.Format("%.6f", gridbox->numdata[i].data));
    for(i=0; i<textcount; i++) gridbox->currgrid->SetCell(gridbox->textdata[i].row, gridbox->textdata[i].col, gridbox->textdata[i].data);


    //infile.close();
    //diagbox->Write("OK\n");
    //WriteVDU(text.Format("OK, %d grid cells\n", cellcount));
    gridbox->WriteVDU("OK\n");
    if(gridbox->gauge) gridbox->gauge->SetValue(0);

    if(!ifp.Open(filetag + "-gridsize.txt")) return NULL;

    readline = ifp.ReadLine();
    while(!readline.IsEmpty()) {
        col = ParseLong(&readline, 'l');
        width = ParseLong(&readline, 0);
        //WriteVDU(text.Format("col %d %d\n", col, width));
        gridbox->currgrid->SetColSize(col, width);
        if(ifp.End()) break;
        readline = ifp.ReadLine();
    }

    ifp.Close();

    return NULL;
}


void GridBox::GridLoad()
{
    TextFile ifp;
    int row, col, width;
    long numdat;
    double cellnum;
    wxString text, filetag, filepath, filename, celldata;
    wxString datstring, readline;
    wxColour redpen("#dd0000"), blackpen("#000000");
    string line, sfilename;
    int numlines, linecount, cellcount;
    int numrows, rowmax;
    int numcols, colmax;


    filepath = mod->GetPath() + "/Grids";
    filetag = paramstoretag->GetValue();
    filename = filepath + "/" + filetag + "-grid.txt";

    if(!ifp.Exists(filename)) {
        filename = filetag + "-grid.txt";            // Backwards compatibility, try old grid file location
        if(!ifp.Exists(filename)) {
            paramstoretag->SetValue("Not found");
            return;
        }
    }
    sfilename = filename.ToStdString();
    ifstream readfile(sfilename.c_str());

    // Param file history
    short tagpos = paramstoretag->FindString(filetag);
    if(tagpos != wxNOT_FOUND) paramstoretag->Delete(tagpos);
    paramstoretag->Insert(filetag, 0);

    redtag = "";
    paramstoretag->SetForegroundColour(blackpen);
    paramstoretag->SetValue("");
    paramstoretag->SetValue(filetag);

    currgrid->ClearGrid();
    textdatagrid.Clear();
    numdatagrid.Clear();

    WriteVDU("Reading file...");

    numlines = count(std::istreambuf_iterator<char>(readfile), std::istreambuf_iterator<char>(), '\n');
    if(!numlines) {
        WriteVDU("File empty\n");
        return;
    }
    readfile.clear();
    //readfile.seekg(0, ios::beg);
    linecount = 0;

    //string filetext = (ReadFile(filename.c_str()));
    //istringstream filetext(ReadFile(filename.c_str()));

    //ifstream infile(filename, std::ios::in | std::ios::binary);

    string contents;
    readfile.seekg(0, readfile.end);
    contents.resize(readfile.tellg());
    readfile.seekg(0, readfile.beg);
    readfile.read(&contents[0], contents.size());
    readfile.close();
    std::istringstream infile(contents);

    /*
    diagbox->Write("Contents codes:\n");
    for(i=0; i<contents.length(); i++) {
    diagbox->Write(text.Format("%d ", contents[i]));
    if(contents[i] == '\n') diagbox->Write("\n");
    }
    diagbox->Write("EOF Contents\n");

    diagbox->Write(text.Format("File length %d size %d numlines %d :\n%s\n", (int)contents.length(), (int)contents.size(), numlines, contents));
    */

    cellcount = 0;

    //readline = ifp.ReadLine();
    while(getline(infile, line)) {
        //diagbox->Write(text.Format(" line length %d first %d\n", (int)line.length(), (char)line[0]));
        wxString readline(line);
        
        if(readline.IsEmpty() || !readline[0]) break;

        datstring = readline.BeforeFirst(' ');
        datstring.ToLong(&numdat);
        row = numdat;
        readline = readline.AfterFirst(' ');

        datstring = readline.BeforeFirst(' ');
        datstring.ToLong(&numdat);
        col = numdat;
        readline = readline.AfterFirst(' ');

        readline.Trim();
        celldata = readline;

        if(ostype != Mac) {
            if(celldata.ToDouble(&cellnum)) SetNumCell(row, col, cellnum);
            else SetTextCell(row, col, celldata);
        }
        else currgrid->SetCell(row, col, celldata);

        //diagbox->Write(text.Format(" setcell %d %d %s\n", row, col, cell));
        cellcount++;
        
        linecount++;
        //if(gauge && (linecount % (numlines / 10)) == 0) {
        //    //diagbox->Write(text.Format("Gauge %d%%\n", 100 * linecount / numlines));
        //    gauge->SetValue(100 * linecount / numlines);
        //}
        if(gauge) gauge->SetValue(100 * linecount / numlines);
    }


    // Transfer cell data to display grid

    
    if(ostype != Mac) {
        rowmax = numdatagrid.rowmax;
        if(textdatagrid.rowmax > rowmax) rowmax = textdatagrid.rowmax;
        colmax = numdatagrid.colmax;
        if(textdatagrid.colmax > colmax) colmax = textdatagrid.colmax;

        numrows = currgrid->GetNumberRows();
        numcols = currgrid->GetNumberCols();
        if(rowmax > numrows) currgrid->AppendRows(rowmax - numrows);
        if(colmax > numcols) currgrid->AppendCols(colmax - numcols);

        for(i=0; i<numdatagrid.count; i++) currgrid->SetCell(numdata[i].row, numdata[i].col, text.Format("%.6f", numdata[i].data));
        for(i=0; i<textdatagrid.count; i++) currgrid->SetCell(textdata[i].row, textdata[i].col, textdata[i].data);
    }
    

    //infile.close();
    //diagbox->Write("OK\n");
    //WriteVDU(text.Format("OK, %d grid cells\n", cellcount));
    WriteVDU("OK\n");
    if(gauge) gauge->SetValue(0);

    if(!ifp.Open(filetag + "-gridsize.txt")) return;

    readline = ifp.ReadLine();
    while(!readline.IsEmpty()) {
        col = ParseLong(&readline, 'l');
        width = ParseLong(&readline, 0);
        //WriteVDU(text.Format("col %d %d\n", col, width));
        currgrid->SetColSize(col, width);
        if(ifp.End()) break;
        readline = ifp.ReadLine();
    }

    ifp.Close();
}

/*
void GridBox::OnCellChange(wxGridEvent& event)
{
    int col = event.GetCol();

    diagbox->Write(text.Format("plot grid cell change col %d\n", col));

    plotbox->DataEdit(col);
}


void GridBox::ColumnSelect(int col)
{
    GridBox::ColumnSelect(col);

    plotbox->SetColumn(col);
}*/



void GridBox::NeuroGridFilter(int mode)
{
    int i;
    int col;
    int newcol[1000];
    int numcols, newnumcols;

    // mode = 0   remove selected cells
    // mode = 1   keep selected cells
    
    numcols = currgrid->GetNumberCols();

    for(i=0; i<numcols; i++) colflag[i] = 0;

    for(i=0; i<neurobox->cellpanel->neurocount; i++) {
        col = (*celldata)[i].gridcol;
        if(mode == 1) colflag[col] = 1 - (*celldata)[i].filter;
        else colflag[col] = (*celldata)[i].filter;
    }

    // Mark columns with their replacement
    col = 0;
    for(i=0; i<currgrid->GetNumberCols(); i++) {
        if(!colflag[i]) {
            newcol[col] = i;
            col++;
        }
    }
    newnumcols = col - 1;

    // Copy over replacement columns
    for(i=0; i<newnumcols; i++) currgrid->CopyColumn(newcol[i], i);

    // Delete remaining copied columns
    currgrid->DeleteCols(newnumcols, numcols - newnumcols);
}


void GridBox::OnNeuroScan(wxCommandEvent& event)
{
    NeuroScan();
}


void GridBox::NeuroScan()
{
    int col, row;
    int spikecount, cellcount;
    wxString text, celltext;
    double cellval;
    int view = 0;
    int filterthresh;
    int numcols;
    double spikeint, spiketime;
    double spikestart, startthresh;
    wxNumberFormatter numform;
    wxString typetext;

    diagbox->Write("Neuro data scan\n");
    //mod->cellbox->datneuron->SetLabel("OK");
    WriteVDU("Neural data scan...");

    ParamStore *params = neurobox->GetParams();
    filterthresh = (*params)["filterthresh"];
    startthresh = 10;

    cellcount = 0;
    col = 0;
    celltext = currgrid->GetCell(0, 0);
    celltext.Trim();
    
    numcols = currgrid->numcols;

    while(!celltext.IsEmpty()) {
        celltext = currgrid->GetCell(0, col);
        celltext.Trim();
        if(celldata->size() <= cellcount) celldata->resize(cellcount + 10);
        //diagbox->Write(text.Format("cellcount %d  cell data size %d\n", cellcount, (int)mod->celldata.size()));
        (*celldata)[cellcount].name = celltext;
        celltext = currgrid->GetCell(1, col);
        celltext.Trim();
        spikecount = 0;
        spikestart = 0;
        row = 1;

        // Specific to data with type label, reject non-vasopressin types
        typetext = currgrid->GetCell(3, col);
        /*if(typetext.Contains("OT") || typetext.Contains("NR")) {                           // replace with parameterised string filtering  13/2/20
            //diagbox->Write(text.Format("col %d typetext %s rejected\n", col, typetext));
            (*celldata)[cellcount].filter = 1;
        }
        else {
            //diagbox->Write(text.Format("col %d typetext %s accepted\n", col, typetext));
            (*celldata)[cellcount].filter = 0;
        }*/

        (*celldata)[cellcount].filter = 0;


        // Skip non-spike time rows
        while (!numform.FromString(celltext, &cellval)) {
            //diagbox->Write(text.Format("col %d row %d %s\n", col, row, celltext));
            row++;
            celltext = currgrid->GetCell(row, col);
            celltext.Trim();
        }

        // Read and filter spike time data
        while(!celltext.IsEmpty()) {
            celltext.ToDouble(&cellval);
            if(startshift && spikecount == 0 && cellval > startthresh) spikestart = floor(cellval);     // shift spike times when there's a long initial silent period
            spiketime = (cellval - spikestart) * 1000;
            if(spikecount > 0) {
                spikeint = spiketime - (*celldata)[cellcount].times[spikecount-1];
                //if(spikecount < 10) diagbox->Write(text.Format("col %d spikeint %.2f filter %d\n", col, spikeint, filterthresh));
            }
            if(spikecount >= (*celldata)[cellcount].maxspikes) (*celldata)[cellcount].ReSize();
            if(spikecount == 0 || spikeint > filterthresh) {
                (*celldata)[cellcount].times[spikecount] = spiketime;
                spikecount++;
            }
            row++;
            celltext = currgrid->GetCell(row, col);
            celltext.Trim();
        }

        // Record spike count and initialise next column
        if(gauge) gauge->SetValue(100 * col / numcols);
        (*celldata)[cellcount].spikecount = spikecount;
        (*celldata)[cellcount].gridcol = col;
        cellcount++;
        col++;
        celltext = currgrid->GetCell(0, col);
        celltext.Trim();
        wxYieldIfNeeded();
    }

    if (!cellcount) {
        diagbox->Write("Neuro scan: NO DATA\n");
        //mod->cellbox->datneuron->SetLabel("NO DATA");
    }
    else {
        diagbox->Write(text.Format("Neuro scan: %d cells read OK\n", cellcount));
        //mod->cellcount = cellcount;
        if(neurobox->cellpanel->neuroindex > cellcount) neurobox->cellpanel->neuroindex = 0;
        //neurobox->cellpanel->neurocount = cellcount;
        neurobox->cellpanel->SetCount(cellcount);

        // multi cell analysis
        neurobox->cellpanel->MultiCellAnalysis();
        
        diagbox->Write(text.Format("Neuro data...."));
        neurobox->cellpanel->NeuroData();
        diagbox->Write(text.Format("OK\n"));
    }
    WriteVDU("OK\n");
}



TextGrid::TextGrid(wxWindow *parent, wxSize size)
    : wxGrid(parent, wxID_ANY)
{
    //wxSize size(30, 30);
    ostype = GetSystem();

    CreateGrid(size.x, size.y);
    SetRowLabelSize(35);
    SetColLabelSize(25);
    SetRowLabelAlignment(wxALIGN_RIGHT, wxALIGN_CENTRE);
    SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTRE);
    SetLabelFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

    undogrid = new wxGridStringTable(size.x, size.y);
    vdu = NULL;
    gridbox = NULL;
    diagbox = NULL;
    mod = NULL;
    selectcol = 0;
    selectrow = 0;

    rightmenu = new wxMenu;
    rightmenu->Append(ID_SelectAll, "Select All", "Grid Select", wxITEM_NORMAL);
    rightmenu->Append(ID_Copy, "Copy", "Copy Selection", wxITEM_NORMAL);
    rightmenu->Append(ID_Paste, "Paste", "Paste Clipboard", wxITEM_NORMAL);
    rightmenu->Append(ID_PasteTranspose, "Paste Transpose", "Paste Clipboard", wxITEM_NORMAL);
    rightmenu->Append(ID_Undo, "Undo", "Undo", wxITEM_NORMAL);
    rightmenu->Append(ID_Insert, "Insert Col", "Insert Column", wxITEM_NORMAL);
    //rightmenu->Append(ID_Bold, "Bold", "Set Bold", wxITEM_NORMAL);
    //for(i=0; i<mod->graphbase->numgraphs; i++) menuPlot->AppendRadioItem(1000 + i, (*mod->graphbase)[i]->gname);

    //Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(TextGrid::OnRightClick));
    Connect(wxEVT_GRID_CELL_RIGHT_CLICK, wxGridEventHandler(TextGrid::OnRightClick));
    Connect(wxEVT_GRID_CELL_LEFT_CLICK, wxGridEventHandler(TextGrid::OnLeftClick));
    Connect(wxEVT_GRID_LABEL_LEFT_CLICK, wxGridEventHandler(TextGrid::OnLabelClick));
    Connect(ID_SelectAll, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TextGrid::OnSelectAll));
    Connect(ID_Cut, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TextGrid::OnCut));
    Connect(ID_Copy, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TextGrid::OnCopy));
    Connect(ID_Paste, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TextGrid::OnPaste));
    Connect(ID_PasteTranspose, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TextGrid::OnPaste));
    Connect(ID_Undo, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TextGrid::OnUndo));
    Connect(ID_Bold, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TextGrid::OnBold));
    Connect(wxID_ANY, wxEVT_KEY_DOWN, wxKeyEventHandler(TextGrid::OnKey));
    Connect(wxID_ANY, wxEVT_CHAR, wxKeyEventHandler(TextGrid::OnTypeKey));
    Connect(ID_Insert, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(TextGrid::OnInsertColumn));

}


TextGrid::~TextGrid()
{
    delete undogrid;
    delete rightmenu;
}


void TextGrid::CopyColumn(int source, int dest)
{
    int i;
    wxString celltext;

    int numrows = GetNumberRows();

    for(i=0; i<numrows; i++) {
        celltext = GetCellValue(i, source);
        SetCellValue(i, dest, celltext);
    }
}


void TextGrid::InsertColumn(int col)
{
    InsertCols(col);
}


double TextGrid::ReadDouble(int row, int col)
{
    double celldata;

    wxString celltext = GetCell(row, col);
    celltext.Trim();
    if(!celltext.IsEmpty()) celltext.ToDouble(&celldata);
    else return 0;
    return celldata;
}


bool TextGrid::CheckDouble(int row, int col, double *celldata)
{
    wxString celltext = GetCell(row, col);
    celltext.Trim();
    if(celltext.IsEmpty()) return false;
    return celltext.ToDouble(celldata);
}


wxString TextGrid::GetCell(int row, int col)
{
    int numrows = GetNumberRows();
    int numcols = GetNumberCols();

    if(row >= numrows || col >= numcols) return "";
    else return GetCellValue(row, col);
}


void TextGrid::SetCell(int row, int col, wxString data)
{
    int numrows = GetNumberRows();
    int numcols = GetNumberCols();
    
    if(row > 50000) return;

    if(row >= numrows) {
        AppendRows(row - numrows + 10);
    }
    if(col >= numcols) {
        AppendCols(col - numcols + 10);
    }

    //if(row == 0) diagbox->Write(text.Format("SetCell row %d col %d data %s\n", row, col, data));

    SetCellValue(row, col, data);
}


void TextGrid::OnTypeKey(wxKeyEvent &event)
{
    wxString text;

    int r = GetGridCursorRow();
    int c = GetGridCursorCol();
    //SetCellValue(r, c, text.Format("%c ", event.GetKeyCode()));
    event.Skip();
}


void TextGrid::OnKey(wxKeyEvent &event)
{
    wxString text;

    //SetCellValue(10, 0, text.Format("%d", event.GetUnicodeKey()));

    if(event.GetUnicodeKey() == 'C' && event.ControlDown() == true) Copy();

    else if(event.GetUnicodeKey() == 'V' && event.ControlDown() == true) Paste();

    else if(event.GetUnicodeKey() == 'T' && event.ControlDown() == true) Paste(1);

    else if(event.GetUnicodeKey() == 'X' && event.ControlDown() == true) Cut();

    else if(event.GetUnicodeKey() == 'Z' && event.ControlDown() == true) Undo();

    else if(event.GetUnicodeKey() == 'A' && event.ControlDown() == true) SelectAll();

    else if(event.GetKeyCode() == WXK_DELETE) {
        Delete();
        return;
    }

    /*
    else if(event.GetKeyCode() == 'T') {
    int r = GetGridCursorRow();
    int c = GetGridCursorCol();
    //SetCellValue(r, c, text.Format("%c %c", 'e', event.GetKeyCode()));
    EnableCellEditControl();
    }*/

    event.Skip();
}


void TextGrid::ParseLine(int row, int col, wxString readline)
{
    wxString text;

    while(!readline.IsEmpty()) {
        text = readline.BeforeFirst(' ');
        text.Trim();
        SetCell(row, col++, text);
        readline = readline.AfterFirst(' ');
    }
}


void TextGrid::ParseLabel(int row, int col, wxString readline)
{
    wxString text, label;

    if(readline.GetChar(0) == '\'') {
        label = readline.AfterFirst('\'');
        label = label.BeforeFirst('\'');
    }
    if(readline.GetChar(0) == '\"') {
        label = readline.AfterFirst('\"');
        label = label.BeforeFirst('\"');
    }
    if(!label.BeforeFirst('.').IsEmpty()) label = label.BeforeFirst('.');
    label.Trim();
    SetCell(row, col++, label);
}
 

void TextGrid::ClearCol(int col)
{
    int i;

    for(i=0; i<GetNumberRows(); i++) SetCellValue(i, col, "");
}


void TextGrid::SetBold()
{
    int i, j;

    CopyUndo();

    for(i=0; i<GetNumberRows(); i++)
        for(j=0; j<GetNumberCols(); j++)
            if(IsInSelection(i, j)) SetCellFont(i, j, wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

    Refresh();
}


void TextGrid::OnSelectAll(wxCommandEvent& event)
{
    SelectAll();
}


void TextGrid::OnInsertColumn(wxCommandEvent& event)
{
    int col;

    col = GetGridCursorCol();
    InsertColumn(col);
}


void TextGrid::OnCut(wxCommandEvent& event)
{
    Cut();
}


void TextGrid::OnCopy(wxCommandEvent& event)
{
    Copy();
}



void TextGrid::OnPaste(wxCommandEvent& event)
{
    if(event.GetId() == ID_PasteTranspose) Paste(1);
    else Paste(0);
}


void TextGrid::OnBold(wxCommandEvent& event)
{
    SetBold();
}


void TextGrid::Delete()
{
    int i, j;

    CopyUndo();

    SetCellValue(GetGridCursorRow(), GetGridCursorCol(), "");

    for(i=0; i<GetNumberRows(); i++)
        for(j=0; j<GetNumberCols(); j++)
            if(IsInSelection(i, j)) SetCellValue(i, j, "");
}


void TextGrid::Cut()
{
    Copy();
    Delete();
}


void TextGrid::Copy()
{
    int i, j;

    // grid copy code from wxwidgets forum

    wxString copy_data;
    bool used_line;

    copy_data.Clear();

    for(i=0; i<GetNumberRows(); i++) {
        used_line = false;
        for(j=0; j<GetNumberCols(); j++) {
            if(IsInSelection(i, j)) {
                if(!used_line) {
                    if(!copy_data.IsEmpty()) copy_data.Append("\n");
                    used_line = true;
                }
                else copy_data.Append("\t");
                copy_data.Append(GetCellValue(i, j));
            }
        }
    }

    if(ostype == Mac) {
        if (wxTheClipboard->Open()) {
            wxTextDataObject *clipboard_data = new wxTextDataObject();
            clipboard_data->SetData(wxDF_TEXT,strlen(copy_data.c_str()), copy_data.c_str());
            wxTheClipboard->Clear();

            if (!wxTheClipboard->SetData(clipboard_data)) {
                wxMessageBox(_("Unable to copy data to clipboard."), _("Error"));
            }
            wxTheClipboard->Close();
        } else {
            wxMessageBox(_("Error opening clipboard."), _("Error"));
        }
    }
    if(ostype == Windows) {
        if (wxTheClipboard->Open()) {
            wxTheClipboard->SetData(new wxTextDataObject(copy_data));
            wxTheClipboard->Close();
        }
        else wxMessageBox("Error opening clipboard.", "Error");
    }
    else {
#ifndef OSXClip
        //wxOpenClipboard();
        //wxEmptyClipboard();
        //wxSetClipboardData(wxDF_TEXT, copy_data.c_str(), 0, 0);
        //wxCloseClipboard();
#endif
    }
}


void TextGrid::Paste(int mode)
{
    long i, j, k, datasize;
    wxString text;
    double prog;

    // grid paste code from wxwidgets forum

    CopyUndo();

    wxString copy_data, cur_field, cur_line;

    if(vdu) vdu->AppendText("Transpose Pasting...\n");
    if(vdu) vdu->AppendText("Copy clipboard...");

    if(ostype == Mac || ostype == Windows) {
        wxTheClipboard->Open();
        wxTextDataObject data;
        wxTheClipboard->GetData(data);
        copy_data = data.GetText();
        wxTheClipboard->Close();
    }
    else {
#ifndef OSXClip
        //wxOpenClipboard();
        //copy_data = (char *)wxGetClipboardData(wxDF_TEXT);
        //wxCloseClipboard();
#endif
    }

    datasize = copy_data.Len();
    if(vdu) vdu->AppendText(text.Format("OK, size %d\nWriting cells...", datasize));

    i = GetGridCursorRow();
    j = GetGridCursorCol();
    if(mode == 1) k = i;
    else k = j;
    prog = 0.1;

    while(!copy_data.IsEmpty()) {
        cur_line = copy_data.BeforeFirst('\n');
        //if(vdu) vdu->AppendText(text.Format("\nRow %d", i));
        while(!cur_line.IsEmpty()) {
            cur_field = cur_line.BeforeFirst('\t');
            if(!(cur_field.Trim()).IsEmpty()) SetCell(i, j, cur_field);
            if(mode == 1) i++;  // transpose
            else j++;
            cur_line  = cur_line.AfterFirst ('\t');
        }
        if(mode == 1) {  // transpose
            j++;
            i = k;
        }
        else {           // normal
            i++;
            j = k;
        }
        copy_data = copy_data.AfterFirst('\n');
        if(copy_data.Len() < datasize * (1 - prog)) {
                if(vdu) vdu->AppendText(text.Format(".%.0f%%.", prog * 100));
                prog = prog + 0.1;
        }
    }

    if(vdu) vdu->AppendText("OK\n");
}


void TextGrid::OnUndo(wxCommandEvent& event)
{
    Undo();
}


void TextGrid::OnLabelClick(wxGridEvent& event)
{
    int r, c;
    wxString text;

    //if(vdu) vdu->AppendText("Label Click\n");
    c = event.GetCol();
    r = event.GetRow();

    if(gridbox && c >= 0) {
        gridbox->ColumnSelect(c);
        selectcol = c;}

    if(gridbox && r >= 0) {
        gridbox->RowSelect(r);
        selectrow = r;
    }

    //if(mod) mod->GridColumn(c);

    //diagbox->Write(text.Format("TextGrid LabelClick c %d\n", c));
    //GoToCell(0, c);

    event.Skip();
}


void TextGrid::OnLeftClick(wxGridEvent& event)
{
    int row, col;
    wxString text;

    wxPoint pos = event.GetPosition();
    //row = GetGridCursorRow();
    //col = GetGridCursorCol();
    row = event.GetRow();
    col = event.GetCol();

    selectrow = row;
    selectcol = col;

    if(diagbox) diagbox->Write(text.Format("grid click row %d col %d\n", row, col));

    //GoToCell(row, col);

    //SetGridCursor(row, col);

    /*
    if(event.GetRow() == r && event.GetCol() == c)
    EnableCellEditControl();
    else
    GoToCell(event.GetRow(), event.GetCol());*/

    event.Skip();

    //wxGridCellEditor* celledit = GetCellEditor(r, c);

    /*
    wxGridCellEditor* pEditor = GetCellEditor(ev.GetRow(), ev.GetCol());
    wxPoint* pClickPoint = (wxPoint*)pEditor->GetClientData();
    if (pClickPoint)
    {
    *pClickPoint = ClientToScreen(ev.GetPosition());
    #ifndef __WINDOWS__
    EnableCellEditControl(true);
    #endif
    }
    // hack to prevent selection from being lost when click combobox
    if (ev.GetCol() == 0 && IsInSelection(ev.GetRow(), ev.GetCol()))
    {
    m_selTemp = m_selection;
    m_selection = NULL;
    }
    pEditor->DecRef();
    ev.Skip();
    */
}


void TextGrid::OnRightClick(wxGridEvent& event)
{
    //int id = event.GetId();
    //wxWindow *pos = FindWindowById(id, toolpanel);
    //wxPoint point = this->GetPosition();
    //SetCellValue(10, 0, "right");

    wxPoint pos = event.GetPosition();
    //wxSize size = this->GetSize();
    //menuPlot->Check(1000 + gpos->GetFront()->gindex, true);
    PopupMenu(rightmenu, pos.x - 20, pos.y);
}


void TextGrid::CopyUndo()
{
    int x, y;
    wxString data;

    if(GetNumberRows() > undogrid->GetNumberRows()) undogrid->AppendRows(GetNumberRows() - undogrid->GetNumberRows());
    if(GetNumberCols() > undogrid->GetNumberCols()) undogrid->AppendCols(GetNumberCols() - undogrid->GetNumberCols());

    for(x=0; x<GetNumberCols(); x++)
        for(y=0; y<GetNumberRows(); y++) {
            data = GetCellValue(y, x);
            undogrid->SetValue(y, x, data);
        }
}


void TextGrid::Undo()
{
    int x, y;
    wxString data;

    if(GetNumberRows() > undogrid->GetNumberRows()) undogrid->AppendRows(GetNumberRows() - undogrid->GetNumberRows());
    if(GetNumberCols() > undogrid->GetNumberCols()) undogrid->AppendCols(GetNumberCols() - undogrid->GetNumberCols());

    for(x=0; x<GetNumberCols(); x++)
        for(y=0; y<GetNumberRows(); y++) {
            data = undogrid->GetValue(y, x);
            SetCellValue(y, x, data);
        }
}

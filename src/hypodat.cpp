

#include <hypodat.h>
#include <hypotools.h>


void NeuroDat::ratecalc()
{
	short spikestep;
	int i;
	int maxspikes = 100000;
	//int maxtime = 10000;
	//datdouble times;
	double *isis;
	double stime, isimean;

	//times.setsize(spikecount);
	isis = new double[spikecount+1];
	stime = 0;
	isimean = 0;
	isicount = spikecount - 1;


	for(i=0; i<maxtime; i++) srate[i] = 0;

	for(i=0; i<spikecount-1; i++) {
		//isis[i] = (currentset->times[i+1] - currentset->times[i]) * 1000;   // real
		if(i > maxspikes-1) break;
		//if(neurodat) isis[i] = datneuron->isis[i];
		isis[i] = (times[i+1] - times[i]); // * 1000;
		//stime = stime + isis[i];
		//times[i] = stime/1000;
		//index[i] = i;
		//if(hist1.max < (int)isis[i]) hist1.max = (int)isis[i];
		//if(hist1.data.size() < hist1.max + 1) hist1.data.resize(hist1.max + 1);
		//hist1[(int)isis[i]]++;	
		isimean = isimean + (double)(isis[i] / spikecount);
		//variance = isis[i] * isis[i] / spikecount + variance;
	}

	if(isimean == 0) freq = 0;
	else freq = 1000/isimean;

	spikestep = 0;
	//srate.max = (int)(times[spikecount-1] + 0.5); 
	for(i=0; i<=times[spikecount-1] && i < maxtime; i++) {	     // spike rate count (1s)
		//fprintf(ofp, "%ds. ", i);
		if(spikestep > spikecount || i >= 10000) {
			//fprintf(ofp, "break spikestep %d  spikecount %d\n", spikestep, spikecount);
			break;
		}
		while(times[spikestep] < i+1) {
			//fprintf(ofp, "spike %d  ", spikestep);
			srate[i]++;
			spikestep++;
			if(spikestep >= spikecount) break;
		}
		//fprintf(ofp, "srate %d\n", srate[i]);
	}
	delete[] isis;
}



void SpikeDat::Clear()
{
	int i;

	for(i=0; i<10000; i++) {
		hist1[i] = 0;
		hist5[i] = 0;
		haz1[i] = 0;
		haz5[i] = 0;
	}
	for(i=0; i<100000; i++) {
		srate[i] = 0;
		times[i] = 0;
		isis[i] = 0;
	}
	for(i=0; i<1000000; i++) {
		srate1[i] = 0;
		synsim[i] = 0;
	}
	spikecount = 0;
}


void SpikeDat::ReSize(int newsize)
{
	//srate.data.resize(newsize);
	//srate.max = newsize;
	srate.setsize(newsize);
	times.data.resize(newsize);
	times.max = newsize;
	isis.data.resize(newsize);
	isis.max = newsize;

	maxspikes = newsize;
}


SpikeDat::SpikeDat()
{
	hist1.setsize(10000);
	hist5.setsize(10000);
	haz1.setsize(10000);
	haz5.setsize(10000);
	histquad.setsize(1000);
	histquadx.setsize(1000);
	histquadsm.setsize(1000);
	histquadlin.setsize(1000);
	hazquad.setsize(10000);

	srate.setsize(100000);
	srate1.setsize(1000000);
	srate100.setsize(100000);

	synsim.data.resize(1000100);
	synsim.max = 1000000;

	times.data.resize(100000);
	times.max = 100000;
	isis.data.resize(100000);
	isis.max = 100000;
	winfreq.data.resize(11000);
	winfreq.max = 10000;

	raterec.setsize(1000);
	netinputrec.setsize(1000);

	burstdata = NULL;
	vasomean.data.resize(250);
	vasomean.max = 200;

	count = 0;
	spikecount = 0;
	start = 0;
	freqwindow = 100;
	maxspikes = 100000;
	mainwin = NULL;
}


SpikeDat::~SpikeDat()
{
	if(burstdata) delete burstdata;
}


BurstDat::BurstDat()
{
	burstdisp = 0;
	numbursts = 0;
	hist1.data.resize(10000);
	hist5.data.resize(10000);
	haz1.data.resize(10000);
	haz5.data.resize(10000);
	spikes.data.resize(100000);
	spikes.max = 100000;
	profile.data.resize(1000);
	profile.max = 500;
	tailprofile.data.resize(1000);
	tailprofile.max = 500;
	bursthaz.data.resize(1000);
	bursthaz.max = 500;
	length.data.resize(250);
	length.max = 200;

	tailnum.data.resize(1000);
	tailnum.max = 1000;
	tailmean.data.resize(1000);
	tailmean.max = 1000;
	tailstdev.data.resize(1000);
	tailstdev.max = 1000;
	tailstder.data.resize(1000);
	tailstder.max = 1000;

	profilesm.setsize(500);
	tailprofilesm.setsize(500);
}


AnaDat::AnaDat()
{
	autocorr.data.resize(10100);
	autocorr.max = 10000;
}


CurrentDat::CurrentDat()
{
	int storesize = 1000000;

	I.data.resize(storesize * 1.1);
	I.max = storesize;

	DAP.data.resize(storesize * 1.1);
	DAP.max = storesize;

	DAP2.data.resize(storesize * 1.1);
	DAP2.max = storesize;

	AHP.data.resize(storesize * 1.1);
	AHP.max = storesize;
}


GraphDat::GraphDat()
{
	scrollpos = 0;
	gparam = 0;
	xscale = 1;
	xdis = 0;
	negscale = 0;
	//burstdata = NULL;
	spikedata = NULL;
}


wxString GraphDat::StoreDat()
{
	wxString gtext;

	return gtext.Format("index %d xf %.8f xt %.8f yf %.8f yt %.8f name %s", gindex, xfrom, xto, yfrom, yto, gname);
}


void GraphDat::LoadDat(wxString data)
{
	wxString readline, numstring;

	readline = data.AfterFirst('f');
	numstring = readline.BeforeFirst(' ');
	numstring.ToDouble(&xfrom);
	readline = readline.AfterFirst('t');
	numstring = readline.BeforeFirst(' ');
	numstring.ToDouble(&xto);

	readline = readline.AfterFirst('f');
	numstring = readline.BeforeFirst(' ');
	numstring.ToDouble(&yfrom);
	readline = readline.AfterFirst('t');
	numstring = readline.BeforeFirst(' ');
	numstring.ToDouble(&yto);
}


GraphDat::GraphDat(datdouble *newdat, double xf, double xt, double yf, double yt, wxString name, int gtype, double bin, int gcolour, int xs, int xd)
{
	scrollpos = 0;
	xscale = xs;
	xdis = xd;
	negscale = 0;
	spikedata = NULL;

	gdatadv = newdat;
	gparam = -4;
	xfrom = xf;
	xto = xt;
	yfrom = yf;
	yto = yt;
	gname = name;
	type = gtype;
	colour = gcolour;
	binsize = bin;
	samprate = 1;
}


GraphDat::GraphDat(datint *newdat, double xf, double xt, double yf, double yt, wxString name, int gtype, double bin, int gcolour)
{
	scrollpos = 0;
	xscale = 1;
	xdis = 0;
	negscale = 0;
	spikedata = NULL;

	gdatav = newdat;
	gparam = -3;
	xfrom = xf;
	xto = xt;
	yfrom = yf;
	yto = yt;
	gname = name;
	type = gtype;
	colour = gcolour;
	binsize = bin;
}


GraphDat::GraphDat(datint *newdat, double xf, double xt, double yf, double yt, wxString name, SpikeDat *newspikedata, double bin, int gcolour)
{
	//GraphDat();
	scrollpos = 0;
	xscale = 1;
	xdis = 0;
	negscale = 0;

	gdatav = newdat;
	gparam = -3;
	xfrom = xf;
	xto = xt;
	yfrom = yf;
	yto = yt;
	gname = name;
	type = 3;
	spikedata = newspikedata;
	colour = gcolour;
	binsize = bin;
}


graphdisp::graphdisp()
{
	numplots = 0;
	currentplot = 0;
	spikedisp = 0;
}


void graphdisp::Add(GraphDat *newplot)
{
	if(numplots < 5) {
		plot[numplots] = newplot;
		numplots++;
	}
}


void graphdisp::Front(GraphDat *newplot)
{
	plot[0] = newplot;
	if(numplots == 0) numplots = 1;
}


GraphDat *graphdisp::GetFront()
{
	return plot[0];
}


GraphSet::GraphSet(GraphBase *gbase, int gdex) 
{
	graphbase = gbase; 
	numgraphs = 0;
	numflags = 0;
	modesum = 0;
	single = true;
	if(gdex != -1) Add(gdex);
};


void GraphSet::Add(int gdex, int gcode) 
{
	gindex[numgraphs] = gdex;
	gcodes[numgraphs] = gcode;
	numgraphs++;
	if(numgraphs > 1) single = false;
}


void GraphSet::Add(wxString tag, int gcode) 
{
	gindex[numgraphs] = graphbase->tagindex[tag];
	gcodes[numgraphs] = gcode;
	numgraphs++;
	if(numgraphs > 1) single = false;
}


void GraphSet::AddFlag(wxString flag, int weight)
{
	modeflag[numflags] = flag;
	modeweight[numflags] = weight;
	numflags++;
}


int GraphSet::GetPlot(ParamStore *gflags)
{
	int i, gdex;

	if(single) return gindex[0];

	modesum = 0;
	gdex = gindex[0];
	for(i=0; i<numflags; i++) modesum += (*gflags)[modeflag[i]] * modeweight[i];

	for(i=0; i<numgraphs; i++)
		if(gcodes[i] == modesum) gdex = gindex[i];

	return gdex;
}


wxString GraphSet::Display()
{
	int i;
	wxString text, output = "";

	output += text.Format("\nSet %s\n", tag);
	for(i=0; i<numgraphs; i++) output += text.Format("setindex %d  index %d  code %d\n", i, gindex[i], gcodes[i]);  

	return output;
}


int GraphBase::Add(GraphDat newgraph, wxString tag, wxString settag, bool set)       // default settag = "", set=true
{
	int sdex;
	wxString text;
	GraphSet *graphset;

	// If single graph, create new single graph set, otherwise add to set 'settag'
	if(set) {
		if(settag == "") graphset = NewSet(newgraph.gname, tag);
		else graphset = GetSet(settag);
		if(graphset) graphset->Add(numgraphs);
		//newgraph.sdex = graphset->sdex;
	}

	// Expand graphbase if necessary
	if(numgraphs == storesize) {
		storesize++;
		graphstore.resize(storesize);
	}

	// Add the new graph to graphbase
	graphstore[numgraphs] = newgraph;
	graphstore[numgraphs].gindex = numgraphs;
	tagindex[tag] = numgraphs;
	indextag[numgraphs] = tag;

	if(mainwin->diagbox) mainwin->diagbox->Write(text.Format("new graph sdex %d\n", graphset->sdex));
	
	numgraphs++;
	return numgraphs-1; 
};


GraphSet *GraphBase::NewSet(wxString name, wxString tag) 
{
	wxString text; 

	if(numsets == setstoresize) {
		setstoresize++;
		setstore.resize(setstoresize);
	}
	setstore[numsets] = GraphSet(this);
	setstore[numsets].tag = tag;
	setstore[numsets].name = name;
	setstore[numsets].sdex = numsets;
	settagindex[tag] = numsets;
	setindextag[numsets] = tag;
	numsets++;

	if(mainwin->diagbox) mainwin->diagbox->Write(text.Format("new set name %s tag %s numsets %d\n", name, tag, numsets));

	return &setstore[numsets-1];
}

/*
int GraphBase::AddNewSet(wxString tag, int gdex) 
{
	if(numsets == setstoresize) {
		setstoresize++;
		setstore.resize(setstoresize);
	}
	setstore[numsets] = GraphSet(this, gdex);
	setstore[numsets].Add(gdex);
	setstore[numsets].tag = tag;
	settagindex[tag] = numsets;
	setindextag[numsets] = tag;
	numsets++;
	return numsets-1;
}*/


GraphSet *GraphBase::GetSet(wxString tag) 
{
	if(!settagindex.check(tag)) return NULL;
	int index = settagindex[tag];
	return &(setstore[index]);
}


GraphSet *GraphBase::GetSet(int index) 
{
	return &(setstore[index]);
}


void GraphBase::BaseStore(wxString path, wxString tag)
{
	int i;
	TextFile outfile;
	wxString text, filename, filetag;

	filename = "gbase-" + tag + ".dat";

	//outfile.New(initpath + "/Graphs/" + filename);
	outfile.New(path + "/" + filename);
	for(i=0; i<numgraphs; i++) {
		outfile.WriteLine(graphstore[i].StoreDat());
		//outfile.WriteLine(text.Format
	}
	outfile.Close();
}


void GraphBase::BaseLoad(wxString path, wxString tag, wxTextCtrl *textbox)
{
	int i;
	TextFile infile;
	wxString readline, filename, filetag;
	wxString text, numstring;
	double numdat;

	filename = "gbase-" + tag + ".dat";

	//if(!infile.Open(initpath + "/Graphs/" + filename)) return;
	if(!infile.Open(path + "/" + filename)) return;

	if(textbox) textbox->AppendText(text.Format("Loading %d graphs\n", numgraphs));

	i = 0;
	readline = infile.ReadLine();

	while(!readline.IsEmpty()) {
		//readline = infile.ReadLine();
		//textbox->AppendText(readline + "\n");

		readline = readline.AfterFirst('f');
		readline.Trim(false);
		numstring = readline.BeforeFirst(' ');
		numstring.ToDouble(&numdat);
		graphstore[i].xfrom = numdat;
		//textbox->AppendText(text.Format("xf %.4f\n", numdat));

		readline = readline.AfterFirst('t');
		readline.Trim(false);
		numstring = readline.BeforeFirst(' ');
		numstring.ToDouble(&graphstore[i].xto);

		readline = readline.AfterFirst('f');
		readline.Trim(false);
		numstring = readline.BeforeFirst(' ');
		numstring.ToDouble(&graphstore[i].yfrom);

		readline = readline.AfterFirst('t');
		readline.Trim(false);
		numstring = readline.BeforeFirst(' ');
		numstring.ToDouble(&numdat);
		graphstore[i].yto = numdat;
		//textbox->AppendText(text.Format("yt %.4f\n", numdat));

		//graphstore[i].LoadDat(readline);
			
		if(infile.End()) break;
		readline = infile.ReadLine();	
		i++;	
	}
	infile.Close();
}

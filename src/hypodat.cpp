

#include "hypodat.h"
#include "hypotools.h"


NeuroDat::NeuroDat()
	//: times(std::make_shared<double>(maxspikes)), 
	//srate(std::make_shared<int>(maxtime)),
	//srate10(std::make_shared<int>(maxtime))
{
	maxtime = 100000;
	maxspikes = 100000;

	times.resize(maxspikes);
	//srate.resize(maxtime);
	//srate10.resize(maxtime);

	maxselect = 100;
	selectstore[0].resize(maxselect);
	numselects[0] = 0;
	selectstore[1].resize(maxselect);
	numselects[1] = 0;
	currselect = 0;
	xscrollpos = 0;

	filter = 0;
	burstfitmode = 0;
	xscrollpos = 0;
}


NeuroDat::~NeuroDat()
{
}


void NeuroDat::ReSize()
{
	maxspikes = maxspikes + 10000;
	times.resize(maxspikes);
}


bool NeuroDat::Selected()
{
	if(numselects[currselect]) return true;
	return false;
}


void NeuroDat::diagstore()
{
	maxspikes = 1000000;

	times.resize(maxspikes);
}


void NeuroDat::ratereset()
{
	int i;

	for(i=0; i<maxtime; i++) {
		//srate[i] = 0;
		//srate10[i] = 0;
	}
	for(i=0; i<10000; i++) srate100[i] = 0;
}


void NeuroDat::ratecalc(std::vector<int> &srate)
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

	if(spikecount == 0) {
		delete[] isis;
		freq = 0;
		return;
	}

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


SpikeDat::SpikeDat()
{
	diagbox = NULL;

	hist1.setsize(10000);
	hist5.setsize(10000);
	haz1.setsize(10000);
	haz5.setsize(10000);
	histquad.setsize(1000);
	histquadx.setsize(1000);
	histquadsm.setsize(1000);
	histquadlin.setsize(1000);
	hazquad.setsize(10000);

	histquadsmfit = histquadsm.data.data();
	hazquadfit = hazquad.data.data();
	histquadlinfit = histquadlin.data.data();
	histquadxfit = histquadx.data.data();

	hist1norm.setsize(10000);
	hist5norm.setsize(10000);
	//haz1norm.setsize(10000);
	haz5norm.setsize(10000);

	srate1s.setsize(100000);
	srate1.setsize(1000000);
	srate10.setsize(100000);
	srate100.setsize(100000);
	srate10s.setsize(100000);
	srate100s.setsize(100000);

	// New November 2020 for oxytocin gavage spike rate analysis
	srate30s.setsize(20000);   
	srate300s.setsize(20000);
	srate600s.setsize(20000);

	synsim.data.resize(1000100);
	synsim.max = 1000000;

	maxspikes = 100000;

	times.resize(maxspikes);
	//times.max = 100000;
	isis.data.resize(maxspikes);
	isis.max = 100000;
	winfreq.data.resize(11000);
	winfreq.max = 10000;

	raterec.setsize(1000);
	netinputrec.setsize(1000);

	IoDdata.setsize(100);
	IoDdataX.setsize(100);

	meanV.setsize(5000);

	vasomean.data.resize(250);
	vasomean.max = 200;

	mag.setsize(maxspikes);

	count = 0;
	spikecount = 0;
	start = 0;
	freqwindow = 100;
	normscale = 10000;
	mainwin = NULL;
	graphs = false;
	burstmode = 0;
	burstdispmode = 0;
	dispmodemax = 1;
	burstfitmode = 0;

	fitType = 0;    // 0 for basic, 1 for oxy, 2 for vaso
	label = "";
	fitdiag = false;

	IoDfit = IoDdata.data.data();

	burstdata = NULL;
	selectdata = NULL;
	colourdata = NULL;
	neurodata = NULL;
}


SpikeDat::~SpikeDat()
{
	if(burstdata) delete burstdata;
	if(selectdata) delete selectdata;
}


void SpikeDat::ColourSwitch(int mode)
{
	if(mode == 0) colourdata = NULL;
	if(mode == 1) colourdata = burstdata;
	if(mode == 2) colourdata = selectdata;
}


void SpikeDat::BurstInit()
{
	if(!burstdata) burstdata = new BurstDat(this);
}


void SpikeDat::SelectInit()
{
	if(!selectdata) selectdata = new BurstDat(this, true);
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
		srate1s[i] = 0;
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
	srate1s.setsize(newsize);
	times.resize(newsize);
	//times.max = newsize;
	isis.data.resize(newsize);
	isis.max = newsize;

	maxspikes = newsize;
}


SpikeDatTest::SpikeDatTest()
{
	diagbox = NULL;

	/*
	hist1.setsize(10000);
	hist5.setsize(10000);
	haz1.setsize(10000);
	haz5.setsize(10000);
	histquad.setsize(1000);
	histquadx.setsize(1000);
	histquadsm.setsize(1000);
	histquadlin.setsize(1000);
	hazquad.setsize(10000);

	hist1norm.setsize(10000);
	hist5norm.setsize(10000);
	//haz1norm.setsize(10000);
	//haz5norm.setsize(10000);

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
	mainwin = NULL;*/
}


SpikeDatTest::~SpikeDatTest()
{
	//if(burstdata) delete burstdata;
}


BurstDat::BurstDat(SpikeDat *data, bool select)
{
	spikedata = data;
	selectmode = select; 

	burstdisp = 0;
	numbursts = 0;
	hist1.setsize(10000);
	hist5.setsize(10000);
	hist1norm.setsize(10000);
	hist5norm.setsize(10000);
	haz1.setsize(10000);
	haz5.setsize(10000);
	//spikes.data.resize(100000);
	//spikes.max = 100000;
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
	IoDdata.setsize(100);
	IoDdataX.setsize(100);

	maxbursts = 1000;
	//bustore = new burst[maxbursts];
	bustore.resize(maxbursts);
	burstspikes.resize(100000);

	spikes = burstspikes.data();
	profilefit = profile.data.data();
	profilesmfit = profilesm.data.data();
}


BurstDat::~BurstDat()
{
	//delete[] bustore;
	//delete [] burstspikes;
}


int BurstDat::spikeburst(int spike)
{
	int bindex = 1;

	for(bindex = 1; bindex <= numbursts; bindex++) {
		if(spike <= bustore[bindex].end && spike >= bustore[bindex].start) return bindex;
	}

	return 0;
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


/*
void FontSet::Add(wxString name, int font)
{
	names[numfonts] = name;
	fontindex[numfonts] = font;
	refindex[font] = numfonts;
	numfonts++;
}


int FontSet::GetIndex(int id)
{
	return refindex[id];
}


int FontSet::GetFont(int ref)
{
	return typeindex[ref];
}



wxString TypeSet::List()
{
	wxString list, text;
	int i;

	list = "";
	for(i=0; i<numtypes; i++) 
		list = list + text.Format("Index %d Type %d Name %s\n", i, typeindex[i], names[i]); 
	return list;
}
*/


void OverSet::Add(int overid, int posid, int panel1, int panel2)
{
	buttonindex[num].overid = overid; 
	buttonindex[num].posid = posid; 
	overindex[num].panel1 = panel1;
	overindex[num].panel2 = panel2;
	num++;
}


OverDat *OverSet::GetOver(int id)
{
	for(i=0; i<num; i++)
		if(buttonindex[i].overid == id || buttonindex[i].posid == id) return &overindex[i];
	return NULL;
}


void TypeSet::Add(wxString name, int type)
{
	names[numtypes] = name;
	typeindex[numtypes] = type;
	refindex[type] = numtypes;
	numtypes++;
}


int TypeSet::GetIndex(int type)
{
	if(type == 4) type = 5;    // specific to GraphDat type conversion
	return refindex[type];
}


int TypeSet::GetType(int ref)
{
	return typeindex[ref];
}


wxString TypeSet::GetName(int ref)
{
	return names[ref];
}


wxString TypeSet::List()
{
	wxString list, text;
	int i;

	list = "";
	for(i=0; i<numtypes; i++) 
		list = list + text.Format("Index %d Type %d Name %s\n", i, typeindex[i], names[i]); 
	return list;
}



/* Plot Types  (drawing modes)

(ordering mostly evolved rather than designed)

1 - scaled width bars, histogram

2 - line graph with x data

3 - spike rate data with optional burst colouring

4 - normal line graph - SHOULD BE OUT OF USE

5 - line graph with scaling fix

6 - line graph with sampling

7 - scaled width bars

8 - scatter with sampling

9 - bar chart with x data

10 - scatter plot with x data - MOSTLY REPLACED BY SCATTER OPTION IN TYPE 2

11 - mean field plot (custom plot style for VMN paper)

*/


GraphDat::GraphDat()             // See more specific versions below
{
	scrollpos = 0;
	gparam = 0;
	xscale = 1;
	xdis = 0;
	xstop = 0;
	negscale = 0;
	//burstdata = NULL;
	spikedata = NULL;
}


/*

 GraphDat - graph object, used to store plot parameters and data links

 StoreDat (tag string) - generate a string encoding GraphPanel controlled drawing parameters for text file storage

 * Additional parameters are always added at the end to preserve backwards compatibility with old graph store files *

 gindex - graph index

 tag - graph tag

 xfrom - x-axis from

 xto - x-axis to

 yfrom - y-axis from

 yto - y-axis to

 xlabels - number of x-axis ticks and labels

 xstep - x-axis tick and label step size

 xtickmode - x-axis tick mode (0 = off, 1 = count, 2 = step)  (off only turns off ticks not labels)

 ylabels - number of y-axis ticks and labels

 ystep - y-axis tick and label step size

 ytickmode - y-axis tick mode (0 = off, 1 = count, 2 = step)  (off only turns off ticks not labels)

 colour - stroke colour index (default index 9 is used for custom colour specified by rgb)

 colourtext - string encoded rgb stroke colour

 xshift - x-axis label value shift (usually used to shift data to start from 0) 

 xunitscale - modifier for scaling up x-axis units (default 1 for no scaling)

 plotstroke - stroke line width in points (only used for EPS. not screen)

 storegname - gname (main graph label) reencoded with spaces replaced by underscores for store file

 storextag - xtag (x-axis title) reencoded with spaces replaced by underscores for store file

 storeytag - ytag (y-axis title) reencoded with spaces replaced by underscores for store file

 xplot - x plot size (pixels for screen, points for EPS)

 yplot - y plot size (pixels for screen, points for EPS)

 labelfontsize - axes labels font size (only used for EPS)

 clipmode - clip plot to graph are specified by xplot and yplot, 0 off, 1 on (only used for EPS)

 type - plot type, see above

 xunitdscale - modifier for scaling down x-axis units (default 1 for no scaling)

 xsample - downscales number of datapoints plotted to reduce plot resolution, e.g. 10000 points reduced to 1000 using xsample 10 (only used for EPS)

 barwidth - bar width for type 9 bar plot

 bargap - bar gap for type 9 bar plot

 xlabelplaces - number of decimals for x-axis tick labels, -1 for automatic
 
 ylabelplaces - number of decimals for y-axis tick labels, -1 for automatic
 
 xlabelmode - x-axis label mode, 0 for none, 1 for all (default), 2 for ends only
 
 ylabelmode - y-axis label mode, 0 for none, 1 for all (default), 2 for ends only
 
 xscalemode - x-axis scale mode, 0 for linear (default), 1 for log (no base control yet, specified in code)
 
 yscalemode - y-axis scale mode, 0 for linear (default), 1 for log (no base control yet, specified in code)
 
 xaxis - x-axis line, 0 off, 1 on
 
 yaxis - y-axis line, 0 off, 1 on
 
 yunitdscale - modifier for scaling down y-axis units (default 1 for no scaling)

 xstart - start point for x-axis values (default 0)

 */


wxString GraphDat::StoreDat(wxString tag)
{
	wxString gtext1, gtext2;
	wxString storegname, storextag, storeytag;
    wxString strokecolourtext, fillcolourtext;

	if(!gname.IsEmpty()) storegname = gname;                       // replace spaces with underscores for textfile storing
	else storegname = " ";
	storegname.Replace(" ", "_");

	if(!xtag.IsEmpty()) storextag = xtag;
	else storextag = " ";
	storextag.Replace(" ", "_");

	if(!ytag.IsEmpty()) storeytag = ytag;
	else storeytag = " ";
	storeytag.Replace(" ", "_");
    
    //if(mainwin->ostype == Mac)
    //colourtext = ColourString(strokecolour, 1);

    strokecolourtext = strokecolour.GetAsString(wxC2S_CSS_SYNTAX);
	fillcolourtext = fillcolour.GetAsString(wxC2S_CSS_SYNTAX);
    
    //diagbox->Write("colourtext: " + colourtext);

	gtext1.Printf("v12 index %d tag %s xf %.4f xt %.4f yf %.4f yt %.4f xl %d xs %.4f xm %d yl %d ys %.4f ym %d c %d srgb %s xs %.4f xu %.4f ps %.4f name %s xtag %s ytag %s xp %d yp %d pf %.4f cm %d type %d xd %.4f xsam %.4f bw %.4f bg %.4f yu %.4f ", 
		gindex, tag, xfrom, xto, yfrom, yto, xlabels, xstep, xtickmode, ylabels, ystep, ytickmode, colour, strokecolourtext, xshift, xunitscale, plotstroke, storegname, storextag, storeytag, xplot, yplot, labelfontsize, clipmode, type, xunitdscale, xsample, barwidth, bargap, yunitscale);
		
	gtext2.Printf("xl %d yl %d xm %d ym %d xs %d ys %d xa %d ya %d yd %.4f xg %.4f yg %.4f lf %d sc %.4f frgb %s xfm %d fs %d lm %d sm %d", 
		xlabelplaces, ylabelplaces, xlabelmode, ylabelmode, xscalemode, yscalemode, xaxis, yaxis, yunitdscale, xlabelgap, ylabelgap, labelfont, scattersize, fillcolourtext, fillmode, fillstroke, linemode, scattermode);

	return gtext1 + gtext2;
}


void GraphDat::LoadDat(wxString data, int version)                    // Not in use, see GraphBase::BaseLoad - now in use
{
	wxString readline, numstring, stringdat;
	wxString colourstring;
	long numdat;
	long red, green, blue;
	//int version;

	//if(diagbox) diagbox->Write(data + '\n');

	readline = data.AfterFirst('f');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToDouble(&xfrom);

	readline = readline.AfterFirst('t');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToDouble(&xto);

	readline = readline.AfterFirst('f');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToDouble(&yfrom);

	readline = readline.AfterFirst('t');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToDouble(&yto);

	readline = readline.AfterFirst('l');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToLong(&numdat);
	xlabels = numdat;

	readline = readline.AfterFirst('s');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToDouble(&xstep);

	readline = readline.AfterFirst('m');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToLong(&numdat);
	xtickmode = numdat;

	readline = readline.AfterFirst('l');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToLong(&numdat);
	ylabels = numdat;

	readline = readline.AfterFirst('s');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToDouble(&ystep);

	readline = readline.AfterFirst('m');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToLong(&numdat);
	ytickmode = numdat;

	readline = readline.AfterFirst('c');
	readline.Trim(false);
	numstring = readline.BeforeFirst(' ');
	numstring.ToLong(&numdat);
	colour = numdat;

	wxString redtext, greentext, bluetext;

	readline = readline.AfterFirst('b');
	readline.Trim(false);
	colourstring = readline.BeforeFirst('x');
	colourstring.Trim();
	strokecolour.Set(colourstring);

	/*
	numstring = readline.BeforeFirst(' ');
	numstring.ToLong(&red);
	redtext = numstring;
	readline = readline.AfterFirst(' ');
	numstring = readline.BeforeFirst(' ');
	numstring.ToLong(&green);
	greentext = numstring;
	readline = readline.AfterFirst(' ');
	numstring = readline.BeforeFirst(' ');
	numstring.ToLong(&blue);
	bluetext = numstring;
	strokecolour = wxColour(red, green, blue);
	*/

	//wxString text;
	//diagbox->Write(text.Format("graph %s strokecolour red %s %d green %s %d blue %s %d\n", gname, redtext, red, greentext, green, bluetext, blue));

	if(version > 0) {
		//if(diagbox) diagbox->Write(readline + '\n');

		xshift = ParseDouble(&readline, 's');

		/* ParseDouble
		readline = readline.AfterFirst('s');
		readline.Trim(false);
		numstring = readline.BeforeFirst(' ');
		numstring.ToDouble(&numdat);
		readline = readline.AfterFirst(' ');
		*/

		xunitscale = ParseDouble(&readline, 'u');
		plotstroke = ParseDouble(&readline, 's');

		//if(diagbox) diagbox->Write(readline + '\n');
		
		gname = ParseString(&readline, 'e');
		gname.Replace("_", " ");
		//if(gname == " ") gname = "";

		xtag = ParseString(&readline, 'g');
		xtag.Replace("_", " ");
		if(xtag == " ") xtag = "";

		ytag = ParseString(&readline, 'g');
		ytag.Replace("_", " ");
		if(ytag == " ") ytag = "";
	}	

	if(version >= 3) {
		xplot = ParseDouble(&readline, 'p');
		yplot = ParseDouble(&readline, 'p');
		labelfontsize = ParseDouble(&readline, 'f');
		clipmode = ParseLong(&readline, 'm');
	}

	if(version >= 4) {
		type = ParseLong(&readline, 'e');
	}

	if(version >= 5) {
		xunitdscale = ParseDouble(&readline, 'd');
		xsample = ParseDouble(&readline, 'm');
	}

	if(version >= 6) {
		barwidth = ParseDouble(&readline, 'w');
		bargap = ParseDouble(&readline, 'g');
	}

	if(version >= 7) {
		yunitscale = ParseDouble(&readline, 'u');
	}

	if(version >= 8) {
		xlabelplaces = ParseLong(&readline, 'l');
		ylabelplaces = ParseLong(&readline, 'l');
		xlabelmode = ParseLong(&readline, 'm');
		ylabelmode = ParseLong(&readline, 'm');
		xscalemode = ParseLong(&readline, 's');
		yscalemode = ParseLong(&readline, 's');
	}

	if(version < 9) {         // Convert to new tickmode 0 = off, 1 = count, 2 = step
		xtickmode += 1;
		ytickmode += 1;
	}

	if(version >= 9) {
		xaxis = ParseLong(&readline, 'a');
		yaxis = ParseLong(&readline, 'a');
	}

	if(version >= 10) {
		yunitdscale = ParseDouble(&readline, 'd');
	}

	if(version >= 11) {
		xlabelgap = ParseDouble(&readline, 'g');
		ylabelgap = ParseDouble(&readline, 'g');
		labelfont = ParseLong(&readline, 'f');
		scattersize = ParseDouble(&readline, 'c');
		colourstring = ParseString(&readline, 'b', 'x');
		colourstring.Trim();
		fillcolour.Set(colourstring);
	}

	if(version >= 12) {
		fillmode = ParseLong(&readline, 'm');
		fillstroke = ParseLong(&readline, 's');
		linemode = ParseLong(&readline, 'm');
		scattermode = ParseLong(&readline, 'm');
	}
}


GraphDat::GraphDat(datdouble *newdat, double xf, double xt, double yf, double yt, wxString name, int gtype, double bin, int gcolour, int xs, int xd)
{
	xscale = xs;
	xdis = xd;
	spikedata = NULL;

	gdatax = NULL;
	xcount = 0;
	ycount = 0;
	gdatadv = newdat;
	gparam = -4;
	type = gtype;
	samprate = 1;
	scattermode = 0;
	linemode = 1;
	scattersize = 2;

	xfrom = xf;
	xto = xt;
	yfrom = yf;
	yto = yt;
	gname = name;
	colour = gcolour;
	binsize = bin;
	Init();
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
	type = gtype;
	
	xfrom = xf;
	xto = xt;
	yfrom = yf;
	yto = yt;
	gname = name;
	colour = gcolour;
	binsize = bin;
	Init();
}


GraphDat::GraphDat(datdouble *newdat, double xf, double xt, double yf, double yt, wxString name, SpikeDat *newspikedata, double bin, int gcolour)
{
	xscale = 1;
	xdis = 0;
	negscale = 0;

	type = 3;
	spikedata = newspikedata;
	gdatadv = newdat;
	gparam = -4;

	xfrom = xf;
	xto = xt;
	yfrom = yf;
	yto = yt;
	gname = name;
	colour = gcolour;
	binsize = bin;
	Init();
}


GraphDat::GraphDat(datint *newdat, double xf, double xt, double yf, double yt, wxString name, SpikeDat *newspikedata, double bin, int gcolour)
{
	xscale = 1;
	xdis = 0;
	negscale = 0;

	type = 3;
	spikedata = newspikedata;
	gdatav = newdat;
	gparam = -3;

	xfrom = xf;
	xto = xt;
	yfrom = yf;
	yto = yt;
	gname = name;
	colour = gcolour;
	binsize = bin;
	Init();
}


int GraphDat::MaxDex()
{
	if(gparam == -3) return 0;     // currently no maxindex for datint type
	if(gparam == -4) return gdatadv->maxdex();
	return 0;
}


double GraphDat::GetData(double xval) 
{
	wxString text;
	double data = -1;
	int xindex;

	xindex = xval / binsize;
	if(gparam == -3) data = (*gdatav)[xindex];
	if(gparam == -4) data = (*gdatadv)[xindex];

	//diagbox->Write(text.Format("Get Data %.4f)

	return data;
}


void GraphDat::Init()
{
	diagbox = NULL;
	plotdata = NULL;
    //spikedata = NULL;

	scrollpos = 0;
	negscale = 0;
	xlabels = 0;
	ylabels = 0;
	xstep = 0;
	ystep = 0;
	xtickmode = 1;
	ytickmode = 1;
	plotstroke = 0.5;
	xplot = 500;
	yplot = 200;
	xshift = 0;
	yshift  = 0;
	xsample = 1;
	xunitscale = 1;
	yunitscale = 1;
	xunitdscale = 1;
	yunitdscale = 1;
	strokecolour.Set(0, 0, 0);
	xtag = "X";
	ytag = "Y";
	xlabelgap = 30; //40;
	ylabelgap = 30; // 20 //30;
	labelfontsize = 10;
	tickfontsize = 10;
	clipmode = 0;
	synchx = true;
	barshift = 0;
	barwidth = 10;
	bargap = 10;
	xstop = 0;
	highstart = 0;
	highstop = 0;
	highcolour = red;
	oldset = false;

	xlabelmode = 1;
	ylabelmode = 1;
	xticklength = 5;
	yticklength = 5;
	xlabelplaces = -1;
	ylabelplaces = -1;
	xscalemode = 0;
	yscalemode = 0;
	xlogbase = 2.7183;
	ylogbase = 10;

	scatterfield = 0;
	scattermean = 0;

	drawX = -1;   // default to draw whole X range
	xaxis = 1;
	yaxis = 1;
	axistrace = 0;

	labelfont = 0;  //default Helvetica
	fillmode = 1;
	fillstroke = 0;

	errmode = 0;
	gdataerr = NULL;

	oversync = 0;
	xstart = 0;
}


GraphDisp::GraphDisp()
{
	numplots = 0;
	currentplot = 0;
	spikedisp = 0;
	diagbox = NULL;
}


void GraphDisp::Add(GraphDat *newplot)
{
	if(numplots < maxplots) {
		plot[numplots] = newplot;
		numplots++;
	}
}


void GraphDisp::Remove(int index)       // Remove plot layer from GraphDisp set
{
	int i;

	if(index >= numplots) return;
	for(i=index; i<numplots-1; i++) plot[i] = plot[i+1];   // re-index
	numplots--;	
}


void GraphDisp::Display() 
{
	int i;
	wxString text;

	diagbox->Write(text.Format("\nGraphDisp numplots %d\n", numplots));
	for(i=0; i<numplots; i++) diagbox->Write(text.Format("GraphDisp plot %d graph %s\n", i, plot[i]->gname));
}



void GraphDisp::Set(int index, GraphDat *newplot)
{
	if(numplots <= index) Add(newplot);
	else plot[index] = newplot;
}


void GraphDisp::XYSynch(GraphDat *graph0)      // Synchronise X and Y axes for all plots
{
	int i;
	GraphDat *graph;

	//graph0 = plot[0];
	if(!graph0) graph0 = plot[0];
	
	for(i=1; i<numplots; i++) {
		graph = plot[i];
		graph->yfrom = graph0->yfrom; 
		graph->yto = graph0->yto; 
		graph->xfrom = graph0->xfrom; 
		graph->xto = graph0->xto; 
	}
}


void GraphDisp::Front(GraphDat *newplot)
{
	plot[0] = newplot;
	if(numplots == 0) numplots = 1;
}


GraphDat *GraphDisp::GetFront()
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
	diagbox = NULL;
	if(gdex != -1) Add(gdex); 
	current = 0;
	submenu = 0;
	subtag = "";
	
	for(int i=0; i<10; i++) subplot[i] = 0;

	modeflag.resize(10);         
	modeweight.resize(10);

	capacity = 0;
	Expand();
}


void GraphSet::Expand() {
	capacity += 10;
	gindex.resize(capacity);
	gcodes.resize(capacity);
}


void GraphSet::Add(int gdex, int gcode) 
{
	if(numgraphs == capacity) Expand();
	gindex[numgraphs] = gdex;
	gcodes[numgraphs] = gcode;
	numgraphs++;
	if(numgraphs > 1) single = false;

	//if(diagbox) diagbox->Write(text.Format("GraphSet %s Add numgraphs %d gdex %d gcode %d\n", name, numgraphs, gdex, gcode));
}


void GraphSet::Add(wxString tag, int gcode) 
{
	if(numgraphs == capacity) Expand();
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


int GraphSet::GetPlot(int pos, ParamStore *gflags)
{
	int i, gdex;

	if(single) return gindex[0];

	if(submenu) {
		if(subplot[pos]) return subplot[pos];    
		else return gindex[0];
	}

	modesum = 0;
	gdex = gindex[0];
	//gdex = current;

	for(i=0; i<numflags; i++) modesum += (*gflags)[modeflag[i]] * modeweight[i];

	for(i=0; i<numgraphs; i++)
		if(gcodes[i] == modesum) gdex = gindex[i];

	//current = gdex;
	return gdex;
}


GraphDat *GraphSet::GetPlot(int index)
{
	int gdex;

	gdex = gindex[index];
	return graphbase->GetGraph(gdex);
}


// Display, generate string to display GraphSet contents for diagnostics
wxString GraphSet::Display()
{
	int i;
	wxString text, output = "";
	GraphDat *graph;

	output += text.Format("\nSet %s\n", tag);
	for(i=0; i<numgraphs; i++) {
		graph = graphbase->GetGraph(gindex[i]);
		output += text.Format("setindex %d  index %d  code %d  name %s\n", i, gindex[i], gcodes[i], graph->gname);  
	}
	return output;
}


void GraphSet::IntervalSetBasic(wxString tag, bool burst, bool select, int selectcode)
{
	AddFlag("hazmode1", 10);
	AddFlag("binrestog1", 1);
	
	Add(tag + "hist1ms", 0);
	Add(tag + "haz1ms", 10);
	Add(tag + "hist5ms", 1);
	Add(tag + "haz5ms", 11);
	
	
	/*if(select) {
		Add(tag + "selecthist1ms", selectcode);
		Add(tag + "selecthaz1ms", selectcode + 10);
		Add(tag + "selecthist5ms", selectcode + 1);
		Add(tag + "selecthaz5ms", selectcode + 11);
	}
	else {
		Add(tag + "hist1ms", selectcode);
		Add(tag + "haz1ms", selectcode + 10);
		Add(tag + "hist5ms", selectcode + 1);
		Add(tag + "haz5ms", selectcode + 11);
	}*/
}


// IntervalSet, sets up graph switching for ISI analysis plots, currently specific to VasoModel graph button panel
void GraphSet::IntervalSet(wxString tag, bool burst, bool select, int selectcode)
{
	//if(burst && select) selectcode = 200;
	//else selectcode = 100;
	//selectcode = 200;

	AddFlag("hazmode1", 10);
	AddFlag("binrestog1", 1);
	AddFlag("burstmode", 100);
	AddFlag("normtog", 1000);
	AddFlag("quadtog", 10000);
	AddFlag("selectmode", 100000);
	selectcode = 100000;

	Add(tag + "hist1ms", 0);
	Add(tag + "haz1ms", 10);
	Add(tag + "hist5ms", 1);
	Add(tag + "haz5ms", 11);
	Add(tag + "normhist1ms", 1000);
	Add(tag + "haz1ms", 1010);
	Add(tag + "normhist5ms", 1001);
	Add(tag + "haz5ms", 1011);

	if(burst) {
		Add(tag + "bursthist1ms", 100);
		Add(tag + "bursthaz1ms", 110);
		Add(tag + "bursthist5ms", 101);
		Add(tag + "bursthaz5ms", 111);
		Add(tag + "burstnormhist1ms", 1100);
		Add(tag + "bursthaz1ms", 1110);
		Add(tag + "burstnormhist5ms", 1101);
		Add(tag + "bursthaz5ms", 1111);
	}

	if(select) {
		Add(tag + "selecthist1ms", selectcode);
		Add(tag + "selecthaz1ms", selectcode + 10);
		Add(tag + "selecthist5ms", selectcode + 1);
		Add(tag + "selecthaz5ms", selectcode + 11);
		Add(tag + "selectnormhist1ms", selectcode + 1000);
		Add(tag + "selecthaz1ms", selectcode + 1010);
		Add(tag + "selectnormhist5ms", selectcode + 1001);
		Add(tag + "selecthaz5ms", selectcode + 1011);
		Add(tag + "histquadsmooth", selectcode + 10000);
		Add(tag + "histquadsmooth", selectcode + 10001);
		Add(tag + "hazquad", selectcode + 10010);
		Add(tag + "hazquad", selectcode + 10011);
		Add(tag + "histquadsmooth", selectcode + 11000);
		Add(tag + "histquadsmooth", selectcode + 11001);
		Add(tag + "hazquad", selectcode + 11010);
		Add(tag + "hazquad", selectcode + 11011);
	}
	else {
		Add(tag + "hist1ms", selectcode);
		Add(tag + "haz1ms", selectcode + 10);
		Add(tag + "hist5ms", selectcode + 1);
		Add(tag + "haz5ms", selectcode + 11);
		Add(tag + "normhist1ms", selectcode + 1000);
		Add(tag + "haz1ms", selectcode + 1010);
		Add(tag + "normhist5ms", selectcode + 1001);
		Add(tag + "haz5ms", selectcode + 1011);
		Add(tag + "histquadsmooth", selectcode + 10000);
		Add(tag + "histquadsmooth", selectcode + 10001);
		Add(tag + "hazquad", selectcode + 10010);
		Add(tag + "hazquad", selectcode + 10011);
		Add(tag + "histquadsmooth", selectcode + 11000);
		Add(tag + "histquadsmooth", selectcode + 11001);
		Add(tag + "hazquad", selectcode + 11010);
		Add(tag + "hazquad", selectcode + 11011);
	}

	Add(tag + "histquadsmooth", 10000);
	Add(tag + "histquadsmooth", 11000);
	Add(tag + "hazquad", 10010);
	Add(tag + "hazquad", 11010);
	Add(tag + "histquadsmooth", 10001);
	Add(tag + "histquadsmooth", 11001);
	Add(tag + "hazquad", 10011);
	Add(tag + "hazquad", 11011);
}


int GraphBase::Add(GraphDat newgraph, wxString tag, wxString settag)       // default settag = "", for no set use settag = "null"          set=true - OLD
{
	//int sdex;
	wxString text;
	GraphSet *graphset = NULL;
	bool diag = false;

	if(diag) mainwin->diagbox->Write(text.Format("Graphbase Add %s to set %s, set flag %d  numgraphs %d storesize %d\n", tag, settag, numgraphs, storesize));
    
    
    // colour setting is done here since GraphDat doesn't have access to mainwin colour chart

	newgraph.diagbox = mainwin->diagbox;
	newgraph.strokecolour = mainwin->colourpen[newgraph.colour];
    newgraph.fillcolour = newgraph.strokecolour;
	newgraph.gtag = tag;
	numgraphs++;          // numgraphs gives the graphbase index of the new graph
    
    //mainwin->diagbox->Write(text.Format("GraphBase Add colour index %d string %s\n", newgraph.colour, ColourString(newgraph.strokecolour, 1)));

	// If single graph, create new single graph set, otherwise add to set 'settag'
	if(settag != "null") { 
		if(settag == "") graphset = NewSet(newgraph.gname, tag);
		else graphset = GetSet(settag);
		
		if(graphset) {    // extra check, should only fail if 'settag' is invalid
			graphset->Add(numgraphs);
			graphset->diagbox = mainwin->diagbox;
			newgraph.sdex = graphset->sdex;
		}
		//else mainwin->diagbox->Write("Graphset not found\n"); 
	
	}

	if(diag) mainwin->diagbox->Write("GraphSet Add OK\n");

	// Expand graphbase if necessary
	if(numgraphs == storesize) {
		storesize += 10;
		if(diag) mainwin->diagbox->Write(text.Format("GraphBase expand %d", storesize));
		graphstore.resize(storesize);
		if(diag) mainwin->diagbox->Write(text.Format(" newsize %d\n", (int)graphstore.size()));
	}
	else if(diag) mainwin->diagbox->Write(text.Format("GraphBase sufficient %d size %d\n", storesize, (int)graphstore.size()));
	
	// Add the new graph to graphbase
	//graphstore.push_back(newgraph);
	graphstore[numgraphs] = newgraph;
	graphstore[numgraphs].gindex = numgraphs;
	tagindex[tag] = numgraphs;
	nameindex[newgraph.gname] = numgraphs;
	indextag[numgraphs] = tag;

	//if(graphset && mainwin->diagbox) mainwin->diagbox->Write(text.Format("new graph sdex %d\n", graphset->sdex));
	
	//numgraphs++;

	if(diag) mainwin->diagbox->Write("GraphBase Add OK\n");

	//return numgraphs-1; 
	return numgraphs;
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

	//if(mainwin->diagbox) mainwin->diagbox->Write(text.Format("new set name %s tag %s numsets %d\n", name, tag, numsets));

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


GraphDat *GraphBase::GetGraph(wxString tag) 
{
	if(!tagindex.check(tag)) return NULL;
	int index = tagindex[tag];
	return &(graphstore[index]);
}


GraphDat *GraphBase::GetGraph(int index) 
{
	return &(graphstore[index]);
}


GraphDat *GraphBase::GetGraphFromName(wxString name) 
{
	if(!nameindex.check(name)) return NULL;
	int index = nameindex[name];
	return &(graphstore[index]);
}


bool GraphBase::GraphExists(wxString tag)
{
	if(!tagindex.check(tag)) return false;
	else return true;
}


bool GraphBase::SetExists(wxString tag)
{
	if(!settagindex.check(tag)) return false;
	else return true;
}


void GraphBase::BaseStore(wxString path, wxString tag)
{
	int i;
	TextFile outfile;
	wxString text, filename, filetag;

	filename = "gbase-" + tag + ".dat";

	//outfile.New(initpath + "/Graphs/" + filename);
	outfile.New(path + "/" + filename);
	for(i=1; i<=numgraphs; i++) {
		outfile.WriteLine(graphstore[i].StoreDat(GetTag(i)));
	}
	outfile.Close();

	mainwin->diagbox->Write(text.Format("BaseStore %d graphs\n", numgraphs));
}


void GraphBase::BaseLoad(wxString path, wxString tag, wxTextCtrl *textbox)
{
	int i;
	TextFile infile;
	wxString readline, filename, filetag;
	wxString text, numstring, namestring, basestring;
	wxString gtag, gname;
	//double numdat;
	GraphDat *graph;
	int version;

	filename = "gbase-" + tag + ".dat";

	//if(!infile.Open(initpath + "/Graphs/" + filename)) return;
	if(!infile.Open(path + "/" + filename)) return;

	if(textbox) textbox->AppendText(text.Format("Loading %d graphs\n", numgraphs));

	i = 0;
	readline = infile.ReadLine();

	// Version check

	//fileversion = ParseLong(&vstring, 'v');
	//textbox->AppendText(text.Format("Base file version %d\n", fileversion));
	//if(fileversion < version) textbox->AppendText(text.Format("Create base index\n"));

	while(!readline.IsEmpty()) {

		if(readline.GetChar(0) == 'v') version = ParseLong(&readline, 'v');          // check file version for backwards compatability
		else version = 0;
		//textbox->AppendText(text.Format("Base file version %d\n", version));
		//textbox->AppendText(text.Format("Readline %s\n", readline));

		if(version >= 2) {                                                  // Modern, reference by tag
			gtag = ParseString(&readline, 'g');
			graph = GetGraph(gtag);
			if(graph) graph->LoadDat(readline, version);
		}
		else {
			//GetGraphFromName(gname)->LoadDat(readline, version);              // Should add code to chop off any tag first
			//int ndex = readline.Find("name");
			//textbox->AppendText(text.Format("Base file version %d\n", version));
			//textbox->AppendText(text.Format("Readline %s\n", readline));
			namestring = readline.Mid(readline.Find("name"));
			//textbox->AppendText(text.Format("Name string %s\n", namestring));
			gname = ParseString(&namestring, 'e');
			gname.Replace("_", " ");
			if(textbox) textbox->AppendText(text.Format("gname %s\n", gname));
			graph = GetGraphFromName(gname);
			if(graph) graph->LoadDat(readline, version);
		}
			
		//graphstore[i].LoadDat(readline, version);
			
		if(infile.End()) break;
		readline = infile.ReadLine();	
		i++;	
	}
	infile.Close();


	// Read graphbase entries

	/*
	while(!readline.IsEmpty()) {
		graphstore[i].diagbox = mainwin->diagbox;
		graphstore[i].LoadDat(readline, version);
			
		if(infile.End()) break;
		readline = infile.ReadLine();	
		i++;	
	}
	infile.Close();
	*/
}


FitConSet::FitConSet(int size)
{
	count = 0;
	max = size;
	tags = new wxString[size];
	cons = new FitCon[size];
	fitconparams = new ParamStore();
}


FitConSet::~FitConSet()
{
	delete[] cons;
	delete[] tags;
	delete fitconparams;
}


void FitConSet::AddCon(wxString tag, wxString label, double value)
{
	ref[tag] = count;
	tags[count] = tag;
	cons[count] = FitCon(tag, label, value);
	count++;
}


FitCon FitConSet::GetCon(wxString tag)
{
	return cons[(int)ref[tag]];
}


ParamStore *FitConSet::GetParams(ParamStore *pstore)
{ 
	if(pstore == NULL) pstore = fitconparams;
	for(int i=0; i<count; i++) (*fitconparams)[tags[i]] = cons[i].value;

	return pstore;
}


FitSet::FitSet(int size)
{
	measureCount = 0;
	measureMax = size;
	measures = new FitMeasure[size];
	tags = new wxString[size];
}


FitSet::~FitSet()
{
	delete[] measures;
	delete[] tags;
}


void FitSet::AddMeasure(wxString tag, wxString label, double weight)
{
	ref[tag] = measureCount;
	tags[measureCount] = tag;
	measures[measureCount] = FitMeasure(tag, label, weight);
	measureCount++;
}


FitMeasure FitSet::GetMeasure(wxString tag)
{
	return measures[(int)ref[tag]];
}


void FitDat::ScoreConvert()
{
	scores["RMSFirstNBins"] = RMSFirstNBins;
	scores["RMSBinRange"] = RMSBinRange;
	scores["RMSHaz"] = RMSHaz;
	scores["RMSHazHead"] = RMSHazHead;
	scores["RMSIoD"] = RMSIoD;

	scores["RMSBurstHead"] = RMSBurstHead;
	scores["BurstMode"] = burstmode;
	scores["BurstLengthMean"] = burstlengthmean;
	scores["BurstLengthSD"] = burstlengthsd;
	scores["BurstSilenceMean"] = burstsilencemean;
	scores["BurstSilenceSD"] = burstsilencesd;
	scores["BurstIntraFreq"] = burstintrafreq;
	scores["RMSBurstIoD"] = RMSBurstIoD;
	scores["RMSBurstPeak"] = RMSBurstPeak;
}



double IoDcalc(int binsize, int spikecount, datdouble *times)
{
	int i;
	int maxbin = 10000;
	int spikerate[10000];
	int laststep;
	double mean, variance, dispersion = 0;
	double timeshift = 0;

	if((*times)[0] > 1000) timeshift = (*times)[0] - 1000;        // for data where recording starts at non-zero time point

	// calculate spike rate for binsize
	for(i=0; i<maxbin; i++) spikerate[i] = 0;
	for(i=0; i<spikecount; i++) if(((*times)[i] - timeshift) / binsize < maxbin) spikerate[(int)(((*times)[i] - timeshift) + 0.5) / binsize]++;
	laststep = ((int)((*times)[spikecount - 1] - timeshift)/ binsize) - 4;
	if(laststep > maxbin) laststep = maxbin;

	// calculate index of dispersion
	mean = 0;
	variance = 0;
	for(i=0; i<laststep; i++) mean = mean +  spikerate[i];	//mean
	mean = mean / laststep;
	for(i=0; i<laststep; i++) variance += (mean - spikerate[i]) * (mean - spikerate[i]);	// variance
	variance = variance / laststep;
	dispersion = variance / mean;		// dispersion

	return dispersion;
}

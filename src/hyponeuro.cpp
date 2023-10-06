


#include "hyponeuro.h"
#include "hypodata.h"



NeuroMod::NeuroMod(int type, wxString name, HypoMain *main)
	: Model(type, name, main)
{
	evodata = NULL;
	evodata2 = NULL;
	fitbox = NULL;
}


NeuroMod::~NeuroMod()
{
	if(evodata) delete evodata;
}


void NeuroMod::IoDGraph(datdouble *IoDdata, datdouble *IoDdataX, wxString label, wxString tag, int colour, int barshift, wxString settag)
{
	graphbase->Add(GraphDat(IoDdata, 0, 70, 0, 2, label, 9, 1, colour), tag, settag);
	graphbase->GetGraph(tag)->gdatax = IoDdataX;
	graphbase->GetGraph(tag)->xcount = 7;   // 7  // 5 for Trystan data
	graphbase->GetGraph(tag)->synchx = false;
	graphbase->GetGraph(tag)->barshift  = barshift;
}


void NeuroMod::EvoGraphs()
{
	evodata->PlotSet(graphbase, "Evo ", purple, 1, "evo");

	GraphSet *graphset = graphbase->NewSet("Evo Intervals", "evointervals");
	graphset->AddFlag("hazmode1", 10);
	graphset->AddFlag("binrestog1", 1);
	graphset->AddFlag("burstmode", 100);
	graphset->AddFlag("normtog", 1000);
	graphset->AddFlag("quadtog", 10000);
	graphset->Add("evohist1ms", 0);
	graphset->Add("evohaz1ms", 10);
	graphset->Add("evohist5ms", 1);
	graphset->Add("evohaz5ms", 11);
	graphset->Add("evobursthist1ms", 100);
	graphset->Add("evobursthaz1ms", 110);
	graphset->Add("evobursthist5ms", 101);
	graphset->Add("evobursthaz5ms", 111);
	graphset->Add("evonormhist1ms", 1000);
	graphset->Add("evonormhist5ms", 1001);
	graphset->Add("evohistquadsmooth", 10001);
	graphset->Add("evohistquadsmooth", 11001);
	if(diagbox) diagbox->textbox->AppendText(graphset->Display());

	graphbase->Add(GraphDat(&evodata->IoDdata, 0, 70, 0, 2, "Evo IoD", 9, 1, lightred), "iodevo");
	graphbase->GetGraph("iodevo")->gdatax = &evodata->IoDdataX;
	graphbase->GetGraph("iodevo")->xcount = 7;  
	graphbase->GetGraph("iodevo")->synchx = false; 
	graphbase->GetGraph("iodevo")->barshift  = 20;
}


void NeuroMod::DataSelect(wxString gname, double from, double to)
{
	wxString text; 

	diagbox->Write(text.Format("DataSelect graph %s from %.2f to %.2f\n", gname, from, to));
	if(gname.BeforeFirst(' ') == "Cell" && neurobox->cellpanel) {
		neurobox->cellpanel->SetSelectRange(from, to);
		neurobox->auitabpanel->SetSelection(0);
	}
	if(gname.BeforeFirst(' ') == "Model" && neurobox->modpanel) {
		neurobox->modpanel->SetSelectRange(from, to);
		neurobox->auitabpanel->SetSelection(1);
	}
}



NeuroPop::NeuroPop()
{
	maxtime = 200000;
	maxtimeRate1s = maxtime;
	maxtimeRate10ms = maxtime * 100;
	maxtimeRate1ms = maxtime * 1000;
	maxtimeLong = 35000;  // minutes, 30000 sufficient for 20 days simulation and recording

	srate1s.setsize(maxtime);
	srate10s.setsize(maxtime/10);
	srate30s.setsize(maxtime/30);
	srate300s.setsize(maxtime/300);
	srate600s.setsize(maxtime/600);

	popdat = new SpikeDat();
	neurodat = new SpikeDat();

	neurons = NULL;
	numneurons = 0;
}


void NeuroPop::SpikeAnalysis()
{
	// 'popdat' is a SpikeDat used to store population summed/mean neuron analysis 
	// 'neurodat' is a SpikeDat used for temporary individual neuron analysis

	int i, step;

	// Reset population counts
	//for(i=0; i<maxtime; i++) popdat->srate1s[i] = 0;
	//for(i=0; i<maxtime/10; i++) popdat->srate10s[i] = 0;
	//for(i=0; i<maxtime/30; i++) popdat->srate30s[i] = 0;
	//for(i=0; i<maxtime/300; i++) popdat->srate300s[i] = 0;
	//for(i=0; i<maxtime/600; i++) popdat->srate600s[i] = 0;

	//for(i=0; i<1000000; i++) popdat->srate1[i] = 0;   // 1ms bins for individual spikes

	for(i=0; i<10000; i++) {
		popdat->hist1[i] = 0;
		popdat->hist5[i] = 0;
		popdat->haz1[i] = 0;
		popdat->haz5[i] = 0;
	}

	popfreq = 0;

	// Analyse and sum each neuron
	for(i=0; i<numneurons; i++) {
		neurodat->neurocalc(&(*neurons)[i]);
		popfreq += neurodat->freq;

		//for(step=0; step<maxtime; step++) popdat->srate1s[step] += neurodat->srate1s[step];  // 1s bins
		//for(step=0; step<maxtime/10; step++) popdat->srate10s[step] += neurodat->srate10s[step];  // 10s bins
		//for(step=0; step<maxtime/30; step++) popdat->srate30s[step] += neurodat->srate30s[step];  // 30s bins
		//for(step=0; step<maxtime/300; step++) popdat->srate300s[step] += neurodat->srate300s[step];  // 300s bins
		//for(step=0; step<maxtime/600; step++) popdat->srate600s[step] += neurodat->srate600s[step];  // 600s bins

		//for(step=0; step<1000000; step++) popdat->srate1[step] += neurodat->srate1[step];  // 1 ms bins

		for(step=0; step<10000; step++) {
			popdat->hist1[step] += neurodat->hist1[step];
			popdat->hist5[step] += neurodat->hist5[step];
			popdat->haz1[step] += neurodat->haz1[step];
			popdat->haz5[step] += neurodat->haz5[step];
		}
	}

	// Convert sums to means
	//for(step=0; step<maxtime; step++) popdat->srate1s[step] = popdat->srate1s[step] / numneurons;  // 1s bins
	//for(step=0; step<maxtime/10; step++) popdat->srate10s[step] = popdat->srate10s[step] / numneurons;  // 10s bins
	//for(step=0; step<maxtime/30; step++) popdat->srate30s[step] = popdat->srate30s[step] / numneurons;  // 30s bins
	//for(step=0; step<maxtime/300; step++) popdat->srate300s[step] = popdat->srate300s[step] / numneurons;  // 300s bins
	//for(step=0; step<maxtime/600; step++) popdat->srate600s[step] = popdat->srate600s[step] / numneurons;  // 600s bins

	for(step=0; step<10000; step++) {
		popdat->hist1[step] = popdat->hist1[step] / numneurons;
		popdat->hist5[step] = popdat->hist5[step] / numneurons;
		popdat->haz1[step] = popdat->haz1[step] / numneurons;
		popdat->haz5[step] = popdat->haz5[step] / numneurons;
	}

	popfreq = popfreq / numneurons;
}





/*void NeuroMod::ExpDataSwitch(SpikeDat *data)
{
	if(fitbox) fitbox->expdata = data;
}*/


/*void NeuroMod::SpikeBox(int modmode)
{
int boxwidth, boxheight;

//wxString tag;

//SetStatusText("Burst Box");
//burstdata->spikedata = vasodata->spikedat;
if(ostype == Mac) {
boxwidth = 285;
boxheight = 380;
}
else {
boxwidth = 425;
boxheight = 500;
}

diagbox->Write(text.Format("Spike box init type %d\n", modmode));


if(modmode == 2) burstbox = new BurstBox(mod, "Spike Data Load and Analysis", wxPoint(0, 500), wxSize(boxwidth, boxheight), 0, "Selected");

if(modmode == 1) burstbox = new BurstBox(mod, "Spike Data", wxPoint(0, 500), wxSize(boxwidth, boxheight), 0, "Selected", false, 0);
//mainpos = GetPosition();

//burstbox = new BurstBox(this, "Analysis", wxPoint(320, 485), wxSize(330, 430), 0, "Selected");
burstbox->loaddata = expdata;

diagbox->Write(text.Format("SpikeModule modmode %d\n", modmode));

if(!expdata->graphs) {
SpikeModule(mod);
if(!modmode) scalebox->GraphSwitch();
}

diagbox->Write(text.Format("SpikeModule OK\n"));

mod->modtools.AddBox(burstbox, true);

//toolset->AddBox(burstbox);
burstbox->Show(true);
*/
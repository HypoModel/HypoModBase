#pragma once


//#include "wx/wx.h"
#include "hypodat.h"
#include "hypotools.h"
#include "hypomod.h"
#include "hypomain.h"


class Project
{
public:
	Mod *mod;
	TagSet *tagset;
	wxString protag;
	HypoMain *mainwin;

	wxString path;
	wxString boxfile;
	wxString graphfile;
	wxString prefsfile;
	wxString tagfile;
	wxString projectfile;

	Project(HypoMain *mainwin);

	void Load();
	void Store();
	void TagSetDisp();
	void Init(wxString tag, Mod *mod=NULL);
};

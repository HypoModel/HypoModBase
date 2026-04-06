//
//  hypoproto.h
//  HypoModOSX
//
//  Created by Duncan MacGregor on 06/04/2026.
//


#include "hypopanels.h"




class ProtocolBox : public ParamBox
{
public:

    //wxTextCtrl *datfiletag;

    //ParamStore *artspikeparams;
    //ParamStore *protoparams;
    //ParamSet *artspikecon;
    wxStaticText *currentinput;
    wxStaticText *status;

    ProtocolBox(Model *model, const wxString& title, const wxPoint& pos, const wxSize& size);
    //void BurstData();
    //void SpikeDat();
    //void GetArtSpikeParams();
    void OnRun(wxCommandEvent& event);
    //void BurstScan();
    //void SetPosition();
    //void OnMove(wxMoveEvent& event);
    //void OnClose(wxCloseEvent& event);
};




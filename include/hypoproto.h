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

    ProtocolBox(Mod *mod, const wxString& title, const wxPoint& pos, const wxSize& size);
    //void BurstData();
    //void SpikeDat();
    //void GetArtSpikeParams();
    void OnRun(wxCommandEvent& event);
    //void BurstScan();
    //void SetPosition();
    //void OnMove(wxMoveEvent& event);
    //void OnClose(wxCloseEvent& event);
};



enum {
    ID_inputproto = 2000,
    ID_RunSyn,
    ID_RunSynRate,
    ID_RunRamp,
    ID_RunFreq,
    ID_RunDura,
};

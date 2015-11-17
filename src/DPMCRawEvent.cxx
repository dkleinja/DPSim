#include "DPMCRawEvent.h"

#include <TMath.h>

ClassImp(DPMCDimuon)
ClassImp(DPMCHeader)
ClassImp(DPMCTrack)
ClassImp(DPMCHit)
ClassImp(DPMCRawEvent)

void DPMCDimuon::calcVariables()
{
    Double_t mp = 0.938;
    Double_t ebeam = 120.;

    TLorentzVector p_beam(0., 0., sqrt(ebeam*ebeam - mp*mp), ebeam);
    TLorentzVector p_target(0., 0., 0., mp);

    TLorentzVector p_cms = p_beam + p_target;
    TLorentzVector p_sum = fPosMomentum + fNegMomentum;

    fMass = p_sum.M();
    fpT = p_sum.Perp();

    fx1 = (p_target*p_sum)/(p_target*p_cms);
    fx2 = (p_beam*p_sum)/(p_beam*p_cms);

    Double_t s = p_cms.M2();
    Double_t sqrts = p_cms.M();
    TVector3 bv_cms = p_cms.BoostVector();
    p_sum.Boost(-bv_cms);

    fxF = 2.*p_sum.Pz()/sqrts/(1. - fMass*fMass/s);
    fCosTh = 2.*(fNegMomentum.E()*fPosMomentum.Pz() - fPosMomentum.E()*fNegMomentum.Pz())/fMass/TMath::Sqrt(fMass*fMass + fpT*fpT);
    fPhi = TMath::ATan(2.*TMath::Sqrt(fMass*fMass + fpT*fpT)/fMass*(fNegMomentum.Px()*fPosMomentum.Py() - fPosMomentum.Px()*fNegMomentum.Py())/(fPosMomentum.Px()*fPosMomentum.Px() - fNegMomentum.Px()*fNegMomentum.Px() + fPosMomentum.Py()*fPosMomentum.Py() - fNegMomentum.Py()*fNegMomentum.Py()));
}

DPMCTrack::DPMCTrack()
{
    for(int i = 0; i < 4; ++i)
    {
        fInHodoAcc[i] = false;
        fInChamberAcc[i] = false;
    }

    fHitIDs.clear();
}

void DPMCTrack::addHit(DPMCHit& hit)
{
    //add the hit ID
    fHitIDs.push_back(hit.fHitID);

    //assign acceptance bit
    if(hit.fDetectorID == 25 || hit.fDetectorID == 26)
    {
        fInHodoAcc[0] = true;
    }
    else if(hit.fDetectorID == 31 || hit.fDetectorID == 32)
    {
        fInHodoAcc[1] = true;
    }
    else if(hit.fDetectorID == 33 || hit.fDetectorID == 34)
    {
        fInHodoAcc[2] = true;
    }
    else if(hit.fDetectorID == 39 || hit.fDetectorID == 40)
    {
        fInHodoAcc[3] = true;
    }

    if(hit.fDetectorID > 0 && hit.fDetectorID < 7)
    {
        fInChamberAcc[0] = true;
    }
    else if(hit.fDetectorID > 6 && hit.fDetectorID < 13)
    {
        fInChamberAcc[1] = true;
    }
    else if(hit.fDetectorID > 12 && hit.fDetectorID < 25)
    {
        fInChamberAcc[2] = true;
    }
    else if(hit.fDetectorID > 40)
    {
        fInChamberAcc[3] = true;
    }
}

DPMCRawEvent::DPMCRawEvent()
{
    fDimuons = new TClonesArray("DPMCDimuon");
    fNDimuons = 0;

    fTracks = new TClonesArray("DPMCTrack");
    fNTracks = 0;

    fHits = new TClonesArray("DPMCHit");
    fNHits = 0;

    fEvtHeader.fRunID = -1;
    fEvtHeader.fSpillID = -1;
    fEvtHeader.fEventID = -1;
}

DPMCRawEvent::~DPMCRawEvent()
{
    clear();
    delete fDimuons;
    delete fTracks;
    delete fHits;
}

void DPMCRawEvent::clear()
{
    fNDimuons = 0;
    fNTracks = 0;
    fNHits = 0;

    fDimuons->Clear();
    fTracks->Clear();
    fHits->Clear();
}

UInt_t DPMCRawEvent::addDimuon(DPMCDimuon dimuon, Int_t index)
{
    dimuon.fDimuonID = index > 0 ? index : fNDimuons;

    TClonesArray& Dimuons = *fDimuons;
    new(Dimuons[fNDimuons++]) DPMCDimuon(dimuon);

    return dimuon.fDimuonID;
}

UInt_t DPMCRawEvent::addTrack(DPMCTrack track, Int_t index)
{
    track.fTrackID = index > 0 ? index : fNTracks;

    TClonesArray& Tracks = *fTracks;
    new(Tracks[fNTracks++]) DPMCTrack(track);

    return track.fTrackID;
}

UInt_t DPMCRawEvent::addHit(DPMCHit hit, Int_t trackID, Int_t index)
{
    hit.fHitID = index > 0 ? index : fNHits;

    TClonesArray& Hits = *fHits;
    new(Hits[fNHits++]) DPMCHit(hit);

    if(trackID >= 0 && trackID < fNTracks)
    {
        DPMCTrack* track = (DPMCTrack*)fTracks->At(trackID);
        track->fHitIDs.push_back(hit.fHitID);
    }

    return hit.fHitID;
}

void DPMCRawEvent::print()
{
    using namespace std;

    cout << "---------- Run: " << fEvtHeader.fRunID << " Spill " << fEvtHeader.fSpillID << " Event " << fEvtHeader.fEventID << " weight " << fEvtHeader.fSigWeight << "----------" << endl;
    cout << "---------- has " << fNDimuons << " dimuons." << endl;
    for(int i = 0; i < fNDimuons; ++i)
    {
        cout << *(DPMCDimuon*)(fDimuons->At(i)) << endl;
    }

    cout << "---------- has " << fNTracks << " tracks." << endl;
    for(int i = 0; i < fNTracks; ++i)
    {
        cout << *(DPMCTrack*)(fTracks->At(i)) << endl;
    }

    cout << "---------- has " << fNHits << " hits." << endl;
    for(int i = 0; i < fNHits; ++i)
    {
        cout << *(DPMCHit*)(fHits->At(i)) << endl;
    }
    cout << "----------------- END ------------------" << endl;
}

std::ostream& operator << (std::ostream& os, const DPMCDimuon& dimuon)
{
    os << "Dimuon " << dimuon.fDimuonID << " mass: " << dimuon.fMass << "GeV x1 " << dimuon.fx1 << " x2 " << dimuon.fx2 << "\n"
       << "   vertex: (" << dimuon.fVertex.X() << ", " << dimuon.fVertex.Y() << ", " << dimuon.fVertex.Z() << ")\n"
       << "   mu+ ID: " << dimuon.fPosTrackID << " (" << dimuon.fPosMomentum.X() << ", " << dimuon.fPosMomentum.Y() << ", " << dimuon.fPosMomentum.Z() << ") GeV\n"
       << "   mu- ID: " << dimuon.fNegTrackID << " (" << dimuon.fNegMomentum.X() << ", " << dimuon.fNegMomentum.Y() << ", " << dimuon.fNegMomentum.Z() << ") GeV\n"
       << "   " << (dimuon.fAccepted ? "In Acceptance" : " NOT In Acceptance");
    return os;
}

std::ostream& operator << (std::ostream& os, const DPMCTrack& track)
{
    os << "Track " << track.fTrackID << " PDG " << track.fPDGCode << " generated by " << track.fParentID << " in " << track.fProcess << "\n"
       << "   start point (" << track.fInitialPos.X() << ", " << track.fInitialPos.Y() << ", " << track.fInitialPos.Z() << ") cm, "
       << " (" << track.fInitialMom.X() << ", " << track.fInitialMom.Y() << ", " << track.fInitialMom.Z() << ") GeV \n"
       << "   end   point (" << track.fFinalPos.X() << ", " << track.fFinalPos.Y() << ", " << track.fFinalPos.Z() << ") cm, "
       << " (" << track.fFinalMom.X() << ", " << track.fFinalMom.Y() << ", " << track.fFinalMom.Z() << ") GeV";
    return os;
}

std::ostream& operator << (std::ostream& os, const DPMCHit& hit)
{
    os << "Hit " << hit.fHitID << " comes from track " << hit.fTrackID << "\n"
       << "   on detector " << hit.fDetectorID << " element " << hit.fElementID << " with distance to center = " << hit.fDriftDistance << "\n"
       << "   real position = (" << hit.fPosition.X() << ", " << hit.fPosition.Y() << ", " << hit.fPosition.Z() << ") cm\n"
       << "   real momentum = (" << hit.fMomentum.X() << ", " << hit.fMomentum.Y() << ", " << hit.fMomentum.Z() << ") GeV";
    return os;
}

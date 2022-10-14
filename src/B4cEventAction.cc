//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// 
/// \file B4cEventAction.cc
/// \brief Implementation of the B4cEventAction class

#include "B4cEventAction.hh"
#include "B4cCalorimeterSD.hh"
#include "B4cCalorHit.hh"
#include "B4Analysis.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

#include "G4SystemOfUnits.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cEventAction::B4cEventAction(G4String output, G4int RunNumber)
 : G4UserEventAction(),
   fAbsHCID(-1),
   fGapHCID(-1)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cEventAction::~B4cEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cCalorHitsCollection* 
B4cEventAction::GetHitsCollection(G4int hcID,
                                  const G4Event* event) const
{
  auto hitsCollection 
    = static_cast<B4cCalorHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));
  
  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("B4cEventAction::GetHitsCollection()",
      "MyCode0003", FatalException, msg);
  }         

  return hitsCollection;
}    

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::PrintEventStatistics(
                              G4double absoEdep, G4double absoTrackLength,
                              G4double gapEdep, G4double gapTrackLength) const
{
  // print event statistics
  G4cout
     << "   Absorber: total energy: " 
     << std::setw(7) << G4BestUnit(absoEdep, "Energy")
     << "       total track length: " 
     << std::setw(7) << G4BestUnit(absoTrackLength, "Length")
     << G4endl
     << "        Gap: total energy: " 
     << std::setw(7) << G4BestUnit(gapEdep, "Energy")
     << "       total track length: " 
     << std::setw(7) << G4BestUnit(gapTrackLength, "Length")
     << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::BeginOfEventAction(const G4Event* /*event*/)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::EndOfEventAction(const G4Event* event)
{  
  // Get hits collections IDs (only once)
  if ( fAbsHCID == -1 ) {
    fAbsHCID 
      = G4SDManager::GetSDMpointer()->GetCollectionID("AbsorberHitsCollection");
    fGapHCID 
      = G4SDManager::GetSDMpointer()->GetCollectionID("GapHitsCollection");
  }

  // Get hits collections
  auto absoHC = GetHitsCollection(fAbsHCID, event);

  // Get hit with total values
  auto absoHit = (*absoHC)[absoHC->entries()-1];
 
  // Print per event (modulo n)
  //
  auto eventID = event->GetEventID();
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
    G4cout << "---> End of event: " << eventID << G4endl;     

  }  
  
  // Fill histograms, ntuple
  //

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();
 
  G4HCofThisEvent* HCTE = static_cast<G4HCofThisEvent*>( event -> GetHCofThisEvent() );

  G4SDManager* sdManager = G4SDManager::GetSDMpointer(); 
  G4int MTID = sdManager->GetCollectionID("AbsorberHitsCollection");  
  B4cCalorHitsCollection* HC = static_cast<B4cCalorHitsCollection*>(HCTE->GetHC(MTID));

  G4int evt = event -> GetEventID();
  G4int nhits = HC -> entries();

  for (G4int i=0; i<nhits; i++) {

    B4cCalorHit* hit = (*HC)[i];

    G4int trkid = hit -> GetTrackID();
    G4String particle = hit -> GetParticle();
    G4ThreeVector position = hit -> GetPosition();
    G4double mom = hit -> GetMomentum();
    G4double thX = hit -> GetThX();
    G4double thY = hit -> GetThY();
    G4String processname = hit -> GetProccess();
    G4int parent = hit -> GetParent();
    G4String creatorproc = hit -> GetCreatorproc();

    analysisManager -> FillNtupleIColumn(0, evt);
    analysisManager -> FillNtupleIColumn(1, nhits);
    analysisManager -> FillNtupleIColumn(2, trkid);
    analysisManager -> FillNtupleSColumn(3, particle);
    analysisManager -> FillNtupleDColumn(4, position.x()/cm);
    analysisManager -> FillNtupleDColumn(5, position.y()/cm);
    analysisManager -> FillNtupleDColumn(6, position.z()/cm);
    analysisManager -> FillNtupleDColumn(7, mom / GeV);
    analysisManager -> FillNtupleDColumn(8, thX);
    analysisManager -> FillNtupleDColumn(9, thY);
    analysisManager -> FillNtupleSColumn(10, processname);
    analysisManager -> FillNtupleIColumn(11, parent);
    analysisManager -> FillNtupleSColumn(12, creatorproc);

    analysisManager -> AddNtupleRow();

  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

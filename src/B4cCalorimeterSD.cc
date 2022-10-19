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
/// \file B4cCalorimeterSD.cc
/// \brief Implementation of the B4cCalorimeterSD class

#include "B4cCalorimeterSD.hh"
#include "B4cCalorHit.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4HCtable.hh"
#include "G4VTouchable.hh"
#include "G4SystemOfUnits.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cCalorimeterSD::B4cCalorimeterSD(
                            const G4String& name, 
                            const G4String& hitsCollectionName,
                            G4int nofCells)
 : G4VSensitiveDetector(name),
   fHitsCollection(nullptr),
   fNofCells(nofCells)
{
  collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cCalorimeterSD::~B4cCalorimeterSD() 
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cCalorimeterSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection
  fHitsCollection 
    = new B4cCalorHitsCollection(SensitiveDetectorName, collectionName[0]); 

  // Add this collection in hce
  auto hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool B4cCalorimeterSD::ProcessHits(G4Step* step, 
                                     G4TouchableHistory*)
{  
  B4cCalorHit* newHit = new B4cCalorHit();

  G4ThreeVector pre_position = step -> GetPreStepPoint() -> GetPosition(); // position in mm
  G4ThreeVector displace = step -> GetDeltaPosition();


  G4Track* track = step -> GetTrack();

  G4int trkid = track -> GetTrackID();
  G4String particle = track -> GetDynamicParticle() -> GetParticleDefinition() -> GetParticleName();
  G4int parent = track -> GetParentID();

  G4ThreeVector momentum = step -> GetPreStepPoint() -> GetMomentum();
  G4double total_momentum = momentum.mag();
  G4double thX = atan(displace.x() / displace.z());
  G4double thY = atan(displace.y() / displace.z());

  const G4VProcess* process = step -> GetPostStepPoint() -> GetProcessDefinedStep();
  G4String processname = process -> GetProcessName();

  G4String creatorProcName;
  if (parent > 0) {
    creatorProcName = step -> GetTrack() -> GetCreatorProcess() -> GetProcessName();
  } else {
    creatorProcName = "";
  }



  // set CalorHit
  newHit -> SetTrackID(trkid);
  newHit -> SetParticle(particle);
  newHit -> SetPosition(pre_position);
  newHit -> SetMomentum(total_momentum);
  newHit -> SetThX(thX);
  newHit -> SetThY(thY);
  newHit -> SetProcess(processname);
  newHit -> SetParent(parent);
  newHit -> SetCreatorproc(creatorProcName);

  fHitsCollection -> insert(newHit);
 
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cCalorimeterSD::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) { 
     auto nofHits = fHitsCollection->entries();
     G4cout
       << G4endl 
       << "-------->Hits Collection: in this event they are " << nofHits 
       << " hits in the tracker chambers: " << G4endl;
     for ( std::size_t i=0; i<nofHits; ++i ) (*fHitsCollection)[i]->Print();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

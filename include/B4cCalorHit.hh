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
/// \file B4cCalorHit.hh
/// \brief Definition of the B4cCalorHit class

#ifndef B4cCalorHit_h
#define B4cCalorHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Threading.hh"

/// Calorimeter hit class
///
/// It defines data members to store the the energy deposit and track lengths
/// of charged particles in a selected volume:
/// - fEdep, fTrackLength

class B4cCalorHit : public G4VHit
{
  public:
    B4cCalorHit();
    B4cCalorHit(const B4cCalorHit&);
    virtual ~B4cCalorHit();

    // operators
    const B4cCalorHit& operator=(const B4cCalorHit&);
    G4bool operator==(const B4cCalorHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw() {}
    virtual void Print();

    // methods to handle data
    void Add(G4double de, G4double dl);

    // SET & GET methods

    void SetTrackID(G4int trkid) { this -> m_trkid = trkid; }
    G4int GetTrackID() { return this -> m_trkid; }

    void SetParticle(G4String particle) { this -> m_particle = particle; }
    G4String GetParticle() { return this -> m_particle; }

    void SetPosition(G4ThreeVector position) { this -> m_position = position; }
    G4ThreeVector GetPosition() { return this -> m_position; }

    void SetMomentum(G4double mom) { this -> m_mom = mom; }
    G4double GetMomentum() { return this -> m_mom; }

    void SetThX(G4double thX) { this -> m_thX = thX; }
    G4double GetThX() { return this -> m_thX; }
    
    void SetThY(G4double thY) { this -> m_thY = thY; }
    G4double GetThY() { return this -> m_thY; }

    void SetProcess(G4String proc) { this -> m_process = proc; } 
    G4String GetProccess() { return this -> m_process; }

    void SetParent(G4int parent) { this -> m_parent = parent; }
    G4int GetParent() { return this -> m_parent; }

    void SetCreatorproc(G4String creatorproc) { this -> m_creatorproc = creatorproc; }
    G4String GetCreatorproc() { return this -> m_creatorproc; }

      
  private:
    G4int m_trkid;
    G4String m_particle;
    G4ThreeVector m_position;
    G4double m_mom;
    G4double m_thX;
    G4double m_thY;
    G4String m_process;
    G4int m_parent;
    G4String m_creatorproc;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using B4cCalorHitsCollection = G4THitsCollection<B4cCalorHit>;

extern G4ThreadLocal G4Allocator<B4cCalorHit>* B4cCalorHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* B4cCalorHit::operator new(size_t)
{
  if (!B4cCalorHitAllocator) {
    B4cCalorHitAllocator = new G4Allocator<B4cCalorHit>;
  }
  void *hit;
  hit = (void *) B4cCalorHitAllocator->MallocSingle();
  return hit;
}

inline void B4cCalorHit::operator delete(void *hit)
{
  if (!B4cCalorHitAllocator) {
    B4cCalorHitAllocator = new G4Allocator<B4cCalorHit>;
  }
  B4cCalorHitAllocator->FreeSingle((B4cCalorHit*) hit);
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

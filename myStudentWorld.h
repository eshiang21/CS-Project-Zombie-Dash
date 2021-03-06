// A possible interface for StudentWorld.  You may use all, some, or none
// of this, as you wish.

// One of the design goals is to reduce the coupling between StudentWorld.h
// and Actor.h as much as possible.  Notice that in fact, StudentWorld.h
// does not need to include Actor.h at all, and knows only two names in the
// Actor hierarchy.

#ifndef STUDENTWORLD_INCLUDED
#define STUDENTWORLD_INCLUDED

#include "GameWorld.h"
#include <string>
#include "Level.h"

class Actor;
class Penelope;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    virtual ~StudentWorld();
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    // Add an actor to the world.
    void addActor(Actor* a);
    
    // Record that one more citizen on the current level is gone (exited,
    // died, or turned into a zombie).
    void recordCitizenGone();
    
    // Indicate that the player has finished the level if all citizens
    // are gone.
    void recordLevelFinishedIfAllCitizensGone();
    
    // For each actor overlapping a, activate a if appropriate.
    void activateOnAppropriateActors(Actor* a);
    
    bool withinActivatedDist(Actor* a, Actor* b) const;
    bool overlap(double aColL, double aRowB, double pColL, double pRowB) const;
    
    // Is an agent blocked from moving to the indicated location?
    bool isAgentMovementBlockedAt(Actor* a, double x, double y) const;
    
    
    int getPenX() const;
    int getPenY() const;
    void decrCitizenCount();
    void setGameStatText();
    void loadLevel();
    
    
    
    // Is creation of a flame blocked at the indicated location?
    bool isFlameBlockedAt(double x, double y) const;
    
    // Is there something at the indicated location that might cause a
    // zombie to vomit (i.e., a human)?
    bool isZombieVomitTriggerAt(double x, double y) const;
    
    // Return true if there is a living human, otherwise false.  If true,
    // otherX, otherY, and distance will be set to the location and distance
    // of the human nearest to (x,y).
    bool locateNearestVomitTrigger(double x, double y, double& otherX, double& otherY, double& distance);
    /**
    // Return true if there is a living zombie or Penelope, otherwise false.
    // If true, otherX, otherY, and distance will be set to the location and
    // distance of the one nearest to (x,y), and isThreat will be set to true
    // if it's a zombie, false if a Penelope.
    bool locateNearestCitizenTrigger(double x, double y, double& otherX, double& otherY, double& distance, bool& isThreat) const;
    **/
    // Return true if there is a living zombie, false otherwise.  If true,
    // otherX, otherY and distance will be set to the location and distance
    // of the one nearest to (x,y).
    bool locateNearestCitizenThreat(double x, double y, double& otherX, double& otherY, double& distance);
    
    bool ifAllCitizensGone();
    
    double getDistance(Actor* a, double x, double y) const;
    
    bool isAnyObjectAt(double x, double y);
    
    private:
        Penelope * pen;
        bool m_levelDone;
        bool m_gameDone;
        std::vector <Actor*> actorVec;
        int m_nCitizens;
        bool m_goodFormat;
        bool m_levelLoaded;
        bool m_fileNotFound;
    
};

#endif // STUDENTWORLD_INCLUDED

#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include <iostream>
#include <math.h>
#include <iomanip>


using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_levelDone = false;
    m_gameDone = false;
    m_nCitizens = 0;
    m_goodFormat = true;
    m_levelLoaded = false;
    m_fileNotFound = false;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    pen = nullptr;
    loadLevel();
    m_levelLoaded = true;
    if(getLevel() == 99 || m_fileNotFound)
        return GWSTATUS_PLAYER_WON;
    if(m_goodFormat)
        return GWSTATUS_CONTINUE_GAME;
    return GWSTATUS_LEVEL_ERROR;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    vector<Actor*>::iterator p = actorVec.begin();
    while(p!= actorVec.end())
    {
        if(!(*p)->isDead())
        {
            (*p)->doSomething();
            activateOnAppropriateActors(*p);
            if(pen->isDead())
            {
                decLives();
                playSound(SOUND_PLAYER_DIE);
                return GWSTATUS_PLAYER_DIED;
            }
        }
        if(m_levelDone)
        {
            playSound(SOUND_LEVEL_FINISHED);
            m_levelDone = false;
            cleanUp();
            return GWSTATUS_FINISHED_LEVEL;
        }
        p++;
    }
    pen->doSomething();
    
    vector<Actor*>::iterator d = actorVec.begin();
    while(d!= actorVec.end())
    {
        if((*d)->isDead())
        {
            delete *d;
            d = actorVec.erase(d);
        }
        else d++;
    }
    setGameStatText();
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    if(m_levelLoaded)
    {
        delete pen;
        pen = nullptr;
        vector<Actor*>::iterator p = actorVec.begin();
        while(p != actorVec.end())
        {
            delete *p;
            p = actorVec.erase(p);
        }
    }
}

// Add an actor to the world.
void StudentWorld::addActor(Actor* a)
{
    actorVec.push_back(a);
}

// Record that one more citizen on the current level is gone (exited,
// died, or turned into a zombie).
void StudentWorld::recordCitizenGone()
{
    m_nCitizens--;
    std::cerr << "num citizens now: " << m_nCitizens << std::endl;
}

// Indicate that the player has finished the level if all citizens
// are gone.
void StudentWorld::recordLevelFinishedIfAllCitizensGone()
{
    m_levelDone = true;
}

// For each actor overlapping a, activate a if appropriate.
void StudentWorld::activateOnAppropriateActors(Actor* a)
{
    if(a == pen)
    {
        vector<Actor*>::iterator p = actorVec.begin();
        while(p != actorVec.end())
        {
            if(withinActivatedDist(*p, a) && !a->isDead())
            {
                a->activateIfAppropriate(*p);
            }
            p++;
        }
    }
    else
    {
        if(withinActivatedDist(pen, a))
        {
            a->activateIfAppropriate(pen);
        }
        vector<Actor*>::iterator p = actorVec.begin();
        while(p != actorVec.end())
        {
            if(withinActivatedDist(*p, a) && (*p) != a && !a->isDead())
            {
                a->activateIfAppropriate(*p);
            }
            p++;
        }
    }
}

bool StudentWorld::withinActivatedDist(Actor* a, Actor* b) const
{
    return (pow(a->getX() - b->getX(), 2) + pow(a->getY() - b->getY(), 2) <= 100);
}
bool StudentWorld::overlap(double aColL, double aRowB, double pColL, double pRowB) const
{
    double pColR = pColL + SPRITE_WIDTH - 1;
    double pRowT = pRowB + SPRITE_HEIGHT - 1;
    bool rowBounds = false;
    bool colBounds = false;
    double aColR = aColL + SPRITE_WIDTH - 1;
    double aRowT = aRowB + SPRITE_HEIGHT - 1;
    
    if((aColR >= pColL && aColR <= pColR) || (aColL >= pColL && aColL <= pColR))
    {
        colBounds = true;
    }
    if((aRowB >= pRowB && aRowB <= pRowT) || (aRowT >= pRowB && aRowT <= pRowT))
    {
        rowBounds = true;
    }
    return colBounds && rowBounds;
}

// Is an agent blocked from moving to the indicated location?
bool StudentWorld::isAgentMovementBlockedAt(Actor* a, double pColL, double pRowB) const
{
    if(a == pen)
    {
        vector<Actor*>::const_iterator e = actorVec.begin();
        while(e != actorVec.end())
        {
            if((*e)->blocksMovement())
            {
                double eColL = (*e)->getX();
                double eRowB = (*e)->getY();
                if(overlap(pColL, pRowB, eColL, eRowB))
                    return true;
            }
            e++;
        }
    }
    else
    {
        if(overlap(pen->getX(), pen->getY(), pColL, pRowB))
            return true;
        vector<Actor*>::const_iterator e = actorVec.begin();
        while(e != actorVec.end())
        {
            if((*e)->blocksMovement() && (*e)!= a)
            {
                double eColL = (*e)->getX();
                double eRowB = (*e)->getY();
                if(overlap(pColL, pRowB, eColL, eRowB))
                    return true;
            }
            e++;
        }
    }
    return false;
}

int StudentWorld::getPenX() const
{
    return pen->getX();
}
int StudentWorld::getPenY() const
{
    return pen->getY();
}
void StudentWorld::setGameStatText()
{
   ostringstream oss;  // oss is a name of our choosing.
    oss.setf(ios::fixed);
    oss << "Score: ";
    oss.fill('0');
    oss << setw(6) << getScore();
    oss << " Level: " << getLevel() << " Lives: " << getLives() << " Vaccines: " << pen->getNumVaccines() << " Flames: " << pen->getNumFlameCharges() << " Mines: " << pen->getNumLandmines() << " Infected: " << pen->getInfectionCount();
    string header = oss.str();
    //GameWorld::setGameStatText(header);
    GameWorld::setGameStatText("header");
    GameWorld::setGameStatText(header);
    
}

void StudentWorld::loadLevel()
{
    if(getLevel() == 99)
        return;
    else
    {
        m_levelDone = false;
        m_gameDone = false;
        m_nCitizens = 0;
        m_goodFormat = true;
        m_fileNotFound = false;
        Level lev(assetPath());
        ostringstream oss;  // oss is a name of our choosing.
        oss.setf(ios::fixed);
        oss << "level0" << getLevel() << ".txt";
        string levelFile = oss.str();
        Level::LoadResult result = lev.loadLevel(levelFile); if (result == Level::load_fail_file_not_found)
        {m_fileNotFound = true; cerr << "Cannot find " << levelFile << " data file" << endl; } else if (result == Level::load_fail_bad_format)
            {m_goodFormat = false; cerr << "Your level was improperly formatted" << endl; } else if (result == Level::load_success)
                {
                    cerr << "Successfully loaded level" << endl;
                    for(int i = 0; i < 16; i++)
                    {
                        for(int j = 0; j < 16; j++)
                        {
                            Level::MazeEntry ge = lev.getContentsOf(i,j); // level_x=5, level_y=10
                            switch (ge) // so x=80 and y=160
                            {
                                case Level::player:
                                    pen = new Penelope(this, 16*i, 16*j);
                                    break;
                                case Level::wall:
                                    addActor(new Wall(this, 16*i, 16*j));
                                    break;  
                                case Level::exit:
                                    addActor(new Exit(this, 16*i, 16*j));
                                    break;
                                case Level::gas_can_goodie:
                                    addActor(new GasCanGoodie(this, 16*i, 16*j));
                                    break;
                                case Level::landmine_goodie:
                                    addActor(new LandmineGoodie(this, 16*i, 16*j));
                                    break;
                                case Level::vaccine_goodie:
                                    addActor(new VaccineGoodie(this, 16*i, 16*j));
                                    break;
                                case Level::dumb_zombie:
                                    addActor(new DumbZombie(this, 16*i, 16*j));
                                    break;
                                case Level::smart_zombie:
                                    addActor(new SmartZombie(this, 16*i, 16*j));
                                    break;
                                case Level::citizen:
                                    addActor(new Citizen(this, 16*i, 16*j));
                                    m_nCitizens++;
                                    break;
                                case Level::pit:
                                    addActor(new Pit(this, 16*i, 16*j));
                                    break;
                                default:
                                    break;
    
                                 
                    }
                }
            }
        }
    }
}


bool StudentWorld::isFlameBlockedAt(double x, double y) const
{
    vector<Actor*>::const_iterator e = actorVec.begin();
    while(e != actorVec.end())
    {
        if((*e)->blocksFlame() && overlap((*e)->getX(), (*e)->getY(), x, y))
        {
            return true;
        }
        e++;
    }
    return false;
}
bool StudentWorld::locateNearestCitizenThreat(double x, double y, double& otherX, double& otherY, double& distance) 
{
    distance = 10000;
    bool atLeastOneAlive = false;
    vector<Actor*>::const_iterator e = actorVec.begin();
    while(e != actorVec.end())
    {
        if((*e)->threatensCitizens() && !(*e)->isDead())
        {
            atLeastOneAlive = true;
            double zombDist = sqrt(pow((*e)->getX() - x, 2) + pow((*e)->getY() - y, 2));
            if(zombDist < distance)
            {
                otherX = (*e)->getX();
                otherY = (*e)->getY();
                distance = zombDist;
            }
            
        }
        e++;
    }
    return atLeastOneAlive;
}


bool StudentWorld::ifAllCitizensGone()
{
    return (m_nCitizens == 0);
}

// Return true if there is a living human, otherwise false.  If true,
// otherX, otherY, and distance will be set to the location and distance
// of the human nearest to (x,y).
bool StudentWorld::locateNearestVomitTrigger(double x, double y, double& otherX, double& otherY, double& distance)
{
    distance = 10000;
    bool atLeastOneAlive = false;
    vector<Actor*>::const_iterator e = actorVec.begin();
    while(e != actorVec.end())
    {
        if((*e)->triggersZombieVomit() && !(*e)->isDead())
        {
            atLeastOneAlive = true;
            double humanDist = sqrt(pow((*e)->getX() - x, 2) + pow((*e)->getY() - y, 2));
            if(humanDist < distance)
            {
                otherX = (*e)->getX();
                otherY = (*e)->getY();
                distance = humanDist;
            }
            
        }
        e++;
    }
    if(!pen->isDead())
    {
        atLeastOneAlive = true;
        double humanDistP = sqrt(pow(pen->getX() - x, 2) + pow(pen->getY() - y, 2));
        if(humanDistP < distance)
        {
            otherX = pen->getX();
            otherY = pen->getY();
            distance = humanDistP;
        }
    }
    return atLeastOneAlive;
}

// Is there something at the indicated location that might cause a
// zombie to vomit (i.e., a human)?
bool StudentWorld::isZombieVomitTriggerAt(double x, double y) const
{
    vector<Actor*>::const_iterator e = actorVec.begin();
    int randNum = randInt(1, 3);
    while(e != actorVec.end())
    {
        if((*e)->triggersZombieVomit() && !(*e)->isDead())
        {
            if(getDistance((*e), x, y) <= 10 && randNum == 1)
            {
                return true;
            }
            
        }
        e++;
    }
    if(!pen->isDead())
    {
        if(pen->triggersZombieVomit() && !pen->isDead())
        {
            if(getDistance(pen, x, y) <= 10 && randNum == 1)
            {
                return true;
            }
            
        }
    }
    return false;
}


double StudentWorld::getDistance(Actor * a, double x, double y) const
{
    return sqrt(pow(a->getX() - x, 2) + pow(a->getY() - y, 2));
}

bool StudentWorld::isAnyObjectAt(double x, double y)
{
    if(overlap(x, y, getPenX(), getPenY()))
        return true;
    vector<Actor*>::const_iterator e = actorVec.begin();
    while(e != actorVec.end())
    {
        if(overlap(x, y, (*e)->getX(), (*e)->getY()))
            return true;
        e++;
    }
    return false;
}

#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
#include <queue>

// Comment for COMSW4156
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(StudentWorld* w, int imageID, double x, double y, int dir, int depth): GraphObject(imageID, x, y, dir, depth)
{
    m_studentWorld = w;
    m_alive = true;
    m_tick = 0;
}
    
// Is this actor dead?
bool Actor::isDead() const
{
    return !m_alive;
}

// Mark this actor as dead.
void Actor::setDead()
{
    m_alive = false;
}

// Get this actor's world
StudentWorld* Actor::world()
{
    return m_studentWorld;
}

// If this is an activated object, perform its effect on a (e.g., for an
// Exit have a use the exit).
void Actor::activateIfAppropriate(Actor* a)
{
    a->blocksFlame();
}

// If this object uses exits, use the exit.
void Actor::useExitIfAppropriate()
{
}

// If this object can die by falling into a pit or burning, die.
void Actor::dieByFallOrBurnIfAppropriate()
{
}

// If this object can be infected by vomit, get infected.
void Actor::beVomitedOnIfAppropriate()
{
}

// If this object can pick up goodies, pick up g
bool Actor::pickUpGoodieIfAppropriate(Goodie* g)
{
    g->blocksFlame();
    return false;
}

// Does this object block agent movement?
bool Actor::blocksMovement() const
{
    return false;
}

// Does this object block flames?
bool Actor::blocksFlame() const
{
    return false;
}

// Does this object trigger landmines only when they're active?
bool Actor::triggersOnlyActiveLandmines() const
{
    return false;
}

// Can this object cause a zombie to vomit?
bool Actor::triggersZombieVomit() const
{
    return false;
}

// Is this object a threat to citizens?
bool Actor::threatensCitizens() const
{
    return false;
}

// Does this object trigger citizens to follow it or flee it?
bool Actor::triggersCitizens() const
{
    return false;
}

void Actor::increaseTick()
{
    m_tick++;
}

int Actor::getTick()
{
    return m_tick;
}

bool Actor::isParalyzed()
{
    if(getTick() % 2 == 0)
    {
        return true;
    }
    return false;
}




//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------


Wall::Wall(StudentWorld* w, double x, double y):Actor(w, IID_WALL, x, y, right, 0)
{
}
void Wall::doSomething()
{
}
bool Wall::blocksMovement() const
{
    return true;
}
bool Wall::blocksFlame() const
{
    return true;
}



//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

ActivatingObject::ActivatingObject(StudentWorld* w, int imageID, double x, double y, int dir, int depth):Actor(w, imageID, x, y, dir, depth)
{
}



//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Exit::Exit(StudentWorld* w, double x, double y):ActivatingObject(w, IID_EXIT, x, y, right, 1)
{
    
}
void Exit::doSomething()
{
}
void Exit::activateIfAppropriate(Actor* a)
{
    a->useExitIfAppropriate();
}
bool Exit::blocksFlame() const
{
    return true;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------


Goodie::Goodie(StudentWorld* w, int imageID, double x, double y):ActivatingObject(w, imageID, x, y, right, 1)
{

}
void Goodie::activateIfAppropriate(Actor* a)
{
    if(a->pickUpGoodieIfAppropriate(this))
    {
        world()->playSound(SOUND_GOT_GOODIE);
        //--//-- add 50 points
        dieByFallOrBurnIfAppropriate();
    }
    
}
void Goodie::dieByFallOrBurnIfAppropriate()
{
    setDead();
}


VaccineGoodie::VaccineGoodie(StudentWorld* w, double x, double y):Goodie(w, IID_VACCINE_GOODIE, x, y)
{
}
void VaccineGoodie::doSomething()
{
    if(isDead())
        return;
}
void VaccineGoodie::pickUp(Penelope* p)
{
    p->increaseVaccines();
}


GasCanGoodie::GasCanGoodie(StudentWorld* w, double x, double y):Goodie(w, IID_GAS_CAN_GOODIE, x, y)
{
}
void GasCanGoodie::doSomething()
{

}
void GasCanGoodie::pickUp(Penelope* p)
{
    p->increaseFlameCharges();
}


LandmineGoodie::LandmineGoodie(StudentWorld* w, double x, double y):Goodie(w, IID_LANDMINE_GOODIE, x, y)
{
}
void LandmineGoodie::doSomething()
{
}
void LandmineGoodie::pickUp(Penelope* p)
{
    p->increaseLandmines();
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Agent::Agent(StudentWorld* w, int imageID, double x, double y, int dir):Actor(w, imageID, x, y, dir, 0)
{
}

bool Agent::blocksMovement() const
{
    return true;
}
bool Agent::triggersOnlyActiveLandmines() const
{
    return true;
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------


Human::Human(StudentWorld* w, int imageID, double x, double y):Agent(w, imageID, x, y, right)
{
    m_isInfected = false;
    m_infectCount = 0;
}
void Human::beVomitedOnIfAppropriate()
{
    if(!isInfected() && !triggersCitizens())
        world()->playSound(SOUND_CITIZEN_INFECTED);
    m_isInfected = true;
}
bool Human::triggersZombieVomit() const
{
    return true;
}
// Make this human uninfected by vomit.
void Human::clearInfection()
{
    m_infectCount = 0;
    m_isInfected = false;
}
// How many ticks since this human was infected by vomit?
int Human::getInfectionCount() const
{
    return m_infectCount;
}
bool Human::isInfected() const
{
    return m_isInfected;
}
void Human::increaseInfectionCount()
{
    m_infectCount++;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------


Penelope::Penelope(StudentWorld* w, double x, double y):Human(w, IID_PLAYER, x, y)
{
    m_nFlamethrowers = 0;
    m_nLandmines = 0;
    m_nVaccines = 0;
    
}
void Penelope::doSomething()
{
    if(isDead())
        return;
    if(isInfected())
        increaseInfectionCount();
    if(getInfectionCount() == 500)
    {
        setDead();
    }
    int key;
    if(world()->getKey(key))
    {
        switch(key)
        {
            case KEY_PRESS_LEFT:
                evalMove(left);
                break;
            case KEY_PRESS_RIGHT:
                evalMove(right);
                break;
            case KEY_PRESS_UP:
                evalMove(up);
                break;
            case KEY_PRESS_DOWN:
                evalMove(down);
                break;
            case KEY_PRESS_SPACE:
                if(m_nFlamethrowers > 0)
                {
                    world()->playSound(SOUND_PLAYER_FIRE);
                    m_nFlamethrowers--;
                    int i = 1;
                    bool blocked = false;
                    while(i <= 3 && !blocked)
                    {
                        
                        switch(this->getDirection())
                        {
                                
                            case left:
                                if(!world()->isFlameBlockedAt(getX() - i*SPRITE_WIDTH, getY()))
                                {
                                    world()->addActor(new Flame(world(), getX() - i*SPRITE_WIDTH, getY(), right));
                                }
                                else blocked = true;
                                break;
                            case right:
                                if(!world()->isFlameBlockedAt(getX() + i*SPRITE_WIDTH, getY()))
                                {
                                     world()->addActor(new Flame(world(), getX() + i*SPRITE_WIDTH, getY(), right));
                                }
                                else blocked = true;
                                break;
                            case up:
                                if(!world()->isFlameBlockedAt(getX(), getY() + i*SPRITE_HEIGHT))
                                {
                                     world()->addActor(new Flame(world(), getX(), getY() + i*SPRITE_HEIGHT, right));
                                }
                                else blocked = true;
                                break;
                            case down:
                                if(!world()->isFlameBlockedAt(getX(), getY() - i*SPRITE_HEIGHT))
                                {
                                     world()->addActor(new Flame(world(), getX(), getY() - i*SPRITE_HEIGHT, right));
                                }
                                else blocked = true;
                                break;
                        }
                        i++;
                    
                
                    }
                }
                break;
            case KEY_PRESS_ENTER:
                if(m_nVaccines > 0)
                {
                    m_nVaccines--;
                    clearInfection();
                }
                break;
            case KEY_PRESS_TAB:
                if(m_nLandmines > 0)
                {
                    m_nLandmines--;
                    world()->addActor(new Landmine(world(), getX(), getY()));
                }
                break;
        }
    }
}
void Penelope::evalMove(Direction dir)
{
    setDirection(dir);
    switch(dir)
    {
        
        case left:
           
            if(!world()->isAgentMovementBlockedAt(this, getX() - 4, getY()))
            {
                moveTo(getX() - 4, getY());
 
            }
            break;
        case right:
            if(!world()->isAgentMovementBlockedAt(this, getX() + 4, getY()))
            {
                moveTo(getX() + 4, getY());
            }
            break;
        case up:
            if(!world()->isAgentMovementBlockedAt(this, getX(), getY() + 4))
            {
                moveTo(getX(), getY() + 4);
            }
            break;
        case down:
            if(!world()->isAgentMovementBlockedAt(this, getX(), getY() - 4))
            {
                moveTo(getX(), getY() - 4);
            }
            break;
    }
    
}
void Penelope::useExitIfAppropriate()
{
    if(world()->ifAllCitizensGone())
    {
        world()->recordLevelFinishedIfAllCitizensGone();
    }
    
}

void Penelope::dieByFallOrBurnIfAppropriate()
{
    setDead();
}


bool Penelope::pickUpGoodieIfAppropriate(Goodie* g)
{
    world()->increaseScore(50);
    g->pickUp(this);
    return true;
    
}
// Increase the number of vaccines the object has.
void Penelope::increaseVaccines()
{
    m_nVaccines++;
}
// Increase the number of flame charges the object has.
void Penelope::increaseFlameCharges()
{
    m_nFlamethrowers+=5;
}
// Increase the number of landmines the object has.
void Penelope::increaseLandmines()
{
    m_nLandmines+=2;
}
// How many vaccines does the object have?
int Penelope::getNumVaccines() const
{
    return m_nVaccines;
}
// How many flame charges does the object have?
int Penelope::getNumFlameCharges() const
{
    return m_nFlamethrowers;
}
// How many landmines does the object have?
int Penelope::getNumLandmines() const
{
    return m_nLandmines;
}
bool Penelope::triggersCitizens() const
{
    return true;
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------


Zombie::Zombie(StudentWorld* w,  double x, double y):Agent(w, IID_ZOMBIE, x, y, right)
{
    m_movePlan = 0;
}
bool Zombie::threatensCitizens() const
{
    return true;
}
bool Zombie::triggersCitizens() const
{
    return true;
}

void Zombie::setMovePlan(int num)
{
    m_movePlan = num;
}

int Zombie::getMovePlan()
{
    return m_movePlan;
}

void Zombie::inflictVomit()
{
    switch(getDirection())
    {
        case up:
            if(world()->isZombieVomitTriggerAt(getX(), getY() + SPRITE_HEIGHT))
            {
                world()->playSound(SOUND_ZOMBIE_VOMIT);
                world()->addActor(new Vomit(world(), getX(), getY() + SPRITE_HEIGHT));
            }
            break;
        case down:
            if(world()->isZombieVomitTriggerAt(getX(), getY() - SPRITE_HEIGHT))
            {
                world()->playSound(SOUND_ZOMBIE_VOMIT);
                world()->addActor(new Vomit(world(), getX(), getY() - SPRITE_HEIGHT));
            }
            break;
            
        case left:
            if(world()->isZombieVomitTriggerAt(getX() - SPRITE_WIDTH, getY()))
            {
                world()->playSound(SOUND_ZOMBIE_VOMIT);
                world()->addActor(new Vomit(world(), getX() - SPRITE_WIDTH, getY()));
            }
            break;
            
        case right:
            if(world()->isZombieVomitTriggerAt(getX() + SPRITE_WIDTH, getY()))
            {
                world()->playSound(SOUND_ZOMBIE_VOMIT);
                world()->addActor(new Vomit(world(), getX() + SPRITE_WIDTH, getY()));
            }
            break;
    }
 
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

DumbZombie::DumbZombie(StudentWorld* w,  double x, double y):Zombie(w, x, y)
{
    
}

void DumbZombie::doSomething()
{
    increaseTick();
    if(isParalyzed())
        return;
    inflictVomit();
    if(getMovePlan() == 0)
    {
        setMovePlan(randInt(3, 10));
        this->setDirection(randInt(0, 3) * 90);
    }
        switch(this->getDirection())
        {
                
            case left:
                
                if(!world()->isAgentMovementBlockedAt(this, getX() - 1, getY()))
                {
                    moveTo(getX() - 1, getY());
                    setMovePlan(getMovePlan() - 1);
                }
                else setMovePlan(0);
                break;
            case right:
                if(!world()->isAgentMovementBlockedAt(this, getX() + 1, getY()))
                {
                    moveTo(getX() + 1, getY());
                    setMovePlan(getMovePlan() - 1);
                }
                else setMovePlan(0);
                break;
            case up:
                if(!world()->isAgentMovementBlockedAt(this, getX(), getY() + 1))
                {
                    moveTo(getX(), getY() + 1);
                    setMovePlan(getMovePlan() - 1);
                }
                else setMovePlan(0);
                break;
            case down:
                if(!world()->isAgentMovementBlockedAt(this, getX(), getY() - 1))
                {
                    moveTo(getX(), getY() - 1);
                    setMovePlan(getMovePlan() - 1);
                }
                else setMovePlan(0);
                break;
        }
        
    
}
void DumbZombie::dieByFallOrBurnIfAppropriate()
{
    world()->increaseScore(1000); //SCORE//
    world()->playSound(SOUND_ZOMBIE_DIE); //SOUND//
    int randNum = randInt(1, 10);
    //std::cerr << "REACHED END" << randNum << std::endl;
    if(randNum == 1)
    {
        int randDirection = randInt(0,3)*90;
        switch(randDirection)
        {
            case up:
                //std::cerr << "REACHED UP" << std::endl;
                if(!world()->isAnyObjectAt(getX(), getY() + SPRITE_HEIGHT))
                {
                    world()->addActor(new VaccineGoodie(world(), getX(), getY() + SPRITE_HEIGHT));
                    //std::cerr << "YAYYY GOODIE" << std::endl;
                }
                break;
            case down:
                //std::cerr << "REACHED DOWN" << std::endl;
                if(!world()->isAnyObjectAt(getX(), getY() - SPRITE_HEIGHT))
                {
                    world()->addActor(new VaccineGoodie(world(), getX(), getY() - SPRITE_HEIGHT));
                    //std::cerr << "YAYYY GOODIE" << std::endl;
                }
                break;
            case left:
                //std::cerr << "REACHED LEFT" << std::endl;
                if(!world()->isAnyObjectAt(getX() - SPRITE_WIDTH, getY()))
                {
                    world()->addActor(new VaccineGoodie(world(), getX() - SPRITE_WIDTH, getY()));
                    //std::cerr << "YAYYY GOODIE" << std::endl;
                }
                break;
            case right:
                //std::cerr << "REACHED RIGHT" << std::endl;
                if(!world()->isAnyObjectAt(getX() + SPRITE_WIDTH, getY()))
                {
                    world()->addActor(new VaccineGoodie(world(), getX() + SPRITE_WIDTH, getY()));
                    //std::cerr << "YAYYY GOODIE" << std::endl;
                }
                break;
        }
    }
    setDead();
    //--//-- increase score 1000
    //introduce vaccine goodie
    
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Flame::Flame(StudentWorld* w, double x, double y, int dir):ActivatingObject(w, IID_FLAME, x, y, dir, 0)
{
}
    
void Flame::doSomething()
{
    if(isDead())
        return;
    increaseTick();
    if(getTick() == 2)
        setDead();
}
void Flame::activateIfAppropriate(Actor* a)
{
    if(!a->blocksFlame() && !a->isDead())
        a->dieByFallOrBurnIfAppropriate();
}
bool Flame::triggersOnlyActiveLandmines()
{
    return true;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Citizen::Citizen(StudentWorld* w,  double x, double y):Human(w, IID_CITIZEN, x, y)
{
    
}
void Citizen::doSomething()
{
    increaseTick();
    if(isDead())
        return;
    if(isInfected())
        increaseInfectionCount();
    
    if(getInfectionCount() == 500)
    {
        world()->increaseScore(-1000);
        world()->recordCitizenGone();
        setDead();
        //--//-- decr 1000 points
        world()->playSound(SOUND_ZOMBIE_BORN);
        int smartOrDumb = randInt(1, 10);
        
        if(smartOrDumb > 3)
            world()->addActor(new DumbZombie(world(), this->getX(), this->getY()));
        else world()->addActor(new SmartZombie(world(), this->getX(), this->getY()));
        return;
    }
    if(!isParalyzed())
    {
        bool skipToZombieCheck = false;
        double penDist = sqrt(pow(world()->getPenX() - getX(), 2) + pow(world()->getPenY() - getY(), 2));
        double otherX = -1;
        double otherY = -1;
        double zomDist = 1000;
        world()->locateNearestCitizenThreat(getX(), getY(), otherX, otherY, zomDist);
        //std::cerr << zomDist << " ";
        if(zomDist > penDist || zomDist == -5)
        {
            if(penDist <= 80)
            {
                //SAME ROW OR COL
                double diffCol = world()->getPenX()- getX();
                double diffRow = world()->getPenY()- getY();
                if(diffRow == 0 || diffCol == 0)
                {
                    if(diffRow == 0) // same row
                    {
                        if(diffCol > 0) // pen to right move right
                        {
                            if(moveTowardsPenelopeIfPossible(right))
                                return;
                            else skipToZombieCheck = true;
                        }
                        else // pen to left move left
                        {
                            if(moveTowardsPenelopeIfPossible(left))
                                return;
                            else skipToZombieCheck = true;
                        }
                    }
                    else if(diffCol == 0)
                    {
                        if(diffRow > 0) // pen up move up
                        {
                            if(moveTowardsPenelopeIfPossible(up))
                                return;
                            else skipToZombieCheck = true;
                        }
                        else // pen down move down
                        {
                            if(moveTowardsPenelopeIfPossible(down))
                                return;
                            else skipToZombieCheck = true;
                        }
                    }
                }
                //diff row & col
                if(!skipToZombieCheck)
                {
                    int randDirection = randInt(1, 2);
                    
                    //pen right and up
                    if(diffCol > 0 && diffRow > 0)
                    {
                        switch(randDirection)
                        {
                            case 1: // right
                                if(moveTowardsPenelopeIfPossible(right))
                                    return;
                                else if(moveTowardsPenelopeIfPossible(up))
                                    return;
                                break;
                            case 2: // up
                                if(moveTowardsPenelopeIfPossible(up))
                                    return;
                                else if(moveTowardsPenelopeIfPossible(right))
                                    return;
                                break;
                        }
                    }
                    //pen down and right
                    else if(diffCol > 0 && diffRow < 0)
                    {
                        switch(randDirection)
                        {
                            case 1: // right
                                if(moveTowardsPenelopeIfPossible(right))
                                    return;
                                else if(moveTowardsPenelopeIfPossible(down))
                                    return;
                                break;
                            case 2: // down
                                if(moveTowardsPenelopeIfPossible(down))
                                    return;
                                else if(moveTowardsPenelopeIfPossible(right))
                                    return;
                                break;
                        }
                    }
                    //pen up and left
                    else if(diffCol < 0 && diffRow > 0)
                    {
                        switch(randDirection)
                        {
                            case 1: // left
                                if(moveTowardsPenelopeIfPossible(left))
                                    return;
                                else if(moveTowardsPenelopeIfPossible(up))
                                    return;
                                break;
                            case 2: // up
                                if(moveTowardsPenelopeIfPossible(up))
                                    return;
                                else if(moveTowardsPenelopeIfPossible(left))
                                    return;
                                break;
                        }
                    }
                    //pen down and left
                    else if(diffCol < 0 && diffRow < 0)
                    {
                        switch(randDirection)
                        {
                            case 1: // left
                                if(moveTowardsPenelopeIfPossible(left))
                                    return;
                                else if(moveTowardsPenelopeIfPossible(down))
                                    return;
                                break;
                            case 2: // down
                                if(moveTowardsPenelopeIfPossible(down))
                                    return;
                                else if(moveTowardsPenelopeIfPossible(left))
                                    return;
                                break;
                        }
                    }
                }
            }
        }
                
            
        if(zomDist <= 80)
        {
            std::queue<double> distances;
            double upDist = zomDist;
            double downDist = zomDist;
            double leftDist = zomDist;
            double rightDist = zomDist;
            
            //std::cerr << "ZombieDist: " << zomDist << std::endl;
            //std::cerr << "otherX: " << otherX << " otherY: " << otherY << std::endl;
            if(!world()->isAgentMovementBlockedAt(this, getX(), getY() + 2))
            {
                world()->locateNearestCitizenThreat(getX(), getY() + 2, otherX, otherY, upDist);
                //std::cerr << " Going up dist: "<< upDist << std::endl;
            }
            if(!world()->isAgentMovementBlockedAt(this, getX(), getY() - 2))
            {
                world()->locateNearestCitizenThreat(getX(), getY() - 2, otherX, otherY, downDist);
                //std::cerr << " Going down dist: "<< downDist << std::endl;
            }
            if(!world()->isAgentMovementBlockedAt(this, getX() - 2, getY()))
            {
                world()->locateNearestCitizenThreat(getX() - 2, getY(), otherX, otherY, leftDist);
                //std::cerr << " Going left dist: "<< leftDist << std::endl;
            }
            if(!world()->isAgentMovementBlockedAt(this, getX() + 2, getY()))
            {
                world()->locateNearestCitizenThreat(getX() + 2, getY(), otherX, otherY, rightDist);
                //std::cerr << " Going right dist: "<< rightDist << std::endl;
            }
            if(rightDist <= zomDist && leftDist <= zomDist && upDist <= zomDist && downDist <= zomDist)
                return;
            distances.push(upDist);
            distances.push(downDist);
            distances.push(leftDist);
            distances.push(rightDist);

            int maxDex = 0;
            double max = distances.front();
            distances.pop();
            int index = 1;
            while(!distances.empty())
            {
                double dist = distances.front();
                distances.pop();
                if(dist > max)
                {
                    max = dist;
                    maxDex = index;
                }
                index++;
            }
            
            switch(maxDex)
            {
                case 0: // up
                    setDirection(up);
                    //std::cerr << " Going UP" << std::endl << std::endl;
                    moveTo(getX(), getY() + 2);
                    break;
                case 1: // down
                    setDirection(down);
                    //std::cerr << " Going DOWN" << std::endl << std::endl;
                    moveTo(getX(), getY() - 2);
                    break;
                case 2: // left
                    setDirection(left);
                    //std::cerr << " Going LEFT" << std::endl << std::endl;
                    moveTo(getX() - 2, getY());
                    break;
                case 3: // right
                    setDirection(right);
                    //std::cerr << " Going RIGHT" << std::endl << std::endl;
                    moveTo(getX() + 2, getY());
                    break;
            }

            
        }
    }
}
void Citizen::useExitIfAppropriate()
{
    if(isDead())
        return;
    world()->increaseScore(500);
    world()->playSound(SOUND_CITIZEN_SAVED);
    world()->recordCitizenGone();
    //std::cerr << "USED EXIT YAY" << std::endl;
    setDead();
}
void Citizen::dieByFallOrBurnIfAppropriate()
{
    world()->increaseScore(-1000);
    if(isDead())
        return;
    world()->playSound(SOUND_CITIZEN_DIE);
    world()->recordCitizenGone();
    //std::cerr << "OH NUUUUUUUU" << std::endl;
    setDead();
}

bool Citizen::moveTowardsPenelopeIfPossible(Direction dir)
{
    switch(dir)
    {
        case up:
            if(!world()->isAgentMovementBlockedAt(this, getX(), getY() + 2))
            {
                setDirection(up);
                moveTo(getX(), getY() + 2);
                return true;
            }
            return false;
        case down:
            if(!world()->isAgentMovementBlockedAt(this, getX(), getY() - 2))
            {
                setDirection(down);
                moveTo(getX(), getY() - 2);
                return true;
            }
            return false;
        case left:
            if(!world()->isAgentMovementBlockedAt(this, getX() - 2, getY()))
            {
                setDirection(left);
                moveTo(getX() - 2, getY());
                return true;
            }
            return false;
        case right:
            if(!world()->isAgentMovementBlockedAt(this, getX() + 2, getY()))
            {
                setDirection(right);
                moveTo(getX() + 2, getY());
                return true;
            }
            return false;
    }
    return false;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

SmartZombie::SmartZombie(StudentWorld* w,  double x, double y):Zombie(w, x, y)
{
    
}
void SmartZombie::doSomething()
{
    double otherX;
    double otherY;
    double distance;
    increaseTick();
    if(isParalyzed())
        return;
    inflictVomit();
    if(getMovePlan() == 0)
    {
        setMovePlan(randInt(3, 10));
        if(world()->locateNearestVomitTrigger(getX(), getY(), otherX, otherY, distance))
        {
            //std::cerr << distance << " ";
            if(distance > 80)
            {
                setDirection(randInt(0, 3) * 90);
            }
            else
            {
                double diffCol = otherX - getX();
                double diffRow = otherY - getY();
                if(diffRow == 0 || diffCol == 0)
                {
                    if(diffRow == 0) // same row
                    {
                        if(diffCol > 0) // pen to right move right
                                setDirection(right);
                        else // pen to left move left
                                setDirection(left);
                    }
                    else
                    {
                        if(diffRow > 0)
                            setDirection(up);
                        else // pen down move down
                            setDirection(down);
                    }
                }
                //diff row & col
                else
                {
                    int randDirection = randInt(1, 2);
                    //pen right and up
                    if(diffCol > 0 && diffRow > 0)
                    {
                        switch(randDirection)
                        {
                            case 1: // right
                                    setDirection(up);
                                break;
                            case 2: // up

                                    setDirection(right);
                                break;
                        }
                    }
                    //pen down and right
                    else if(diffCol > 0 && diffRow < 0)
                    {
                        switch(randDirection)
                        {
                            case 1: // right
                                    setDirection(right);
                                break;
                            case 2: // down
                                    setDirection(down);
                                break;
                        }
                    }
                    //pen up and left
                    else if(diffCol < 0 && diffRow > 0)
                    {
                        switch(randDirection)
                        {
                            case 1: // left
                                    setDirection(up);
                                break;
                            case 2: // up
                                    setDirection(left);
                                break;
                        }
                    }
                    //pen down and left
                    else if(diffCol < 0 && diffRow < 0)
                    {
                        switch(randDirection)
                        {
                            case 1: // left
                                    setDirection(down);
                                break;
                            case 2: // down
                                    setDirection(left);
                                break;
                        }
                    }
                }
            }
        }
    }
    if(isMoveToHumanPossible(getDirection()))
    {
        setMovePlan(getMovePlan() - 1);
    }
    else setMovePlan(0);
}
        
        
void SmartZombie::dieByFallOrBurnIfAppropriate()
{
    world()->increaseScore(2000); //SCORE//
    world()->playSound(SOUND_ZOMBIE_DIE);
    setDead();
}

bool SmartZombie::isMoveToHumanPossible(Direction dir)
{
    switch(dir)
    {
        case up:
            if(!world()->isAgentMovementBlockedAt(this, getX(), getY() + 1))
            {
                moveTo(getX(), getY() + 1);
                return true;
            }
            return false;
        case down:
            if(!world()->isAgentMovementBlockedAt(this, getX(), getY() - 1))
            {
                moveTo(getX(), getY() - 1);
                return true;
            }
            return false;
        case left:
            if(!world()->isAgentMovementBlockedAt(this, getX() - 1, getY()))
            {
                moveTo(getX() - 1, getY());
                return true;
            }
            return false;
        case right:
            if(!world()->isAgentMovementBlockedAt(this, getX() + 1, getY()))
            {
                moveTo(getX() + 1, getY());
                return true;
            }
            return false;
    }
    return false;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------


Vomit::Vomit(StudentWorld* w, double x, double y):ActivatingObject(w, IID_VOMIT, x, y, right, 0)
{
}
void Vomit::doSomething()
{
    increaseTick();
    if(isDead())
        return;
    if(getTick() == 2)
        setDead();
}
void Vomit::activateIfAppropriate(Actor* a)
{
    if(a->triggersZombieVomit())
    {
        a->beVomitedOnIfAppropriate();
    }
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Pit::Pit(StudentWorld* w, double x, double y):ActivatingObject(w, IID_PIT, x, y, right, 0)
{
}
void Pit::doSomething()
{
    if(isDead())
        return;
}
void Pit::activateIfAppropriate(Actor* a)
{
    a->dieByFallOrBurnIfAppropriate();
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Landmine::Landmine(StudentWorld* w, double x, double y):ActivatingObject(w, IID_LANDMINE, x, y, right, 1)
{
    m_setOff = false;
}

void Landmine::doSomething()
{
    if(isDead())
        return;
    increaseTick();
    if(getTick() >= 30)
    {
        m_setOff = true;
    }
    
}
bool Landmine::isSetOff()
{
    return m_setOff;
}
void Landmine::activateIfAppropriate(Actor* actr)
{
    if(isSetOff() && actr->triggersOnlyActiveLandmines())
    {
        landmineExplosion();
        
    }
}
void Landmine::dieByFallOrBurnIfAppropriate()
{
    landmineExplosion();
}
void Landmine::possiblyCreateFlame(double x, double y)
{
    if(!world()->isFlameBlockedAt(x, y))
        world()->addActor(new Flame(world(), x, y, up));
    
}
void Landmine::landmineExplosion()
{
    world()->playSound(SOUND_LANDMINE_EXPLODE);
    if(!isDead())
    {
        setDead();
        double aX = getX();
        double aY = getY();
        //above row
        possiblyCreateFlame(aX - SPRITE_WIDTH, aY + SPRITE_HEIGHT);
        possiblyCreateFlame(aX, aY + SPRITE_HEIGHT);
        possiblyCreateFlame(aX + SPRITE_WIDTH, aY + SPRITE_HEIGHT);
        //row of landmine
        possiblyCreateFlame(aX - SPRITE_WIDTH, aY);
        possiblyCreateFlame(aX, aY);
        possiblyCreateFlame(aX + SPRITE_WIDTH, aY);
        //row below
        possiblyCreateFlame(aX - SPRITE_WIDTH, aY - SPRITE_HEIGHT);
        possiblyCreateFlame(aX, aY - SPRITE_HEIGHT);
        possiblyCreateFlame(aX + SPRITE_WIDTH, aY - SPRITE_HEIGHT);
        world()->addActor(new Pit(world(), aX, aY));
    }
}

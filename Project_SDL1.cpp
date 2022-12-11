#include "Project_SDL1.h"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <numeric>
#include <random>
#include <string>
// Defining a namespace without a name -> Anonymous workspace
// Its purpose is to indicate to the compiler that everything
// inside of it is UNIQUELY used within this source file.
namespace
{
    SDL_Surface* load_surface_for(const std::string& path, SDL_Surface* window_surface_ptr)
    {
        SDL_Surface* loadedSurface = IMG_Load(path.c_str());
        if (loadedSurface == NULL)
            throw std::runtime_error("Unable to load image " + std::string(path.c_str()));
        //Convert surface to screen format
        SDL_Surface* optimizedSurface = SDL_ConvertSurface(loadedSurface, window_surface_ptr->format, 0);
        if (optimizedSurface == NULL)
            throw std::runtime_error("Unable to optimize image!");
        SDL_FreeSurface(loadedSurface);
        return optimizedSurface;
    }
} // namespace

//*****************************************************************************
// ********************************* OBJECT ***********************************
//*****************************************************************************
object::object()
{}
/////////////////////////////////////////////
void object::addPropertie(std::string pPropertie)
{
    this->properties_.push_back(pPropertie);
}
/////////////////////////////////////////////
bool object::hasPropertie(std::string pPropertie)
{
    return (std::find(this->properties_.begin(), this->properties_.end(), pPropertie) != this->properties_.end());
}
/////////////////////////////////////////////
bool object::removePropertie(std::string pPropertie)
{
    std::vector<std::string>::iterator itr = std::find(this->properties_.begin(), this->properties_.end(), pPropertie);
    if (itr != this->properties_.end())
    {
        this->properties_.erase(itr);
        return true;
    }
    return false;
}
//*****************************************************************************
// ***************************** RENDERED OBJECT ******************************
//*****************************************************************************
renderedObject::renderedObject(const std::string& file_path, SDL_Surface* window_surface_ptr, int width, int height, int x, int y) :
    object()
{
    this->image_ptr_ = load_surface_for(file_path, window_surface_ptr);
    this->window_surface_ptr_ = window_surface_ptr;
    this->width_ = width;
    this->height_ = height;
    this->x_ = x;
    this->y_ = y;
}
/////////////////////////////////////////////
int renderedObject::getHeightBox() { return this->height_ * 4 / 5; }
int renderedObject::getWidthBox() { return this->width_ / 2; }
int renderedObject::getXBox() { return this->x_ + (this->width_ - this->getWidthBox()) / 2; }
int renderedObject::getYBox() { return this->y_ + (this->height_ - this->getHeightBox()) / 2; }
int renderedObject::getX() { return this->x_; }
int renderedObject::getY() { return this->y_; }

/////////////////////////////////////////////
int renderedObject::getDistance(renderedObject* pO2)
{
    int xDistance = std::min(abs(this->getXBox() - pO2->getXBox()), abs(this->getXBox() - pO2->getXBox() - pO2->getWidthBox()));
    int yDistance = std::min(abs(this->getYBox() - pO2->getYBox()), abs(this->getYBox() - pO2->getYBox() - pO2->getHeightBox()));
    return (int)std::sqrt(xDistance * xDistance + yDistance * yDistance);
}
/////////////////////////////////////////////
bool renderedObject::overlap(renderedObject* vO2)
{
    return!((this->getXBox() > vO2->getXBox() + vO2->getWidthBox())
        || (this->getXBox() + this->getWidthBox() < vO2->getXBox())
        || (this->getYBox() > vO2->getYBox() + vO2->getHeightBox())
        || (this->getYBox() + this->getHeightBox() < vO2->getYBox()));
}
/////////////////////////////////////////////
void renderedObject::draw()
{
    //La position (et pas la taille) de ce rectangle définie l'endroit ou la surface est collée
    SDL_Rect vRect = { this->x_, this->y_,0, 0 };
    SDL_BlitSurface(this->image_ptr_, NULL, this->window_surface_ptr_, &vRect);
}
//*****************************************************************************
// ****************************** MOVING OBJECT *******************************
//*****************************************************************************
movingObject::movingObject(int totalVelocity)
{
    this->totalVelocity_ = totalVelocity;
    this->setRandomVelocitys();
}
/////////////////////////////////////////////
bool movingObject::canMoveX() { return (this->getXBox() + this->xVelocity_ + this->getWidthBox() < frame_width) && (this->getXBox() + this->xVelocity_ > 0); }
bool movingObject::canMoveY() { return (this->getYBox() + this->yVelocity_ + this->getHeightBox() < frame_height) && (this->getYBox() + this->yVelocity_ > 0); }
/////////////////////////////////////////////
void movingObject::goToward(renderedObject* vMO) { this->goToward(vMO->getXBox(), vMO->getYBox()); }
void movingObject::goToward(int x, int y)
{
    this->xVelocity_ = x - this->getXBox();
    this->yVelocity_ = y - this->getYBox();
    this->adjustVelocitys();
}
/////////////////////////////////////////////
void movingObject::runAway(renderedObject* vMO) { this->runAway(vMO->getXBox(), vMO->getYBox()); }
void movingObject::runAway(int x, int y)
{ 
    this->xVelocity_ = this->getXBox() - x;
    this->yVelocity_ = this->getYBox() - y;  
    this->adjustVelocitys();
}
/////////////////////////////////////////////
void movingObject::setRandomVelocitys()
{
    this->xVelocity_ = (rand() % this->totalVelocity_ * 2) - this->totalVelocity_;
    if (!canMoveX())
        this->xVelocity_ = -this->xVelocity_;
    this->yVelocity_ = (((rand() % 1) * 2) - 1) * (this->totalVelocity_ - abs(this->xVelocity_));
    if (!canMoveY())
        this->yVelocity_ = -this->yVelocity_;
}
/////////////////////////////////////////////
void movingObject::adjustVelocitys()
{
    //Hors map
    while (!canMoveX() && this->xVelocity_ != 0)
        this->xVelocity_ -= (this->xVelocity_ > 0 ? 1 : -1);
    while (!canMoveY() && this->yVelocity_ != 0)
        this->yVelocity_ -= (this->yVelocity_ > 0 ? 1 : -1);
    //Vitesse trop élevé
    while (abs(this->xVelocity_) + abs(this->yVelocity_) > abs(this->totalVelocity_))
    {
        this->xVelocity_ -= (this->xVelocity_ > 0 ? 1 : -1);
        if (abs(this->xVelocity_) + abs(this->yVelocity_) > abs(this->totalVelocity_))
            this->yVelocity_ -= (this->yVelocity_ > 0 ? 1 : -1);
    }
    //Vitesse trop faible
    if (abs(this->xVelocity_) + abs(this->yVelocity_) < abs(this->totalVelocity_))
        this->setRandomVelocitys();
}
/////////////////////////////////////////////
void movingObject::interact(renderedObject* pO2)
{
    if (this->hasPropertie("wolf") && pO2->hasPropertie("prey"))
    {
        if (this->overlap(pO2))
        {
            this->addPropertie("eat");
            pO2->addPropertie("dead");
        } 
        else
            ((wolf*)this)->choosePrey(pO2);
    }
    if (this->hasPropertie("prey") && pO2->hasPropertie("wolf") && this->getDistance(pO2) < 200)
        this->runAway(pO2);
    if (this->hasPropertie("canprocreate") && this->hasPropertie("male")
        && pO2->hasPropertie("canprocreate") && pO2->hasPropertie("female") && this->overlap(pO2))
    {
        this->removePropertie("canprocreate");
        pO2->removePropertie("canprocreate");
        pO2->addPropertie("pregnant");
    }
}
/////////////////////////////////////////////
void movingObject::move()
{
    if (!canMoveX() || !canMoveY())
        this->setRandomVelocitys();
    this->x_ += this->xVelocity_;
    this->y_ += this->yVelocity_;
}
//*****************************************************************************
// ***************************** ANIMATED OBJECT ******************************
//*****************************************************************************
animatedObject::animatedObject(int frameInterval)
{
    this->frameInterval_ = frameInterval;
    this->frameDuration_ = frameInterval;
    this->frameIndex_ = 0;
}
/////////////////////////////////////////////
void animatedObject::setSurfaceMap()
{
    std::map<std::string, std::vector<std::string>> pathMap = this->getPathMap();
    std::map<std::string, std::vector<std::string>>::iterator it;
    for (it = pathMap.begin(); it != pathMap.end(); it++)
    {
        std::string vKey = it->first;
        std::vector<std::string> vPaths = it->second;
        std::vector<SDL_Surface*> vSurfaces = {};
        for (std::string vPath : vPaths)
            vSurfaces.push_back(load_surface_for(vPath, this->window_surface_ptr_));
        this->images_.insert({ vKey,vSurfaces });
    }
}
/////////////////////////////////////////////
void animatedObject::updateFrameDuration()
{
    this->frameDuration_++;
    if (this->frameDuration_ >= this->frameInterval_)
        this->nextFrame();
}
/////////////////////////////////////////////
void animatedObject::nextFrame()
{
    this->frameIndex_++;
    std::string imageKey = this->getImageKey();
    if (this->frameIndex_ >= this->images_.at(imageKey).size())
        this->frameIndex_ = 0;
    this->image_ptr_ = this->images_.at(imageKey)[this->frameIndex_];
    this->frameDuration_ = 0;
}
//*****************************************************************************
// ********************************* SHEPERD **********************************
//*****************************************************************************
int shepherd::ImgW = 49;
int shepherd::ImgH = 49;
shepherd::shepherd(SDL_Surface* window_surface_ptr) :
    renderedObject("media/shepherd.png", window_surface_ptr, shepherd::ImgW, shepherd::ImgH, frame_width / 2, frame_height / 2), movingObject(4)
{
    this->properties_ = { "shepherd" };
}
/////////////////////////////////////////////
void shepherd::update()
{
    this->move();
    this->draw();
}
/////////////////////////////////////////////
void shepherd::move()
{
    const uint8_t* keystate = SDL_GetKeyboardState(0);
    //Horizontal Velocity
    if (keystate[SDL_SCANCODE_LEFT])  
        this->xVelocity_ = -this->totalVelocity_; 
    else if (keystate[SDL_SCANCODE_RIGHT]) 
        this->xVelocity_ = this->totalVelocity_; 
    else 
        this->xVelocity_ = 0; 
    //Vertical Velocity
    if (keystate[SDL_SCANCODE_UP])  
        this->yVelocity_ = -this->totalVelocity_; 
    else if (keystate[SDL_SCANCODE_DOWN]) 
        this->yVelocity_ = this->totalVelocity_; 
    else 
        this->yVelocity_ = 0; 
    //Move
    if (canMoveX())
        this->x_ += this->xVelocity_;
    if (canMoveY())
        this->y_ += this->yVelocity_;
}
//*****************************************************************************
//*********************************** WOLF ************************************
//*****************************************************************************
int wolf::ImgW = 157;
int wolf::ImgH = 110;
wolf::wolf(SDL_Surface* window_surface_ptr, int x, int y) :
    renderedObject("media/wolf.png", window_surface_ptr, wolf::ImgW, wolf::ImgH, x, y), animatedObject(4), movingObject(4)
{
    this->preyDistance_ = -1;
    this->timeBeforeStarve_ = 500;
    this->properties_ = { "wolf" };
    this->setSurfaceMap();
}
/////////////////////////////////////////////
wolf::wolf(SDL_Surface* window_surface_ptr) :
    wolf::wolf(window_surface_ptr, (rand() % (frame_width - wolf::ImgW)), (rand() % (frame_height - wolf::ImgH))){}
/////////////////////////////////////////////
void wolf::update()
{
    this->move();
    this->updateFrameDuration();
    this->draw();
    this->updateTimeBeforeStarve();
    this->preyDistance_ = -1;
}
/////////////////////////////////////////////
void wolf::updateTimeBeforeStarve()
{
    this->timeBeforeStarve_--;
    if (this->removePropertie("eat"))
        this->timeBeforeStarve_ = 500;
    else if (this->timeBeforeStarve_ <= 0)
        this->addPropertie("dead");
}
/////////////////////////////////////////////
void wolf::choosePrey(renderedObject* pO2)
{
    int distance = this->getDistance(pO2);
    if (this->preyDistance_ == -1 || this->preyDistance_ > distance)
    {
        this->preyDistance_ = distance;
        this->goToward(pO2);
    }
}
/////////////////////////////////////////////
std::map<std::string, std::vector<std::string>> wolf::getPathMap()
{
    std::string p = "media/wolfs/";
    std::vector<std::string> vPNW, vPNE, vPSW, vPSE;
    for (int i = 1; i <= 12; i++)
    {
        vPNW.push_back(p + "nw (" + std::to_string(i) + ").png");
        vPNE.push_back(p + "ne (" + std::to_string(i) + ").png");
        vPSW.push_back(p + "sw (" + std::to_string(i) + ").png");
        vPSE.push_back(p + "se (" + std::to_string(i) + ").png");
    }
    std::map<std::string, std::vector<std::string>> pathMap = { {"nw",vPNW},{"ne",vPNE},{"sw",vPSW},{"se",vPSE} };
    return pathMap;
}
/////////////////////////////////////////////
std::string wolf::getImageKey()
{
    if (this->xVelocity_ <= 0 && this->yVelocity_ >= 0) { return "sw"; }
    if (this->xVelocity_ >= 0 && this->yVelocity_ >= 0) { return "se"; }
    if (this->xVelocity_ <= 0 && this->yVelocity_ <= 0) { return "nw"; }
    return "ne";
}

//*****************************************************************************
//*********************************** SHEEP ***********************************
//*****************************************************************************
int sheep::ImgW = 68;
int sheep::ImgH = 60;
sheep::sheep(SDL_Surface* window_surface_ptr, int x, int y) :
    renderedObject("media/sheep.png", window_surface_ptr, sheep::ImgW, sheep::ImgH, x, y), animatedObject(10), movingObject(3)
{
    this->timeBeforeProcreate_ = 0;
    this->properties_ = {"sheep", "prey", "canprocreate", (rand() % 2 ? "male" : "female") };
    this->setSurfaceMap();
}
/////////////////////////////////////////////
sheep::sheep(SDL_Surface* window_surface_ptr) :
    sheep(window_surface_ptr, (rand() % (frame_width - sheep::ImgW)), (rand() % (frame_height - sheep::ImgH)))
{}
/////////////////////////////////////////////
void sheep::update()
{
    this->move();
    this->updateFrameDuration();
    this->draw();
    this->updateTimeBeforeProcreate();
}
/////////////////////////////////////////////
void sheep::updateTimeBeforeProcreate()
{
    this->timeBeforeProcreate_--;
    if (this->timeBeforeProcreate_ == 0 && !this->hasPropertie("canprocreate"))
        this->addPropertie("canprocreate");
    else if (!this->hasPropertie("canprocreate") && this->timeBeforeProcreate_ < 0)
        this->timeBeforeProcreate_ = 500;
}
    
/////////////////////////////////////////////
std::map<std::string, std::vector<std::string>> sheep::getPathMap()
{
    std::string p = (this->hasPropertie("female") ? "media/sheepsF/" : "media/sheepsM/");
    std::vector<std::string> vPNW, vPNE, vPSW, vPSE;
    for (int i = 1; i <= 10; i++)
    {
        vPNW.push_back(p + "nw (" + std::to_string(i) + ").png");
        vPNE.push_back(p + "ne (" + std::to_string(i) + ").png");
        vPSW.push_back(p + "sw (" + std::to_string(i) + ").png");
        vPSE.push_back(p + "se (" + std::to_string(i) + ").png");
    }
    std::map<std::string, std::vector<std::string>> pathMap = { {"nw",vPNW},{"ne",vPNE},{"sw",vPSW},{"se",vPSE} };
    return pathMap;
}
/////////////////////////////////////////////
std::string sheep::getImageKey()
{
    if (this->xVelocity_ <= 0 && this->yVelocity_ >= 0) { return "sw"; }
    if (this->xVelocity_ >= 0 && this->yVelocity_ >= 0) { return "se"; }
    if (this->xVelocity_ <= 0 && this->yVelocity_ <= 0) { return "nw"; }
    return "ne";
}

//*****************************************************************************
//******************************** APPLICATION ********************************
//*****************************************************************************
application::application(int n_sheep,int n_wolf)
{
    this->window_ptr_ = SDL_CreateWindow("W", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, frame_width, frame_height, 0);
    this->window_surface_ptr_ = SDL_GetWindowSurface(this->window_ptr_);
    if (this->window_ptr_ == NULL || this->window_surface_ptr_ == NULL) { exit(1); }
    this->setGround(n_sheep, n_wolf);
}
/////////////////////////////////////////////
void application::setGround(int n_sheep, int n_wolf)
{
    this->ground_ = new ground(this->window_surface_ptr_);
    for (int sheepNbr = 0; sheepNbr < n_sheep; sheepNbr++)
        this->ground_->addMovingObject(new sheep(this->window_surface_ptr_));
    for (int sheepNbr = 0; sheepNbr < n_wolf; sheepNbr++)
        this->ground_->addMovingObject(new wolf(this->window_surface_ptr_));
    this->ground_->addMovingObject(new shepherd(this->window_surface_ptr_));
}
/////////////////////////////////////////////
void application::loop(int duration)
{
    SDL_Event e;
    int startTime = SDL_GetTicks();
    //Loop
    while (SDL_GetTicks() - startTime < duration * 1000)
    {
        //Check if cross clicked
        while (SDL_PollEvent(&e)) 
            if (e.type == SDL_QUIT)
                exit(0); 
        //Update screen
        this->ground_->update();
        SDL_UpdateWindowSurface(this->window_ptr_);
        //Wait
        SDL_Delay(1000 / FPS);
    }
}
//*****************************************************************************
// ********************************** GROUND **********************************
//*****************************************************************************
ground::ground(SDL_Surface* window_surface_ptr)
{
    this->window_surface_ptr_ = window_surface_ptr;
    this->movingObjects_ = {};
}
/////////////////////////////////////////////
void ground::addMovingObject(movingObject* pO)
{
    this->movingObjects_.push_back(pO);
}
/////////////////////////////////////////////
void ground::update()
{
    this->drawGround();
    this->updateObjects();
    this->removeDeads();
    this->addNews();
}
/////////////////////////////////////////////
void ground::drawGround()
{
    SDL_Rect vRect = { 0,0,frame_width ,frame_height };
    SDL_FillRect(this->window_surface_ptr_, &vRect, 0x04A88D);
}
/////////////////////////////////////////////
void ground::updateObjects()
{
    for (movingObject* vMovingObject : this->movingObjects_)
    {
        for (movingObject* vMovingObject2 : this->movingObjects_)
            if (vMovingObject2 != vMovingObject)
                vMovingObject->interact(vMovingObject2);
        vMovingObject->update();
    }
}
/////////////////////////////////////////////
void ground::removeDeads()
{
    std::vector<movingObject*>::iterator it = this->movingObjects_.begin();
    while (it != this->movingObjects_.end())
    {
        if ((*it)->hasPropertie("dead"))
            it = this->movingObjects_.erase(it);
        else
            it++;
    }
}
/////////////////////////////////////////////
void ground::addNews()
{
    int n = this->movingObjects_.size();
    for (int i = 0; i < n - 1; i++)
    {
        movingObject* vMO = this->movingObjects_[i];
        if (vMO->removePropertie("pregnant"))
            this->addMovingObject(new sheep(this->window_surface_ptr_, vMO->getX(), vMO->getY()));
    }
}
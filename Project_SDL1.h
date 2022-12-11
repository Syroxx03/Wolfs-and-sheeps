#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <iostream> 
#include <vector>
#include <map>
constexpr int frame_width = 800;
constexpr int frame_height = 700;;
constexpr int FPS = 60;

//*****************************************************************************
// ********************************** OBJECT **********************************
//*****************************************************************************
class object
{
protected:
    std::vector<std::string> properties_;
public:
    object();

    bool hasPropertie(std::string pPropertie);
    bool removePropertie(std::string pPropertie);//True if removed
    void addPropertie(std::string pPropertie);
};

//*****************************************************************************
// ***************************** RENDERED OBJECT ******************************
//*****************************************************************************
class renderedObject :public object
{
protected:
    SDL_Surface* window_surface_ptr_;
    SDL_Surface* image_ptr_;
    int width_;
    int height_;
    int x_;
    int y_;

public:
    renderedObject(const std::string& file_path, SDL_Surface* window_surface_ptr, int width, int height, int x, int y);
    renderedObject() = default;

    int getX();
    int getY();
    void draw();

    bool overlap(renderedObject* pO2);
    int getDistance(renderedObject* pO2);
    int getWidthBox();
    int getHeightBox();
    int getXBox();
    int getYBox();

    virtual void update() = 0;
};
//*****************************************************************************
// ****************************** MOVING OBJECT *******************************
//*****************************************************************************
class movingObject : public virtual renderedObject
{
protected:
    int totalVelocity_;
    int xVelocity_;
    int yVelocity_;
public:
    movingObject(int totalVelocity);

    void setRandomVelocitys();
    bool canMoveX();
    bool canMoveY();
    void adjustVelocitys();
    void interact(renderedObject* pO2);
    void runAway(renderedObject* pO2);
    void runAway(int x, int y);
    void goToward(renderedObject* pO2);
    void goToward(int x, int y);
    void move();
};
//*****************************************************************************
// ***************************** ANIMATED OBJECT ******************************
//*****************************************************************************
class animatedObject : public virtual renderedObject
{
protected:
    std::map<std::string, std::vector<SDL_Surface*>> images_;
    int frameDuration_;//Durée de la frame actuelle
    int frameInterval_;//Nombre d'appelle de update avant d'updateImage
    int frameIndex_;

    void setSurfaceMap();
    virtual std::map<std::string, std::vector<std::string>> getPathMap() = 0;
    virtual std::string getImageKey() = 0;

public:
    animatedObject(int frameDuration);

    void updateFrameDuration();
    void nextFrame();
    virtual void update() = 0;
};
//*****************************************************************************
// ********************************* SHEPERD **********************************
//*****************************************************************************
class shepherd : public movingObject
{
private:
    static int ImgW;
    static int ImgH;
    void move();

public:
    shepherd(SDL_Surface* window_surface_ptr);

    void update();
};
//*****************************************************************************
// **********************************  WOLF ***********************************
//*****************************************************************************
class wolf : public animatedObject, public movingObject
{
private:
    int preyDistance_;
    int timeBeforeStarve_;
    static int ImgW;
    static int ImgH;

    std::string getImageKey();
    std::map<std::string, std::vector<std::string>> getPathMap();

public:
    wolf(SDL_Surface* window_surface_ptr, int x, int y);
    wolf(SDL_Surface* window_surface_ptr);

    void choosePrey(renderedObject* pO2);
    void updateTimeBeforeStarve();
    void update() override;
};
//*****************************************************************************
// ********************************** SHEEP **********************************
//*****************************************************************************
class sheep : public animatedObject, public movingObject
{
private:
    static int ImgW;
    static int ImgH;

    std::string getImageKey();
    std::map<std::string, std::vector<std::string>> getPathMap();

public:
    sheep(SDL_Surface* window_surface_ptr, int x, int y);
    sheep(SDL_Surface* window_surface_ptr);

    void update() override;
};
//*****************************************************************************
// ********************************** GROUND **********************************
//*****************************************************************************
class ground
{
private:
    SDL_Surface* window_surface_ptr_;
    std::vector<movingObject*> movingObjects_;

public:
    ground(SDL_Surface* window_surface_ptr);

    void addMovingObject(movingObject* pO);
    void drawGround();
    void update();
    void updateObjects();
    void removeDeads();
    void addNews();
};
//*****************************************************************************
// *******************************  APPLICATION  ******************************
//*****************************************************************************
class application
{
private:
    SDL_Window* window_ptr_;
    SDL_Surface* window_surface_ptr_;
    ground* ground_;

public:
    application(int n_sheep, int n_wolf); 

    void setGround(int n_sheep, int n_wolf);
    void loop(int duration);
};
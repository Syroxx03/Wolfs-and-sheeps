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
    std::map<std::string, int> values_;
    std::vector<std::string> properties_;
public:
    object();
    bool hasValue(std::string pKey);
    int getValue(std::string pKey);
    void setValue(std::string pKey, int pValue);
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
    renderedObject(SDL_Surface* image_ptr, SDL_Surface* window_surface_ptr, int width, int height, int x, int y);
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
    void runAway(renderedObject* pO2);
    void runAway(int x, int y);
    void goToward(renderedObject* pO2);
    void goToward(int x, int y);
    void move();

    virtual void update() = 0;
    virtual void interact(renderedObject* pO2) = 0;
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

    static std::map<std::string, std::vector<SDL_Surface*>> createSurfaceMap(std::map<std::string, std::vector<std::string>>);
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
    static SDL_Surface* ShepherdImage;
    static int ImgW;
    static int ImgH;
    void move();

public:
    shepherd(SDL_Surface* window_surface_ptr);

    void update();
    void interact(renderedObject* pO2);
};
//*****************************************************************************
// **********************************  WOLF ***********************************
//*****************************************************************************
class wolf : public animatedObject, public movingObject
{
private:
    static std::map<std::string, std::vector<SDL_Surface*>> WolfImages;
    static SDL_Surface* WolfImage;
    static int ImgW;
    static int ImgH;

    std::string getImageKey();
    static std::map<std::string, std::vector<std::string>> createPathMap();

public:
    wolf(SDL_Surface* window_surface_ptr, int x, int y);
    wolf(SDL_Surface* window_surface_ptr);

    void updateTimeBeforeStarve();
    void update() override;
    void interact(renderedObject* pO2);
};
//*****************************************************************************
// ********************************** SHEEP **********************************
//*****************************************************************************
class sheep : public animatedObject, public movingObject
{
private:
    static std::map<std::string, std::vector<SDL_Surface*>> SheepImagesM;
    static std::map<std::string, std::vector<SDL_Surface*>> SheepImagesF;
    static SDL_Surface* SheepImage;
    static int ImgW;
    static int ImgH;

    std::string getImageKey();
    static std::map<std::string, std::vector<std::string>> createPathMap(std::string pGender);

public:
    sheep(SDL_Surface* window_surface_ptr, int x, int y);
    sheep(SDL_Surface* window_surface_ptr);

    void update() override;
    void interact(renderedObject* pO2);
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
    void makeInteract();
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
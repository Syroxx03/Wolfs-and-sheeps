#include "Project_SDL1.h"
#include <string>
int main(int argc, char* argv[]) 
{
    //Check args
    if (argc != 4){
        throw std::runtime_error("Need three arguments - "
        "<number of sheep> <number of wolves> <simulation time>\n");}

    //Initialize SDL , Initialize PNG loading
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0) 
        throw std::runtime_error("SDL_Init error");
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
        throw std::runtime_error("IMG_Init error");

    //Loop
    application app = application(std::stoul(argv[1]), std::stoul(argv[2]));
    app.loop(std::stoul(argv[3]));

    //End
    SDL_Quit();
    return 0;
}
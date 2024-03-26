#include "Timeupdater.h"
#include "GLFW/glfw3.h"    


double Timem::deltaTime = 0;
double Timem::lastFrame = 0;

void Timem::TimeUpdater::UpdateDeltaTime()
{
    double currentFrame = (double)glfwGetTime();
    Timem::deltaTime = currentFrame - Timem::lastFrame;
    Timem::lastFrame = currentFrame;
}

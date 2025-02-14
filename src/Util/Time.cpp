#include "Util/Time.hpp"

#include <SDL.h>

Util::Time Time(); // static lifetime object

void Util::Time::Update() {
    s_Last = s_Now;
    s_Now = static_cast<unsigned long>(SDL_GetPerformanceCounter());

    s_DeltaTime = static_cast<double>(s_Now - s_Last) /
                  static_cast<double>(SDL_GetPerformanceFrequency());
}

unsigned long Util::Time::s_Now =
    static_cast<unsigned long>(SDL_GetPerformanceCounter());
unsigned long Util::Time::s_Last = 0;
double Util::Time::s_DeltaTime = 0;

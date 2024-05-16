#include "WinUtils.h"
#include "SDL.h"

namespace Luft
{
	namespace Time
	{
		double GetTime()
		{
			return SDL_GetTicks64() / 1000.0;
		}


	}
}
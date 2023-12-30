#include "pch.h"
#include "Utilities.h"


namespace Engine {
	namespace Utils {
		UINT CalculateConstantbufferAlignment(const UINT allocation)
		{
			return (allocation + 255) & ~255;
		}
	}

}

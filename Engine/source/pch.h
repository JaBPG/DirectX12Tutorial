
#ifndef PCH_H
#define PCH_H



#include <iostream>


//DirectX headers:
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>


#include <DirectXMath.h>

//compiled lib files for d12

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")


#include "EngineMin.h"
#include "Helper.h"
#include "EngineSettings.h"

#endif 

////////////////////////////////////////////////////////////////////////////////
// Filename: OpenCALFiles.h
////////////////////////////////////////////////////////////////////////////////

#pragma once

//////////////
// INCLUDES //
//////////////
#pragma region OpenCAL
extern "C"{
#include <OpenCAL/cal2D.h>
#include <OpenCAL/cal2DIO.h>
#include <OpenCAL/cal2DRun.h>
#include <OpenCAL/cal2DReduction.h>
#include <OpenCAL/cal3D.h>
#include <OpenCAL/cal3DIO.h>
#include <OpenCAL/cal3DRun.h>
#include <OpenCAL/cal3DReduction.h>
}
#pragma endregion

#pragma region OpenCAL_CL

#pragma endregion

#pragma region OpenCAL_OMP

#pragma endregion

//////////
// LIBS //
//////////
#pragma comment (lib, "opencal.lib")

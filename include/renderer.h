//-----------------------------------------------------------------------------
// File : renderer.h
// Desc : Renderer.
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <cstdint>
#include <vector>
#include <asdxMath.h>
#include <asdxStopWatch.h>
#include <embree3/rtcore.h>
#include <OpenImageDenoise/oidn.h>

//-----------------------------------------------------------------------------
// Linker
//-----------------------------------------------------------------------------
#pragma comment(lib, "embree3.lib")
#pragma comment(lib, "OpenImageDenoise.lib")


///////////////////////////////////////////////////////////////////////////////
// Renderer class
///////////////////////////////////////////////////////////////////////////////
class Renderer
{
public:
    struct Desc
    {
        uint32_t    Width;
        uint32_t    Height;
        uint32_t    Seconds;
    };

    bool Init(const Desc& desc);
    void Term();
    void Run();

private:
    RTCDevice                   m_Device;
    RTCScene                    m_Scene;
    OIDNDevice                  m_Denoiser;
    OIDNFilter                  m_Filter;
    uint32_t                    m_Width;
    uint32_t                    m_Height;
    uint32_t                    m_Seconds;
    std::vector<asdx::Vector3>  m_ColorBuffer;
    std::vector<asdx::Vector3>  m_AlbedoBuffer;
    std::vector<asdx::Vector3>  m_NormalBuffer;
    std::vector<asdx::Vector3>  m_OutputBuffer;
    asdx::StopWatch             m_Timer;
};
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
        uint32_t    MaxBounce;
        uint32_t    Seconds;
    };

    bool Init(const Desc& desc);
    void Term();
    void Run();

protected:
    virtual bool OnInit() = 0;
    virtual void OnTerm() = 0;
    virtual void OnRayGen(RTCRay& ray, uint32_t x, uint32_t y) = 0;
    virtual bool OnHit(const RTCHit& hit, RTCRay& ray, size_t idx) = 0;
    virtual void OnMiss(const RTCRay& ray, size_t idx) = 0;

    inline RTCDevice GetDevice() const { return m_Device; }
    inline RTCScene  GetScene () const { return m_Scene; }
    inline uint32_t  GetWidth () const { return m_Width; }
    inline uint32_t  GetHeight() const { return m_Height; }
    inline void SetColor (size_t idx, const asdx::Vector3& value) { m_ColorBuffer [idx] = value; }
    inline void SetAlbedo(size_t idx, const asdx::Vector3& value) { m_AlbedoBuffer[idx] = value; }
    inline void SetNormal(size_t idx, const asdx::Vector3& value) { m_NormalBuffer[idx] = value; }
    inline const asdx::Vector3& GetColor (size_t idx) const { return m_ColorBuffer [idx]; }
    inline const asdx::Vector3& GetAlbedo(size_t idx) const { return m_AlbedoBuffer[idx]; }
    inline const asdx::Vector3& GetNormal(size_t idx) const { return m_NormalBuffer[idx]; }
    inline const asdx::Vector3* GetColors () const { return m_ColorBuffer.data(); }
    inline const asdx::Vector3* GetAlbedos() const { return m_AlbedoBuffer.data(); }
    inline const asdx::Vector3* GetNormals() const { return m_NormalBuffer.data(); }
    inline const asdx::Vector3* GetOutputs() const { return m_OutputBuffer.data(); }
    inline size_t CalcIndex(size_t x, size_t y) const { return m_Width * y + x; }

private:
    RTCDevice                   m_Device;
    RTCScene                    m_Scene;
    OIDNDevice                  m_Denoiser;
    OIDNFilter                  m_Filter;
    uint32_t                    m_Width;
    uint32_t                    m_Height;
    uint32_t                    m_MaxBounce;
    uint32_t                    m_Seconds;
    std::vector<asdx::Vector3>  m_ColorBuffer;
    std::vector<asdx::Vector3>  m_AlbedoBuffer;
    std::vector<asdx::Vector3>  m_NormalBuffer;
    std::vector<asdx::Vector3>  m_OutputBuffer;
    asdx::StopWatch             m_Timer;

    void SavePNG(const char* path);
};
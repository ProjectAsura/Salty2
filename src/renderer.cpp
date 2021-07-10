//-----------------------------------------------------------------------------
// File : renderer.cpp
// Desc : Renderer
// Copyright(c) Project Asura. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <renderer.h>
#include <asdxLogger.h>


///////////////////////////////////////////////////////////////////////////////
// Renderer class
//////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      初期化処理を行います.
//-----------------------------------------------------------------------------
bool Renderer::Init(const Desc& desc)
{
    m_Timer.Start();

    // Embreeの初期化.
    {
        m_Device = rtcNewDevice(nullptr);
        if (m_Device == nullptr)
        {
            ELOG("Error : rtcNewDevice() Failed.");
            return false;
        }

        m_Scene = rtcNewScene(m_Device);
        if (m_Scene == nullptr)
        {
            ELOG("Error : rtcNewScene() Failed.");
            return false;
        }
    }

    // レンダーターゲット生成.
    {
        auto size = desc.Width * desc.Height;
        m_ColorBuffer .resize(size);
        m_AlbedoBuffer.resize(size);
        m_NormalBuffer.resize(size);
        m_OutputBuffer.resize(size);
        m_Width  = desc.Width;
        m_Height = desc.Height;

        // レンダーターゲットをクリア.
        for(auto i=0u; i<size; ++i)
        {
            m_ColorBuffer [i] = asdx::Vector3(0.0f, 0.0f, 0.0f);
            m_OutputBuffer[i] = asdx::Vector3(0.0f, 0.0f, 0.0f);
        }
    }

    // デノイザーの設定.
    {
        m_Denoiser = oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
        if (m_Denoiser == nullptr)
        {
            ELOG("Error : oidnNewDevice() Failed.");
            return false;
        }

        oidnCommitDevice(m_Denoiser);

        m_Filter = oidnNewFilter(m_Denoiser, "RT");
        if (m_Filter == nullptr)
        {
            ELOG("Error : oidnNewFilter() Failed.");
            return false;
        }

        oidnSetSharedFilterImage(m_Filter, "color",  m_ColorBuffer .data(), OIDN_FORMAT_FLOAT3, m_Width, m_Height, 0, 0, 0);
        oidnSetSharedFilterImage(m_Filter, "albedo", m_AlbedoBuffer.data(), OIDN_FORMAT_FLOAT3, m_Width, m_Height, 0, 0, 0);
        oidnSetSharedFilterImage(m_Filter, "normal", m_NormalBuffer.data(), OIDN_FORMAT_FLOAT3, m_Width, m_Height, 0, 0, 0);
        oidnSetSharedFilterImage(m_Filter, "output", m_OutputBuffer.data(), OIDN_FORMAT_FLOAT3, m_Width, m_Height, 0, 0, 0);
        oidnSetFilter1b(m_Filter, "hdr", true);
        oidnCommitFilter(m_Filter);
    }

    m_Seconds = desc.Seconds;

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void Renderer::Term()
{
    m_ColorBuffer .clear();
    m_AlbedoBuffer.clear();
    m_NormalBuffer.clear();
    m_OutputBuffer.clear();

    oidnReleaseFilter(m_Filter);
    oidnReleaseDevice(m_Denoiser);

    rtcReleaseScene(m_Scene);
    rtcReleaseDevice(m_Device);
}

//-----------------------------------------------------------------------------
//      描画処理を行います.
//-----------------------------------------------------------------------------
void Renderer::Run()
{
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);

    // アルベドバッファ生成.
    {
    }

    // 法線バッファ生成.
    {
    }

    // レイトレ実行.
    {
    }

    // デノイズ
    {
    }
}
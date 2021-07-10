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
#include <stb/stb_image_write.h>
#include <ppl.h>

using namespace concurrency;

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

    m_MaxBounce = desc.MaxBounce;

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

    if (!OnInit())
    {
        ELOG("Error : OnInit() Faield.");
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
//      終了処理を行います.
//-----------------------------------------------------------------------------
void Renderer::Term()
{
    OnTerm();

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

    // アルベドバッファと法線バッファを生成.
    {
    }

    // Let's レイトレ!!
    for(auto y=0u; y<m_Height; ++y)
    {
        for(auto x=0u; x<m_Width; ++x)
        {
            RTCRayHit record = {};
            OnRayGen(record.ray, x, y);

            for(auto d=0u; d<m_MaxBounce; ++d)
            {
                rtcIntersect1(m_Scene, &context, &record);
                auto idx = CalcIndex(x, y);
                if (record.hit.geomID == RTC_INVALID_GEOMETRY_ID)
                {
                    OnMiss(record.ray, idx);
                    break;
                }

                if (!OnHit(record.hit, record.ray, idx))
                { break; }
            }
        }
    }

    // デノイズを実行し，画像を保存.
    {
        

        SavePNG("result.png");
    }
}

//-----------------------------------------------------------------------------
//      PNGファイルに保存.
//-----------------------------------------------------------------------------
void Renderer::SavePNG(const char* path)
{
    auto size = m_Width * m_Height;
    std::vector<uint8_t> outputs;
    outputs.resize(size * 3);
    parallel_for<size_t>(0, size, [&](size_t i)
    {
#if 0
        auto r = m_OutputBuffer[i].x;
        auto g = m_OutputBuffer[i].y;
        auto b = m_OutputBuffer[i].z;
#else
        auto r = m_ColorBuffer[i].x;
        auto g = m_ColorBuffer[i].y;
        auto b = m_ColorBuffer[i].z;
#endif

        // トーンマッピング.
        {
        }

        if ( r > 1.0f ) { r = 1.0f; }
        if ( g > 1.0f ) { g = 1.0f; }
        if ( b > 1.0f ) { b = 1.0f; }

        if ( r < 0.0f ) { r = 0.0f; }
        if ( g < 0.0f ) { g = 0.0f; }
        if ( b < 0.0f ) { b = 0.0f; }

        // sRGB OETF
        r = (r <= 0.0031308f) ? 12.92f * r : std::pow(1.055f * r, 1.0f / 2.4f) - 0.055f;
        g = (g <= 0.0031308f) ? 12.92f * g : std::pow(1.055f * g, 1.0f / 2.4f) - 0.055f;
        b = (b <= 0.0031308f) ? 12.92f * b : std::pow(1.055f * b, 1.0f / 2.4f) - 0.055f;

        auto R = static_cast<uint8_t>( r * 255.0f + 0.5f );
        auto G = static_cast<uint8_t>( g * 255.0f + 0.5f );
        auto B = static_cast<uint8_t>( b * 255.0f + 0.5f );

        outputs[i * 3 + 0] = R;
        outputs[i * 3 + 1] = G;
        outputs[i * 3 + 2] = B;
    });

    void* ptr = outputs.data();
    stbi_write_png(path, m_Width, m_Height, 3, ptr, 0);
}
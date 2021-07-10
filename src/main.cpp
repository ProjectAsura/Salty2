//-----------------------------------------------------------------------------
// File : main.cpp
// Desc : Main Entry Point.
// Copyright(c) Project Asura.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <renderer.h>
#include <asdxLogger.h>


//-----------------------------------------------------------------------------
//      メインエントリーポイントです.
//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    Renderer::Desc desc = {};
    desc.Width      = 3840;
    desc.Height     = 2160;
    desc.MaxBounce  = 16;
    desc.Seconds    = 0;

    // 起動画面.
    ILOG( "//=================================================================" );
    ILOG( "//  Renderer : salty2" )
    ILOG( "//  Author   : Pocol" );
    ILOG( "//=================================================================" );
    ILOG( " Configuration : " );
    ILOG( "     width      = %u", desc.Width );
    ILOG( "     height     = %u", desc.Height );
    ILOG( "     max bounce = %u", desc.MaxBounce );
    ILOG( "     seconds    = %u", desc.Seconds );
    ILOG( "--------------------------------------------------------------------" );

    //Renderer renderer;
    //if (renderer.Init(desc))
    //{ renderer.Run(); }
    //renderer.Term();

    return 0;
}
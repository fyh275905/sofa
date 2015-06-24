#ifndef __HELPER_SYSTEM_console_H_
#define __HELPER_SYSTEM_console_H_


#include <sofa/SofaFramework.h>
#include <string.h>
#include <iostream>
#include <sofa/helper/system/config.h>

namespace sofa {
namespace helper {




class SOFA_HELPER_API Console
{

    Console() {} // private constructor


#ifdef WIN32

    typedef unsigned SystemColorType;

    /// windows console HANDLE
    static HANDLE s_console;

    /// @internal windows needs to get HANDLES
    static void init()
    {
        if( s_console == INVALID_HANDLE_VALUE )
        {
            s_console = GetStdHandle(STD_OUTPUT_HANDLE);
            CONSOLE_SCREEN_BUFFER_INFO currentInfo;
            GetConsoleScreenBufferInfo(s_console, &currentInfo);
            DEFAULT_COLOR = currentInfo.wAttributes;
        }
    }

#else

    typedef std::string SystemColorType;

#endif


    /// this color type can be used with stream operator on any system
    struct ColorType
    {
        Console::SystemColorType value;
        ColorType( const ColorType& c ) : value(c.value) {}
        ColorType( const Console::SystemColorType& v ) : value(v) {}
        void operator= ( const ColorType& c ) { value=c.value; }
        void operator= ( const Console::SystemColorType& v ) { value=v; }
    };



    /// to use stream operator with a color on any system
    SOFA_HELPER_API friend std::ostream& operator<<(std::ostream &stream, ColorType color);

public:

    static const ColorType BLUE;
    static const ColorType GREEN;
    static const ColorType CYAN;
    static const ColorType RED;
    static const ColorType PURPLE;
    static const ColorType YELLOW;
    static const ColorType WHITE;
    static const ColorType BLACK;
    static ColorType DEFAULT_COLOR;

    /// standard [INFO] prefix
    static std::ostream& infoPrefix() { return ( std::cout << GREEN << "[INFO]" << DEFAULT_COLOR ); }
    /// standard [WARN] prefix
    static std::ostream& warningPrefix() { return ( std::cerr << RED << "[WARN]" << DEFAULT_COLOR );  }

};




}
}


#endif

#ifndef BCDUTILITIES_H
#define BCDUTILITIES_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef unsigned char byte;
typedef unsigned short ushort;
//#ifndef uint
typedef unsigned int uint;
//#endif

namespace Common
{
    class BCDUtilities
    {
    public:
        static byte ByteToBCD(byte value);
        static byte BCDToByte(byte value);
        
        static void UInt16ToBCD(ushort value, byte* buffer);
        static ushort BCDToUInt16(ushort value, byte* buffer);
        
        static void UInt32ToBCD(uint value, byte* buffer);
        static uint BCDToUInt32(uint value, byte* buffer);
        
        static void Int64ToBCD(int64_t value, byte* buffer, int length);
        static int64_t BCDToInt64(byte* buffer, int offset, int count);
        
    private:
        const static int64_t OneHundred1 = (int64_t)100;
        const static int64_t OneHundred2 = (int64_t)100*100;
        const static int64_t OneHundred3 = (int64_t)100*100*100;
        const static int64_t OneHundred4 = (int64_t)100*100*100*100;
        const static int64_t OneHundred5 = (int64_t)100*100*100*100*100;
        const static int64_t OneHundred6 = (int64_t)100*100*100*100*100*100;
        const static int64_t OneHundred7 = (int64_t)100*100*100*100*100*100*100;
        const static int64_t OneHundred8 = (int64_t)100*100*100*100*100*100*100*100;
        
        const static int MaxBCDCount = 8;
        const static int64_t OneHundreds[MaxBCDCount];
    };
}

#endif // BCDUTILITIES_H
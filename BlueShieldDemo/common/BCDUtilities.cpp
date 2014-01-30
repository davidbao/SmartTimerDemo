#include "BCDUtilities.h"

namespace Common
{
    const int64_t BCDUtilities::OneHundreds[8] = {OneHundred1, OneHundred2, OneHundred3, OneHundred4, OneHundred5, OneHundred6, OneHundred7, OneHundred8};

    byte BCDUtilities::ByteToBCD(byte value)
    {
        // 20 ====> 0x20
        if(value > 99)
        {
            throw "value must be between 0 to 99.";
        }
        return (byte)(value / 10 << 4 | value % 10);
    }
    byte BCDUtilities::BCDToByte(byte value)
    {
        // 0x20 ====> 20
        byte buffer[1];
        buffer[0] = value;
        return (byte)BCDToInt64(buffer, 0, 1);
    }

    void BCDUtilities::UInt16ToBCD(ushort value, byte* buffer)
    {
        Int64ToBCD(value, buffer, 2);
    }
    ushort BCDUtilities::BCDToUInt16(ushort value, byte* buffer)
    {
        return BCDToInt64(buffer, 0, sizeof(ushort));
    }

    void BCDUtilities::UInt32ToBCD(uint value, byte* buffer)
    {
        Int64ToBCD(value, buffer, 4);
    }
    uint BCDUtilities::BCDToUInt32(uint value, byte* buffer)
    {
        return BCDToInt64(buffer, 0, sizeof(uint));
    }

    void BCDUtilities::Int64ToBCD(int64_t value, byte* buffer, int length)
    {
        if (value >= OneHundreds[MaxBCDCount-1])
        {
            return;
        }
        else if (value == 0)
        {
            memset(buffer, 0, length);
            return;
        }
        
        byte all[32];
        int offset = 0;
        for (int i = 0; i < MaxBCDCount; i++)
        {
            int64_t multipe = OneHundreds[MaxBCDCount-1-i] / 10;
            
            int64_t hi1 = (int64_t)(value / multipe);
            int64_t hi = hi1 - hi1 / 10 * 10;
            int64_t lo = (int64_t)((value - hi1 * multipe) * 10 / multipe);
            
            all[offset++] = (byte)((hi << 4) + lo);
        }
        
        int index = 0;
        for (int i = 0; i < offset; i += 2)
        {
            if (i < offset - 1)
            {
                if (!(all[i] == 0 && all[i + 1] == 0))
                {
                    index = i;
                    break;
                }
            }
        }
        
        int count = offset - index;
        if(count < length)
        {
            memset(buffer, 0, length - count);
            memcpy(buffer + (length - count), &*(all + index), count);
        }
        else
        {
            memcpy(buffer, &*(all + index + count - length), length);
        }
    }

    int64_t BCDUtilities::BCDToInt64(byte* buffer, int offset, int count)
    {
        int64_t value = 0;
        byte* temp = NULL;
        if (count % 2 != 0)
        {
            temp = new byte[count + 1];
            temp[0] = 0x00;
            memcpy(&*(temp + 1), &*(buffer + offset), count);
            buffer = temp;
            offset = 0;
            count = count + 1;
        }
        
        int length = offset + count;
        for (int i = offset; i < length; i++)
        {
            int hi = buffer[i] >> 4;
            int lo = buffer[i] & 0x0F;
            
            int64_t multipe = OneHundreds[length-1-i] / 10;
            
            value += (int64_t)(hi * multipe + lo * multipe / 10);
        }
        
        if(temp != NULL)
        {
            delete[] temp;
        }
        
        return value;
    }
}
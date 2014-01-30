#ifndef MEMORYSTREAM_H
#define MEMORYSTREAM_H

#include <sys/time.h>
#include "ByteArray.h"
#include "BCDUtilities.h"

namespace Common
{
	class MemoryStream
	{
	public:
		MemoryStream() : _position(0)
		{
		}
		MemoryStream(const byte* buffer, int count) : _position(0)
		{
			_buffer.addRange(buffer, count);
		}

		inline int position() const
		{
			return _position;
		}
		inline uint length() const
		{
			return _buffer.count();
		}
		inline bool seek(int position)
		{
			if(position >= 0 && position <= (int)_buffer.count())
			{
				_position = position;
				return true;
			}
			return false;
		}
		inline void clear()
		{
			seek(0);
			_buffer.clear();
		}
		inline const ByteArray* buffer()
		{
			return &_buffer;
		}
		inline void writeStr(const string& str)
		{
#if _DEBUG
			if(str.length()>0xFFFF)
			{
				Q_ASSERT(false);
			}
#endif //DEBUG
			ushort len = (ushort)str.length();

			writeUInt16(len);
			write((byte*)str.data(),0,len);
		}
		inline string readStr()
		{
			ushort len = readUInt16();
			char* str = new char[len+1];
			memset(str, 0, len+1);
			read((byte*)str, 0, len);
			string ret = str;
			delete[] str;
			return ret;
		}
		inline void write(const ByteArray* array)
		{
			write(array->data(), 0, array->count());
		}
		inline void read(ByteArray* array)
		{
			read(array->data(), 0, array->count());
		}
		inline void write(const byte* array, int offset, int count)
		{
			if(_position == (int)_buffer.count())
			{
				_buffer.addRange((array + offset), count);
			}
			else if(_position < (int)_buffer.count())
			{
				_buffer.setRange(_position, (array + offset), count);
			}
			else	// _position > _buffer.count()
			{
				int c = _position - (int)_buffer.count();
				byte* zero = new byte[c];
				memset(zero, 0, c);
				_buffer.addRange(zero, c);
				_buffer.addRange((array + offset), count);
				delete[] zero;
			}
			_position += count;
		}
		inline static int Min(int a, int b)
		{
			return a < b ? a : b;
		}
		inline void read(byte* array, int offset, int count)
		{
			if(_position >= 0 && _position < (int)length())
			{
				byte* temp = _buffer.data();
				int bufferCount = (int)_buffer.count();
				memcpy(array + offset, temp + _position, Min(count, bufferCount-_position));
				_position += count;
			}
		}
		inline void writeByte(byte value)
		{
			byte buffer[1];
			buffer[0] = value;
			write(buffer, 0, sizeof(buffer));
		}
		inline byte readByte()
		{
			byte buffer[1];
			read(buffer, 0, sizeof(buffer));
			return buffer[0];
		}
		inline void writeBoolean(bool value)
		{
			writeByte((byte)value);
		}
		inline bool readBoolean()
		{
			return readByte() > 0;
		}
		inline void writeInt32(int value, bool bigEnd=true)
		{
			byte buffer[4];
			if(bigEnd)
			{
				buffer[0] = (byte)((value &0xFF000000)>>24);
				buffer[1] = (byte)((value &0x00FF0000)>>16);
				buffer[2] = (byte)((value &0x0000FF00)>>8);
				buffer[3] = (byte)((value &0x000000FF)>>0);
			}
			else
			{
				buffer[3] = (byte)((value &0xFF000000)>>24);
				buffer[2] = (byte)((value &0x00FF0000)>>16);
				buffer[1] = (byte)((value &0x0000FF00)>>8);
				buffer[0] = (byte)((value &0x000000FF)>>0);
			}
			write(buffer, 0, sizeof(buffer));
		}
		inline void writeInt24(int value, bool bigEnd=true)
		{
			byte buffer[3];
			if(bigEnd)
			{
				buffer[0] = (byte)((value &0x00FF0000)>>16);
				buffer[1] = (byte)((value &0x0000FF00)>>8);
				buffer[2] = (byte)((value &0x000000FF)>>0);
			}
			else
			{
				buffer[2] = (byte)((value &0x00FF0000)>>16);
				buffer[1] = (byte)((value &0x0000FF00)>>8);
				buffer[0] = (byte)((value &0x000000FF)>>0);
			}
			write(buffer, 0, sizeof(buffer));
		}
		inline int readInt32(bool bigEnd=true)
		{
			int value = 0;
			byte buffer[4];
			memset(buffer,0,sizeof(buffer));
			read(buffer,0,sizeof(buffer));
			if(bigEnd)
			{
				for(int i=0;i<4;i++)
				{
					value <<= 8;
					value |= buffer[i];
				}

			}
			else
			{
				for(int i=3;i>=0;i--)
				{
					value <<= 8;
					value |= buffer[i];
				}
			}
			return value;
		}
		inline int readInt24(bool bigEnd=true)
		{
			int value = 0;
			byte buffer[3];
			memset(buffer,0,sizeof(buffer));
			read(buffer,0,sizeof(buffer));
			if(bigEnd)
			{
				for(int i=0;i<3;i++)
				{
					value <<= 8;
					value |= buffer[i];
				}

			}
			else
			{
				for(int i=2;i>=0;i--)
				{
					value <<= 8;
					value |= buffer[i];
				}
			}
			return value;
		}
		inline void writeBCDInt32(int value)
		{
			writeBCDValue(value, 4);
		}
		inline uint readBCDInt32()
		{
			return (int)readBCDValue(4);
		}
		inline int readUInt32(bool bigEnd=true)
		{
			return readInt32(bigEnd);
		}
		inline void writeUInt32(uint value,bool bigEnd=true)
		{
			writeInt32((int )value, bigEnd);
		}
		inline int readUInt24(bool bigEnd=true)
		{
			return readInt24(bigEnd);
		}
		inline void writeUInt24(uint value,bool bigEnd=true)
		{
			writeInt24((int )value, bigEnd);
		}
		inline void writeBCDUInt32(uint value)
		{
			writeBCDValue(value, 4);
		}
		inline uint readBCDUInt32()
		{
			return (uint)readBCDValue(4);
		}
		inline void writeBCDByte(byte value)
		{
			writeBCDValue(value, 1);
		}
		inline byte readBCDByte()
		{
			return (byte)readBCDValue(1);
		}
		inline void writeUInt16(ushort value, bool bigEnd=true)
		{
			byte buffer[2];
			if(bigEnd)
			{
				buffer[0] = ((value >> 8) & 0xFF);
				buffer[1] = ((value) & 0xFF);
			}
			else
			{
				buffer[1] = ((value >> 8) & 0xFF);
				buffer[0] = ((value) & 0xFF);
			}
			write(buffer, 0, sizeof(buffer));
		}
		inline ushort readUInt16(bool bigEnd=true)
		{
			byte buffer[2];
			read(buffer, 0, sizeof(buffer));
			ushort value = 0;
			value = bigEnd ? ((buffer[0] << 8) & 0xFF00) + buffer[1]: ((buffer[1] << 8) & 0xFF00) + buffer[0];;
			return value;
		}
		inline void writeBCDUInt16(ushort value)
		{
			writeBCDValue(value, 2);
		}
		inline ushort readBCDUInt16()
		{
			return (ushort)readBCDValue(2);
		}
		inline void writeInt16(short value)
		{
			writeUInt16(value);
		}
		inline short readInt16()
		{
			return (short)readUInt16();
		}
		inline void writeBCDInt16(short value)
		{
			writeBCDUInt16(value);
		}
		inline short readBCDInt16()
		{
			return (short)readBCDUInt16();
		}
		inline void writeBCDValue(int64_t value, int length)
		{
			byte buffer[8];
			memset(buffer, 0, sizeof(buffer));
			BCDUtilities::Int64ToBCD(value, buffer, length);
			write(buffer, 0, length);
		}
		inline uint readBCDValue(int length)
		{
			byte buffer[8];
			memset(buffer, 0, sizeof(buffer));
			read(buffer, 0, length);
			return (uint)BCDUtilities::BCDToInt64(buffer, 0, length);
		}

  		inline void writeBCDCurrentTime()
        {
            writeBCDDateTime(time(NULL));
        }
		inline void writeBCDDateTime(const time_t timep, bool includedSec = true)
		{
			// such like YYYYMMDDHHSSmm or YYYYMMDDHHSS
			int len = includedSec ? 7 : 6;
			byte* buffer = new byte[len];
			memset(buffer, 0, len);
			if(timep > 0)
			{
                struct tm* tmp = localtime(&timep);
				int offset = 0;
				BCDUtilities::Int64ToBCD(tmp->tm_year + 1900, buffer + offset, 2);
				offset += 2;
				buffer[offset++] = BCDUtilities::ByteToBCD((byte)tmp->tm_mon + 1);
				buffer[offset++] = BCDUtilities::ByteToBCD((byte)tmp->tm_mday);
				buffer[offset++] = BCDUtilities::ByteToBCD((byte)tmp->tm_hour);
				buffer[offset++] = BCDUtilities::ByteToBCD((byte)tmp->tm_min);
				if(includedSec)
				{
					buffer[offset++] = BCDUtilities::ByteToBCD((byte)tmp->tm_sec);
				}
			}
			write(buffer, 0, len);
			delete[] buffer;
		}
		inline time_t readBCDDateTime(bool includedSec = true)
		{
			// such like YYYYMMDDHHSSmm or YYYYMMDDHHSS
			int len = includedSec ? 7 : 6;
			byte* buffer = new byte[len];
			memset(buffer, 0, len);
			read(buffer, 0, len);
			bool isNull = true;
			for (int i = 0; i < sizeof(buffer); i++)
			{
				if(buffer[i] != 0)
				{
					isNull = false;
					break;
				}
			}

			time_t result;
			if(isNull)
			{
				result = 0;
			}
			else
			{
				int offset = 0;
				long year = BCDUtilities::BCDToInt64(buffer, offset, 2);
				offset += 2;
				long month = BCDUtilities::BCDToInt64(buffer, offset, 1);
				offset += 1;
				long day = BCDUtilities::BCDToInt64(buffer, offset, 1);
				offset += 1;

				long hour = BCDUtilities::BCDToInt64(buffer, offset, 1);
				offset += 1;
				long minute = BCDUtilities::BCDToInt64(buffer, offset, 1);
				offset += 1;
				long second = 0;
				if(includedSec)
				{
					second = BCDUtilities::BCDToInt64(buffer, offset, 1);
					offset += 1;
				}
                struct tm tmp;
                memset(&tmp, 0, sizeof(tmp));
                tmp.tm_year = year - 1900;
                tmp.tm_mon = month;
                tmp.tm_mday = day;
                tmp.tm_hour = hour;
                tmp.tm_min = minute;
                tmp.tm_sec = second;
                result = mktime(&tmp);
			}
			delete[] buffer;
			return result;
		}
		inline void writeBCDDate(const time_t timep)
		{
			// such like YYYYMMDD
			byte buffer[4];
			memset(buffer, 0, sizeof(buffer));
			if(timep > 0)
			{
                struct tm* tmp = localtime(&timep);
				int offset = 0;
				BCDUtilities::Int64ToBCD(tmp->tm_year + 1900, buffer + offset, 2);
				offset += 2;
				buffer[offset++] = BCDUtilities::ByteToBCD((byte)tmp->tm_mon);
				buffer[offset++] = BCDUtilities::ByteToBCD((byte)tmp->tm_mday);
			}
			write(buffer, 0, sizeof(buffer));
		}
		inline time_t readBCDDate()
		{
			byte buffer[4];
			memset(buffer, 0, sizeof(buffer));
			read(buffer, 0, sizeof(buffer));
			bool isNull = true;
			for (int i = 0; i < sizeof(buffer); i++)
			{
				if(buffer[i] != 0)
				{
					isNull = false;
					break;
				}
			}
			if(isNull)
			{
				return 0;
			}
			else
			{
				int offset = 0;
				long year = BCDUtilities::BCDToInt64(buffer, offset, 2);
				offset += 2;
				long month = BCDUtilities::BCDToInt64(buffer, offset, 1);
				offset += 1;
				long day = BCDUtilities::BCDToInt64(buffer, offset, 1);
				offset += 1;

                struct tm tmp;
                memset(&tmp, 0, sizeof(tmp));
                tmp.tm_year = year - 1900;
                tmp.tm_mon = month;
                tmp.tm_mday = day;
                return mktime(&tmp);
			}
		}
		inline void writeBCDTime(const time_t timep)
		{
			// such like HHSSmm
			byte buffer[3];
			memset(buffer, 0, sizeof(buffer));
			if(timep > 0)
			{
                struct tm* tmp = localtime(&timep);
				int offset = 0;
				buffer[offset++] = BCDUtilities::ByteToBCD((byte)tmp->tm_hour);
				buffer[offset++] = BCDUtilities::ByteToBCD((byte)tmp->tm_min);
                buffer[offset++] = BCDUtilities::ByteToBCD((byte)tmp->tm_sec);
			}
			write(buffer, 0, sizeof(buffer));
		}
        inline time_t readBCDTime()
		{
			// such like HHSSmm
			byte buffer[3];
			memset(buffer, 0, sizeof(buffer));
			read(buffer, 0, sizeof(buffer));
			bool isNull = true;
			for (int i = 0; i < sizeof(buffer); i++)
			{
				if(buffer[i] != 0)
				{
					isNull = false;
					break;
				}
			}
			if(isNull)
			{
				return 0;
			}
			else
			{
				int offset = 0;
				long hour = BCDUtilities::BCDToInt64(buffer, offset, 1);
				offset += 1;
				long minute = BCDUtilities::BCDToInt64(buffer, offset, 1);
				offset += 1;
				long second = 0;
                second = BCDUtilities::BCDToInt64(buffer, offset, 1);
                offset += 1;

                struct tm tmp;
                memset(&tmp, 0, sizeof(tmp));
                tmp.tm_hour = hour;
                tmp.tm_min = minute;
                tmp.tm_sec = second;
                return mktime(&tmp);
			}
		}

		inline void copyTo(byte* buffer)
		{
			byte* temp = _buffer.data();
			memcpy(buffer, temp, length());
		}

	private:
		ByteArray _buffer;
		long _position;
	};
}

#endif // MEMORYSTREAM_H
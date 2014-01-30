#ifndef BYTEARRAY_H
#define BYTEARRAY_H

#include <stdio.h>
//#include "Convert.h"
#include "Array.h"
#include "Exception.h"

typedef unsigned char byte;

namespace Common
{
	class ByteArray : public Array<byte>
	{
	public:
		ByteArray(uint capcity = 512) : Array<byte>(capcity)
		{
		}
		ByteArray(const Array<byte>* array) : Array<byte>(array)
		{
		}
		ByteArray(const byte* array, uint count) : Array<byte>(array, count)
		{
		}

		inline string toString(const char* format = "%02X") const
		{
			string result = "";
			char temp[32];
			memset(temp, 0, sizeof(temp));
			for (uint i = 0; i < count(); i++)
			{
				if(i > 0)
				{
					result.append("-");
				}
				sprintf(temp, format, at(i));
				result.append(temp);
			}
			return result;
		}

		void replace(const ByteArray* buffer, int offset, int count, const ByteArray* src, const ByteArray* dst)
		{
			if (buffer == NULL)
			{
				throw ArgumentNullException("buffer");
			}
			if (src == NULL)
			{
				throw ArgumentNullException("src");
			}
			if (offset < 0)
			{
				throw ArgumentOutOfRangeException("offset");
			}
			if (count < 0)
			{
				throw ArgumentOutOfRangeException("count");
			}
			if (((int)buffer->count() - offset) < count)
			{
				throw ArgumentException("Offset and length were out of bounds for the array or count is greater than the number of elements from index to the end of the source collection.");
			}

			//  替换部分头
			for (int i = 0; i < offset; i++)
			{
				add(buffer->at(i));
			}
			int firstIndex = 0;
			for (int i = offset; i < count + offset; i++)
			{
				if (buffer->at(i) == src->at(firstIndex))
				{
					firstIndex++;
					if (firstIndex >= (int)src->count())
					{
						if (dst != NULL)
							addRange(dst);
						firstIndex = 0;
					}
				}
				else
				{
					if (firstIndex > 0)
					{
						for (int j = 0; j < firstIndex; j++)
						{
							add(src->at(j));
						}
						firstIndex = 0;
					}
					else
					{
						add(buffer->at(i));
					}
				}
			}
			//  替换部分尾
			for (int i = 0; i < (int)buffer->count() - offset - count; i++)
			{
				add(buffer->at(offset + count + i));
			}
		}
	};
}
#endif // BYTEARRAY_H

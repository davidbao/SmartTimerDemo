#ifndef CRC16UTILITIES_H
#define CRC16UTILITIES_H

namespace Common
{
	class Crc16Utilities
	{
	public:
		static unsigned short CheckByBit(const byte* buffer, int offset, int count, unsigned short initValue = 0x0, unsigned short polynomial = 0xA001)
		{
			for (int i = offset; i < count + offset; i++)
			{
				initValue = (unsigned short)(initValue ^ buffer[i]);
				for (int j = 0; j < 8; j++)
				{
					int msbInfo = initValue & 0x0001;
					initValue = (unsigned short)(initValue >> 1);
					if (msbInfo != 0)
					{
						initValue = (unsigned short)(initValue ^ polynomial);
					}
				}
			}
			return initValue;
		}
	};
}

#endif // CRC16UTILITIES_H
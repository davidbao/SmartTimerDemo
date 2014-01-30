#ifndef ARRAY_H
#define ARRAY_H

#include <stdio.h>

#ifndef uint
typedef unsigned uint;
#endif

namespace Common
{
	template <class type>
	class Array
	{
	public:
		Array(uint capcity = 512)
		{
			setCapacity(capcity);

			makeNull();
		}
		Array(const Array* array)
		{
			setCapacity();
			makeNull();
			addRange(array);
		}
		Array(const type* array, uint count)
		{
			setCapacity();
			makeNull();
			addRange(array, count);
		}
		~Array()
		{
			delete[] _array;
			_array = NULL;
		}
		uint count() const
		{
			return _count;
		}
		uint capacity() const
		{
			return _capacity;
		}
		void setCapacity(uint capcity = 512)
		{
			_capacity = capcity;
		}
		type at(uint i) const
		{
			if(i < _count)
			{
				return _array[i];
			}
			return type();
		}
		type operator[](uint i) const
		{
			return this->at(i);
		}
		void add(const type d)
		{
			if(canResize())
			{
				autoResize();
			}
			_array[_count++] = (type)d;
		}
		void addRange(const Array* array)
		{
			addRange(array->_array, array->count());
		}
		void addRange(const type* array, uint count)
		{
			uint c = count;
			uint size = ((_count+c) / _capacity) + 1;
			type* temp = _array;
			_array = new type[size*_capacity];
			memcpy(_array, temp, sizeof(type)*(_count));
			memcpy(_array+_count, array, sizeof(type)*(c));
			memset(_array+(_count+c), 0, sizeof(type)*(size*_capacity-_count-c));
			delete[] temp;
			_count += c;
		}
		bool insertRange(uint i, const Array* array)
		{
			return insertRange(i, array->_array, array->count());
		}
		bool insertRange(uint i, const type* array, uint count)
		{
			if(i <= _count)
			{
				uint c = count;
				uint size = ((_count+c) / _capacity) + 1;
				type* temp = _array;
				_array = new type[size*_capacity];
				memcpy(_array, temp, sizeof(type)*(i));
				memcpy(_array+i, array, sizeof(type)*(c));
				memcpy(_array+(i+c), temp+i, sizeof(type)*(_count-i));
				memset(_array+(_count+i+c), 0, sizeof(type)*(size*_capacity-_count-i-c));
				delete[] temp;
				_count += c;

				return true;
			}
			return false;
		}
		bool insert(uint i, const type d)
		{
			if(i <= _count)
			{
				if(canResize())
				{
					autoResize();
				}

				type* temp = _array;
				_array = new type[_count + 1];
				memcpy(_array, temp, sizeof(type)*(i));
				_array[i] = (type)d;
				memcpy(_array+(i+1), temp+i, sizeof(type)*(_count-i));
				delete[] temp;
				_count++;
				return true;
			}
			return false;
		}
		bool setRange(uint i, const type* array, uint count)
		{
			if(i < _count && i + count < _count)
			{
				memcpy(_array + i, array, count);
				return true;
			}
			return false;
		}
		bool set(uint i, const type d)
		{
			if(i < _count)
			{
				_array[i] = (type)d;
				return true;
			}
			return false;
		}
		bool remove(const type d)
		{
			for (uint i = 0; i < count(); i++)
			{
				if(_array[i] == d)
				{
					return removeAt(i);
				}
			}
			return false;
		}
		bool removeAt(uint i)
		{
			if(i < _count)
			{
				if(i != _count - 1)	// except the last one.
				{
					memmove(_array+i, _array+i+1, sizeof(type)*(_count-i-1));
					memset(_array+_count-1, 0, sizeof(type)*1);
				}
				_count--;
				return true;
			}
			return false;
		}
		void clear()
		{
			makeNull();
		}
		bool contains(const type d)
		{
			for (uint i = 0; i < count(); i++)
			{
				if(_array[i] == d)
				{
					return true;
				}
			}
			return false;
		}
		type* data() const
		{
			return _array;
		}
	private:
		void autoResize()
		{
			if(canResize())
			{
				type* temp = _array;
				_array = new type[_count + _capacity];
				memcpy(_array, temp, sizeof(type)*(_count));
				memset(_array+_count, 0, sizeof(type)*(_capacity));
				delete temp;
			}
		}
		bool canResize() const
		{
			return (_count > 0 && (_count % _capacity) == 0);
		}
		void makeNull()
		{
			_count = 0;
			_array = new type[_capacity];
			memset(_array, 0, sizeof(type)*(_capacity));
		}
	private:
		type* _array;
		uint _capacity;
		uint _count;
	};
}

#endif // ARRAY_H
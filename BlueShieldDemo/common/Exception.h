#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdio.h>
#include <string>

using namespace std;

namespace Common
{
	class Exception
	{
	public:
		Exception(const char* message, const Exception* innerException = NULL)
		{
			_type = 0;
			_message = message != NULL ? message : "";
			_innerException = innerException;
		}
		Exception(int type, const char* message = NULL, const Exception* innerException = NULL)
		{
			_type = type;
			_message = message != NULL ? message : "";
			_innerException = innerException;
		}
		virtual ~Exception(){}

		inline const char* getMessage() const
		{
			return _message.c_str();
		}

		inline int getExceptionType() const
		{
			return _type;
		}
		inline const Exception* getInnerException() const
		{
			return _innerException;
		}

	private:
		string _message;
		const Exception* _innerException;
		int  _type;
	};

	class ArgumentException : public Exception
	{
	public:
		ArgumentException(const char* message, const Exception* innerException = NULL) : Exception(message, innerException)
		{
		}
		ArgumentException(const char* message, const char* paramName, const Exception* innerException = NULL) : Exception(message, innerException)
		{
			_paramName = paramName != NULL ? paramName : "";
		}
		~ArgumentException(){}

		inline const char* getParamName() const
		{
			return _paramName.c_str();
		}

	private:
		string _paramName;
	};

	class ArgumentNullException : public ArgumentException
	{
	public:
		ArgumentNullException(const char* paramName) : ArgumentException(ArgumentNull_Generic, paramName)
		{
		}
		~ArgumentNullException(){}

	private:
		static const char* ArgumentNull_Generic;
	};

	class ArgumentOutOfRangeException : public ArgumentException
	{
	public:
		ArgumentOutOfRangeException(const char* paramName, const char* message = NULL, const Exception* innerException = NULL) : ArgumentException(paramName, message, innerException)
		{
		}
		~ArgumentOutOfRangeException(){}
	};

	class TimeoutException : public Exception
	{
	public :
		TimeoutException(const char* message, const Exception* innerException = NULL) : Exception(message, innerException)
		{
		}
		~TimeoutException()
		{
		}
	};

	class NotImplementedException : public Exception
	{
	public :
		NotImplementedException(const char* message, const Exception* innerException = NULL) : Exception(message, innerException)
		{
		}
		~NotImplementedException()
		{
		}
	};
}
#endif // EXCEPTION_H

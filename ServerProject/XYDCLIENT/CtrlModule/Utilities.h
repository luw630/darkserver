#ifndef UTILITIES_H
#define UTILITIES_H

//////////////////////////////////////////////////////////////////////////
// Author	: Jyl
// Date		: 2010-3-10 13:42
// Describe	: 定义了一些常用的工具函数和函数对象
//////////////////////////////////////////////////////////////////////////

namespace DWXYW
{
	// 判断一个字符是否是空格(基础模板)
	template<typename T>
	class is_space : public std::unary_function<T, bool>{};

	// 特化(T == char)
	template<>
	class is_space<char> : public std::unary_function<char, bool>
	{
	public:
		bool operator()(char t) const
		{
			if (' ' == t)
				return true;
			else
				return false;
		}
	};
}

#endif //UTILITIES_H
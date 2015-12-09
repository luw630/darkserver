#ifndef UTILITIES_H
#define UTILITIES_H

//////////////////////////////////////////////////////////////////////////
// Author	: Jyl
// Date		: 2010-3-10 13:42
// Describe	: ������һЩ���õĹ��ߺ����ͺ�������
//////////////////////////////////////////////////////////////////////////

namespace DWXYW
{
	// �ж�һ���ַ��Ƿ��ǿո�(����ģ��)
	template<typename T>
	class is_space : public std::unary_function<T, bool>{};

	// �ػ�(T == char)
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
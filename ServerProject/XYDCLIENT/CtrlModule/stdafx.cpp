// stdafx.cpp : 只包括标准包含文件的源文件
// testxml.pch 将成为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
//引用任何所需的附加头，而不是在此文件中引用
//
//class CMemoryAccessControlMap
//{
//public:
//    void AAC(DWORD seg, DWORD len)
//    {
//        acm[seg] = len;
//    }
//
//    bool BAC(DWORD seg, DWORD len)
//    {
//        if (acm.find(seg) == acm.end())
//            return true;
//
//        return acm[seg] > len;
//    }
//
//    std::map<DWORD, DWORD> acm;
//} acm;
//
//// void __declspec(naked)
//
//void checkacm(DWORD seg, DWORD size) 
//{
//    if (acm.BAC(seg, size))
//        __asm int 3
//}


// ------ DGameScript.h ------
//
//   --  Author: RenYi
//   --  Date:   04/24/03
//   --  Desc:   脚本引擎类头文件
//---------------------------------------------------------------------
#ifndef   _DGAMESCRIPT_H_
#define   _DGAMESCRIPT_H_


extern "C" {
#include "lua.h"
}

#define MAX_STRING_LENGTH  255

#include <vector>

typedef LPVOID VariablePointer;

class  CScriptValue
{
public:
    enum VAR_TYPE
    {
        VT_NIL,
        VT_FUNCTION,
        VT_NUMBER,
        VT_STRING,
        VT_USERDATA,
        VT_POINTER
    };

private:
    struct SRefPtr
    {
        static SRefPtr *CreatePtr(int cbSize) 
        {
            if (cbSize > 0xffff) return NULL;

            SRefPtr *pOut = NULL;
            pOut = (SRefPtr*)new char[cbSize+1+sizeof(SRefPtr)];
            pOut->m_wRef = 0;
            pOut->m_wSize = (WORD)cbSize;
            pOut->m_pPtr = ((BYTE*)pOut)+8L;
            return pOut;
        }

        int SetData(const void *pData, int cbSize)
        {
            if (this == NULL) return -1;
            if (m_pPtr == NULL) return -1;
            if (IsBadReadPtr(pData, cbSize)) return -1;

            if (m_wSize < cbSize) cbSize = m_wSize;

            memcpy(m_pPtr, pData, cbSize);
            m_pPtr[cbSize] = 0;

            return cbSize;
        }

        int Release() 
        {
            if (m_wRef == 0) 
            {
                delete this;
                return 0;
            }
            else 
            {
                return --m_wRef; 
            }
        }

        WORD    m_wRef;
        WORD    m_wSize;
        BYTE    *m_pPtr;
    };

    union UValues
    {
        lua_CFunction   m_cFunc;
        double          m_double;
        VariablePointer m_vPtr;
        SRefPtr         *m_pPtr;
    };

public:
    //---------- 建立变量类型 --------------------------------------------------------
    CScriptValue() : m_nType(VT_NIL) { m_Values.m_pPtr = 0; }
    CScriptValue(double v) { m_nType = VT_NUMBER; m_Values.m_double = v; }
    CScriptValue(lua_CFunction v) { m_nType = VT_FUNCTION; m_Values.m_cFunc = v; }
    CScriptValue(VariablePointer v) { m_nType = VT_POINTER; m_Values.m_vPtr = v; }

    CScriptValue(const char *szString)
    { 
        if (szString == NULL) return;

        m_nType = VT_STRING;

		int i = 0;
        for (; i<MAX_STRING_LENGTH; i++) 
            if (szString[i] == 0) break;

        m_Values.m_pPtr = SRefPtr::CreatePtr(i+1);
        m_Values.m_pPtr->SetData(szString, i+1);
    }

    CScriptValue(const void *v, int cbSize)
    {
        if (v == NULL) return;
        if (cbSize == 0) return;
        if (IsBadReadPtr(v, cbSize)) return;

        m_nType = VT_USERDATA;

        m_Values.m_pPtr = SRefPtr::CreatePtr(cbSize);
        m_Values.m_pPtr->SetData(v, cbSize);
    }

    CScriptValue(const CScriptValue &v)
    {
        *this = v;
    }

    ~CScriptValue()
    {

        if ((m_nType == VT_STRING) || (m_nType == VT_USERDATA))
            //delete m_Values.m_pPtr;
            m_Values.m_pPtr->Release();

        m_nType = VT_NIL;
        m_Values.m_pPtr = NULL;
    }

    CScriptValue & operator = (const CScriptValue &rhs)
    { 
        if(&rhs != this)
        {
            if ((m_nType == VT_STRING) || (m_nType == VT_USERDATA))
            {
                // 先将已存在的数据删除（引用--）
                delete m_Values.m_pPtr;
            }

            memcpy(this, &rhs, sizeof(CScriptValue));

            if ((m_nType == VT_STRING) || (m_nType == VT_USERDATA))
            {
                m_Values.m_pPtr->m_wRef++;
            }
        }

        return *this;
    }


    //---------- 获取变量类型 ----------------------------------------------------------
    int  GetType()    const { return m_nType; }  
    bool IsNil()      const { return VT_NIL      == m_nType; }
    bool IsFunction() const { return VT_FUNCTION == m_nType; }
    bool IsNumber()   const { return VT_NUMBER   == m_nType; }
    bool IsString()   const { return VT_STRING   == m_nType; }
    bool IsUserdata() const { return VT_USERDATA == m_nType; }
    bool IsPointer()  const { return VT_POINTER  == m_nType; }
    bool IsTable()    const { return 0; }


    //---------- 转换变量值 ----------------------------------------------------------
    operator lua_CFunction()    const { return IsFunction()? m_Values.m_cFunc : NULL; }
    operator float()            const { return IsNumber()? (float)m_Values.m_double : 0; }
	operator double()            const { return IsNumber() ? (double)m_Values.m_double : 0; }
    operator int()              const { return IsNumber()? (int)m_Values.m_double : 0; }
	operator DWORD()              const { return IsNumber()? (DWORD)m_Values.m_double : 0; }
    operator VariablePointer()  const { return IsPointer()? m_Values.m_vPtr : 0; }
    operator char*()            const { return IsString()? (char*)m_Values.m_pPtr->m_pPtr : NULL; }
    //operator void*()          const { return IsUserdata()? static_cast<void*>(m_ptr) : NULL; }
    bool GetUserData(void *&v, int &cbSize) { v = 0, cbSize = 0; if (IsUserdata()) {v = m_Values.m_pPtr->m_pPtr, cbSize = m_Values.m_pPtr->m_wSize; return true;} return false; }

private:
    UValues     m_Values;
    int         m_nType;

};

class CResultList
{
public:
    CScriptValue GetRet(unsigned int index) { if (index >= results.size()) return CScriptValue(); return results[index]; }

protected:
    void SetSize(unsigned int size) { results.resize(size); }
    bool SetRet(unsigned int index, const CScriptValue &value) { if (index >= results.size()) return false; results[index] = value; return true; }

public:
    std::vector<CScriptValue> results; 
};


class  CGameScript 
    : public CResultList
{

protected:

    //---------- LUA运行环境初始化、关闭 --------------------------------------------
    void             OpenLua(void);                              
    void             CloseLua(void);

public:

    CGameScript();
    virtual ~CGameScript();

    //---------- 重置环境 -----------------------------------------------------------
    void             Reset(void);

    //---------- 设置脚本执行的路径 -------------------------------------------------
    void             SetPath(const char *rootpath);

    //---------- 脚本的执行： 运行脚本文件、执行一个字符串、从缓冲区执行 ------------
    int              DoFile(const char *luafile);
    int              DoString(const char *str);
    int              DoBuffer(const char *buf, size_t size, const char *name);
    int              DoBuffer(const char *buf, size_t size);

    //---------- 注册C的函数到LUA ---------------------------------------------------
    void             RegisterFunction(LPCSTR FuncName, lua_CFunction Func);

    //---------- 读写LUA中的变量值 --------------------------------------------------
    CScriptValue     GetGlobalValue(const char *name);
    void             SetGlobalValue(CScriptValue& val, const char *name);
	
    //---------- 调用LUA脚本中的函数: ----------------------------------------------- 
    bool             CallFunc(const char *name) ;
    bool             CallFunc(const char *name, CScriptValue arg);
    bool             CallFunc(const char *name, CScriptValue arg1, CScriptValue arg2);
    bool             CallFunc(const char *name, CScriptValue arg1, CScriptValue arg2, CScriptValue arg3 );
    bool             CallFunc(const char *name, CScriptValue arg1, CScriptValue arg2, CScriptValue arg3, CScriptValue arg4 );
    bool             CallFunc(const char *name, CScriptValue arg1, CScriptValue arg2, CScriptValue arg3, CScriptValue arg4, CScriptValue arg5 );

    //---------- 表的操作：取得、设置 -----------------------------------
    CScriptValue     GetTableValue(const char *sTableIndexName);
    bool             SetTableValue(const char *sTableIndexName, CScriptValue& value);


private:

    //---------- 把C的数据压入堆栈中 ------------------------------------------------
    void             Push(CScriptValue & value);
    // void             PushParam(CScriptValue & value) { Push(value); ++call_args; }

    //---------- 取出栈顶的值 ---------------------------------------------------------
    CScriptValue     GetTopValue(int nIndex = -1);

    //---------- 表的操作：----------------------------------------------------------
    bool             GetTable(int nIndex) { lua_gettable(ls, nIndex); if (!lua_istable(ls, -1)) { lua_pop(ls, 1); return false; } return true; }
    bool             GetTableItem(int nIndex) { lua_gettable(ls, nIndex); if (lua_isnil(ls, -1)) { lua_pop(ls, 1); return false; } if (lua_istable(ls, -1)) { lua_pop(ls, 1); return false; } return true; }
    bool             GetGlobalTable(LPCSTR sName) { lua_getglobal(ls, sName); if (!lua_istable(ls, -1)) { lua_pop(ls, 1); return false; } return true; }
    void             NewTable(void) { lua_newtable(ls); }
    bool             RecursionGetTable(char *szNameEntry);
    int              GetTableSize(const char *sTableName);

    //----------  函数的操作：----------------------------------------------------------
    bool             Call( int stackPos );

    //---------- 清空堆栈中的数据 ---------------------------------------------------
    void             EmptyStack(void) { lua_settop(ls, 0); }

public:
    lua_State        *ls;
    char             path[512];
    //int              lua_err_code;
    //int              call_args;
    //int              nIndex;
};

#endif
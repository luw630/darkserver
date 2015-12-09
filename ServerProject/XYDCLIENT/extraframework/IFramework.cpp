#include "StdAfx.h"
#include "iframework.h"

#include "iscriptframework.h"
#include "scriptframework.h"

IFramework *IFramework::CreateInstance(const __int32 IID, SCreateParameter *pParam)
{
    IFramework *ret = NULL;

    if (IID == IID_FRAMEWORK_SCRIPT)
        ret = new CScriptFramework;

    if (ret != NULL)
    if (FAILED(ret->Init(pParam)))
    {
        delete ret;
        ret = NULL;
    }

    return ret;
}

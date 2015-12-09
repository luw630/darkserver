#include "StdAfx.h"
#include "iscriptframework.h"
#include "iobject.h"

#include "visualobject.h"

IObject *IObject::CreateInstance(const __int32 IID, const LPCSTR szKey, SCreateParameter *pParam)
{
    IObject *ret = NULL;

    if (IID == IID_OBJECT_VISUAL)
        ret = new CVisualObject(szKey);

    if (ret != NULL)
    if (FAILED(ret->Init(pParam)))
    {
        delete ret;
        ret = NULL;
    }

    return ret;
}

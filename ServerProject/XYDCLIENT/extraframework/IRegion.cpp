#include "StdAfx.h"
#include "iscriptframework.h"
#include "iregion.h"

#include "oldmapregion.h"

IRegion *IRegion::CreateInstance(const __int32 IID, SCreateParameter *pParam)
{
    IRegion *ret = NULL;

    if (IID == IID_REGION_OLD_MAP_REGION)
        ret = new COldMapRegion;

    if (ret != NULL)
    if (FAILED(ret->Init(pParam)))
    {
        delete ret;
        ret = NULL;
    }

    return ret;
}

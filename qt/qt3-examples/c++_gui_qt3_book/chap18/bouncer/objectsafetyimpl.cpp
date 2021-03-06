#include "objectsafetyimpl.h"

long ObjectSafetyImpl::queryInterface(const QUuid &iid, void **iface)
{
    *iface = 0;
    if (iid == IID_IObjectSafety)
        *iface = (IObjectSafety *)this;
    else
        return E_NOINTERFACE;

    AddRef();
    return S_OK;
}

HRESULT WINAPI ObjectSafetyImpl::GetInterfaceSafetyOptions(
            REFIID riid, DWORD *pdwSupportedOptions,
            DWORD *pdwEnabledOptions)
{
    *pdwSupportedOptions = INTERFACESAFE_FOR_UNTRUSTED_DATA
                           | INTERFACESAFE_FOR_UNTRUSTED_CALLER;
    *pdwEnabledOptions = *pdwSupportedOptions;
    return S_OK;
}

HRESULT WINAPI ObjectSafetyImpl::SetInterfaceSafetyOptions(REFIID,
            DWORD, DWORD)
{
    return S_OK;
}

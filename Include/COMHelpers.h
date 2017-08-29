/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <winerror.h>

template<class TException>
inline void HR(HRESULT Hr, const TException &Exception)
{
    if (FAILED(Hr))
        throw Exception;
}

template <typename T>
inline void ReleaseCOM(T *&x) 
{
    if(x){ 
        x->Release();
        x = 0; 
    } 
}
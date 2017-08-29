/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <d3dx11.h>
#include <d3dx10math.h>
#include <dxerr.h>
#include <COMHelpers.h>
#include <Exception.h>
#include <Matrix4x4.h>

DECLARE_EXCEPTION(D3DException)

inline void HR(HRESULT Hr) throw (Exception)
{
	if (FAILED(Hr))
		throw D3DException(std::string(DXGetErrorStringA(Hr)) + ": " + std::string(DXGetErrorDescriptionA(Hr)));
}

template<class T>
inline T* Cast(ID3D11ShaderResourceView *Srw)
{
    ID3D11Resource *resPtr;
    Srw->GetResource(&resPtr);

    Utils::AutoCOM<ID3D11Resource> res = resPtr;

    T *out;
    HR(res->QueryInterface<T>(&out), D3DException("resource has no such interface"));

    return out;
}

template<class T>
inline T Cast(const Matrix4x4 &V);

template<>
inline D3DXMATRIX Cast<D3DXMATRIX>(const Matrix4x4 &V)
{
    D3DXMATRIX mOut;

    for(int32_t r = 0; r < 4; r++)
        for(int32_t c = 0; c < 4; c++)
            mOut.m[r][c] = V(r, c);

    return mOut;
}

template<class T>
inline T Cast(const POINT &Var){ return T(Var.x, Var.y);}

template<class T>
inline T Cast(const D3DXVECTOR2 &Var){return T(Var.x, Var.y);}

template<>
inline D3DXVECTOR3 Cast<D3DXVECTOR3>(const D3DXVECTOR2 &Var){ return {Var.x, Var.y, 0.0f}; }

template<>
inline D3DXVECTOR4 Cast<D3DXVECTOR4>(const D3DXVECTOR2 &Var){ return {Var.x, Var.y, 0.0f, 0.0f};}

template<class T>
inline T Cast(const D3DXVECTOR3 &Var){return T(Var.x, Var.y, Var.z);}

template<>
inline D3DXVECTOR2 Cast<D3DXVECTOR2>(const D3DXVECTOR3 &Var){ return {Var.x, Var.y};}

template<>
inline D3DXVECTOR4 Cast<D3DXVECTOR4>(const D3DXVECTOR3 &Var){ return {Var.x, Var.y, Var.z, 0.0f};}

template<class T>
inline T Cast(const D3DXVECTOR4 &Var){return T(Var.x, Var.y, Var.z, Var.w);}

template<>
inline D3DXVECTOR3 Cast<D3DXVECTOR3>(const D3DXVECTOR4 &Var){ return {Var.x, Var.y, Var.z};}

template<>
inline D3DXVECTOR2 Cast<D3DXVECTOR2>(const D3DXVECTOR4 &Var){ return {Var.x, Var.y};}

template<class T>
inline T Cast(const D3DXCOLOR &Color){ return T(Color.r, Color.g, Color.b, Color.a);}

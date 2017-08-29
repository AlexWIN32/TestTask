/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <COMHelpers.h>

namespace Utils
{

template<class TVar>
class SharedCOM
{
private:
    TVar *data = nullptr;
public:
    SharedCOM &operator= (const SharedCOM &Var)
    {
        ReleaseCOM(data);

        data = Var.data;

        if(data != nullptr)
            data->AddRef();

        return *this;
    }
    SharedCOM(const SharedCOM &Var)
    {
        data = Var.data;

        if(data != nullptr)
            data->AddRef();
    }
    SharedCOM(){}
    SharedCOM(TVar *Data){data = Data;}
    ~SharedCOM()
    {
        ReleaseCOM(data);
    }
    TVar* operator -> () const {return data;}
    operator TVar* () const {return data;}
    TVar* Get() const {return data;}
    void Release()
    {
        ReleaseCOM(data);
    }
};

};
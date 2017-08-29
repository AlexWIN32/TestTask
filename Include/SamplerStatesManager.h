/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <Utils/DirectX.h>
#include <Utils/SharedCOM.h>
#include <D3DHeaders.h>
#include <map>

namespace SamplerStates
{

class Manager
{
private:
    std::map<Utils::DirectX::SamplerStateDescription, Utils::SharedCOM<ID3D11SamplerState>> samplerStates;
    static Manager *instance;
    Manager(){}
public:
    static Manager *GetInstance()
    {
        if(instance == nullptr)
            instance = new Manager();
        return instance;
    }
    static void ReleaseInstance()
    {
        delete instance;
        instance = nullptr;
    }
    Utils::SharedCOM<ID3D11SamplerState> Get(const Utils::DirectX::SamplerStateDescription &Description);
};

inline Utils::SharedCOM<ID3D11SamplerState> Get(const Utils::DirectX::SamplerStateDescription &Description)
{
    return Manager::GetInstance()->Get(Description);
}

}
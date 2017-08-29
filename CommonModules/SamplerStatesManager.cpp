/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <SamplerStatesManager.h>

namespace SamplerStates
{

Manager *Manager::instance = nullptr;

Utils::SharedCOM<ID3D11SamplerState> Manager::Get(const Utils::DirectX::SamplerStateDescription &Description)
{
    auto it = samplerStates.find(Description);
    if(it != samplerStates.end())
        return it->second;

    Utils::SharedCOM<ID3D11SamplerState> state = Utils::DirectX::CreateSamplerState(Description);

    samplerStates.insert({Description, state});

    return state;
}

}
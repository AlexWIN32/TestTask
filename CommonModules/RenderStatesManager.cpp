/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <RenderStatesManager.h>
#include <DeviceKeeper.h>
#include <Utils/SharedCOM.h>
#include <map>
#include <memory>

namespace RenderStates
{

Manager *Manager::instance = nullptr;

void BlendState::Init(const BlendStateDescription &Desc) throw (Exception)
{
    sampleMask = Desc.sampleMask;

    FLOAT bf[] = {Desc.blendFactor.r, Desc.blendFactor.g, Desc.blendFactor.b, Desc.blendFactor.a};

    memcpy(blendFactor, bf, sizeof(FLOAT) * 4);

    ID3D11BlendState *bState;
    HR(DeviceKeeper::GetDevice()->CreateBlendState(&Desc.blendDescription, &bState));

    state = bState;
}

void BlendState::Apply()
{
    DeviceKeeper::GetDeviceContext()->OMSetBlendState(state, blendFactor, sampleMask);
}

void BlendState::Reset()
{
    DeviceKeeper::GetDeviceContext()->OMSetBlendState(nullptr, nullptr, 0xffffffff);
}

void DepthStencilState::Init(const DepthStencilDescription &Desc)
{
    ref = Desc.stencilRef;

    ID3D11DepthStencilState *sState;
    HR(DeviceKeeper::GetDevice()->CreateDepthStencilState(&Desc.stencilDescription, &sState));

    state = sState;
}

void DepthStencilState::Apply()
{
    DeviceKeeper::GetDeviceContext()->OMSetDepthStencilState(state, ref);
}

void DepthStencilState::Reset()
{
    DeviceKeeper::GetDeviceContext()->OMSetDepthStencilState(nullptr, 0);
}

void RasteriserState::Init(const D3D11_RASTERIZER_DESC &Desc)
{
    ID3D11RasterizerState *rState;
    HR(DeviceKeeper::GetDevice()->CreateRasterizerState(&Desc, &rState), D3DException("cant create rasteriser state"));

    state = rState;
}

void RasteriserState::Apply()
{
    DeviceKeeper::GetDeviceContext()->RSSetState(state);
}

void RasteriserState::Reset()
{
    DeviceKeeper::GetDeviceContext()->RSSetState(nullptr);
}

void Manager::CreateRenderState(const std::string &StateName, const DepthStencilDescription &DepthStencilDesc) throw (Exception)
{
    if(FindState(StateName))
        throw RenderStatesManagerException("State " + StateName + " is used");

    std::unique_ptr<DepthStencilState> statePtr(new DepthStencilState());

    statePtr->Init(DepthStencilDesc);

    renderStates.insert({StateName, statePtr.release()});
}

void Manager::CreateRenderState(const std::string &StateName, const BlendStateDescription &BlendDesc) throw (Exception)
{
    if(FindState(StateName))
        throw RenderStatesManagerException("State " + StateName + " is used");

    std::unique_ptr<BlendState> statePtr(new BlendState());

    statePtr->Init(BlendDesc);

    renderStates.insert({StateName, statePtr.release()});
}

void Manager::CreateRenderState(const std::string &StateName, const D3D11_RASTERIZER_DESC &RasteriserDesc) throw (Exception)
{
    if(FindState(StateName))
        throw RenderStatesManagerException("State " + StateName + " is used");

    std::unique_ptr<RasteriserState> statePtr(new RasteriserState());

    statePtr->Init(RasteriserDesc);

    renderStates.insert({StateName, statePtr.release()});
}

DepthStencilState Manager::CreateRenderState(const DepthStencilDescription &DepthStencilDesc) throw (Exception)
{
    DepthStencilState state;

    state.Init(DepthStencilDesc);

    return state;
}

BlendState Manager::CreateRenderState(const BlendStateDescription &BlendDesc) throw (Exception)
{
    BlendState state;

    state.Init(BlendDesc);

    return state;
}

RasteriserState Manager::CreateRenderState(const D3D11_RASTERIZER_DESC &RasteriserDesc) throw (Exception)
{
    RasteriserState state;

    state.Init(RasteriserDesc);

    return state;
}

void Manager::RemoveRenderState(const std::string &StateName)
{
    auto rsIt = renderStates.find(StateName);

    if(rsIt == renderStates.end())
        return;

    if(currDepthStencilState == StateName){
        rsIt->second->Reset();
        currDepthStencilState = "";
    }else if(currBlendState == StateName){
        rsIt->second->Reset();
        currBlendState = "";
    }else if(currRasteriserState == StateName){
        rsIt->second->Reset();
        currRasteriserState = "";
    }

    renderStates.erase(rsIt);
    delete rsIt->second;
}

std::string Manager::ApplyState(const std::string &StateName) throw (Exception)
{

    auto rsIt = renderStates.find(StateName);
    if(rsIt == renderStates.end())
        throw RenderStatesManagerException("State " + StateName + " not found");

    std::string *updatedState;

    if(dynamic_cast<BlendState*>(rsIt->second))
        updatedState = &currBlendState;
    
    if(dynamic_cast<DepthStencilState*>(rsIt->second))
        updatedState = &currDepthStencilState;

    if(dynamic_cast<RasteriserState*>(rsIt->second))
        updatedState = &currRasteriserState;

    rsIt->second->Apply();

    std::string prevState = *updatedState;

    *updatedState = StateName;

    return prevState;
}

void Manager::ProcessWithStates(const NamesStorage &StatesNames, const Procedure &Proc) throw (Exception)
{
    std::map<std::string, std::string> prevStates;

    for(const std::string &name : StatesNames){

        std::string prevState = ApplyState(name);

        prevStates.insert({name, prevState});
    }

    Proc();

    for(auto pair : prevStates){

        if(pair.second != "")
            ApplyState(pair.second);
        else{

            RenderState *renderState = renderStates[pair.first];

            renderState->Reset();

            if(dynamic_cast<BlendState*>(renderState))
                currBlendState = "";
    
            if(dynamic_cast<DepthStencilState*>(renderState))
                currDepthStencilState = "";

            if(dynamic_cast<RasteriserState*>(renderState))
                currRasteriserState = "";
        }
    }
}

bool Manager::FindState(const std::string &StateName) const
{
    return renderStates.find(StateName) != renderStates.end();
}

Manager::~Manager()
{
    for(auto pair : renderStates)
        delete pair.second;
}

}
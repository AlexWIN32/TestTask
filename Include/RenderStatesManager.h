/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <D3DHeaders.h>
#include <Utils/SharedCOM.h>
#include <Exception.h>
#include <string>
#include <unordered_map>
#include <functional>

DECLARE_EXCEPTION(RenderStatesManagerException)

namespace RenderStates
{

struct BlendStateDescription
{
    D3D11_BLEND_DESC blendDescription;
    ColorF blendFactor;
    UINT sampleMask;
    BlendStateDescription() : sampleMask(0)
    {
        memset(&blendDescription, 0, sizeof(D3D11_BLEND_DESC));
    }
    BlendStateDescription(const D3D11_BLEND_DESC &BlendDescription, ColorF BlendFactor, UINT SampleMask)
        : blendDescription(BlendDescription), blendFactor(BlendFactor), sampleMask(SampleMask)
    {}
};

struct DepthStencilDescription
{
    D3D11_DEPTH_STENCIL_DESC stencilDescription;
    UINT stencilRef;
    DepthStencilDescription() : stencilRef(0)
    {
        memset(&stencilDescription, 0, sizeof(D3D11_DEPTH_STENCIL_DESC));
    }
    DepthStencilDescription(const D3D11_DEPTH_STENCIL_DESC &StencilDescription, UINT StencilRef) 
        : stencilDescription(StencilDescription), stencilRef(StencilRef)
    {}
};

class RenderState
{
public:
    virtual ~RenderState(){}
    virtual void Apply() = 0;
    virtual void Reset() = 0;
};

class BlendState : public RenderState
{
private:
    Utils::SharedCOM<ID3D11BlendState> state;
    FLOAT blendFactor[4];
    UINT sampleMask = 0;
public:
    virtual ~BlendState(){}
    void Init(const BlendStateDescription &Desc) throw (Exception);
    virtual void Apply();
    virtual void Reset();
};

class DepthStencilState : public RenderState
{
private:
    Utils::SharedCOM<ID3D11DepthStencilState> state;
    UINT ref = 0;
public:
    virtual ~DepthStencilState(){}
    void Init(const DepthStencilDescription &Desc);
    virtual void Apply();
    virtual void Reset();
};

class RasteriserState : public RenderState
{
private:
    Utils::SharedCOM<ID3D11RasterizerState> state;
public:
    virtual ~RasteriserState(){}
    void Init(const D3D11_RASTERIZER_DESC &Desc);
    virtual void Apply();
    virtual void Reset();
};

class Manager
{
public:
    typedef std::vector<std::string> NamesStorage;
    typedef std::function<void()> Procedure;
private:
    std::unordered_map<std::string, RenderState*> renderStates;
    std::string currBlendState;
    std::string currDepthStencilState;
    std::string currRasteriserState;
    Manager(){}
    ~Manager();
    static Manager *instance;
public:
    static Manager *GetInstance()
    {
        if(!instance)
            instance = new Manager();
        return instance;
    }
    static void ReleaseInstance()
    {
        delete instance;
        instance = NULL;
    }   
    Manager(Manager &) = delete;
    Manager & operator=(Manager &) = delete;
    void CreateRenderState(const std::string &StateName, const DepthStencilDescription &DepthStencilDesc) throw (Exception);
    void CreateRenderState(const std::string &StateName, const BlendStateDescription &BlendDesc) throw (Exception);
	void CreateRenderState(const std::string &StateName, const D3D11_RASTERIZER_DESC &RasteriserDesc) throw (Exception);
    DepthStencilState CreateRenderState(const DepthStencilDescription &DepthStencilDesc) throw (Exception);
    BlendState CreateRenderState(const BlendStateDescription &BlendDesc) throw (Exception);
    RasteriserState CreateRenderState(const D3D11_RASTERIZER_DESC &RasteriserDesc) throw (Exception);
    std::string ApplyState(const std::string &StateName) throw (Exception);
    bool FindState(const std::string &StateName) const;
    void RemoveRenderState(const std::string &StateName);
    void ProcessWithStates(const NamesStorage &StatesNames, const Procedure &Proc) throw (Exception);
};

template<class TFunc>
inline void Apply(const std::vector<RenderState*> &RenderStates, TFunc Function)
{
    for(RenderState *state : RenderStates)
        state->Apply();

    Function();

    for(RenderState *state : RenderStates)
        state->Reset();
}

}
/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <DeviceKeeper.h>
#include <vector>
#include <D3DHeaders.h>
#include <Utils/VertexArray.h>
#include <Utils/AutoEvent.h>
#include <Utils/SharedCOM.h>
#include <functional>

namespace Utils
{

namespace DirectX
{

struct DSParams
{
    BOOL depthEnable = true, stencilEnable = false;
    D3D11_DEPTH_WRITE_MASK depthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    D3D11_COMPARISON_FUNC depthFunc = D3D11_COMPARISON_LESS;
    UINT8 stencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    UINT8 stencilWriteMask = D3D11_DEFAULT_STENCIL_READ_MASK;

    DSParams(){}
    DSParams(BOOL DepthEnable,
             BOOL StencilEnable,
             D3D11_DEPTH_WRITE_MASK DepthWriteMask,
             D3D11_COMPARISON_FUNC DepthFunc,
             UINT8 StencilReadMask,
             UINT8 StencilWriteMask)
        : depthEnable(DepthEnable),
          stencilEnable(StencilEnable),
          depthWriteMask(DepthWriteMask),
          depthFunc(DepthFunc),
          stencilReadMask(StencilReadMask),
          stencilWriteMask(StencilWriteMask)
    {}
};

struct DSOperation
{
    D3D11_STENCIL_OP stencilFail = D3D11_STENCIL_OP_KEEP;
    D3D11_STENCIL_OP stencilDepthFail = D3D11_STENCIL_OP_KEEP;
    D3D11_STENCIL_OP stencilPass = D3D11_STENCIL_OP_KEEP;
    D3D11_COMPARISON_FUNC stencilFunc = D3D11_COMPARISON_ALWAYS;
    DSOperation(){}
    DSOperation(D3D11_STENCIL_OP StencilFail,
                D3D11_STENCIL_OP StencilDepthFail,
                D3D11_STENCIL_OP StencilPass,
                D3D11_COMPARISON_FUNC StencilFunc)
        : stencilFail(StencilFail),
          stencilDepthFail(StencilDepthFail),
          stencilPass(StencilPass),
          stencilFunc(StencilFunc)
    {}
};

struct BlendParameters
{
    D3D11_BLEND source = D3D11_BLEND_ZERO;
    D3D11_BLEND destination = D3D11_BLEND_ZERO;
    D3D11_BLEND_OP operation = D3D11_BLEND_OP_ADD;

    BlendParameters(){}
    BlendParameters(D3D11_BLEND Source, D3D11_BLEND Destination, D3D11_BLEND_OP Operation)
        :source(Source), destination(Destination), operation(Operation){}
};

struct RenderTargetBlend
{
    BlendParameters color, alpha;
    BOOL enabled = false;
    UINT8 writeMask = 0;

    RenderTargetBlend(){}
    RenderTargetBlend(const BlendParameters &Color, const BlendParameters &Alpha, BOOL Enabled, UINT8 WriteMask)
        :color(Color), alpha(Alpha), enabled(Enabled), writeMask(WriteMask){}
};

inline D3D11_BLEND_DESC CreateBlendDescription(const std::vector<RenderTargetBlend> &RenderTargets, 
                                               BOOL AlphaToCoverageEnable = false,
                                               BOOL IndependentBlendEnable = false) throw (Exception)
{
    if(RenderTargets.size() > 8)
        throw D3DException("Invalid render states count");

    D3D11_BLEND_DESC blendDesc = {};

    for(UINT i = 0; i < RenderTargets.size(); i++){

        const RenderTargetBlend &rt = RenderTargets[i];

        blendDesc.RenderTarget[i].BlendEnable = rt.enabled;
        blendDesc.RenderTarget[i].SrcBlend = rt.color.source;
        blendDesc.RenderTarget[i].DestBlend = rt.color.destination;
        blendDesc.RenderTarget[i].BlendOp = rt.color.operation;
        blendDesc.RenderTarget[i].SrcBlendAlpha = rt.alpha.source;
        blendDesc.RenderTarget[i].DestBlendAlpha = rt.alpha.destination;
        blendDesc.RenderTarget[i].BlendOpAlpha = rt.alpha.operation;
        blendDesc.RenderTarget[i].RenderTargetWriteMask = rt.writeMask;
    }

    blendDesc.AlphaToCoverageEnable = AlphaToCoverageEnable;
    blendDesc.IndependentBlendEnable = IndependentBlendEnable;

    return blendDesc;
}

inline D3D11_DEPTH_STENCIL_DESC CreateDepthStencilDescription(const DSParams &Params,
                                                              const DSOperation &FrontFace,
                                                              const DSOperation &BackFace)
{
    D3D11_DEPTH_STENCIL_DESC sDesc;

    sDesc.DepthEnable = Params.depthEnable;
    sDesc.DepthWriteMask = Params.depthWriteMask;
    sDesc.DepthFunc = Params.depthFunc;

    sDesc.StencilEnable = Params.stencilEnable;
    sDesc.StencilReadMask = Params.stencilReadMask;
    sDesc.StencilWriteMask = Params.stencilWriteMask;

    sDesc.FrontFace.StencilFailOp = FrontFace.stencilFail;
    sDesc.FrontFace.StencilDepthFailOp = FrontFace.stencilDepthFail;
    sDesc.FrontFace.StencilPassOp = FrontFace.stencilPass;
    sDesc.FrontFace.StencilFunc = FrontFace.stencilFunc;

    sDesc.BackFace.StencilFailOp = BackFace.stencilFail;
    sDesc.BackFace.StencilDepthFailOp = BackFace.stencilDepthFail;
    sDesc.BackFace.StencilPassOp = BackFace.stencilPass;
    sDesc.BackFace.StencilFunc = BackFace.stencilFunc;

    return sDesc;
}

struct SamplerStateDescription
{
    D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    D3D11_TEXTURE_ADDRESS_MODE addressU = D3D11_TEXTURE_ADDRESS_WRAP;
    D3D11_TEXTURE_ADDRESS_MODE addressV = D3D11_TEXTURE_ADDRESS_WRAP;
    D3D11_TEXTURE_ADDRESS_MODE addressW = D3D11_TEXTURE_ADDRESS_WRAP;

    UINT maxAnisotropy = 1;

    D3D11_COMPARISON_FUNC comparisonFunc = D3D11_COMPARISON_ALWAYS;

    ColorF borderColor;

    FLOAT minLOD = 0, maxLOD = D3D11_FLOAT32_MAX;

    SamplerStateDescription(){}

    SamplerStateDescription(D3D11_FILTER Filter, D3D11_TEXTURE_ADDRESS_MODE AddressMode)
    {
        filter = Filter;
        addressU = addressV = addressW = AddressMode;
    }

    SamplerStateDescription(D3D11_FILTER Filter,
                            D3D11_TEXTURE_ADDRESS_MODE AddressMode,
                            D3D11_COMPARISON_FUNC ComparisonFunc)
    {
        filter = Filter;
        addressU = addressV = addressW = AddressMode;
        comparisonFunc = ComparisonFunc;
    }

    SamplerStateDescription(D3D11_FILTER Filter, ColorF BorderColor)
    {
        filter = Filter;
        addressU = addressV = addressW = D3D11_TEXTURE_ADDRESS_BORDER;
        borderColor = BorderColor;
    }

    bool operator< (const SamplerStateDescription &V) const
    {
        if(filter < V.filter)
            return true;
        else if(filter > V.filter)
            return false;

        if(addressU < V.addressU)
            return true;
        else if(addressU > V.addressU)
            return false;

        if(addressV < V.addressV)
            return true;
        else if(addressV > V.addressV)
            return false;

        if(addressW < V.addressW)
            return true;
        else if(addressW > V.addressW)
            return false;

        if(maxAnisotropy < V.maxAnisotropy)
            return true;
        else if(maxAnisotropy > V.maxAnisotropy)
            return false;

        if(comparisonFunc < V.comparisonFunc)
            return true;
        else if(comparisonFunc > V.comparisonFunc)
            return false;

        if(borderColor < V.borderColor)
            return true;
        else if(borderColor > V.borderColor)
            return false;

        if(minLOD < V.minLOD)
            return true;
        else if(minLOD > V.minLOD)
            return false;

        return maxLOD < V.maxLOD;
    }
};

inline ID3D11SamplerState* CreateSamplerState(const SamplerStateDescription &Description)
{
    FLOAT brdrCol[4] = {Description.borderColor.r, Description.borderColor.g, Description.borderColor.b, Description.borderColor.a};

    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = Description.filter;
    samplerDesc.AddressU = Description.addressU;
    samplerDesc.AddressV = Description.addressV;
    samplerDesc.AddressW = Description.addressW;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = Description.maxAnisotropy;
    samplerDesc.ComparisonFunc = Description.comparisonFunc;
    memcpy(samplerDesc.BorderColor, brdrCol, sizeof(UINT) * 4);
    samplerDesc.MinLOD = Description.minLOD;
    samplerDesc.MaxLOD = Description.maxLOD;

    ID3D11SamplerState *state;
    HR(DeviceKeeper::GetDevice()->CreateSamplerState(&samplerDesc, &state));

    return state;
}

inline ID3D11Buffer* CreateBuffer(const char* RawData,
                size_t BufferSize, 
                UINT BindFlags,
                D3D11_USAGE Usage = D3D11_USAGE_DEFAULT,
                UINT CPUAccessFlags = 0,
                UINT MiscFlags = 0,
                UINT StructureByteStride = 0) throw (Exception)
{
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = Usage;
    bd.ByteWidth = BufferSize;
    bd.BindFlags = BindFlags;
    bd.CPUAccessFlags = CPUAccessFlags;
    bd.MiscFlags = MiscFlags;
    bd.StructureByteStride = StructureByteStride;

    ID3D11Buffer* buffer;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = RawData;
    HR(DeviceKeeper::GetDevice()->CreateBuffer(&bd, &initData, &buffer));

    return buffer;
}

template<class TData> 
inline ID3D11Buffer* CreateBuffer(const std::vector<TData> &Data, 
            UINT BindFlags,
            D3D11_USAGE Usage = D3D11_USAGE_DEFAULT,
            UINT CPUAccessFlags = 0,
            UINT MiscFlags = 0,
            UINT StructureByteStride = 0) throw (Exception)
{
    return CreateBuffer(reinterpret_cast<const char*>(&Data[0]),
                        sizeof(TData) * Data.size(),
                        BindFlags,
                        Usage,
                        CPUAccessFlags,
                        MiscFlags,
                        StructureByteStride);
}

inline ID3D11Buffer* CreateBuffer(size_t BufferSize, 
                  UINT BindFlags,
                  D3D11_USAGE Usage = D3D11_USAGE_DEFAULT,
                  UINT CPUAccessFlags = 0,
                  UINT MiscFlags = 0,
                  UINT StructureByteStride = 0) throw (Exception)
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = Usage;
    bufferDesc.ByteWidth = BufferSize;
    bufferDesc.BindFlags = BindFlags;
    bufferDesc.CPUAccessFlags = CPUAccessFlags;
    bufferDesc.MiscFlags = MiscFlags;
    bufferDesc.StructureByteStride = StructureByteStride;

    ID3D11Buffer *buffer;
    HR(DeviceKeeper::GetDevice()->CreateBuffer(&bufferDesc, NULL, &buffer));

    return buffer;
}

inline ID3D11Buffer* CreateBuffer(const VertexArray &Array,
                  D3D11_USAGE Usage = D3D11_USAGE_DEFAULT,
                  UINT CPUAccessFlags = 0,
                  UINT MiscFlags = 0,
                  UINT StructureByteStride = 0) throw (Exception)
{
    return CreateBuffer(Array.GetRawData(),
                        Array.GetVerticesCount() * Array.GetVertixSize(),
                        D3D11_BIND_VERTEX_BUFFER,
                        Usage,
                        CPUAccessFlags,
                        MiscFlags,
                        StructureByteStride);
}

inline ID3D11Buffer* CopyBuffer(ID3D11Buffer *Buffer)
{
    D3D11_BUFFER_DESC desc;
    Buffer->GetDesc(&desc);

    ID3D11Buffer *newBuffer;
    HR(DeviceKeeper::GetDevice()->CreateBuffer(&desc, NULL, &newBuffer));

    DeviceKeeper::GetDeviceContext()->CopyResource(newBuffer, Buffer);

    return newBuffer;
}

template<class TData>
inline void Map(ID3D11Resource *Resource,
                      std::function<void(TData*)> Proc,
                      D3D11_MAP MappingType = D3D11_MAP_WRITE_DISCARD) throw (Exception)
{
    D3D11_MAPPED_SUBRESOURCE rawData;
    HR(DeviceKeeper::GetDeviceContext()->Map(Resource, 0, MappingType, 0, &rawData));

    TData *data = reinterpret_cast<TData*>(rawData.pData);

    Utils::AutoEvent unmapEvnt([&]{DeviceKeeper::GetDeviceContext()->Unmap(Resource, 0);});

    Proc(data);
}

inline UINT GetStructureCount(ID3D11UnorderedAccessView *UAW, ID3D11Buffer* statBuffer)
{
    DeviceKeeper::GetDeviceContext()->CopyStructureCount(statBuffer, 0, UAW);

    UINT data;
    Map<UINT>(statBuffer, [&](UINT *Data)
    {
        data = *Data;
    }, D3D11_MAP_READ);

    return data;
}

inline UINT GetStructureCount(ID3D11UnorderedAccessView *UAW)
{
    Utils::SharedCOM<ID3D11Buffer> stat = CreateBuffer(sizeof(UINT),
                                                       0,
                                                       D3D11_USAGE_STAGING,
                                                       D3D11_CPU_ACCESS_READ,
                                                       0,
                                                       0);

   return GetStructureCount(UAW, stat);
}

template<class TVal>
std::vector<TVal> GetData(ID3D11Buffer *Buffer, size_t ElemsCnt, ID3D11Buffer* ResultBuffer)
{
    DeviceKeeper::GetDeviceContext()->CopyResource(ResultBuffer, Buffer);

    std::vector<TVal> outData(ElemsCnt);

    Map<TVal>(ResultBuffer, [&](TVal *Data)
    {
        for(TVal &v : outData)
            v = *Data++;
    }, D3D11_MAP_READ);

    return outData;
}

template<class TVal>
std::vector<TVal> GetData(ID3D11Buffer *Buffer, size_t ElemsCnt)
{
    Utils::SharedCOM<ID3D11Buffer> result = CreateBuffer(sizeof(TVal) * ElemsCnt,
                                                         0,
                                                         D3D11_USAGE_STAGING,
                                                         D3D11_CPU_ACCESS_READ,
                                                         0,
                                                         0);

    return GetData<TVal>(Buffer, ElemsCnt, result);
}

inline void SetPrimitiveStream(const std::vector<ID3D11Buffer*> &VertexBuffers,
                               ID3D11Buffer *IndexBuffer,
                               std::vector<UINT> Strides,
                               D3D_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{

    std::vector<UINT> offsets(VertexBuffers.size(), 0);

    DeviceKeeper::GetDeviceContext()->IASetVertexBuffers(0, VertexBuffers.size(), &VertexBuffers[0], &Strides[0], &offsets[0]);
    DeviceKeeper::GetDeviceContext()->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    DeviceKeeper::GetDeviceContext()->IASetPrimitiveTopology(Topology);
}

inline ID3D11DepthStencilView* CreateDepthStencilView(ID3D11Resource *Resource)
{
    ID3D11DepthStencilView *outDsv;
    HR(DeviceKeeper::GetDevice()->CreateDepthStencilView(Resource, nullptr, &outDsv));
    return outDsv;
}

inline ID3D11DepthStencilView* CreateDepthStencilView(ID3D11Resource *Resource,
                                                      DXGI_FORMAT Format,
                                                      D3D11_DSV_DIMENSION Dimension,
                                                      const D3D11_TEX2D_DSV &Texture2Dparams,
                                                      UINT Flags = 0)
{
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = Flags;
    dsvDesc.Format = Format;
    dsvDesc.ViewDimension = Dimension;
    dsvDesc.Texture2D = Texture2Dparams;

    ID3D11DepthStencilView *outDsv;
    HR(DeviceKeeper::GetDevice()->CreateDepthStencilView(Resource, &dsvDesc, &outDsv));

    return outDsv;
}

inline ID3D11DepthStencilView* CreateDepthStencilView(ID3D11Resource *Resource,
                                                      DXGI_FORMAT Format,
                                                      D3D11_DSV_DIMENSION Dimension,
                                                      const D3D11_TEX2D_ARRAY_DSV &TextureArrayParams,
                                                      UINT Flags = 0)
{
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = Flags;
    dsvDesc.Format = Format;
    dsvDesc.ViewDimension = Dimension;
    dsvDesc.Texture2DArray = TextureArrayParams;

    ID3D11DepthStencilView *outDsv;
    HR(DeviceKeeper::GetDevice()->CreateDepthStencilView(Resource, &dsvDesc, &outDsv));

    return outDsv;
}

inline ID3D11ShaderResourceView* CreateShaderResourceView(ID3D11Resource *Resource)
{
    ID3D11ShaderResourceView *outSRV;
    HR(DeviceKeeper::GetDevice()->CreateShaderResourceView(Resource, 0, &outSRV));

    return outSRV;
}

inline ID3D11ShaderResourceView* CreateShaderResourceView(ID3D11Resource *Resource,
                                                          DXGI_FORMAT Format,
                                                          D3D11_SRV_DIMENSION Dimension,
                                                          const D3D11_TEX2D_SRV &Texture2Dparams)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = Format;
    srvDesc.ViewDimension = Dimension;
    srvDesc.Texture2D = Texture2Dparams;

    ID3D11ShaderResourceView *outSRV;
    HR(DeviceKeeper::GetDevice()->CreateShaderResourceView(Resource, &srvDesc, &outSRV));

    return outSRV;
}

inline ID3D11ShaderResourceView* CreateShaderResourceView(ID3D11Resource *Resource,
                                                          DXGI_FORMAT Format,
                                                          const D3D11_TEX2D_ARRAY_SRV &TextureArrayParams)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray = TextureArrayParams;

    ID3D11ShaderResourceView *outSRV;
    HR(DeviceKeeper::GetDevice()->CreateShaderResourceView(Resource, &srvDesc, &outSRV));

    return outSRV;
}

inline ID3D11ShaderResourceView* CreateShaderResourceView(ID3D11Resource *Resource,
                                                          const D3D11_BUFFEREX_SRV &RawBufferParams)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
    srvDesc.BufferEx = RawBufferParams;

    ID3D11ShaderResourceView *outSRV;
    HR(DeviceKeeper::GetDevice()->CreateShaderResourceView(Resource, &srvDesc, &outSRV));

    return outSRV;
}

inline ID3D11ShaderResourceView* CreateShaderResourceView(ID3D11Resource *Resource,
                                                          DXGI_FORMAT Format,
                                                          const D3D11_TEXCUBE_SRV &TextureCubeParams)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube = TextureCubeParams;

    ID3D11ShaderResourceView *outSRV;
    HR(DeviceKeeper::GetDevice()->CreateShaderResourceView(Resource, &srvDesc, &outSRV));

    return outSRV;
}

inline ID3D11UnorderedAccessView* CreateUnorderedAccessView(ID3D11Resource *Resource)
{
    ID3D11UnorderedAccessView *outUAV;
    HR(DeviceKeeper::GetDevice()->CreateUnorderedAccessView(Resource, nullptr, &outUAV));

    return outUAV;
}

inline ID3D11UnorderedAccessView* CreateUnorderedAccessView(ID3D11Resource *Resource,
                                                            const D3D11_BUFFER_UAV &BufferParams,
                                                            DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN)
{
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    uavDesc.Format = Format;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer = BufferParams;

    ID3D11UnorderedAccessView *outUAV;
    HR(DeviceKeeper::GetDevice()->CreateUnorderedAccessView(Resource, &uavDesc, &outUAV));

    return outUAV;
}

inline ID3D11UnorderedAccessView* CreateUnorderedAccessView(ID3D11Resource *Resource,
                                                            DXGI_FORMAT Format,
                                                            const D3D11_TEX2D_UAV &Tex2DParams)
{
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    uavDesc.Format = Format;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D = Tex2DParams;

    ID3D11UnorderedAccessView *outUAV;
    HR(DeviceKeeper::GetDevice()->CreateUnorderedAccessView(Resource, &uavDesc, &outUAV));

    return outUAV;
}

inline ID3D11RenderTargetView* CreateRenderTargetView(ID3D11Resource *Resource)
{
    ID3D11RenderTargetView *rtvPtr;
    HR(DeviceKeeper::GetDevice()->CreateRenderTargetView(Resource, nullptr, &rtvPtr));

    return rtvPtr;
}

inline ID3D11RenderTargetView* CreateRenderTargetView(ID3D11Resource *Resource,
                                                      DXGI_FORMAT Format,
                                                      const D3D11_TEX2D_RTV &Tex2DParams)
{
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;

    memset(&rtvDesc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

    D3D11_RTV_DIMENSION dimension;
    if(DeviceKeeper::GetMultisampleCount() == 1)
        dimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    else
        dimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

    rtvDesc.Format = Format;
    rtvDesc.ViewDimension = dimension;
    rtvDesc.Texture2D = Tex2DParams;

    ID3D11RenderTargetView *rtvPtr;
    HR(DeviceKeeper::GetDevice()->CreateRenderTargetView(Resource, &rtvDesc, &rtvPtr));

    return rtvPtr;
}

inline ID3D11RenderTargetView* CreateRenderTargetView(ID3D11Resource *Resource,
                                                      DXGI_FORMAT Format,
                                                      const D3D11_TEX2D_ARRAY_RTV &Tex2DArrayParams)
{
    D3D11_RTV_DIMENSION dimension;
    if(DeviceKeeper::GetMultisampleCount() == 1)
        dimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
    else
        dimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = Format;
    rtvDesc.ViewDimension = dimension;
    rtvDesc.Texture2DArray = Tex2DArrayParams;

    ID3D11RenderTargetView *rtvPtr;
    HR(DeviceKeeper::GetDevice()->CreateRenderTargetView(Resource, &rtvDesc, &rtvPtr));

    return rtvPtr;

}

}

}
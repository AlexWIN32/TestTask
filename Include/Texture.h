/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <Exception.h>
#include <windows.h>
#include <functional>
#include <Vector2.h>
#include <Utils/SharedCOM.h>
#include <vector>
#include <memory>

struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11Texture2D;
struct D3D11_VIEWPORT;

enum DXGI_FORMAT;
enum D3D11_USAGE;

namespace Texture
{

DECLARE_EXCEPTION(TextureException);

typedef std::function<void(UCHAR*, const POINT &)> PixelOperator;

struct SubresourceData
{
    const void *data = nullptr;
    UINT pitch = 0;
    UINT slice = 0;
    SubresourceData(){}
    SubresourceData(const void *Data, UINT Pitch, UINT Slice)
        :data(Data), pitch(Pitch), slice(Slice){}
};

ID3D11Texture2D *CreateTexture2D(const SizeUS &Size,
                                 UINT MipLevels,
                                 const std::vector<SubresourceData> &Subresources,
                                 DXGI_FORMAT Format,
                                 UINT SampleDescCount,
                                 UINT SampleDescQuality,
                                 D3D11_USAGE Usage,
                                 UINT BindFlags,
                                 UINT CPUAccessFlags,
                                 UINT MiscFlags) throw (Exception);

ID3D11Texture2D *CreateTexture2D(const SizeUS &Size,
                                 UINT MipLevels,
                                 UINT ArraySize,
                                 DXGI_FORMAT Format,
                                 UINT SampleDescCount,
                                 UINT SampleDescQuality,
                                 D3D11_USAGE Usage,
                                 UINT BindFlags,
                                 UINT CPUAccessFlags,
                                 UINT MiscFlags) throw (Exception);

ID3D11ShaderResourceView * LoadTexture2DFromFile(const std::wstring &FileName) throw (Exception);
ID3D11ShaderResourceView * LoadTexture2DFromFile(const std::string &FileName) throw (Exception);
ID3D11ShaderResourceView * LoadTexture2DFromFile(const std::wstring &Path, const Point2US &Pos, const SizeUS &RegionSize) throw (Exception);
ID3D11ShaderResourceView * CreateTexture2D(const SizeUS &Size, DXGI_FORMAT Format, const char* Data) throw (Exception);
ID3D11ShaderResourceView * CreateTexture2D(const SizeUS &Size, DXGI_FORMAT Format, const PixelOperator &Operator) throw (Exception);
ID3D11ShaderResourceView * CreateTextureCube(const SizeUS &Size, DXGI_FORMAT Format, const std::vector<const char*> &Data) throw (Exception);
ID3D11ShaderResourceView * LoadTextureCubeFromFile(const std::wstring &Path) throw (Exception);
ID3D11ShaderResourceView * LoadTextureArrayFromFiles(const std::vector<std::wstring> &Files) throw (Exception);

DECLARE_EXCEPTION(RenderTargetException);

class RenderTarget
{
private:
    Utils::SharedCOM<ID3D11RenderTargetView> rtv;
    Utils::SharedCOM<ID3D11ShaderResourceView> srv;
    std::shared_ptr<D3D11_VIEWPORT> viewport;
    SizeUS size;
    DXGI_FORMAT format;
public:
    RenderTarget();
    const SizeUS &GetSize() const {return size;}
    DXGI_FORMAT GetFormat() const {return format;}
    const D3D11_VIEWPORT &GetViewport() const {return *viewport.get();}
    ID3D11RenderTargetView *GetRenderTargetView() const {return rtv;}
    ID3D11ShaderResourceView *GetSahderResourceView() const {return srv;}
    void Init(DXGI_FORMAT Format, BOOL UseViewport = false, INT ViewportInd = -1) throw (Exception);
    void Init(DXGI_FORMAT Format, const SizeUS &Size) throw (Exception);
};

class DepthStencil
{
private:
    Utils::SharedCOM<ID3D11DepthStencilView> dsv;
    Utils::SharedCOM<ID3D11ShaderResourceView> srv;
    std::shared_ptr<D3D11_VIEWPORT> viewport;
    SizeUS size;
    DXGI_FORMAT textureFormat;
    DXGI_FORMAT dsvFormat;
    DXGI_FORMAT srvFormat;
public:
    DepthStencil();
    const SizeUS &GetSize() const {return size;}
    DXGI_FORMAT GetTextureFormat() const {return textureFormat;}
    DXGI_FORMAT GetDSVFormat() const {return dsvFormat;}
    DXGI_FORMAT GetSRVFormat() const {return srvFormat;}
    const D3D11_VIEWPORT &GetViewport() const {return *viewport.get();}
    ID3D11DepthStencilView *GetDepthStencilView() const {return dsv;}
    ID3D11ShaderResourceView *GetSahderResourceView() const {return srv;}
    void Init(DXGI_FORMAT TextureFormat,
              DXGI_FORMAT SRVFormat,
              DXGI_FORMAT DSVFormat,
              SizeUS Size) throw (Exception);
};

class RenderTargetCube
{
private:
    Utils::SharedCOM<ID3D11RenderTargetView> rtv;
    Utils::SharedCOM<ID3D11RenderTargetView> edgeRtv[6];
    Utils::SharedCOM<ID3D11ShaderResourceView> edgeSrv[6];
    Utils::SharedCOM<ID3D11ShaderResourceView> srv;
    std::shared_ptr<D3D11_VIEWPORT> viewport;
    SizeUS size;
    DXGI_FORMAT format;
public:
    RenderTargetCube();
    const SizeUS &GetSize() const {return size;}
    DXGI_FORMAT GetFormat() const {return format;}
    const D3D11_VIEWPORT &GetViewport() const {return *viewport.get();}
    void Init(DXGI_FORMAT Format, USHORT Size, UINT MipLevelsCnt = 1) throw (Exception);
    ID3D11RenderTargetView *GetRenderTargetView(UINT Edge) const throw (Exception);
    ID3D11ShaderResourceView *GetShaderResourceView(UINT Edge) const throw (Exception);
    ID3D11RenderTargetView *GetRenderTargetView() const {return rtv;}
    ID3D11ShaderResourceView *GetShaderResourceView() const {return srv;}
};

class DepthStencilCube
{
private:
    Utils::SharedCOM<ID3D11RenderTargetView> rtv;
public:
    Utils::SharedCOM<ID3D11DepthStencilView> dsv;
    Utils::SharedCOM<ID3D11DepthStencilView> edgeDsv[6];
    Utils::SharedCOM<ID3D11ShaderResourceView> edgeSrv[6];
    Utils::SharedCOM<ID3D11ShaderResourceView> srv;
    std::shared_ptr<D3D11_VIEWPORT> viewport;
    SizeUS size;
    DXGI_FORMAT textureFormat;
    DXGI_FORMAT dsvFormat;
    DXGI_FORMAT srvFormat;
public:
    DepthStencilCube();
    const SizeUS &GetSize() const {return size;}
    DXGI_FORMAT GetTextureFormat() const {return textureFormat;}
    DXGI_FORMAT GetDSVFormat() const {return dsvFormat;}
    DXGI_FORMAT GetSRVFormat() const {return srvFormat;}
    const D3D11_VIEWPORT &GetViewport() const {return *viewport.get();}
    ID3D11DepthStencilView *GetDepthStencilView() const {return dsv;}
    ID3D11DepthStencilView *GetDepthStencilView(UINT Edge) const throw (Exception);
    ID3D11ShaderResourceView *GetSahderResourceView() const {return srv;}
    ID3D11ShaderResourceView *GetSahderResourceView(UINT Edge) const throw (Exception);
    void Init(DXGI_FORMAT TextureFormat,
              DXGI_FORMAT SRVFormat,
              DXGI_FORMAT DSVFormat,
              SizeUS Size) throw (Exception);
};

}
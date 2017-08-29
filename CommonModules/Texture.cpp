/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <D2DBaseTypes.h>
typedef D3DCOLORVALUE DXGI_RGBA;

#include <Texture.h>
#include "dxgiformat.h"
#include <D3DHeaders.h>
#include <DirectXTex.h>
#include <DeviceKeeper.h>
#include <CommonParams.h>
#include <Vector2.h>
#include <vector>
#include <Utils/AutoCOM.h>
#include <Utils/DirectX.h>
#include <Utils/ToString.h>
using namespace DirectX;

namespace Texture
{

#ifdef LoadImage
#undef LoadImage
#endif

static void LoadImage(const std::wstring &FilePath, ScratchImage &Image, TexMetadata &ImageInfo) throw (Exception)
{
    size_t dotPos = FilePath.find_last_of('.');
    if (dotPos == std::string::npos)
        throw TextureException(L"Unknown extension for " + FilePath);

    std::wstring ext = FilePath.substr(dotPos + 1);

    if (ext == L"DDS" || ext == L"dds")
        HR(LoadFromDDSFile(FilePath.c_str(), DDS_FLAGS_NONE, &ImageInfo, Image));
    else if (ext == L"tga" || ext == L"TGA")
        HR(LoadFromTGAFile(FilePath.c_str(), &ImageInfo, Image));
    else
        HR(LoadFromWICFile(FilePath.c_str(), 0, &ImageInfo, Image));
}

ID3D11Texture2D *CreateTexture2D(const SizeUS &Size,
                                 UINT MipLevels,
                                 UINT ArraySize,
                                 DXGI_FORMAT Format,
                                 UINT SampleDescCount,
                                 UINT SampleDescQuality,
                                 D3D11_USAGE Usage,
                                 UINT BindFlags,
                                 UINT CPUAccessFlags,
                                 UINT MiscFlags) throw (Exception)
{
    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Width = Size.width;
    textureDesc.Height = Size.height;
    textureDesc.MipLevels = MipLevels;
    textureDesc.ArraySize = ArraySize;
    textureDesc.Format = Format;
    textureDesc.SampleDesc.Count = SampleDescCount;
    textureDesc.SampleDesc.Quality = SampleDescQuality;
    textureDesc.Usage = Usage;
    textureDesc.BindFlags = BindFlags;
    textureDesc.CPUAccessFlags = CPUAccessFlags;
    textureDesc.MiscFlags = MiscFlags;

    ID3D11Texture2D* texture;
    HR(DeviceKeeper::GetDevice()->CreateTexture2D(&textureDesc, NULL, &texture));

    return texture;
}

ID3D11Texture2D *CreateTexture2D(const SizeUS &Size,
                                 UINT MipLevels,
                                 const std::vector<SubresourceData> &Subresources,
                                 DXGI_FORMAT Format,
                                 UINT SampleDescCount,
                                 UINT SampleDescQuality,
                                 D3D11_USAGE Usage,
                                 UINT BindFlags,
                                 UINT CPUAccessFlags,
                                 UINT MiscFlags) throw (Exception)
{
    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Width = Size.width;
    textureDesc.Height = Size.height;
    textureDesc.MipLevels = MipLevels;
    textureDesc.ArraySize = Subresources.size();
    textureDesc.Format = Format;
    textureDesc.SampleDesc.Count = SampleDescCount;
    textureDesc.SampleDesc.Quality = SampleDescQuality;
    textureDesc.Usage = Usage;
    textureDesc.BindFlags = BindFlags;
    textureDesc.CPUAccessFlags = CPUAccessFlags;
    textureDesc.MiscFlags = MiscFlags;

    std::vector<D3D11_SUBRESOURCE_DATA> pData;

    for(const SubresourceData &s : Subresources){

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = s.data;
        data.SysMemPitch = s.pitch;
        data.SysMemSlicePitch = s.slice;

        pData.push_back(data);
    }

    ID3D11Texture2D* texPtr = nullptr;
    HR(DeviceKeeper::GetDevice()->CreateTexture2D(&textureDesc, &pData[0], &texPtr));

    return texPtr;
}

ID3D11ShaderResourceView * LoadTexture2DFromFile(const std::wstring &FileName) throw (Exception)
{
	ScratchImage img;
	TexMetadata info;

    LoadImage(FileName, img, info);

    if (info.depth > 1 || info.arraySize > 1)
         throw TextureException(FileName + L": cant load cubemap or 3D texture");

    Utils::AutoCOM<ID3D11Texture2D> texturePtr = CreateTexture2D(SizeUS(info.width, info.height),
                                                                 info.mipLevels,
                                                                 1,//ArraySize
                                                                 info.format,
                                                                 1,//sample desc count
                                                                 0,//sample desc quality
                                                                 D3D11_USAGE_DEFAULT,
                                                                 D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
                                                                 0,//cpu access flags
                                                                 D3D11_RESOURCE_MISC_GENERATE_MIPS);

	size_t rowPitch, slicePitch;
	ComputePitch(info.format, info.width, info.height, rowPitch, slicePitch);
	
	const Image *img2 = img.GetImage(0, 0, 0);
	DeviceKeeper::GetDeviceContext()->UpdateSubresource(texturePtr, 0, NULL, img2->pixels, rowPitch, 0);

    ID3D11ShaderResourceView* textureSRV = Utils::DirectX::CreateShaderResourceView(texturePtr,
                                                    info.format,
                                                    D3D11_SRV_DIMENSION_TEXTURE2D,
                                                    {0, -1});
	
	DeviceKeeper::GetDeviceContext()->GenerateMips(textureSRV);

	return textureSRV;
}

ID3D11ShaderResourceView * LoadTexture2DFromFile(const std::string &FileName) throw (Exception)
{
    return LoadTexture2DFromFile(Utils::ToWString(FileName));
}

ID3D11ShaderResourceView* LoadTexture2DFromFile(const std::wstring &Path, const Point2US &Pos, const SizeUS &RegionSize)
{
    DirectX::ScratchImage img;
    DirectX::TexMetadata info;

    LoadImage(Path, img, info);

    if (info.depth > 1 || info.arraySize > 1)
        throw TextureException(Path + L": cant load cubemap or 3D texture");

    if(Pos.x > info.width)
        throw IOException("invalid pos x");

    if(Pos.y > info.height)
        throw IOException("invalid pos y");

    if(RegionSize.width == 0 || RegionSize.width > info.width)
        throw IOException("invalid size width");

    if(RegionSize.height == 0 || RegionSize.height > info.height)
        throw IOException("invalid size height");

    const DirectX::Image* img2 = img.GetImage(0,0,0);
    UCHAR* pixelsData = img2->pixels;

    size_t rowPitch, slicePitch;
    DirectX::ComputePitch(info.format, info.width, info.height, rowPitch, slicePitch);
    
    std::vector<UINT> srwData(RegionSize.width * RegionSize.height);

    for(USHORT y = Pos.y; y < Pos.y + RegionSize.height; y++)
        for(USHORT x = Pos.x; x < Pos.x + RegionSize.width; x++){
            UCHAR* pixel = &pixelsData[rowPitch * y + sizeof(UINT) * x];

            UINT index = (y - Pos.y) * RegionSize.width + (x - Pos.x);
            srwData[index] = *reinterpret_cast<UINT*>(pixel);
        }

    return Texture::CreateTexture2D(RegionSize,
                                   DXGI_FORMAT_R8G8B8A8_UNORM,
                                   reinterpret_cast<CHAR*>(&srwData[0]));
}

ID3D11ShaderResourceView * CreateTexture2D(const SizeUS &Size, DXGI_FORMAT Format, const char* Data) throw (Exception)
{
    size_t rowPitch, slicePitch;
    DirectX::ComputePitch(Format, Size.width, Size.height, rowPitch, slicePitch);

    std::vector<SubresourceData> subresources = {{Data, rowPitch, 0}};
    Utils::AutoCOM<ID3D11Texture2D> tex = CreateTexture2D(Size, 1, subresources, Format, 1, 0, D3D11_USAGE_IMMUTABLE,
                                                          D3D11_BIND_SHADER_RESOURCE, 0, 0);

    return Utils::DirectX::CreateShaderResourceView(tex);
}

ID3D11ShaderResourceView * CreateTexture2D(const SizeUS &Size, DXGI_FORMAT Format, const PixelOperator &Operator) throw (Exception)
{
    size_t rowPitch, slicePitch;
    ComputePitch(Format, Size.width, Size.height, rowPitch, slicePitch);

    UINT pixelSize = rowPitch / Size.height;

    std::vector<UCHAR> data(slicePitch);

    for(UINT y = 0; y < Size.height; y++)
        for(UINT x = 0; x < Size.width; x++){

            UCHAR *px = &data[(y * Size.width + x) * pixelSize];

            POINT pt;
            pt.x = x;
            pt.y = y;

            Operator(px, pt);
        }
    
    return Texture::CreateTexture2D(Size, Format, reinterpret_cast<CHAR*>(&data[0]));
}

ID3D11ShaderResourceView * CreateTextureCube(const SizeUS &Size, DXGI_FORMAT Format, const std::vector<const char*> &Data) throw (Exception)
{
    size_t rowPitch, slicePitch;
    ComputePitch(Format, Size.width, Size.height, rowPitch, slicePitch);

    std::vector<SubresourceData> subresources(6);

    for(int32_t f = 0; f < 6; f++)
        subresources[f] = {Data[f], rowPitch, 0};

    Utils::AutoCOM<ID3D11Texture2D> tex = CreateTexture2D(Size, 1, subresources, Format, 1, 0, D3D11_USAGE_DEFAULT,
                                                        D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE);

    return Utils::DirectX::CreateShaderResourceView(tex, Format, D3D11_TEXCUBE_SRV{0, 1});
}

ID3D11ShaderResourceView * LoadTextureCubeFromFile(const std::wstring &Path) throw (Exception)
{
    ScratchImage img;
    TexMetadata info;

    LoadImage(Path, img, info);

    if(info.arraySize != 6)
        throw TextureException(Path + L" is not a cube texture");

    size_t rowPitch, slicePitch;
    ComputePitch(info.format, info.width, info.height, rowPitch, slicePitch);

    std::vector<SubresourceData> subresources(6);
    for(int32_t f = 0; f < 6; f++)
        subresources[f] = {img.GetImage(0, f, 0)->pixels, rowPitch, slicePitch};

    Utils::AutoCOM<ID3D11Texture2D> tex = CreateTexture2D(SizeUS(info.width, info.height),
                                                          1,
                                                          subresources, info.format,
                                                          1,
                                                          0,
                                                          D3D11_USAGE_DEFAULT,
                                                          D3D11_BIND_SHADER_RESOURCE,
                                                          0,
                                                          D3D11_RESOURCE_MISC_TEXTURECUBE);

    return Utils::DirectX::CreateShaderResourceView(tex, info.format, D3D11_TEXCUBE_SRV{0, 1});
}

ID3D11ShaderResourceView * LoadTextureArrayFromFiles(const std::vector<std::wstring> &Files) throw (Exception)
{
    if(Files.size() == 0)
        throw TextureException("files set is empty");

    std::vector<Utils::SharedCOM<ID3D11Texture2D>> textures;

    for(const std::wstring &file : Files){

        Utils::SharedCOM<ID3D11ShaderResourceView> srw = LoadTexture2DFromFile(file);

        textures.push_back(Cast<ID3D11Texture2D>(srw));
    }

    D3D11_TEXTURE2D_DESC fD;
    textures[0]->GetDesc(&fD);

    for(ID3D11Texture2D *tex : textures){

        D3D11_TEXTURE2D_DESC d;
        tex->GetDesc(&d);

        if(d.Format != fD.Format || d.Width != fD.Width || d.Height != fD.Height)
           throw TextureException("textures must have same format and dimension");
    }

    Utils::SharedCOM<ID3D11Texture2D> tex = CreateTexture2D(SizeUS(fD.Width, fD.Height),
                                                            fD.MipLevels,
                                                            textures.size(),
                                                            fD.Format,
                                                            1,//sample desc count
                                                            0,//sample desc quality
                                                            D3D11_USAGE_DEFAULT,
                                                            D3D11_BIND_SHADER_RESOURCE,
                                                            0,//cpu access flags
                                                            0);//misc flags

    for(UINT i = 0; i < textures.size(); ++i){

        for(UINT j = 0; j < fD.MipLevels; ++j){

            UINT dstSubresource = D3D11CalcSubresource(j, i, fD.MipLevels);

            DeviceKeeper::GetDeviceContext()->CopySubresourceRegion(tex, dstSubresource, 0, 0, 0, textures[i], j, nullptr);
        }
    }

    D3D11_TEX2D_ARRAY_SRV srvDesc = {};
    srvDesc.MipLevels = fD.MipLevels;
    srvDesc.ArraySize = textures.size();

    return Utils::DirectX::CreateShaderResourceView(tex, fD.Format, srvDesc);
}

RenderTargetCube::RenderTargetCube() : srv(NULL), format(DXGI_FORMAT_UNKNOWN)
{
}

void RenderTargetCube::Init(DXGI_FORMAT Format, USHORT Size, UINT MipLevelsCnt) throw (Exception)
{
    size = {Size, Size};
    format = Format;

    viewport.reset(new D3D11_VIEWPORT());

    viewport->TopLeftX = 0.0f;
    viewport->TopLeftY = 0.0f;
    viewport->Width    = static_cast<float>(Size);
    viewport->Height   = static_cast<float>(Size);
    viewport->MinDepth = 0.0f;
    viewport->MaxDepth = 1.0f;

    UINT miscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    if(MipLevelsCnt == 0)
        miscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

    Utils::AutoCOM<ID3D11Texture2D> cubeTex = CreateTexture2D({Size, Size},
                                                              MipLevelsCnt,
                                                              6,//ArraySize
                                                              Format,
                                                              1,//sample desc count
                                                              0,//sample desc quality
                                                              D3D11_USAGE_DEFAULT,
                                                              D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
                                                              0,//cpu access flags
                                                              miscFlags);

    for(int i = 0; i < 6; ++i)
        edgeRtv[i] = Utils::DirectX::CreateRenderTargetView(cubeTex, Format, D3D11_TEX2D_ARRAY_RTV{0, i, 1});

    for(int i = 0; i < 6; ++i)
        edgeSrv[i] = Utils::DirectX::CreateShaderResourceView(cubeTex, Format, D3D11_TEX2D_ARRAY_SRV{0, 1, i, 1});

    rtv = Utils::DirectX::CreateRenderTargetView(cubeTex, Format, D3D11_TEX2D_ARRAY_RTV{0, 0, 6});

    UINT mipLvlsCnt = (MipLevelsCnt == 0) ? -1 : MipLevelsCnt;
    srv = Utils::DirectX::CreateShaderResourceView(cubeTex, Format, D3D11_TEXCUBE_SRV{0, mipLvlsCnt});
}

ID3D11RenderTargetView *RenderTargetCube::GetRenderTargetView(UINT Edge) const throw (Exception)
{
    if(Edge >= 6)
        throw RenderTargetException("Invalid render target cub edge");

    return edgeRtv[Edge];
}

ID3D11ShaderResourceView *RenderTargetCube::GetShaderResourceView(UINT Edge) const throw (Exception)
{
    if(Edge >= 6)
        throw RenderTargetException("Invalid render target cub edge");

    return edgeSrv[Edge];
}

RenderTarget::RenderTarget() : rtv(nullptr), srv(nullptr), format(DXGI_FORMAT_UNKNOWN){}

void RenderTarget::Init(DXGI_FORMAT Format, BOOL UseViewport, INT ViewportInd) throw (Exception)
{
    SizeF viewportSize(CommonParams::GetScreenWidth(), CommonParams::GetScreenHeight());

    if(UseViewport){
        UINT viewportsCnt = 0;
        DeviceKeeper::GetDeviceContext()->RSGetViewports(&viewportsCnt, NULL);

        if(!viewportsCnt)
            throw TextureException("Viewports not set");

        std::vector<D3D11_VIEWPORT> viewpors;
        viewpors.resize(viewportsCnt);

        DeviceKeeper::GetDeviceContext()->RSGetViewports(&viewportsCnt, static_cast<D3D11_VIEWPORT*>(&viewpors[0]));

        D3D11_VIEWPORT viewport;
        if(ViewportInd == -1)
            viewport = viewpors[0];
        else{
            if(ViewportInd >= viewportsCnt)
                throw TextureException("Invalid viewport index");
            viewport = viewpors[ViewportInd];
        }

        viewportSize.width = viewport.Width;
        viewportSize.height = viewport.Height;
    }

    Init(Format, Cast<SizeUS>(viewportSize));
}

void RenderTarget::Init(DXGI_FORMAT Format, const SizeUS &Size) throw (Exception)
{
    size = Size;
    format = Format;

    viewport.reset(new D3D11_VIEWPORT());

    viewport->TopLeftX = 0.0f;
    viewport->TopLeftY = 0.0f;
    viewport->Width    = static_cast<float>(Size.width);
    viewport->Height   = static_cast<float>(Size.height);
    viewport->MinDepth = 0.0f;
    viewport->MaxDepth = 1.0f;

    Utils::AutoCOM<ID3D11Texture2D> texture = CreateTexture2D(Size,
                                                              1,//Mip levels
                                                              1,//Array size
                                                              Format,
                                                              DeviceKeeper::GetMultisampleCount(),
                                                              DeviceKeeper::GetMultisampleQuality() - 1,
                                                              D3D11_USAGE_DEFAULT,
                                                              D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
                                                              0,//cpu access flags
                                                              0);//misc flags

    rtv = Utils::DirectX::CreateRenderTargetView(texture, Format, D3D11_TEX2D_RTV{0});

    D3D_SRV_DIMENSION dimension;
    if(DeviceKeeper::GetMultisampleCount() == 1)
        dimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    else
        dimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;

    srv = Utils::DirectX::CreateShaderResourceView(texture, Format, dimension, D3D11_TEX2D_SRV{0, 1});
}

DepthStencil::DepthStencil() : 
            textureFormat(DXGI_FORMAT_UNKNOWN),
            dsvFormat(DXGI_FORMAT_UNKNOWN),
            srvFormat(DXGI_FORMAT_UNKNOWN)
{}

void DepthStencil::Init(DXGI_FORMAT TextureFormat,
                        DXGI_FORMAT SRVFormat,
                        DXGI_FORMAT DSVFormat,
                        SizeUS Size) throw (Exception)
{
    size = Size;

    textureFormat = TextureFormat;
    dsvFormat = DSVFormat;
    srvFormat = SRVFormat;

    viewport.reset(new D3D11_VIEWPORT());

    viewport->TopLeftX = 0.0f;
    viewport->TopLeftY = 0.0f;
    viewport->Width    = static_cast<float>(size.width);
    viewport->Height   = static_cast<float>(size.height);
    viewport->MinDepth = 0.0f;
    viewport->MaxDepth = 1.0f;

    Utils::AutoCOM<ID3D11Texture2D> texture = CreateTexture2D(Size,
                                                            1,//Mip levels
                                                            1,//Array size
                                                            TextureFormat,
                                                            DeviceKeeper::GetMultisampleCount(),
                                                            DeviceKeeper::GetMultisampleQuality() - 1,
                                                            D3D11_USAGE_DEFAULT,
                                                            D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,
                                                            0,//cpu access flags
                                                            0);//misc flags

    D3D_SRV_DIMENSION srvDimension;
    D3D11_DSV_DIMENSION dsvDimension;

    if(DeviceKeeper::GetMultisampleCount() == 1){
        srvDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        dsvDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    }else{
        srvDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
        dsvDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    }

    dsv = Utils::DirectX::CreateDepthStencilView(texture, DSVFormat, dsvDimension, D3D11_TEX2D_DSV{0});

    srv = Utils::DirectX::CreateShaderResourceView(texture, SRVFormat, srvDimension, D3D11_TEX2D_SRV{0, 1});
}

ID3D11DepthStencilView *DepthStencilCube::GetDepthStencilView(UINT Edge) const throw (Exception)
{
    if(Edge >= 6)
        throw RenderTargetException("Invalid depth stencil cube edge");

    return edgeDsv[Edge];
}

DepthStencilCube::DepthStencilCube() : 
            textureFormat(DXGI_FORMAT_UNKNOWN),
            dsvFormat(DXGI_FORMAT_UNKNOWN),
            srvFormat(DXGI_FORMAT_UNKNOWN)
{}

void DepthStencilCube::Init(DXGI_FORMAT TextureFormat,
                            DXGI_FORMAT SRVFormat,
                            DXGI_FORMAT DSVFormat,
                            SizeUS Size) throw (Exception)
{
    size = Size;

    textureFormat = TextureFormat;
    dsvFormat = DSVFormat;
    srvFormat = SRVFormat;

    viewport.reset(new D3D11_VIEWPORT());

    viewport->TopLeftX = 0.0f;
    viewport->TopLeftY = 0.0f;
    viewport->Width    = static_cast<float>(size.width);
    viewport->Height   = static_cast<float>(size.height);
    viewport->MinDepth = 0.0f;
    viewport->MaxDepth = 1.0f;

    Utils::AutoCOM<ID3D11Texture2D> texture = CreateTexture2D(Size,
                                                            1,//Mip levels
                                                            6,//Array size
                                                            TextureFormat,
                                                            DeviceKeeper::GetMultisampleCount(),
                                                            DeviceKeeper::GetMultisampleQuality() - 1,
                                                            D3D11_USAGE_DEFAULT,
                                                            D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,
                                                            0,//cpu access flags
                                                            D3D11_RESOURCE_MISC_TEXTURECUBE);
    D3D11_DSV_DIMENSION dsvDimension;

    if(DeviceKeeper::GetMultisampleCount() == 1)
        dsvDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    else
        dsvDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;

    for(int i = 0; i < 6; ++i)
        edgeDsv[i] = Utils::DirectX::CreateDepthStencilView(texture, DSVFormat, dsvDimension, D3D11_TEX2D_ARRAY_DSV{0, i, 1});

    for(int i = 0; i < 6; ++i)
        edgeSrv[i] = Utils::DirectX::CreateShaderResourceView(texture, SRVFormat, D3D11_TEX2D_ARRAY_SRV{0, 1, i, 1});

    dsv = Utils::DirectX::CreateDepthStencilView(texture, DSVFormat, dsvDimension, D3D11_TEX2D_ARRAY_DSV{0, 0, 6});

    srv = Utils::DirectX::CreateShaderResourceView(texture, SRVFormat, D3D11_TEXCUBE_SRV{0, 1});
}

ID3D11ShaderResourceView *DepthStencilCube::GetSahderResourceView(UINT Edge) const throw (Exception)
{
    if(Edge >= 6)
        throw RenderTargetException("Invalid depth stencil cube edge");

    return edgeSrv[Edge];
}

}
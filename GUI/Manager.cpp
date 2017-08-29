/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <GUI/Manager.h>
#include <RenderStatesManager.h>
#include <SamplerStatesManager.h>
#include <CommonParams.h>
#include <DirectInput.h>
#include <Meshes.h>
#include <Texture.h>
#include <Utils/ToString.h>
#include <Utils/Algorithm.h>

namespace GUI
{

Manager *Manager::instance = NULL;

Manager::~Manager()
{
}

void Manager::Init()
{
    Meshes::VertexMetadata vertexMetadata = 
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},        
        {"TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    vs.Load(L"../Resources/Shaders/gui.vs", "ProcessVertex", vertexMetadata);
    ps.Load(L"../Resources/Shaders/gui.ps", "ProcessPixel");
    gs.Load(L"../Resources/Shaders/gui.gs", "ProcessVertex");

    ps.CreateShaderResourceViewVariable("colorTex", 0, SamplerStates::Get({D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP}));

    dataVb = Utils::DirectX::CreateBuffer(sizeof(TransformedRect) * MaxControlsCount, 
                                          D3D11_BIND_VERTEX_BUFFER, 
                                          D3D11_USAGE_DYNAMIC, 
                                          D3D11_CPU_ACCESS_WRITE);

    RenderStates::DepthStencilDescription depthStencilDesc;
    depthStencilDesc.stencilDescription.DepthEnable = false;
    depthStencilState = RenderStates::Manager::GetInstance()->CreateRenderState(depthStencilDesc);

    Utils::DirectX::BlendParameters color = {D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD};
    Utils::DirectX::BlendParameters alpha = {D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD};
    D3D11_BLEND_DESC bDesc = Utils::DirectX::CreateBlendDescription({{color, alpha, true, D3D11_COLOR_WRITE_ENABLE_ALL}});

    blendState = RenderStates::Manager::GetInstance()->CreateRenderState(RenderStates::BlendStateDescription(bDesc, {}, 0xffffffff));

    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.ScissorEnable = true;

    rasteriserState = RenderStates::Manager::GetInstance()->CreateRenderState(rasterDesc);
}

ID3D11ShaderResourceView *Manager::GetTexture(const std::string &FileName) throw (Exception)
{
    return GetTexture(Utils::ToWString(FileName));
}

ID3D11ShaderResourceView *Manager::GetTexture(const std::wstring &FileName) throw (Exception)
{
    StringHash texHash = std::hash<std::wstring>()(FileName);

    auto ci = textures.find(texHash);
    if(ci != textures.end())
        return ci->second;

    ID3D11ShaderResourceView *newTex = Texture::LoadTexture2DFromFile(L"../Resources/Textures/" + FileName);
    textures.insert({texHash, newTex});

    return newTex;
}

void Manager::AddExternalTheme(const Theme &NewTheme) throw (Exception)
{
    std::hash<std::string> hash;
    StringHash strHash = hash(NewTheme.GetName());
    
    if(themes.find(strHash) != themes.end())
        throw ThemeException("theme " + NewTheme.GetName() + " already exsists");

    themes.insert(std::make_pair(strHash, NewTheme));
}

const Theme &Manager::GetTheme(const std::string &ThemeName) throw (Exception)
{
    if(ThemeName == ""){
        if(currentTheme == 0)
            throw ThemeException("theme not set");
        return themes[currentTheme];
    }

    std::hash<std::string> hash;
    StringHash strHash = hash(ThemeName);

    return Utils::Find(themes, strHash, ThemeException("theme " + ThemeName + " not found"));
}

void Manager::SetTheme(const std::string &NewThemeName) throw (Exception)
{
    std::hash<std::string> hash;
    StringHash strHash = hash(NewThemeName);

    const Theme &newTheme = Utils::Find(themes, strHash, ThemeException("theme " + NewThemeName + " not found"));

    for(auto control : controls)
        control->OnThemeChange(newTheme);
    
    currentTheme = strHash;
}

void Manager::AddControl(Control *NewControl)
{
    controls.push_back(NewControl);
}

void Manager::RemoveControl(Control *ControlToRemove)
{    
    Utils::Remove(controls, ControlToRemove, [](Control *Cntrl) {/* delete Cntrl;*/});
}

struct RectsGroupData
{
    INT count = 0, offset = 0;
    ID3D11ShaderResourceView *texture = NULL;
    ID3D11SamplerState *samplerState = NULL;
    RectF scissorRect;
};

void Manager::Draw(const ControlsStorage &Controls) throw (Exception)
{

    SortedDrawingObjectsStorage drawingObjects;

    for(Control *cntrl : Controls)
        CollectDrawingObjects(cntrl, drawingObjects, DP_LOW);

    if(!drawingObjects.size())
        return;

    D3D11_MAPPED_SUBRESOURCE rawData;
    HR(DeviceKeeper::GetDeviceContext()->Map(dataVb, 0, D3D11_MAP_WRITE_DISCARD, 0, &rawData));

    TransformedRect *rects = reinterpret_cast<TransformedRect*>(rawData.pData);

    int rInd = 0;

    typedef std::vector<RectsGroupData> RectsGroupsStorage;
    RectsGroupsStorage rectsGroups;

    RectsGroupData lastGroup;
    
    for(const auto &pair : drawingObjects){
        for(const DrawingObjectData &doData : pair.second){

            if(rInd >= MaxControlsCount)
                break;

            if(doData.object->GetTexture() != lastGroup.texture || 
               doData.object->GetSamplerState() != lastGroup.samplerState || 
               doData.scissorRect != lastGroup.scissorRect)
            {

                if(lastGroup.count != 0)
                    rectsGroups.push_back(lastGroup);

                lastGroup.count = 1;
                lastGroup.offset = rInd;
                lastGroup.texture = doData.object->GetTexture();
                lastGroup.samplerState = doData.object->GetSamplerState();
                lastGroup.scissorRect = doData.scissorRect;

            }else
                lastGroup.count++;

            doData.object->Transform(rects[rInd++]);
        }
    }    

    rectsGroups.push_back(lastGroup);

    DeviceKeeper::GetDeviceContext()->Unmap(dataVb, 0);

    vs.Apply();
    ps.Apply();
    gs.Apply();

    Utils::DirectX::SetPrimitiveStream({dataVb}, nullptr, {sizeof(TransformedRect)}, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    
    RenderStates::Apply({&depthStencilState, &blendState}, [&]()
    {
        bool defSmpStateSet = false;

        for(RectsGroupData &grp : rectsGroups){
            ps.UseResource("colorTex", grp.texture);

            if(grp.samplerState != NULL){
                ps.UseSamplerState("colorTex", grp.samplerState);
                defSmpStateSet = false;
            }else if(!defSmpStateSet){
                ps.UseSamplerState("colorTex", "colorTexDefaultSamplerState");
                defSmpStateSet = true;
            }

            if(grp.scissorRect.size != SizeF()){

                const Point2F &pos = grp.scissorRect.pos;
                const SizeF &size = grp.scissorRect.size;

                D3D11_RECT rects[1];
                rects[0].left = (LONG)(pos.x * CommonParams::GetScreenWidth());
                rects[0].top = (LONG)(pos.y * CommonParams::GetScreenHeight());
                rects[0].right = (LONG)((pos.x + GUI::Control::WigthToX(size.width)) * CommonParams::GetScreenWidth());
                rects[0].bottom = (LONG)((pos.y + size.height) * CommonParams::GetScreenHeight());

                DeviceKeeper::GetDeviceContext()->RSSetScissorRects(1, rects );

                rasteriserState.Apply();

                DeviceKeeper::GetDeviceContext()->Draw(grp.count, grp.offset);

                RenderStates::Manager::GetInstance()->ApplyState("NoCull");

            }else
                DeviceKeeper::GetDeviceContext()->Draw(grp.count, grp.offset);
        }
    });

    ps.CleanUp();
    gs.CleanUp();
}

void Manager::DrawSystemControls() throw (Exception)
{
}

void Manager::CollectDrawingObjects(const Control *Cntrl, SortedDrawingObjectsStorage &AllObjects, DrawingPriority ParentPriority )
{
    if(!Cntrl->IsVisible())
        return;

    DrawingPriority priority = DP_LOW;

    if(Cntrl == lockedControl)
        priority = DP_HIGH;
    else{
        auto it = std::find(drawPriorityControls.begin(), drawPriorityControls.end(), Cntrl);
        if(it != drawPriorityControls.end())
            priority = DP_MEDIUM;
    }

    if(priority < ParentPriority)
        priority = ParentPriority;

    for(const DrawingObject *dObj : Cntrl->GetDrawingObjects()){

        DrawingObjectData doData;
        doData.object = dObj;
        doData.scissorRect = Cntrl->GetScissorRect();

        AllObjects[priority].push_back(doData);
    }

    for(const Control *childCntrl : Cntrl->childControls)
        CollectDrawingObjects(childCntrl, AllObjects, priority);
}

void Manager::Invalidate(float Tf)
{
    POINT cursorPs = DirectInput::GetInsance()->GetCursorPos();

    D3DXVECTOR2 cursorPos;
    cursorPos.x = (FLOAT)cursorPs.x / CommonParams::GetScreenWidth();
    cursorPos.y = (FLOAT)cursorPs.y / CommonParams::GetScreenHeight();
        
    cursorDelta = cursorPos - prevCursorPos;

    bool isMouseDown = DirectInput::GetInsance()->IsMouseDown(0);

    if(isMouseDown && firstCursorPosOnMouseDown.x == -1.0f && firstCursorPosOnMouseDown.y == -1.0f)
        firstCursorPosOnMouseDown = cursorPos;

    if(!isMouseDown && firstCursorPosOnMouseDown.x != -1.0f && firstCursorPosOnMouseDown.y != -1.0f)
        firstCursorPosOnMouseDown = {-1.0f, -1.0f};

    Invalidate(Tf, controls);

    prevCursorPos = cursorPos;
}

void Manager::Invalidate(float Tf, const ControlsStorage &Controls)
{

    POINT cursorPs = DirectInput::GetInsance()->GetCursorPos();

    D3DXVECTOR2 cursorPos;
    cursorPos.x = (FLOAT)cursorPs.x / CommonParams::GetScreenWidth();
    cursorPos.y = (FLOAT)cursorPs.y / CommonParams::GetScreenHeight();

    ControlsStorage::const_reverse_iterator ci;
    for(ci = Controls.rbegin(); ci != Controls.rend(); ++ci)
        if((*ci)->IsVisible())
            (*ci)->Invalidate(cursorPos, Tf);
}

void Manager::Draw() throw (Exception)
{
    Draw(controls);
}

BOOL Manager::TryToLockControl(Control *ControlToLock)
{
    if(!lockedControl)
        lockedControl = ControlToLock;

    return lockedControl == ControlToLock;
}

void Manager::UnlockControl(Control *ControlToUnlock)
{
    if(lockedControl == ControlToUnlock)
        lockedControl = NULL;
}

void Manager::ResetContext()
{
    for(Control *cntrl : controls)
        cntrl->OnManagerResetContext();

    lockedControl = NULL;
    drawPriorityControls.clear();
}

void Manager::RiseDrawPriority(Control *ControlToRise)
{
    if(ControlToRise == lockedControl)
        return;

    auto it = std::find(drawPriorityControls.begin(), drawPriorityControls.end(), ControlToRise);
    if(it == drawPriorityControls.end())
        drawPriorityControls.push_back(ControlToRise); 
}

void Manager::LowDrawPriority(Control *ControlToLow)
{
    auto it = std::find(drawPriorityControls.begin(), drawPriorityControls.end(), ControlToLow);
    if(it != drawPriorityControls.end())
        drawPriorityControls.erase(it); 
}

}

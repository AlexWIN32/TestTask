/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once 
#include <D3DHeaders.h>
#include <GUI/Control.h>
#include <GUI/Theme.h>
#include <Utils/SharedCOM.h>
#include <RenderStatesManager.h>
#include <string>
#include <map>

namespace GUI
{

class Manager
{
private:
    typedef INT StringHash;
    typedef std::map<StringHash, Utils::SharedCOM<ID3D11ShaderResourceView>> TexturesStorage;
    typedef std::map<StringHash, Theme> ThemesStorage;
    enum DrawingPriority
    {
        DP_LOW,
        DP_MEDIUM,
        DP_HIGH
    };
    struct DrawingObjectData
    {
        RectF scissorRect;
        const DrawingObject *object = nullptr;
    };
    typedef std::vector<DrawingObjectData> DODataStorage;
    typedef std::map<DrawingPriority, DODataStorage> SortedDrawingObjectsStorage;
    ThemesStorage themes;
    ControlsStorage controls;
    TexturesStorage textures;
    Shaders::VertexShader vs;
    Shaders::PixelShader ps;
    Shaders::GeometryShader gs;
    Utils::SharedCOM<ID3D11Buffer> dataVb;
    D3DXVECTOR2 prevCursorPos, cursorDelta;
    D3DXVECTOR2 firstCursorPosOnMouseDown = {-1.0f, -1.0f};
    INT cotrolsCount;
    StringHash currentTheme;
    Control *lockedControl;
    ControlsStorage drawPriorityControls;
    float eplasedTime;
    static const INT MaxControlsCount = 10000;
    void CollectDrawingObjects(const Control *Cntrl, SortedDrawingObjectsStorage &AllObjects, DrawingPriority ParentPriority);
    static Manager *instance;
    RenderStates::DepthStencilState depthStencilState;
    RenderStates::BlendState blendState;
    RenderStates::RasteriserState rasteriserState;
    Manager() : cotrolsCount(0), currentTheme(0), lockedControl(NULL), eplasedTime(0.0f){}
    ~Manager();
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
    BOOL TryToLockControl(Control *ControlToLock);
    void UnlockControl(Control *ControlToUnlock);
    void RiseDrawPriority(Control *ControlToRise);
    void LowDrawPriority(Control *ControlToLow);
    Control * GetLockedControl() {return lockedControl;}
    Manager(Manager &) = delete;
    Manager & operator=(Manager &) = delete;
    ID3D11ShaderResourceView *GetTexture(const std::wstring &FileName) throw (Exception);
    ID3D11ShaderResourceView *GetTexture(const std::string &FileName) throw (Exception);
    void Init() throw (Exception);
    void AddExternalTheme(const Theme &NewTheme) throw (Exception);
    const Theme &GetTheme(const std::string &ThemeName = "") throw (Exception);
    void SetTheme(const std::string &NewThemeName) throw (Exception);
    void AddControl(Control *NewControl);
    void RemoveControl(Control *ControlToRemove);
    void Invalidate(float Tf);
    void Draw() throw (Exception);
    void DrawSystemControls() throw (Exception);
    void Invalidate(float Tf, const ControlsStorage &Controls);
    void Draw(const ControlsStorage &Controls) throw (Exception);
    const D3DXVECTOR2 &GetCursorDelta(){return cursorDelta;}
    const D3DXVECTOR2 &GetFirstCursorPosOnMouseDown() const {return firstCursorPosOnMouseDown;}
    void ResetContext();
    void SetEplasedTime(float EplasedTime) {eplasedTime = EplasedTime;}
    float GetEplasedTime() const {return eplasedTime;}
};

}

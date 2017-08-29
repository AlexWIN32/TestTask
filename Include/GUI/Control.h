/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <Utils/EventsStorage.h>
#include <Utils/SharedCOM.h>
#include <D3DHeaders.h>
#include <Shader.h>
#include <Vector2.h>
#include <vector>
#include <map>
#include <functional>

namespace Utils
{

namespace DirectX
{
    struct SamplerStateDescription;
}

}

namespace GUI
{

class Control;
typedef std::vector<Control*> ControlsStorage;

struct TransformedRect
{
    D3DXVECTOR2 center;
    D3DXVECTOR2 halfSize;
    D3DXCOLOR color;
    D3DXVECTOR4 sidesTexCoords;
};

class DrawingObject
{
private:
    ID3D11ShaderResourceView *texture;
    D3DXVECTOR4 tcBounds;
    Utils::SharedCOM<ID3D11SamplerState> samplerState;
protected:
    void SetTexture(ID3D11ShaderResourceView *Texture){texture = Texture;}
    virtual void SetTexCoordBounds(const D3DXVECTOR4 &TcBounds){tcBounds = TcBounds;}
public:
    virtual ~DrawingObject(){}
    DrawingObject() : texture(NULL){}
    void SetSamplerState(const Utils::DirectX::SamplerStateDescription &SamplerState) throw (Exception);
    ID3D11SamplerState *GetSamplerState() const{ return samplerState;}
    ID3D11ShaderResourceView * GetTexture() const {return texture;}
    const D3DXVECTOR4 &GetTexCoordBounds()const {return tcBounds;}
    virtual void Transform(TransformedRect &Rect) const = 0;
};

typedef std::vector<const DrawingObject*> DrawingObjectsStorage;

class DrawingObjectsContainer
{
public:
    virtual ~DrawingObjectsContainer(){}
    virtual DrawingObjectsStorage GetDrawingObjects() const = 0;
};

class Manager;
class Theme;

class Control : public DrawingObjectsContainer
{
friend class Manager;
public:
    enum Positioning
    {
        CNTRL_POS_CENTER,
        CNTRL_POS_TOP_LEFT,
        CNTRL_POS_MANUAL
    };
protected:
    enum MouseState{
        ST_IDLE,
        ST_DOWN
    };
private:
    MouseState mouseState;
    bool isVisible, isFocus, isEnabled;
    D3DXVECTOR2 pos, bottomRightPos, size;
    ControlsStorage childControls;
    Positioning positioning;
    RectF scissorRect;
    typedef Utils::EventsStorage<void(const Control*, const D3DXVECTOR2&)> PosChangeEventsStorage;
    typedef Utils::EventsStorage<void(const Control*, const D3DXVECTOR2&)> SizeChangeEventsStorage;
    typedef Utils::EventsStorage<void(const Control*, const D3DXVECTOR2&)> MouseEventsStorage;
    typedef Utils::EventsStorage<void(const Control*, bool)> StateChangeEventsStorage;
    PosChangeEventsStorage posChangeEvents;
    SizeChangeEventsStorage sizeChangeEvents;
    MouseEventsStorage mouseDownEvents;
    MouseEventsStorage mouseClickEvents;
    StateChangeEventsStorage visibleChangeEvents;
    StateChangeEventsStorage focusChangeEvents;
    StateChangeEventsStorage enabledChangeEvents;
protected:
    MouseState GetMouseState() const {return mouseState;}
    void ProcessClick(const D3DXVECTOR2 &CursorPos, float Tf);
    virtual void OnMouseDown(const D3DXVECTOR2 &CursorPos, float Tf, bool InRange){}
    virtual void OnMouseUp(const D3DXVECTOR2 &CursorPos, float Tf, bool InRange){}
    void AddControl(Control *NewControl);
    void RemoveControl(Control *ControlToRemove);
    void SetControl(size_t Index, Control *NewControl);
    void ClearControls();        
    void SetPositioning(Positioning PositioningType ) {positioning = PositioningType;}
    const ControlsStorage &GetControls() const{return childControls;}
    bool InRange(const D3DXVECTOR2 &Pos);
    Control();
public:
    static D3DXVECTOR2 TransformPoint(const D3DXVECTOR2 &Point);
    static D3DXVECTOR2 TransformSize(const D3DXVECTOR2 &Size);
    static FLOAT WigthToX(float Width, bool CheckPixelError = false);
    static FLOAT XToWigth(float X);
    static FLOAT CheckPixelErrorForX(float X);
    static FLOAT CheckPixelErrorForY(float Y);
    virtual ~Control();    
    void FixPos(Positioning PositioningType = CNTRL_POS_MANUAL);
    virtual void SetSize(const D3DXVECTOR2 &Size);
    virtual const D3DXVECTOR2 &GetSize() const {return size;}
    virtual void SetPos(const D3DXVECTOR2 &Pos);
    virtual const D3DXVECTOR2 &GetPos() const {return pos;}
    virtual bool IsVisible() const {return isVisible;}
    virtual void SetVisibleState(bool IsVisible);
    virtual bool IsFocus() const {return isFocus;}
    virtual void SetFocusState(bool IsFocus);
    virtual bool IsEnabled() const {return isEnabled;}
    virtual void SetEnabledState(bool IsEnabled);
    virtual void Invalidate(const D3DXVECTOR2 &CursorPos, float Tf){}
    virtual void OnThemeChange(const Theme &NewTheme) throw (Exception) {}
    virtual void OnManagerResetContext() {};
    void SetBottomRightPos(const D3DXVECTOR2 &BottomRightPos){bottomRightPos = BottomRightPos;}
    virtual const D3DXVECTOR2 &GetBottomRightPos() const {return bottomRightPos;}
    Positioning GetPositioning() const { return positioning;}
    Utils::EventId AddOnChangePosEvent(PosChangeEventsStorage::Event NewEvent);
    void RemoveOnChangePosEvent(Utils::EventId DelEvent);
    Utils::EventId AddOnChangeSizeEvent(SizeChangeEventsStorage::Event NewEvent);
    void RemoveOnChangeSizeEvent(Utils::EventId DelEvent);
    Utils::EventId AddMouseDownEvent(MouseEventsStorage::Event NewEvent);
    void RemoveMouseDownEvent(Utils::EventId DelEvent);
    Utils::EventId AddMouseClickEvent(MouseEventsStorage::Event NewEvent);
    void RemoveMouseClickEvent(Utils::EventId DelEvent);
    Utils::EventId AddOnChangeVisibleEvent(StateChangeEventsStorage::Event NewEvent);
    void RemoveOnChangeVisibleEvent(Utils::EventId DelEvent);
    Utils::EventId AddOnChangeFocusEvent(StateChangeEventsStorage::Event NewEvent);
    void RemoveOnChangeFocusEvent(Utils::EventId DelEvent);
    Utils::EventId AddOnChangeEnabledEvent(StateChangeEventsStorage::Event NewEvent);
    void RemoveOnChangeEnabledEvent(Utils::EventId DelEvent);
    void SetScissorRect(const RectF &ScissorRect) { scissorRect = ScissorRect;}
    const RectF &GetScissorRect() const {return scissorRect;}
};

}

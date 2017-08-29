/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <GUI/Control.h>
#include <GUI/Manager.h>
#include <Utils/DirectX.h>
#include <Utils/Algorithm.h>
#include <CommonParams.h>
#include <DirectInput.h>
#include <algorithm>

namespace GUI
{

void DrawingObject::SetSamplerState(const Utils::DirectX::SamplerStateDescription &SamplerState) throw (Exception)
{
    samplerState = Utils::DirectX::CreateSamplerState(SamplerState);
}

Control::Control() :
    isVisible(true), 
    isFocus(false),
    isEnabled(true),
    pos(0.0f, 0.0f), 
    bottomRightPos(0.0f, 0.0f),
    size(0.0f, 0.0f),
    positioning(CNTRL_POS_MANUAL),
    mouseState(ST_IDLE)
{}

Control::~Control()
{
    //ControlsStorage::const_iterator ci;
    //for(ci = childControls.begin(); ci != childControls.end(); ++ci)
    //    delete *ci;
}

void Control::AddControl(Control *NewControl)
{
    childControls.push_back(NewControl);
}

void Control::RemoveControl(Control *ControlToRemove)
{
    Utils::Remove(childControls, ControlToRemove, [](Control *Cntrl) { /*delete Cntrl;*/});
}

void Control::SetControl(size_t Index, Control *NewControl)
{
    if(Index < childControls.size())
        childControls[Index] = NewControl;
}

void Control::ClearControls()
{
    std::for_each(childControls.begin(), childControls.end(), [](Control *Cntrl) { /*delete Cntrl;*/});
    childControls.clear();
}

bool Control::InRange(const D3DXVECTOR2 &Pos)
{
    return Pos.x >= GetPos().x && Pos.x <= GetBottomRightPos().x &&
           Pos.y >= GetPos().y && Pos.y <= GetBottomRightPos().y;
}

void Control::SetSize(const D3DXVECTOR2 &NewSize)
{
    if(NewSize.x <= 0.0f || NewSize.y <= 0.0f)
        return;

    if(size == NewSize)
        return;

    D3DXVECTOR2 sizeDelta = NewSize - size;
    sizeDelta.x = Control::WigthToX(sizeDelta.x);
    SetBottomRightPos(GetBottomRightPos() + sizeDelta);

    for(auto pair : sizeChangeEvents)
        pair.second(this, NewSize);

    size = NewSize;
}

void Control::SetPos(const D3DXVECTOR2 &NewPos)
{
    if(pos == NewPos)
        return;

    if(positioning != CNTRL_POS_MANUAL){
        D3DXVECTOR2 szOffst = {Control::WigthToX(GetSize().x), GetSize().y};
        if(positioning == CNTRL_POS_CENTER)
            SetBottomRightPos(NewPos + szOffst * 0.5f);
        else
            SetBottomRightPos(NewPos + szOffst);
    }

    if(scissorRect.size != SizeF())
        scissorRect.pos += Cast<Point2F>(NewPos - pos);

    for(auto pair : posChangeEvents)
        pair.second(this, NewPos);

    pos = NewPos;
}

void Control::SetVisibleState(bool IsVisible)
{
    if(isVisible == IsVisible)
        return;

    for(auto pair : visibleChangeEvents)
        pair.second(this, IsVisible);

    isVisible = IsVisible;
}

void Control::SetFocusState(bool IsFocus)
{
    if(isFocus == IsFocus)
        return;

    for(auto pair : focusChangeEvents)
        pair.second(this, IsFocus);

    isFocus = IsFocus;
}

void Control::SetEnabledState(bool IsEnabled)
{
    if(isEnabled == IsEnabled)
        return;

    for(auto pair : enabledChangeEvents)
        pair.second(this, IsEnabled);

    isEnabled = IsEnabled;
}

D3DXVECTOR2 Control::TransformPoint(const D3DXVECTOR2 &Point)
{
    D3DXVECTOR2 out;
    out.x = (2.0f * Point.x) - 1.0f;
    out.y = (-2.0f * Point.y) + 1.0f;
    return out;
}

D3DXVECTOR2 Control::TransformSize(const D3DXVECTOR2 &Size)
{
    D3DXVECTOR2 out;
    out.x = 2.0f * Size.x * CommonParams::GetHeightOverWidth();
    out.y = 2.0f * Size.y;
    return out;
}

FLOAT Control::CheckPixelErrorForX(float X)
{
    float pixels = X * CommonParams::GetScreenWidth();
    float error = pixels - floor(pixels);

    if(error >= 0.5f)
        X += (1.0f - error) / CommonParams::GetScreenWidth();
    else
        X -= error / CommonParams::GetScreenWidth();

    return X;
}

FLOAT Control::CheckPixelErrorForY(float Y)
{
    float pixels = Y * CommonParams::GetScreenHeight();
    float error = pixels - floor(pixels);

    if(error >= 0.5f)
        Y += (1.0f - error) / CommonParams::GetScreenHeight();
    else
        Y -= error / CommonParams::GetScreenHeight();
        
    return Y;
}

FLOAT Control::WigthToX(float Width, bool CheckPixelError)
{
    float x = Width * CommonParams::GetHeightOverWidth();

    if(CheckPixelError)
       x = CheckPixelErrorForX(x);

    return x;
}

FLOAT Control::XToWigth(float X)
{
    return X * CommonParams::GetWidthOverHeight();
}

void Control::FixPos(Positioning PositioningType)
{
    Positioning posType = positioning;

    if(posType == CNTRL_POS_MANUAL)
        posType = PositioningType;

    if(posType == CNTRL_POS_MANUAL)
        return;

    D3DXVECTOR2 szOffst = (posType == CNTRL_POS_CENTER) ? size * 0.5f : size;

    szOffst.x = Control::WigthToX(szOffst.x);

    D3DXVECTOR2 corner = pos + szOffst;
    
    float absX = corner.x * CommonParams::GetScreenWidth();
    float absY = corner.y * CommonParams::GetScreenHeight();

    D3DXVECTOR2 error(absX - floor(absX), absY - floor(absY));

    if(error.x >= 0.5f){
        pos.x += (1.0f - error.x) / CommonParams::GetScreenWidth();
        bottomRightPos.x += (1.0f - error.x) / CommonParams::GetScreenWidth();
    }else{
        pos.x -= error.x / CommonParams::GetScreenWidth();
        bottomRightPos.x -= error.x / CommonParams::GetScreenWidth();
    }

    if(error.y >= 0.5f){
        pos.y += (1.0f - error.y) / CommonParams::GetScreenHeight();
        bottomRightPos.y += (1.0f - error.y) / CommonParams::GetScreenHeight();
    }else{
        pos.y -= error.y / CommonParams::GetScreenHeight();
        bottomRightPos.y -= error.y / CommonParams::GetScreenHeight();
    }

}

void Control::ProcessClick(const D3DXVECTOR2 &CursorPos, float Tf)
{
    bool inRange = InRange(CursorPos);

    bool isMouseDown = DirectInput::GetInsance()->IsMouseDown(0);

    if(mouseState == ST_IDLE){
        if(inRange && isMouseDown && Manager::GetInstance()->GetFirstCursorPosOnMouseDown() == CursorPos){
            if(Manager::GetInstance()->TryToLockControl(this))
                mouseState = ST_DOWN;
        }
    }

    if(mouseState == ST_DOWN){

        OnMouseDown(CursorPos, Tf, inRange);

        if(inRange)
            for(auto pair : mouseDownEvents)
                pair.second(this, CursorPos);
    }
    
    if(mouseState == ST_DOWN && !isMouseDown){
        
        Manager::GetInstance()->UnlockControl(this);

        mouseState = ST_IDLE;

        OnMouseUp(CursorPos, Tf, inRange);

        if(inRange)
            for(auto pair : mouseClickEvents)
                pair.second(this, CursorPos);
    }
}

Utils::EventId Control::AddOnChangePosEvent(PosChangeEventsStorage::Event NewEvent)
{
    return posChangeEvents.Add(NewEvent);
}

void Control::RemoveOnChangePosEvent(Utils::EventId DelEvent)
{
    posChangeEvents.Remove(DelEvent);
}

Utils::EventId Control::AddOnChangeSizeEvent(SizeChangeEventsStorage::Event NewEvent)
{
    return sizeChangeEvents.Add(NewEvent);
}

void Control::RemoveOnChangeSizeEvent(Utils::EventId DelEvent)
{
    sizeChangeEvents.Remove(DelEvent);
}

Utils::EventId Control::AddMouseDownEvent(MouseEventsStorage::Event NewEvent)
{
    return mouseDownEvents.Add(NewEvent);
}

void Control::RemoveMouseDownEvent(Utils::EventId DelEvent)
{
    sizeChangeEvents.Remove(DelEvent);
}

Utils::EventId Control::AddMouseClickEvent(MouseEventsStorage::Event NewEvent)
{
    return mouseClickEvents.Add(NewEvent);
}

void Control::RemoveMouseClickEvent(Utils::EventId DelEvent)
{
    mouseClickEvents.Remove(DelEvent);
}

Utils::EventId Control::AddOnChangeVisibleEvent(StateChangeEventsStorage::Event NewEvent)
{
    return visibleChangeEvents.Add(NewEvent);
}

void Control::RemoveOnChangeVisibleEvent(Utils::EventId DelEvent)
{
    visibleChangeEvents.Remove(DelEvent);
}

Utils::EventId Control::AddOnChangeFocusEvent(StateChangeEventsStorage::Event NewEvent)
{
    return focusChangeEvents.Add(NewEvent);
}

void Control::RemoveOnChangeFocusEvent(Utils::EventId DelEvent)
{
    focusChangeEvents.Remove(DelEvent);
}

Utils::EventId Control::AddOnChangeEnabledEvent(StateChangeEventsStorage::Event NewEvent)
{
    return enabledChangeEvents.Add(NewEvent);
}

void Control::RemoveOnChangeEnabledEvent(Utils::EventId DelEvent)
{
    enabledChangeEvents.Remove(DelEvent);
}

}

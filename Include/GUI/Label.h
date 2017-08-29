/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <GUI/Control.h>
#include <GUI/Image.h>
#include <string.h>

namespace GUI
{

class Font;

class Label : public Control
{
public:
    enum TextAlign
    {
        TXT_ALGN_LEFT,
        TXT_ALGN_RIGHT,
        TXT_ALGN_CENTER,
    };
private:
    class Glyph : public Image 
    {
    private:
        wchar_t ch = 0;
    public:
        wchar_t GetChar() const {return ch;}
        void Init(wchar_t Char, const Font * UsingFont, D3DXVECTOR2 &CursorPos, const D3DXCOLOR &Color);
        void SetTexture(ID3D11ShaderResourceView *Texture){Image::SetTexture(Texture);}
    };
    typedef std::vector<Glyph*> GlyphsStorage;
    mutable GlyphsStorage glyphs;
    mutable DrawingObjectsStorage glyphsDrawingObjects;
    mutable bool glypsUpdatedOnDraw = false;
    mutable D3DXVECTOR2 size = {0.0f, 0.0f}, newSize = {0.0f, 0.0f};
    std::wstring caption, newCaption;
    D3DXCOLOR color = {0,0,0,1}, newColor = {0,0,0,1};
    D3DXVECTOR2 newPos = {0,0};
    mutable D3DXVECTOR2 newBottomRightPos = {0.0f, 0.0f};
    TextAlign textAlign = TXT_ALGN_LEFT, newTextAlign = TXT_ALGN_LEFT;
    void SetSize(const D3DXVECTOR2 &NewSize){Control::SetSize(NewSize);}
    const Font * font = NULL;
    void CreateGlyphs() const throw (Exception);
    void Construct(const Label &Var);
public:
    virtual ~Label();
    Label(){}
    Label(const Label &Var){Construct(Var);}
    Label &operator= (const Label &Var)
    {
        Construct(Var);
        return *this;
    }
    void Init(const Font * UsingFont);
    const std::wstring &GetCaption() const;
    void SetCaption(const std::wstring &NewCaption){newCaption = NewCaption;}
    void SetTextAlign(TextAlign NewValue){newTextAlign = NewValue;}
    TextAlign GetTextAlign() const;
    const D3DXCOLOR &GetColor() const;
    void SetColor(const D3DXCOLOR &NewColor){newColor = NewColor;}
    virtual const D3DXVECTOR2 &GetPos() const;
    virtual void SetPos(const D3DXVECTOR2 &NewPos){newPos = NewPos;}
    virtual const D3DXVECTOR2 &GetSize() const;
    virtual const D3DXVECTOR2 &GetBottomRightPos() const;
    const Font * GetFont() const {return font;}
    virtual DrawingObjectsStorage GetDrawingObjects() const;
    virtual void Invalidate(const D3DXVECTOR2 &CursorPos, float Tf){UpdateGlyphs();}
    void UpdateGlyphs() throw (Exception);
};

};
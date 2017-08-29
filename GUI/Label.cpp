/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <GUI/Label.h>
#include <GUI/Font.h>
#include <CommonParams.h>

namespace GUI
{

void Label::Glyph::Init(wchar_t Char, const Font * UsingFont, D3DXVECTOR2 &CursorPos, const D3DXCOLOR &Color)
{
    ch = Char;

    const Font::Glyph &glyph = UsingFont->operator [](Char);

    D3DXVECTOR2 upperLeftPos = CursorPos;
    upperLeftPos.x += glyph.offset.x;
    upperLeftPos.y += glyph.offset.y;

    D3DXVECTOR2 size;
    size.x = Control::XToWigth(glyph.width / CommonParams::GetScreenWidth());
    size.y = glyph.height / CommonParams::GetScreenHeight();

    D3DXVECTOR2 center;
    center.x = (upperLeftPos.x) / CommonParams::GetScreenWidth() + Control::WigthToX(size.x * 0.5f);
    center.y = (upperLeftPos.y) / CommonParams::GetScreenHeight() + (size.y * 0.5f);

    SetTexCoordBounds(D3DXVECTOR4(glyph.tcTopLeft.x, glyph.tcTopLeft.y, glyph.tcBottomRight.x, glyph.tcBottomRight.y));
    SetTexture(UsingFont->GetTexture());
    SetColor(Color);
    SetPos(center);
    SetSize(size);

    CursorPos.x += glyph.xAdvance;

    //FixPos(CNTRL_POS_TOP_LEFT);
}

void Label::CreateGlyphs() const throw (Exception)
{
    GlyphsStorage::const_iterator gCi;
    for(gCi = glyphs.begin(); gCi != glyphs.end(); ++gCi)
        delete *gCi;

    glyphs.clear();
    glyphsDrawingObjects.clear();

    D3DXVECTOR2 startPos = newPos;
    startPos.x *= CommonParams::GetScreenWidth();
    startPos.y *= CommonParams::GetScreenHeight();

    D3DXVECTOR2 cursorUpperLeft = startPos;

    std::vector<Glyph*> lastGlyphLine;
    std::vector<std::vector<Glyph*>> glypsLines;

    std::wstring::const_iterator ci;
    for(ci = newCaption.begin(); ci != newCaption.end(); ++ci){
        wchar_t ch = *ci;
        if(ch == '\n'){

            glypsLines.push_back(lastGlyphLine);
            lastGlyphLine.clear();

            if(size.x < cursorUpperLeft.x - startPos.x)
                size.x = cursorUpperLeft.x - startPos.x;

            cursorUpperLeft.y += font->GetLineHeight();
            cursorUpperLeft.x = startPos.x;

            size.y += font->GetLineHeight();

        }else{
            Glyph* newGlyph = new Glyph();
            newGlyph->Init(ch, font, cursorUpperLeft, newColor);

            glyphs.push_back(newGlyph);
            glyphsDrawingObjects.push_back(newGlyph);
            lastGlyphLine.push_back(newGlyph);

            std::wstring::const_iterator next = ci + 1;
            if(next != newCaption.end())
                cursorUpperLeft.x += font->FindKerning({ch, *next});
        }
    }

    if(cursorUpperLeft.x != startPos.x){
        size.y += font->GetLineHeight();
        if(size.x < cursorUpperLeft.x - startPos.x)
            size.x = cursorUpperLeft.x - startPos.x;
    }

    glypsLines.push_back(lastGlyphLine);

    size.x = Control::XToWigth(size.x / CommonParams::GetScreenWidth());
    size.y = size.y / CommonParams::GetScreenHeight();

    float sizeOffset = Control::WigthToX(size.x);

    for(const std::vector<Glyph*> &line : glypsLines){

        std::wstring lineStr = L"";

        for(Glyph* g : line)
            lineStr += g->GetChar();

        float lineOffst = Control::WigthToX(font->MeasureString(lineStr).x);

        float offset = 0.0f;

        if(textAlign == TXT_ALGN_RIGHT)
            offset = sizeOffset - lineOffst;
        else if(textAlign == TXT_ALGN_CENTER)
            offset = (sizeOffset - lineOffst) * 0.5f;

        for(Glyph* g : line)
            g->SetPos(g->GetPos() + D3DXVECTOR2(offset, 0.0f));
    }
}

void Label::UpdateGlyphs() throw (Exception)
{
    if(Control::GetPos() != newPos || caption != newCaption || color != newColor || textAlign != newTextAlign){
        
        if(!glypsUpdatedOnDraw)
            CreateGlyphs();

        Control::SetPos(newPos);
        Control::SetSize(size);
        Control::SetBottomRightPos(GetPos() + D3DXVECTOR2(Control::WigthToX(size.x), size.y));

        caption = newCaption;
        color = newColor;
        textAlign = newTextAlign;
        glypsUpdatedOnDraw = false;
    }
}

void Label::Init(const Font * UsingFont)
{
    font = UsingFont;
}

Label::~Label()
{
    GlyphsStorage::const_iterator gCi;
    for(gCi = glyphs.begin(); gCi != glyphs.end(); ++gCi)
        delete *gCi;
}

void Label::Construct(const Label &Var)
{
    font = Var.font;
    caption = Var.caption;
    newCaption = Var.newCaption;
    color = Var.color;
    newColor = Var.newColor;
    newPos = Var.newPos;
    newSize = Var.newSize;

    Control::SetPos(Var.GetPos());
    Control::SetSize(Var.GetSize());
    Control::SetBottomRightPos(Var.GetBottomRightPos());
    Control::SetVisibleState(Var.IsVisible());
    Control::SetFocusState(Var.IsFocus());

    for(Glyph *glyph : Var.glyphs){
        Glyph *newGlyph = new Glyph();

        newGlyph->SetTexCoordBounds(glyph->GetTexCoordBounds());
        newGlyph->SetTexture(glyph->GetTexture());
        newGlyph->SetColor(glyph->GetColor());
        newGlyph->SetPos(glyph->GetPos());
        newGlyph->SetSize(glyph->GetSize());    

        glyphs.push_back(newGlyph);
        glyphsDrawingObjects.push_back(newGlyph);
    }
}

const D3DXVECTOR2 &Label::GetPos() const
{
    return (Control::GetPos() != newPos) ? newPos : Control::GetPos();
}

const D3DXVECTOR2 &Label::GetSize() const
{
    if(caption != newCaption){
        newSize = font->MeasureString(newCaption);
        return newSize;
    }
    
    return Control::GetSize();
}

const D3DXCOLOR &Label::GetColor() const
{
    return (newColor != color) ? newColor : color;
}


const std::wstring &Label::GetCaption() const 
{
    return (newCaption != caption) ? newCaption : caption;
}

const D3DXVECTOR2 &Label::GetBottomRightPos() const
{
    if(Control::GetPos() != newPos || Control::GetBottomRightPos() != newBottomRightPos){

        const D3DXVECTOR2 &size = GetSize();

        newBottomRightPos = GetPos() + D3DXVECTOR2(Control::WigthToX(size.x), size.y);

        return newBottomRightPos;
    }

    return Control::GetBottomRightPos();
}

Label::TextAlign Label::GetTextAlign() const
{
    return (textAlign != newTextAlign) ? newTextAlign : textAlign;
}

DrawingObjectsStorage Label::GetDrawingObjects() const
{
    if(Control::GetPos() != newPos || caption != newCaption || color != newColor){

        CreateGlyphs();

        glypsUpdatedOnDraw = true;
    }

    return glyphsDrawingObjects;
}

};

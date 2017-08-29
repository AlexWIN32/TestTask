/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <D3DHeaders.h>
#include <map>
#include <unordered_map>
#include <string>
#include <Utils/SharedCOM.h>

namespace GUI
{

DECLARE_EXCEPTION(FontException);

class Font final
{
public:
    struct Kerning
    {
        wchar_t first, second;
        Kerning() :first(0), second(0){}
        Kerning(wchar_t First, wchar_t Second) : first(First), second(Second){}
        bool operator==(const Kerning &Val) const
        {
            return first == Val.first && second == Val.second;
        }
    };
private:
    class KerningHash
    {
    public:
         std::size_t operator()(const Kerning &KerningData) const 
         {
            std::size_t h1 = std::hash<wchar_t>()(KerningData.first);
            std::size_t h2 = std::hash<wchar_t>()(KerningData.second);
            return h1 ^ (h2 << 1); 
         }
    };
public:
    struct Glyph
    {
        D3DXVECTOR2 tcTopLeft, tcBottomRight;
        D3DXVECTOR2 offset;
        float width, height;
        float xAdvance;
    };
private:
    typedef std::unordered_map<Kerning, INT, KerningHash> KerningsStorage;
    KerningsStorage kernings;
    typedef std::map<wchar_t, Glyph> GlyphDescriptionsStorage;
    GlyphDescriptionsStorage glyphs;
    std::string name;
    INT lineHeight;
    FLOAT lineScreenHeight;
    Utils::SharedCOM<ID3D11ShaderResourceView> texture;
    void LoadFromXML(const std::string &FilePath) throw (Exception);
    void LoadFromBinary(const std::string &FilePath) throw (Exception);
public:
    Font() : lineHeight(0), lineScreenHeight(0), texture(NULL){}
    void Init(const std::string &FileName) throw (Exception);
    const std::string &GetName() const {return name;}
    INT GetLineHeight() const {return lineHeight;}
    FLOAT GetLineScreenHeight() const {return lineScreenHeight;}
    ID3D11ShaderResourceView * GetTexture() const { return texture;}
    const Glyph &operator[] (wchar_t Char) const throw (Exception);
    INT FindKerning(const Kerning &KerningData) const;
    D3DXVECTOR2 MeasureString(const std::wstring &String) const throw (Exception);
};

}
/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <GUI/Font.h>
#include <GUI/Control.h>
#include <Utils/Algorithm.h>
#include <Utils/FileGuard.h>
#include <CommonParams.h>
#include <Xml.h>
#include <Texture.h>

namespace GUI
{

enum BlocksIds
{
    BLK_INFO = 1,
    BLK_COMMON = 2,
    BLK_PAGES = 3,
    BLK_CHARS = 4,
    BLK_KERNINGS = 5
};

void Font::LoadFromXML(const std::string &FontDescriptionFilePath) throw (Exception)
{
    XML::XmlData xmlFile;
    xmlFile.LoadFromFile(FontDescriptionFilePath);
    
    const XML::Node &fontNode = xmlFile.GetRoot();
    const XML::Node &commonNode = fontNode.GetNode("common");

    name = fontNode.GetNode("info").GetProperty("face");

    lineHeight = atoi(commonNode.GetProperty("lineHeight").c_str());
    lineScreenHeight = (float)lineHeight / CommonParams::GetScreenHeight();

    float texWidth = std::stof(commonNode.GetProperty("scaleW"));
    float texHeight = std::stof(commonNode.GetProperty("scaleH"));

    INT pagesCnt = atoi(commonNode.GetProperty("pages").c_str());
    if(pagesCnt != 1)
        throw FontException("only one page in font supported");

    const std::string &texFileName = fontNode.GetNode("pages").GetNode("page").GetProperty("file");

    texture = Texture::LoadTexture2DFromFile("../Resources/Textures/" + texFileName);

    const XML::Node &chars = xmlFile.GetRoot().GetNode("chars");

    size_t charsCnt = chars.GetNodesCount("char");

    for(size_t i = 0; i < charsCnt; i++){
        const XML::Node &charNode = chars.GetNode("char", i);

        wchar_t ch = (wchar_t)atoi(charNode.GetProperty("id").c_str());

        float x = std::stof(charNode.GetProperty("x"));
        float y = std::stof(charNode.GetProperty("y"));

        float width = std::stof(charNode.GetProperty("width"));
        float height = std::stof(charNode.GetProperty("height"));

        float offsetX = std::stof(charNode.GetProperty("xoffset"));
        float offsetY = std::stof(charNode.GetProperty("yoffset"));

        float xAnvance = std::stof(charNode.GetProperty("xadvance"));
        
        Glyph newGlyph;
        newGlyph.tcTopLeft.x = x / texWidth;
        newGlyph.tcTopLeft.y = y / texHeight;
        newGlyph.tcBottomRight.x = (x + width) / texWidth;
        newGlyph.tcBottomRight.y = (y + height) / texHeight;
        newGlyph.offset.x = offsetX;
        newGlyph.offset.y = offsetY;
        newGlyph.width = width;
        newGlyph.height = height;
        newGlyph.xAdvance = xAnvance;

        glyphs.insert(std::make_pair(ch, newGlyph));
    }

    const XML::Node &kerningsNode = xmlFile.GetRoot().GetNode("kernings");

    size_t kerningsCnt = kerningsNode.GetNodesCount("kerning");

    for(size_t k = 0; k < kerningsCnt; k++){
        const XML::Node &kerningNode = kerningsNode.GetNode("kerning", k);
    
        wchar_t first = (wchar_t)atoi(kerningNode.GetProperty("first").c_str());
        wchar_t second = (wchar_t)atoi(kerningNode.GetProperty("second").c_str());

        INT amount = atoi(kerningNode.GetProperty("amount").c_str());

        kernings.insert(std::make_pair(Kerning(first, second), amount));
    }
}

void Font::LoadFromBinary(const std::string &FilePath) throw (Exception)
{
    //BMFont file format description see http://angelcode.com/products/bmfont/doc/file_format.html

    Utils::FileGuard file(FilePath, "rb");

    fseek(file, 4, SEEK_CUR); //skip header and version

    float texWidth = 0.0f, texHeight = 0.0f;
    uint16_t pagesCnt = 0;

    std::vector<std::string> pagesFiles;

    while(true){

        uint8_t blockId;
        if(fread(&blockId, sizeof(uint8_t), 1, file) != 1 && ferror(file))
            throw IOException("cant read from file " + FilePath);
        
        if(feof(file))
            break;

        uint32_t blockSize = file.Read<uint32_t>();

        if(blockId == BLK_INFO){

            fseek(file, 14, SEEK_CUR); //skip all data but font name

            name = file.Read<std::string>();

        }else if(blockId == BLK_COMMON){

            lineHeight = file.Read<uint16_t>();
            lineScreenHeight = (float)lineHeight / CommonParams::GetScreenHeight();

            fseek(file, 2, SEEK_CUR); //skip base

            texWidth = file.Read<uint16_t>();
            texHeight = file.Read<uint16_t>();

            pagesCnt = file.Read<uint16_t>();

            fseek(file, 5, SEEK_CUR); //skip bitField, alphaChnl, redChnl, greenChnl, blueChnl

        }else if(blockId == BLK_PAGES){

            uint32_t bytesReaded = 0;

            while(bytesReaded < blockSize){
                std::string pageFileName = file.Read<std::string>();

                pagesFiles.push_back(pageFileName);

                bytesReaded += pageFileName.length() + 1; // 1 for terminated byte;
            }

            if(pagesFiles.size() != 1)
                throw FontException("only one page in font supported");

            texture = Texture::LoadTexture2DFromFile("../Resources/Textures/" + pagesFiles[0]);

        }else if(blockId == BLK_CHARS){

            const int32_t charDataSize = 20;

            int32_t charsCnt = blockSize / charDataSize;

            for(int32_t c = 0; c < charsCnt; c++){

                uint32_t id = file.Read<uint32_t>();

                uint16_t x = file.Read<uint16_t>();
                uint16_t y = file.Read<uint16_t>();

                uint16_t width = file.Read<uint16_t>();
                uint16_t height = file.Read<uint16_t>();

                int16_t xOffset = file.Read<int16_t>();
                int16_t yOffset = file.Read<int16_t>();

                int16_t xAdvance = file.Read<int16_t>();

                fseek(file, 2, SEEK_CUR); //skip page, chnl

                Glyph newGlyph;
                newGlyph.tcTopLeft.x = (FLOAT)x / texWidth;
                newGlyph.tcTopLeft.y = (FLOAT)y / texHeight;
                newGlyph.tcBottomRight.x = (FLOAT)(x + width) / texWidth;
                newGlyph.tcBottomRight.y = (FLOAT)(y + height) / texHeight;
                newGlyph.offset.x = xOffset;
                newGlyph.offset.y = yOffset;
                newGlyph.width = width;
                newGlyph.height = height;
                newGlyph.xAdvance = xAdvance;

                glyphs.insert({(wchar_t)id, newGlyph});
            }

        }else if(blockId == BLK_KERNINGS){

            const int32_t kerningDataSize = 10;

            int32_t kerningsCnt = blockSize / kerningDataSize;

            for(int32_t k = 0; k < kerningsCnt; k++){

                uint32_t first = file.Read<uint32_t>();
                uint32_t second = file.Read<uint32_t>();

                int16_t amount = file.Read<int16_t>();

                kernings.insert({{(wchar_t)first, (wchar_t)second}, amount});
            }
        }
    }
}

void Font::Init(const std::string &FileName) throw (Exception)
{
    kernings.clear();
    glyphs.clear();
    name = "";
    lineHeight = 0;
    lineScreenHeight = 0.0f;

    std::string filePath = "../Resources/Fonts/" + FileName;

    char header[3] = {};
    fread(header, 1, 3, Utils::FileGuard(filePath, "r"));

    if(header[0] == 'B' && header[1] == 'M' && header[2] == 'F')
        LoadFromBinary(filePath);
    else 
        LoadFromXML(filePath);
}

const Font::Glyph &Font::operator[] (wchar_t Char) const throw (Exception)
{
    return Utils::Find(glyphs, Char, FontException("char " + Utils::ToString((int)Char) + " not found in font " + name));
}

INT Font::FindKerning(const Kerning &KerningData) const
{
    KerningsStorage::const_iterator ci = kernings.find(KerningData);
    if(ci == kernings.end())
        return 0;

    return ci->second;
}

D3DXVECTOR2 Font::MeasureString(const std::wstring &String) const throw (Exception)
{
    D3DXVECTOR2 size(0.0f, 0.0f);
    float rowXOffset = 0.0f;

    std::wstring::const_iterator ci;
    for(ci = String.begin(); ci != String.end(); ++ci){
        wchar_t ch = *ci;
        if(ch == '\n'){
            
            if(size.x < rowXOffset)
                size.x = rowXOffset;

            size.y += GetLineHeight();

            rowXOffset = 0.0f;
        }else{
            const Font::Glyph &glyph = operator[](ch);

            rowXOffset += glyph.xAdvance;

            std::wstring::const_iterator next = ci + 1;
            if(next != String.end())
                rowXOffset += FindKerning({ch, *next});
        }
    }

    if(rowXOffset != 0){
        size.y += GetLineHeight();
        if(size.x < rowXOffset)
            size.x = rowXOffset;
    }

    size.x = Control::XToWigth(size.x / CommonParams::GetScreenWidth());
    size.y = size.y / CommonParams::GetScreenHeight();

    return size;
}

}
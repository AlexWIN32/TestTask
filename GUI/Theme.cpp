/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <GUI/Theme.h>
#include <GUI/Control.h>
#include <Utils/Algorithm.h>
#include <CommonParams.h>
#include <Xml.h>
#include <sstream>
#include <Serializing.h>

namespace GUI
{

class Cursor
{
private:
    std::string themeName;
    std::string controlName;
    std::string elementName;
public:
    void SetThemeName(const std::string &ThemeName){themeName = ThemeName;}
    void SetControlName(const std::string &ControlName){controlName = ControlName;}
    void SetElementName(const std::string &ElementName){elementName = ElementName;}
    ThemeException CreateException(const std::string &Text)
    {
        std::stringstream sstrm;
        sstrm << "theme " << themeName;
        if(controlName != "")
            sstrm << ", control " << controlName;
        if(elementName != "")
            sstrm << ", element " << elementName;

        sstrm << ": " << Text;

        return ThemeException(sstrm.str());
    }
};

struct TextureData
{
    std::string fileName;
    FLOAT widthOverOne, heightOverOne;
};

typedef std::map<std::string, TextureData> TexturesDataStorage;

template<class TElement>
TElement ParseElement(const XML::Node &DataNode, const TexturesDataStorage &TexturesData, Cursor &cursor)
{
    const std::string &texName = DataNode.GetProperty("texture");

    const TextureData &texData = Utils::Find(TexturesData, texName, cursor.CreateException(texName + " texture not declared"));

    SizeF screenSize = SizeFParser::FromString(DataNode.GetProperty("screenSize"));

    Point2F upperLeftPos = Point2FParser::FromString(DataNode.GetProperty("upperLeft"));
    Point2F lowerRightPos = (upperLeftPos + Cast<Point2F>(screenSize));

    TElement newElement;
    newElement.textureFileName = texData.fileName;
    newElement.textureBounds.x = upperLeftPos.x * texData.widthOverOne;
    newElement.textureBounds.y = upperLeftPos.y * texData.heightOverOne;
    newElement.textureBounds.z = lowerRightPos.x * texData.widthOverOne;
    newElement.textureBounds.w = lowerRightPos.y * texData.heightOverOne;
    newElement.screenSize.x = Control::XToWigth(screenSize.width / CommonParams::GetScreenWidth());
    newElement.screenSize.y = screenSize.height / CommonParams::GetScreenHeight();
    return newElement;
}


const ControlView::ElementData &ControlView::GetElement(const std::string &ElementName) const throw (Exception)
{
    return Utils::Find(elements, ElementName, ControlViewElementNotFoundException("element " + ElementName + " not found in " + name + " control view"));
}

BOOL ControlView::FindElement(const std::string &ElementName, ControlView::ElementData &FoundElement) const
{
    ElementsStorage::const_iterator ci = elements.find(ElementName);
    if(ci == elements.end())
        return false;

    FoundElement = ci->second;
    return true;
}

const std::string &ControlView::GetParameter(const std::string &ParameterName) const throw (Exception)
{
    return Utils::Find(params, ParameterName, ControlViewElementNotFoundException("parameter " + ParameterName + " not found in " + name + " control view"));
}

void ControlView::SetParameter(const std::string &ParameterName, const std::string &ParameterValue)
{
    params[ParameterName] = ParameterValue;
}

void ControlView::RemoveParameter(const std::string &ParameterName)
{
    ParamsStorage::iterator it = params.begin();
    if(it != params.end())
        params.erase(ParameterName);
}

BOOL ControlView::FindParameter(const std::string &ParameterName, std::string &value) const
{
    ParamsStorage::const_iterator ci = params.find(ParameterName);
    if(ci == params.end())
        return false;

    value = ci->second;
    return true;
}

const ControlView &Theme::GetControlView(const std::string &Name) const throw (Exception)
{
    return Utils::Find(controlViews, Name, ControlViewNotFoundException(Name +" not found in " + name + " theme"));
}

const Font &Theme::GetFont(const std::string &Name) const throw (Exception)
{
    std::string fontName = Name;
    if(Name == ""){
        if(defaultFontName == "")
            throw FontNotFoundException("default font not set");
        fontName = defaultFontName;
    }

    auto ci = fonts.find(fontName);
    if(ci != fonts.end())
        return ci->second;
        
    throw FontNotFoundException("font "+ fontName +" not found in " + name + " theme");
}

BOOL Theme::FindControlView(const std::string &Name, ControlView &FindedControlView)
{
    ControlViewsStorage::const_iterator ci = controlViews.find(Name);
    if(ci == controlViews.end())
        return false;

    FindedControlView = ci->second;
    return true;
}

void Theme::SetDefaultFont(const std::string &FontName) throw (Exception)
{    
    if(fonts.find(FontName) == fonts.end())
        throw FontNotFoundException("font "+ FontName +" not found in " + name + " theme");

    defaultFontName = FontName;
}

void Theme::Load(const std::string &Name) throw (Exception)
{
    TexturesDataStorage texturesData;

    Cursor cursor;

    name = Name.substr(0, Name.find('.'));

    cursor.SetThemeName(Name);

    XML::XmlData data;
    data.LoadFromFile("../Resources/GuiThemes/" + Name);

    const XML::Node &guiNode = data.GetRoot();
    
    const XML::Node &texturesNode = guiNode.GetNode("textures");

    for(const XML::Node &textureNode : texturesNode){

        SizeF texSize = SizeFParser::FromString(textureNode.GetProperty("size"));

        TextureData newTexData;
        newTexData.widthOverOne = 1.0f / texSize.width;
        newTexData.heightOverOne = 1.0f / texSize.height;
        newTexData.fileName = textureNode.GetProperty("fileName");

        texturesData.insert({textureNode.GetProperty("name"), newTexData});
    }

    const XML::Node &controlsNode = guiNode.GetNode("controls");

    for(const XML::NodesNamesData &controlNodesData : controlsNode.GetNodesNames()){
        cursor.SetControlName(controlNodesData.name);

        if(controlNodesData.count != 1)
            throw cursor.CreateException("control redifinition");

        ControlView newControlView;

        const XML::Node &controlNode = controlsNode.GetNode(controlNodesData.name);
        for(const XML::Node &dataNode : controlNode){
            cursor.SetElementName(dataNode.GetName());

            if(controlsNode.GetNodesCount(dataNode.GetName()) > 1)
                throw cursor.CreateException("element redifinition");

             if(dataNode.GetName() == "params"){
                for(const XML::Node &paramDataNode : dataNode){
                    cursor.SetElementName(dataNode.GetName());

                    if(dataNode.GetNodesCount(paramDataNode.GetName()) > 1)
                        throw cursor.CreateException("element redifinition");

                    newControlView.params.insert({paramDataNode.GetName(), paramDataNode.GetValue()});
                }
             }else
                newControlView.elements.insert({dataNode.GetName(), ParseElement<ControlView::ElementData>(dataNode, texturesData, cursor)});
        }

        cursor.SetElementName("");

        controlViews.insert(std::make_pair(controlNode.GetName(), newControlView));
    }

    const XML::Node &fntsNode = guiNode.GetNode("fonts");
    
    for(const XML::Node &fntNode : fntsNode){

        Font newFont;
        newFont.Init(fntNode.GetProperty("name"));
        fonts.insert({newFont.GetName(), newFont});
    }

    if(fonts.size() > 1)
        defaultFontName = fntsNode.GetNode("defaultFont").GetProperty("name");
    else
        defaultFontName = fonts.begin()->first;

    XML::ConstNodesSet imagesNodes;
    if(guiNode.FindNode("images", imagesNodes, false)){
        if(imagesNodes.size() > 1)
            throw cursor.CreateException("images node redifinition");

        for(const XML::Node &node : **imagesNodes.begin())
            images.insert({node.GetName(), ParseElement<ImageData>(node, texturesData, cursor)});
    }

    XML::ConstNodesSet fontColorsNodes;
    if(guiNode.FindNode("fontColors", fontColorsNodes, false)){

        const XML::Node &node = *fontColorsNodes[0];

        fontColors.default = Cast<D3DXCOLOR>(ColorFParser::FromString(node.GetNode("default").GetValue()));
        fontColors.disabled = Cast<D3DXCOLOR>(ColorFParser::FromString(node.GetNode("disabled").GetValue()));
        fontColors.selected = Cast<D3DXCOLOR>(ColorFParser::FromString(node.GetNode("selected").GetValue()));
    }
}

const ImageData &Theme::GetImageData(const std::string &ImageName) const throw (Exception)
{
    return Utils::Find(images, ImageName, ImageDataNotFoundException("Image data " + ImageName + " not found in " + name + " theme"));
}

BOOL Theme::FindImageData(const std::string &ImageName, ImageData &FoundImageData) const
{
    auto ci = images.find(ImageName);
    if(ci == images.end())
        return false;

    FoundImageData = ci->second;
    return true;
}

}
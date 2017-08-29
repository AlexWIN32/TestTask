/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <Exception.h>

namespace Meshes
{
    class IMesh;
    class IVertexAcessableMesh;
};

namespace Scene
{

DECLARE_EXCEPTION(DrawingContainerException);

class IObject;
class Object2D;
class Object3D;
class Rectangle2D;

typedef std::vector<IObject*> ObjectsGroup;
typedef std::vector<const IObject*> ConstObjectsGroup;
typedef std::vector<const Meshes::IMesh*> ConstMeshesGroup;
typedef std::vector<const Meshes::IVertexAcessableMesh*> ConstVertexAccessableMeshesGroup;
typedef std::vector<Object2D*> Objects2DGroup;
typedef std::vector<Rectangle2D*> Rectangles2DGroup;
typedef std::vector<const Rectangle2D*> ConstRectangles2DGroup;
typedef std::vector<Object3D*> Objects3DGroup;
}
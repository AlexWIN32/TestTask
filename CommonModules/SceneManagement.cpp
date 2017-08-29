/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <SceneManagement.h>
#include <CommonParams.h>
#include <Camera.h>
#include <Matrix3x3.h>
#include <Matrix4x4.h>
#include <MathHelpers.h>
#include <Vector2.h>
#include <sstream>
#include <functional>
#include <RenderStatesManager.h>
#include <SamplerStatesManager.h>
#include <Utils/ToString.h>
#include <Utils/Algorithm.h>
#include <Utils/SemanticSize.h>
#include <Meshes.h>

namespace Scene
{

void IObject::SetMaterial(UINT Subset, const Meshes::MaterialData &Material)
{
    materials[Subset] = Material;
}

bool IObject::FindMaterial(UINT Subset, Meshes::MaterialData &Material) const
{
    auto it = materials.find(Subset);
    if(it == materials.end())
        return false;

    Material = it->second;
    return true;
}

void DrawingContainer::SetMesh(const IObject *Object, const Meshes::IMesh *Mesh) throw (DrawingContainerException)
{
    if(objectsToMeshes.find(Object) == objectsToMeshes.end())
        throw DrawingContainerException("object not found");

    RemoveObject(Object);
    AddObject(Object, Mesh);
}

void DrawingContainer::AddObject(const IObject *Object, const Meshes::IMesh *Mesh) throw (DrawingContainerException)
{
	if(Mesh == NULL)
		throw DrawingContainerException("Invalid mesh");

    DrawingManagerData &data = Utils::Find(meshesToDrawingManagers, Mesh, DrawingContainerException("Drawing manager not found"));

    if(std::find(data.objects.begin(), data.objects.end(), Object) != data.objects.end())
        return;

    if(objectsToMeshes.find(Object) != objectsToMeshes.end())
        return;

    objectsToMeshes[Object] = Mesh;

    data.objects.push_back(Object);
}

void DrawingContainer::SetDrawingManager(const Meshes::IMesh *Mesh, IMeshDrawManager *DrawingManager) throw (DrawingContainerException)
{
	if(DrawingManager == NULL)
		throw DrawingContainerException("Invalid drawing manager");

    meshesToDrawingManagers[Mesh].drawingManager = DrawingManager;
}

void DrawingContainer::RemoveObject(const IObject *Object, bool ClearMesh)
{
	auto it = objectsToMeshes.find(Object);

    if(it == objectsToMeshes.end())
        return;

    auto mIt = meshesToDrawingManagers.find(it->second);

    if(mIt != meshesToDrawingManagers.end()){
            
        Utils::Remove(mIt->second.objects, Object);

        if(mIt->second.objects.size() == 0 && ClearMesh)
            meshesToDrawingManagers.erase(mIt);
    }

    objectsToMeshes.erase(it);

}

void DrawingContainer::ClearObjects(bool ClearMeshes)
{
    objectsToMeshes.clear();

    for(auto &pair : meshesToDrawingManagers)
        pair.second.objects.clear();

    if(ClearMeshes)
        meshesToDrawingManagers.clear();
}

static void DrawObject(const IObject *Object, const Meshes::IMesh *Mesh, IMeshDrawManager *DrawManager, const Camera::ICamera *Camera)
{
    DrawManager->BeginDraw(Object, Mesh, Camera);

    for(INT s = 0; s < Mesh->GetSubsetCount(); s++){

        Meshes::MaterialData material;
        if(Object && Object->FindMaterial(s, material))
            DrawManager->ProcessMaterial(Object, material);
        else
            DrawManager->ProcessMaterial(Object, Mesh->GetSubsetMaterial(s));

        Mesh->Draw(s);
    }

    DrawManager->EndDraw(Object, Mesh);
}

void DrawingContainer::Draw(const Camera::ICamera * Camera, IMeshDrawManager* CommonManager)
{
	if(CommonManager){
		CommonManager->PrepareForDrawing(Camera);

        for(auto pair : objectsToMeshes)
            DrawObject(pair.first, pair.second, CommonManager, Camera);

		CommonManager->StopDrawing();
    }else{

        for(auto pair : meshesToDrawingManagers)
            pair.second.drawingManager->PrepareForDrawing(Camera);

        for(auto pair : objectsToMeshes){

            const Meshes::IMesh *mesh = pair.second;
            IMeshDrawManager *drawManager = meshesToDrawingManagers[mesh].drawingManager;

            DrawObject(pair.first, pair.second, drawManager, Camera);
        }

        for(auto pair : meshesToDrawingManagers)
            pair.second.drawingManager->StopDrawing();
    }
}

void DrawingContainer::ForEachSpecificObject(const ConstObjectsGroup &SpecificObjects, const Camera::ICamera * Camera, ProcessFunction Function)
{
    for(const IObject *obj : SpecificObjects){
        auto oIt = objectsToMeshes.find(obj);
        if(oIt != objectsToMeshes.end()){

            const Meshes::IMesh *mesh = oIt->second;

            auto dIt = meshesToDrawingManagers.find(mesh);

            if(dIt != meshesToDrawingManagers.end())
                Function(obj, mesh, dIt->second.drawingManager, Camera);
        }
    }
}

void DrawingContainer::ForEachSpecificMesh(const ConstMeshesGroup &SpecificMeshes, const Camera::ICamera * Camera, ProcessFunction Function)
{
    for(const Meshes::IMesh *mesh : SpecificMeshes){

        auto dIt = meshesToDrawingManagers.find(mesh);

        if(dIt == meshesToDrawingManagers.end())
            continue;

        for(const IObject *obj : dIt->second.objects)
            Function(obj, mesh, dIt->second.drawingManager, Camera);
    }
}

void DrawingContainer::Draw(const ConstObjectsGroup &SpecificObjects, const Camera::ICamera * Camera, IMeshDrawManager* CommonManager)
{
    if(CommonManager){
        CommonManager->PrepareForDrawing(Camera);

        ForEachSpecificObject(SpecificObjects, Camera, 
        [&](const IObject *Object, const Meshes::IMesh *Mesh, IMeshDrawManager *DrawManager, const Camera::ICamera *Camera)
        {
            DrawObject(Object, Mesh, CommonManager, Camera);
        });

        CommonManager->StopDrawing();
    }else{
        std::vector<IMeshDrawManager*> drawingManagers;

        ForEachSpecificObject(SpecificObjects, Camera, 
        [&](const IObject *Object, const Meshes::IMesh *Mesh, IMeshDrawManager *DrawManager, const Camera::ICamera *Camera)
        {
            drawingManagers.push_back(DrawManager);
        });

        for(IMeshDrawManager *manager : drawingManagers)
            manager->PrepareForDrawing(Camera);

        ForEachSpecificObject(SpecificObjects, Camera, DrawObject);

        for(IMeshDrawManager *manager : drawingManagers)
            manager->StopDrawing();
    }
}

void DrawingContainer::Draw(const ConstMeshesGroup &SpecificMeshes, const Camera::ICamera *Camera, IMeshDrawManager *CommonManager)
{
    if(CommonManager){
        CommonManager->PrepareForDrawing(Camera);

        ForEachSpecificMesh(SpecificMeshes, Camera, 
        [&](const IObject *Object, const Meshes::IMesh *Mesh, IMeshDrawManager *DrawManager, const Camera::ICamera *Camera)
        {
            DrawObject(Object, Mesh, CommonManager, Camera);
        });

        CommonManager->StopDrawing();
    }else{
        std::vector<IMeshDrawManager*> drawingManagers;

        ForEachSpecificMesh(SpecificMeshes, Camera, 
        [&](const IObject *Object, const Meshes::IMesh *Mesh, IMeshDrawManager *DrawManager, const Camera::ICamera *Camera)
        {
            drawingManagers.push_back(DrawManager);
        });

        for(IMeshDrawManager *manager : drawingManagers)
            manager->PrepareForDrawing(Camera);

        ForEachSpecificMesh(SpecificMeshes, Camera, DrawObject);

        for(IMeshDrawManager *manager : drawingManagers)
            manager->StopDrawing();
    }
}

Object3D::Object3D() : GenericObject<Point3F, Size3F, Vector3>()
{
    Scalling() = {1.0f, 1.0f, 1.0f};
    Rotation() = {0.0f, 0.0f, 0.0f};
    Pos() = {0.0f, 0.0f, 0.0f};
}

void Object3D::CalculateMatrix()
{
    Matrix4x4 mTrans = Matrix4x4::Translation(GetPos());
    Matrix4x4 mRot = Matrix4x4::RotationYawPitchRoll(GetRotation());
    Matrix4x4 mScl = Matrix4x4::Scalling(GetScalling());

    WorldMatrix() = mScl * mRot * mTrans;
}

}
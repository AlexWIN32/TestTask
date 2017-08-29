/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <map>
#include <vector>
#include <functional>
#include <Exception.h>
#include <MeshesFwd.h>
#include <Vector2.h>
#include <Matrix4x4.h>
#include <Shader.h>
#include <SceneManagementFwd.h>
#include <Utils/SharedCOM.h>
#include <stdint.h>

namespace Camera
{
    class ICamera;
};   

namespace Scene
{

class IObject
{
private:
    typedef std::map<uint32_t, Meshes::MaterialData> MaterialsStorage;
    MaterialsStorage materials;
public:
    void SetMaterial(uint32_t Subest, const Meshes::MaterialData &Material);
    bool FindMaterial(uint32_t Subest, Meshes::MaterialData &Material) const;
    virtual ~IObject(){}
    virtual const Matrix4x4 &GetWorldMatrix() const = 0;
};

class IMeshDrawManager
{
protected:
    IMeshDrawManager(){}
public:
    virtual ~IMeshDrawManager(){}
    virtual void PrepareForDrawing(const Camera::ICamera * Camera){}
    virtual void BeginDraw(const IObject *Object, const Meshes::IMesh *Mesh, const Camera::ICamera * Camera){}
    virtual void ProcessMaterial(const IObject *Object, const Meshes::MaterialData &Material){}
    virtual void EndDraw(const IObject *Object, const Meshes::IMesh *Mesh){}
    virtual void StopDrawing(){}
};

class DrawingContainer
{
private:
    struct DrawingManagerData
    {
        IMeshDrawManager* drawingManager = NULL;
        std::vector<const IObject *> objects;
    };
    typedef std::map<const Meshes::IMesh*, DrawingManagerData> MeshesToDrawingManagersStorage;
    typedef std::map<const IObject*, const Meshes::IMesh*> ObjectsToMeshesStorage;
    typedef std::function<void(const IObject *Object, 
                      const Meshes::IMesh *Mesh, 
                      IMeshDrawManager *DrawManager, 
                      const Camera::ICamera *Camera)> ProcessFunction;
    MeshesToDrawingManagersStorage meshesToDrawingManagers;
    ObjectsToMeshesStorage objectsToMeshes;
    void ForEachSpecificObject(const ConstObjectsGroup &SpecificObjects, const Camera::ICamera * Camera, ProcessFunction Function);
    void ForEachSpecificMesh(const ConstMeshesGroup &SpecificMeshes, const Camera::ICamera * Camera, ProcessFunction Function);
public:
    void SetDrawingManager(const Meshes::IMesh *Mesh, IMeshDrawManager *DrawingManager) throw (Exception);
    void SetMesh(const IObject *Object, const Meshes::IMesh *Mesh) throw (Exception);
    void AddObject(const IObject *Object, const Meshes::IMesh *Mesh) throw (Exception);
    void RemoveObject(const IObject *Object, bool ClearMesh = true);
    void ClearObjects(bool ClearMeshes = true);
    void Draw(const Camera::ICamera * Camera, IMeshDrawManager* CommonManager = NULL);
    void Draw(const ConstObjectsGroup &SpecificObjects, const Camera::ICamera * Camera, IMeshDrawManager* CommonManager = NULL);
    void Draw(const ConstMeshesGroup &SpecificMeshes, const Camera::ICamera *Camera, IMeshDrawManager *CommonManager = NULL);
};

template<class TPosition, class TScalling, class TRotation>
class GenericObject : public IObject
{
private:
    TScalling scalling;
    TPosition position;
    TRotation rotation;
    Matrix4x4 matWorld;
protected:
    virtual void CalculateMatrix() = 0;
    TScalling &Scalling() {return scalling;}
    TPosition &Pos() {return position;}
    TRotation &Rotation() { return rotation;};
    Matrix4x4 &WorldMatrix() {return matWorld;}
public:
    virtual ~GenericObject(){}
    GenericObject() {}
    virtual void SetScalling(const TScalling &NewScalling)
    {
        if(scalling != NewScalling){
            scalling = NewScalling;
            CalculateMatrix();
        }
    }
    virtual const TScalling &GetScalling() const {return scalling;}
    virtual void SetRotation(const TRotation &NewRotation)
    {
        if(rotation != NewRotation){
            rotation = NewRotation;
            CalculateMatrix();
        }
    }
    virtual const TRotation &GetRotation() const {return rotation;}
    virtual void SetPos(const TPosition &NewPos)
    {
        if(position != NewPos){
            position = NewPos;
            CalculateMatrix();
        }
    }
    virtual const TPosition &GetPos() const { return position; }
    virtual const Matrix4x4 &GetWorldMatrix() const {return matWorld; };
};

class Object3D : public GenericObject<Point3F, Size3F, Vector3>
{
protected:
    virtual void CalculateMatrix();
public:
    virtual ~Object3D(){}
    Object3D();
};

}
/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once 
#include <Vector2.h>
#include <Matrix4x4.h>
#include <Basis.h>

namespace Camera
{

class ICamera
{
public:
	virtual ~ICamera(){}
    virtual const Matrix4x4 &GetViewMatrix() const = 0;
    virtual const Matrix4x4 &GetProjMatrix() const = 0;
    virtual const Point3F &GetPos() const = 0;
    virtual const Vector3 &GetDir() const = 0;
    virtual void Invalidate(float Tf) {}
};

class IMovingCamera : public virtual ICamera
{
public:
    virtual ~IMovingCamera(){}
    virtual void SetSpeed(float NewSpeed) = 0;
    virtual float GetSpeed() const = 0;
};

class EyeCamera : 
    private Basis::UVNBasis,
	public IMovingCamera
{
private:
    Vector2 angles;
    bool isFlying = false;
    mutable Matrix4x4 invertMatrix;
    Matrix4x4 projMatrix;
    float speed = 1.0f;
public:
    EyeCamera(){}
    virtual float GetSpeed() const {return speed;} 
    virtual void SetSpeed(float Speed) {speed = Speed;}
    bool IsFlying() const {return isFlying;}
    void SetFlyingMode(bool IsFlying) {isFlying = IsFlying;}
    void SetPos(const Point3F &Pos){UVNBasis::SetPos(Pos);} 
    virtual const Point3F &GetPos() const {return UVNBasis::GetPos();}
    virtual void SetDir(const Vector3 &Dir);
    virtual const Vector3 &GetDir() const {return UVNBasis::GetDir();}
    void SetProjMatrix(const Matrix4x4 &ProjMatrix) { projMatrix = ProjMatrix; }
    virtual const Matrix4x4 &GetProjMatrix() const { return projMatrix; }
    virtual const Matrix4x4 &GetViewMatrix() const;
	virtual void Invalidate(float Tf);
};

}
/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once 
#include <Vector2.h>
#include <Matrix4x4.h>

namespace Basis
{

class IBasis
{
private:
    mutable Point3F pos = {0.0f, 0.0f, 0.0f};
    mutable Vector3 dir = {1.0f, 0.0f, 0.0f};
    mutable Vector3 up = {0.0f, 1.0f, 0.0f};
    mutable Vector3 right = {0.0f, 0.0f, 1.0f};
    mutable Matrix4x4 matrix;
    mutable bool needMatrixUpdate = true;
protected:
    Point3F &Pos() const {return pos;}
    Vector3 &Dir() const {return dir;}
    Vector3 &Up() const {return up;}
    Vector3 &Right() const {return right;}
    Matrix4x4 &Matrix() const {return matrix;}
    void SetNeedUpdate() {needMatrixUpdate = true;}
    void DropNeedUpdate() const {needMatrixUpdate = false;}
    bool NeedMatrixUpdate() const {return needMatrixUpdate;}
public:
    virtual ~IBasis(){}
    virtual void UpdateMatrix() const = 0;
    const Vector3 &GetDir() const;
    const Point3F &GetPos() const;
    const Vector3 &GetUp() const;
    const Vector3 &GetRight() const;
    const Matrix4x4 &GetMatrix() const;
};

class UVNBasis : public IBasis
{
private:
    bool invertUp = false;
public:
    virtual ~UVNBasis(){}
    virtual void UpdateMatrix() const;
    void SetInvertUp(bool InvertUp){invertUp = InvertUp;}
    bool GetInvertUp() const {return invertUp;}
    void SetDir(const Vector3 &Dir);
    void SetPos(const Point3F &Pos);
};

}
/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <Basis.h>
#include <MathHelpers.h>

namespace Basis
{

const Vector3 &IBasis::GetDir() const
{
    if(needMatrixUpdate){
        UpdateMatrix();
        needMatrixUpdate = false;
    }

    return dir;
}

const Point3F &IBasis::GetPos() const
{
    if(needMatrixUpdate){
        UpdateMatrix();
        needMatrixUpdate = false;
    }

    return pos;
}

const Vector3 &IBasis::GetUp() const
{
    if(needMatrixUpdate){
        UpdateMatrix();
        needMatrixUpdate = false;
    }

    return up;
}

const Vector3 &IBasis::GetRight() const
{
    if(needMatrixUpdate){
        UpdateMatrix();
        needMatrixUpdate = false;
    }

    return right;
}

const Matrix4x4 &IBasis::GetMatrix() const
{
    if(needMatrixUpdate){
        UpdateMatrix();
        needMatrixUpdate = false;
    }

    return matrix;
}

static void Rotate(const Vector3 &Axis, float Angle, Vector3 &Dir, Vector3 &Right, bool InverseCross)
{
    Dir = Vector3::Normalize(Cast<Vector3>(Math::RotationAxis(Dir, Axis, Angle)));

    if(InverseCross)
        Right = Vector3::Normalize(Vector3::Cross(Axis, Dir));
    else 
        Right = Vector3::Normalize(Vector3::Cross(Dir, Axis));
}

void UVNBasis::SetDir(const Vector3 &NewDir)
{
    if(Dir() != NewDir)
        SetNeedUpdate();

    Dir() = NewDir;
}

void UVNBasis::SetPos(const Point3F &NewPos)
{
    if(Pos() != NewPos)
        SetNeedUpdate();

    Pos() = NewPos;
}

void UVNBasis::UpdateMatrix() const
{
    if (!NeedMatrixUpdate())
        return;

    DropNeedUpdate();

    Point3F &pos = Pos();
    Vector3 &up = Up(), &dir = Dir(), &right = Right();

    Vector3 up2(0.0, 1.0, 0.0);

    if(invertUp && Math::Dot({dir.x, dir.y, 0.0f}, {1.0f, 0.0f, 0.0f}) > 0.0f)
        up2 = {0.0, -1.0, 0.0};

    right = Vector3::Normalize(Vector3::Cross(up2, dir));

    if(right.x == 0.0f && right.y == 0.0f && right.z == 0.0f)
        right = Vector3::Normalize(Vector3::Cross({0.0f, 0.0f, 1.0f}, dir));

    up = Vector3::Normalize(Vector3::Cross(dir, right));

    Matrix() = {Vector4(right, 0.0f), Vector4(up, 0.0f), Vector4(dir, 0.0f), Vector4(pos, 1.0f)};
}

}
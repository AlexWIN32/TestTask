/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <Collision.h>
#include <Matrix3x3.h>
#include <MathHelpers.h>
#include <Utils/VertexArray.h>

namespace Collision
{

bool TriangleVsLine(const Point3F &A, const Point3F &B, const Point3F &C, 
                    const Point3F &LineStart, const Vector3 &LineDir,
                    FLOAT LineLen,
                    Point3F &IntersectPos)
{
    Vector3 v1 = B - A, v2 = C - A;
    Vector3 ans = LineStart - A;

    Matrix3x3 coefMatrix(v1, v2, -LineDir);

    Point3F coeffs = Matrix3x3::Transform(Matrix3x3::Inverse(coefMatrix), ans);

    if(coeffs.x >= 0.0f && coeffs.y >= 0.0f && 
       coeffs.x + coeffs.y <= 1.0f && 
       coeffs.z >= 0.0f && coeffs.z <= LineLen)
    {
        IntersectPos = LineStart + LineDir * coeffs.z;
        return true;
    }

    return false;
}

bool TriangleVsLine(const Point3F &A, const Point3F &B, const Point3F &C,
                    const Point3F &LineStart, const Point3F &LineEnd,
                    Point3F &IntersectPos)
{
    Vector3 lineDir = LineEnd - LineStart;

    return TriangleVsLine(A, B, C, LineStart, Vector3::Normalize(lineDir), lineDir.Lenght(), IntersectPos);
}

}

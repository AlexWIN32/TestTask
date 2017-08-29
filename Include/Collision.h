/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <MeshesFwd.h>
#include <Vector2Fwd.h>
#include <vector>

namespace Collision
{

bool TriangleVsLine(const Point3F &A, const Point3F &B, const Point3F &C, 
                    const Point3F &LineStart, const Vector3 &LineDir,
                    FLOAT LineLen,
                    Point3F &IntersectPos);

bool TriangleVsLine(const Point3F &A, const Point3F &B, const Point3F &C,
                    const Point3F &LineStart, const Point3F &LineEnd,
                    Point3F &IntersectPos);

}
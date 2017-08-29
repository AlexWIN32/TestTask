/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <Camera.h>
#include <DirectInput.h>
#include <MathHelpers.h>

namespace Camera
{

void EyeCamera::SetDir(const Vector3 &Dir)
{
    if(Dir.x == 0.0f && Dir.z == 0.0f)
        angles.x = 0.0f;
    else
        angles.x = Math::DirectionToAngle({Dir.x, Dir.z});

    angles.y = acos(Dir.y) - Pi * 0.5f;

    UVNBasis::SetDir(Dir);
}

void EyeCamera::Invalidate(float Tf)
{
	DirectInput::MouseState mState = DirectInput::GetInsance()->GetMouseDelta();	

	if (mState.x || mState.y){

        angles.x = angles.x - mState.x / 80.0f;
        angles.y = Math::Saturate(angles.y + mState.y / 80.0f, RangeF(-Pi * 0.499f, Pi * 0.499f));

        Point3F pt = Math::SphericalToDec({angles.x, angles.y + Pi * 0.5f});

        UVNBasis::SetDir(Vector3::Normalize(Cast<Vector3>(pt)));
	}

    float moveFactor = 0.0f, sideFactor = 0.0f;

    DirectInput::GetInsance()->ProcessKeyboardDown({
        {DIK_W, [&](){moveFactor = 1.0f;}},
        {DIK_S, [&](){moveFactor = -1.0f;}},
        {DIK_D, [&](){sideFactor = 1.0f;}},
        {DIK_A, [&](){sideFactor = -1.0f;}},
    });

    if(moveFactor != 0.0f){
        if(isFlying)
            SetPos(GetPos() + GetDir() * Tf * moveFactor * speed);
        else{                                    

            Vector3 xzPlaneDir = {GetDir().x, 0.0f, GetDir().z};

            SetPos(GetPos() + xzPlaneDir * Tf * moveFactor * speed);
        }
    }

    if(sideFactor != 0.0f){
        GetViewMatrix();
        SetPos(GetPos() + GetRight() * Tf * sideFactor * speed);
    }
}

const Matrix4x4 &EyeCamera::GetViewMatrix() const
{
    invertMatrix = Matrix4x4::Inverse(UVNBasis::GetMatrix());
    return invertMatrix;
}

}
/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

cbuffer buff1 : register(b0)
{
    matrix worldViewProj;
}

struct VIn
{
    float4 posL : POSITION;
};

struct VOut
{
    float4 posS : SV_POSITION;
};

VOut ProcessVertex(VIn input)
{
    VOut output;
    output.posS = mul(input.posL, worldViewProj);
    return output;
}
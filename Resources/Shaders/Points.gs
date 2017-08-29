/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include "Points.fxh"

cbuffer buff1 : register(b0)
{
    matrix invView;
    matrix viewProj;
}

cbuffer buff2 : register(b1)
{
    float4 eyePosW;
}

cbuffer buff3 : register(b2)
{
    float pointScaleA;
    float pointScaleB;
    float pointScaleC;
    float pointScaleMin;
    float pointScaleMax;
    float3 padding;
}

[maxvertexcount(6)]
void ProcessVertex(point VsToGs input[1], inout TriangleStream<GsToPs> stream)
{
    VsToGs inp = input[0];

    float d = length(eyePosW - inp.posW);

    float3 sV = float3(pointScaleA, pointScaleB, pointScaleC);
    float3 dV = float3(1.0f, d, d * d);

    float sz = clamp(inp.pointSize * sqrt(dot(sV, dV)), pointScaleMin, pointScaleMax);

    float4 pts[6];
    pts[0] = float4(-sz, -sz, 0.0f, 0.0f);
    pts[1] = float4(-sz,  sz, 0.0f, 0.0f);
    pts[2] = float4( sz,  sz, 0.0f, 0.0f);
    pts[3] = float4( sz,  sz, 0.0f, 0.0f);
    pts[4] = float4( sz, -sz, 0.0f, 0.0f);
    pts[5] = float4(-sz, -sz, 0.0f, 0.0f);

    GsToPs vert;
    [unroll]
    for(int i = 0; i < 6; i++){
        float4 pt = mul(pts[i], invView) + float4(inp.posW.xyz, 1.0f);

        vert.posH = mul(pt, viewProj);
        vert.color = inp.color;
        stream.Append(vert);
    }

    stream.RestartStrip();
}
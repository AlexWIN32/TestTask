/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

struct VsToGs
{
    float4 posW : POSITION0;
    float4 color : COLOR0;
    float pointSize : PSIZE;
};

struct GsToPs
{
    float4 posH : SV_POSITION;
    float4 color : COLOR;
};
/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <D3DHeaders.h>
#include <Exception.h>
#include <MeshesFwd.h>
#include <stdint.h>

namespace Utils
{

namespace DirectX
{

DECLARE_EXCEPTION(SemanticSizeException);

uint32_t GetSemanticByteSize(DXGI_FORMAT Format) throw (Exception);

uint32_t GetVertexByteSize(const Meshes::VertexMetadata &VertexMeta) throw (Exception);

}

}
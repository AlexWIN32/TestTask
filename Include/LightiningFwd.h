/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <Vector2.h>

namespace Lightining
{

struct Material
{
    ColorF ambient = {0.0f, 0.0f, 0.0f, 0.0f};
    ColorF diffuse = {1.0f, 1.0f, 1.0f, 1.0f};
    ColorF specular = {0.0f, 0.0f, 0.0f, 0.0f};
};

};
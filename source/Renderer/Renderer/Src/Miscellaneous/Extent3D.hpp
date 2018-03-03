/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_Extent3D_HPP___
#define ___Renderer_Extent3D_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies a three dimensional extent.
	*\~french
	*\brief
	*	Définit une étendue en 3 dimensions.
	*/
	struct Extent3D
	{
		uint32_t width;
		uint32_t height;
		uint32_t depth;
	};
}

#endif

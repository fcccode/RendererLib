/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include "GlRenderer/GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	class CommandBase
	{
	public:
		virtual ~CommandBase()noexcept;

		virtual void apply()const = 0;
		virtual CommandPtr clone()const = 0;
	};
}

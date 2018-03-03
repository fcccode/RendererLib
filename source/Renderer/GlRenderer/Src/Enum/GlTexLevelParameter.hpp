/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <Enum/AccessFlag.hpp>

namespace gl_renderer
{
	enum GlTexLevelParameter
	{
		GL_TEXTURE_WIDTH = 0x1000,
		GL_TEXTURE_HEIGHT = 0x1001,
		GL_TEXTURE_INTERNAL_FORMAT = 0x1003,
		GL_TEXTURE_COMPRESSED_IMAGE_SIZE = 0x86A0,
		GL_TEXTURE_COMPRESSED = 0x86A1,
		GL_TEXTURE_RED_TYPE = 0x8C10,
		GL_TEXTURE_GREEN_TYPE = 0x8C11,
		GL_TEXTURE_BLUE_TYPE = 0x8C12,
		GL_TEXTURE_ALPHA_TYPE = 0x8C13,
		GL_TEXTURE_DEPTH_TYPE = 0x8C16,
		GL_TEXTURE_BUFFER_OFFSET = 0x919D,
		GL_TEXTURE_BUFFER_SIZE = 0x919E,
	};
	inline std::string getName( GlTexLevelParameter value )
	{
		switch ( value )
		{
		case GL_TEXTURE_WIDTH:
			return "GL_TEXTURE_WIDTH";

		case GL_TEXTURE_HEIGHT:
			return "GL_TEXTURE_HEIGHT";

		case GL_TEXTURE_INTERNAL_FORMAT:
			return "GL_TEXTURE_INTERNAL_FORMAT";

		case GL_TEXTURE_COMPRESSED_IMAGE_SIZE:
			return "GL_TEXTURE_COMPRESSED_IMAGE_SIZE";

		case GL_TEXTURE_COMPRESSED:
			return "GL_TEXTURE_COMPRESSED";

		case GL_TEXTURE_RED_TYPE:
			return "GL_TEXTURE_RED_TYPE";

		case GL_TEXTURE_GREEN_TYPE:
			return "GL_TEXTURE_GREEN_TYPE";

		case GL_TEXTURE_BLUE_TYPE:
			return "GL_TEXTURE_BLUE_TYPE";

		case GL_TEXTURE_ALPHA_TYPE:
			return "GL_TEXTURE_ALPHA_TYPE";

		case GL_TEXTURE_DEPTH_TYPE:
			return "GL_TEXTURE_DEPTH_TYPE";

		case GL_TEXTURE_BUFFER_OFFSET:
			return "GL_TEXTURE_BUFFER_OFFSET";

		case GL_TEXTURE_BUFFER_SIZE:
			return "GL_TEXTURE_BUFFER_SIZE";

		default:
			assert( false && "Unsupported GlTexLevelParameter" );
			return "GlTexLevelParameter_UNKNOWN";
		}

		return std::string{  };
	}
}

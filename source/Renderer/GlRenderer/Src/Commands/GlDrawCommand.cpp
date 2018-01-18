/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlDrawCommand.hpp"

namespace gl_renderer
{
	DrawCommand::DrawCommand( uint32_t vtxCount
		, uint32_t instCount
		, uint32_t firstVertex
		, uint32_t firstInstance
		, renderer::PrimitiveTopology mode )
		: m_vtxCount{ vtxCount }
		, m_instCount{ instCount }
		, m_firstVertex{ firstVertex }
		, m_firstInstance{ firstInstance }
		, m_mode{ convert( mode ) }
	{
	}

	void DrawCommand::apply()const
	{
		if ( m_instCount > 1 )
		{
			glDrawArraysInstancedBaseInstance( m_mode
				, m_firstVertex
				, m_vtxCount
				, m_instCount
				, m_firstInstance );
		}
		else
		{
			glDrawArrays( m_mode
				, m_firstVertex
				, m_vtxCount );
		}
	}

	CommandPtr DrawCommand::clone()const
	{
		return std::make_unique< DrawCommand >( *this );
	}
}
/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Core/Device.hpp"

#include "Buffer/GeometryBuffers.hpp"
#include "Core/Renderer.hpp"
#include "Core/SwapChain.hpp"
#include "Pipeline/PipelineLayout.hpp"

namespace renderer
{
	Device::Device( Renderer const & renderer
		, Connection const & connection )
		: m_renderer{ renderer }
	{
	}

	void Device::enable()const
	{
		doEnable();
		onEnabled( *this );
	}

	void Device::disable()const
	{
		onDisabled( *this );
		doDisable();
	}

	Mat4 Device::infinitePerspective( Angle fovy
		, float aspect
		, float zNear )const
	{
		float const range = tan( float( fovy ) / float( 2 ) ) * zNear;
		float const left = -range * aspect;
		float const right = range * aspect;
		float const bottom = -range;
		float const top = range;

		Mat4 result{ float{ 0 } };
		result[0][0] = ( float( 2 ) * zNear ) / ( right - left );
		result[1][1] = ( float( 2 ) * zNear ) / ( top - bottom );
		result[2][2] = -float( 1 );
		result[2][3] = -float( 1 );
		result[3][2] = -float( 2 ) * zNear;
		return result;
	}

	ClipDirection Device::getClipDirection()const
	{
		return m_renderer.getClipDirection();
	}

	GeometryBuffersPtr Device::createGeometryBuffers( VertexBufferBase const & vbo
		, uint64_t vboOffset
		, VertexLayout const & layout )const
	{
		return createGeometryBuffers( VertexBufferCRefArray{ vbo }
			, { vboOffset }
			, VertexLayoutCRefArray{ layout } );
	}

	GeometryBuffersPtr Device::createGeometryBuffers( VertexBufferBase const & vbo
		, uint64_t vboOffset
		, VertexLayout const & layout
		, BufferBase const & ibo
		, uint64_t iboOffset
		, IndexType type )const
	{
		return createGeometryBuffers( VertexBufferCRefArray{ vbo }
			, { vboOffset }
			, VertexLayoutCRefArray{ layout }
			, ibo
			, iboOffset
			, type );
	}

	PipelineLayoutPtr Device::createPipelineLayout()const
	{
		return createPipelineLayout( DescriptorSetLayoutCRefArray{}
			, PushConstantRangeCRefArray{} );
	}

	PipelineLayoutPtr Device::createPipelineLayout( DescriptorSetLayout const & layout )const
	{
		return createPipelineLayout( DescriptorSetLayoutCRefArray{ layout }
			, PushConstantRangeCRefArray{} );
	}

	PipelineLayoutPtr Device::createPipelineLayout( PushConstantRange const & pushConstantRange )const
	{
		return createPipelineLayout( DescriptorSetLayoutCRefArray{}
			, PushConstantRangeCRefArray{ pushConstantRange } );
	}

	PipelineLayoutPtr Device::createPipelineLayout( DescriptorSetLayout const & layout
		, PushConstantRange const & pushConstantRange )const
	{
		return createPipelineLayout( DescriptorSetLayoutCRefArray{ layout }
			, PushConstantRangeCRefArray{ pushConstantRange } );
	}

	PipelineLayoutPtr Device::createPipelineLayout( DescriptorSetLayoutCRefArray const & layouts )const
	{
		return createPipelineLayout( layouts
			, PushConstantRangeCRefArray{} );
	}

	PipelineLayoutPtr Device::createPipelineLayout( PushConstantRangeCRefArray const & pushConstantRanges )const
	{
		return createPipelineLayout( DescriptorSetLayoutCRefArray{}
			, PushConstantRangeCRefArray{ pushConstantRanges } );
	}
}

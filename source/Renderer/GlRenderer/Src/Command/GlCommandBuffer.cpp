/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Command/GlCommandBuffer.hpp"

#include "Buffer/GlBuffer.hpp"
#include "Buffer/GlGeometryBuffers.hpp"
#include "Buffer/GlUniformBuffer.hpp"
#include "Command/GlCommandPool.hpp"
#include "Core/GlDevice.hpp"
#include "Core/GlRenderingResources.hpp"
#include "Descriptor/GlDescriptorSet.hpp"
#include "Image/GlTexture.hpp"
#include "Pipeline/GlComputePipeline.hpp"
#include "Pipeline/GlPipeline.hpp"
#include "Pipeline/GlPipelineLayout.hpp"
#include "RenderPass/GlFrameBuffer.hpp"
#include "RenderPass/GlRenderPass.hpp"
#include "Sync/GlBufferMemoryBarrier.hpp"
#include "Sync/GlImageMemoryBarrier.hpp"

#include "Commands/GlBeginQueryCommand.hpp"
#include "Commands/GlBeginRenderPassCommand.hpp"
#include "Commands/GlBindComputePipelineCommand.hpp"
#include "Commands/GlBindDescriptorSetCommand.hpp"
#include "Commands/GlBindGeometryBuffersCommand.hpp"
#include "Commands/GlBindPipelineCommand.hpp"
#include "Commands/GlBlitImageCommand.hpp"
#include "Commands/GlBufferMemoryBarrierCommand.hpp"
#include "Commands/GlClearColourCommand.hpp"
#include "Commands/GlClearDepthStencilCommand.hpp"
#include "Commands/GlCopyBufferCommand.hpp"
#include "Commands/GlCopyBufferToImageCommand.hpp"
#include "Commands/GlCopyImageCommand.hpp"
#include "Commands/GlCopyImageToBufferCommand.hpp"
#include "Commands/GlDispatchCommand.hpp"
#include "Commands/GlDrawCommand.hpp"
#include "Commands/GlDrawIndexedCommand.hpp"
#include "Commands/GlEndQueryCommand.hpp"
#include "Commands/GlEndRenderPassCommand.hpp"
#include "Commands/GlImageMemoryBarrierCommand.hpp"
#include "Commands/GlNextSubpassCommand.hpp"
#include "Commands/GlPushConstantsCommand.hpp"
#include "Commands/GlResetQueryPoolCommand.hpp"
#include "Commands/GlScissorCommand.hpp"
#include "Commands/GlSetLineWidthCommand.hpp"
#include "Commands/GlViewportCommand.hpp"
#include "Commands/GlWriteTimestampCommand.hpp"

#include <Buffer/StagingBuffer.hpp>
#include <Buffer/VertexBuffer.hpp>

namespace gl_renderer
{
	CommandBuffer::CommandBuffer( Device const & device
		, renderer::CommandPool const & pool
		, bool primary )
		: renderer::CommandBuffer{ device, pool, primary }
		, m_device{ device }
	{
	}

	bool CommandBuffer::begin( renderer::CommandBufferUsageFlags flags )const
	{
		m_commands.clear();
		m_beginFlags = flags;
		return true;
	}

	bool CommandBuffer::begin( renderer::CommandBufferUsageFlags flags
		, renderer::RenderPass const & renderPass
		, uint32_t subpass
		, renderer::FrameBuffer const & frameBuffer
		, bool occlusionQueryEnable
		, renderer::QueryControlFlags queryFlags
		, renderer::QueryPipelineStatisticFlags pipelineStatistics )const
	{
		m_commands.clear();
		m_beginFlags = flags;
		return true;
	}

	bool CommandBuffer::end()const
	{
		m_pushConstantBuffers.clear();
		return gl::GetError() == GL_NO_ERROR;
	}

	bool CommandBuffer::reset( renderer::CommandBufferResetFlags flags )const
	{
		m_commands.clear();
		return true;
	}

	void CommandBuffer::beginRenderPass( renderer::RenderPass const & renderPass
		, renderer::FrameBuffer const & frameBuffer
		, renderer::ClearValueArray const & clearValues
		, renderer::SubpassContents contents )const
	{
		m_currentRenderPass = &renderPass;
		m_currentFrameBuffer = &frameBuffer;
		m_currentSubpass = 0u;
		m_commands.emplace_back( std::make_unique< BeginRenderPassCommand >( renderPass
			, frameBuffer
			, clearValues
			, contents
			, m_currentSubpass++ ) );
	}

	void CommandBuffer::nextSubpass( renderer::SubpassContents contents )const
	{
		m_commands.emplace_back( std::make_unique< NextSubpassCommand >( *m_currentRenderPass
			, *m_currentFrameBuffer
			, m_currentSubpass++ ) );
	}

	void CommandBuffer::endRenderPass()const
	{
		m_commands.emplace_back( std::make_unique< EndRenderPassCommand >() );
	}

	void CommandBuffer::executeCommands( renderer::CommandBufferCRefArray const & commands )const
	{
		for ( auto & commandBuffer : commands )
		{
			for ( auto & command : static_cast< CommandBuffer const & >( commandBuffer.get() ).getCommands() )
			{
				m_commands.emplace_back( command->clone() );
			}
		}
	}

	void CommandBuffer::clear( renderer::TextureView const & image
		, renderer::RgbaColour const & colour )const
	{
		m_commands.emplace_back( std::make_unique< ClearColourCommand >( image, colour ) );
	}

	void CommandBuffer::clear( renderer::TextureView const & image
		, renderer::DepthStencilClearValue const & value )const
	{
		m_commands.emplace_back( std::make_unique< ClearDepthStencilCommand >( image, value ) );
	}

	void CommandBuffer::bindPipeline( renderer::Pipeline const & pipeline
		, renderer::PipelineBindPoint bindingPoint )const
	{
		m_currentPipeline = &static_cast< Pipeline const & >( pipeline );
		m_commands.emplace_back( std::make_unique< BindPipelineCommand >( m_device, pipeline, bindingPoint ) );

		for ( auto & pcb : m_pushConstantBuffers )
		{
			m_commands.emplace_back( std::make_unique< PushConstantsCommand >( *pcb.first
				, *pcb.second ) );
		}

		for ( auto & pcb : m_currentPipeline->getConstantsPcbs() )
		{
			m_commands.emplace_back( std::make_unique< PushConstantsCommand >( m_currentPipeline->getLayout()
				, pcb ) );
		}

		m_pushConstantBuffers.clear();
	}

	void CommandBuffer::bindPipeline( renderer::ComputePipeline const & pipeline
		, renderer::PipelineBindPoint bindingPoint )const
	{
		m_currentComputePipeline = &static_cast< ComputePipeline const & >( pipeline );
		m_commands.emplace_back( std::make_unique< BindComputePipelineCommand >( m_device, pipeline, bindingPoint ) );

		for ( auto & pcb : m_pushConstantBuffers )
		{
			m_commands.emplace_back( std::make_unique< PushConstantsCommand >( *pcb.first
				, *pcb.second ) );
		}

		for ( auto & pcb : m_currentComputePipeline->getConstantsPcbs() )
		{
			m_commands.emplace_back( std::make_unique< PushConstantsCommand >( m_currentComputePipeline->getLayout()
				, pcb ) );
		}

		m_pushConstantBuffers.clear();
	}

	void CommandBuffer::bindGeometryBuffers( renderer::GeometryBuffers const & geometryBuffers )const
	{
		m_commands.emplace_back( std::make_unique< BindGeometryBuffersCommand >( geometryBuffers ) );

		if ( geometryBuffers.hasIbo() )
		{
			m_indexType = geometryBuffers.getIbo().type;
		}
	}

	void CommandBuffer::memoryBarrier( renderer::PipelineStageFlags after
		, renderer::PipelineStageFlags before
		, renderer::BufferMemoryBarrier const & transitionBarrier )const
	{
		m_commands.emplace_back( std::make_unique< BufferMemoryBarrierCommand >( after
			, before
			, transitionBarrier ) );
	}

	void CommandBuffer::memoryBarrier( renderer::PipelineStageFlags after
		, renderer::PipelineStageFlags before
		, renderer::ImageMemoryBarrier const & transitionBarrier )const
	{
		m_commands.emplace_back( std::make_unique< ImageMemoryBarrierCommand >( after
			, before
			, transitionBarrier ) );
	}

	void CommandBuffer::bindDescriptorSets( renderer::DescriptorSetCRefArray const & descriptorSets
		, renderer::PipelineLayout const & layout
		, renderer::UInt32Array const & dynamicOffsets
		, renderer::PipelineBindPoint bindingPoint )const
	{
		for ( auto & descriptorSet : descriptorSets )
		{
			m_commands.emplace_back( std::make_unique< BindDescriptorSetCommand >( descriptorSet.get()
				, layout
				, dynamicOffsets
				, bindingPoint ) );
		}
	}

	void CommandBuffer::setViewport( renderer::Viewport const & viewport )const
	{
		m_commands.emplace_back( std::make_unique< ViewportCommand >( m_device, viewport ) );
	}

	void CommandBuffer::setScissor( renderer::Scissor const & scissor )const
	{
		m_commands.emplace_back( std::make_unique< ScissorCommand >( m_device, scissor ) );
	}

	void CommandBuffer::draw( uint32_t vtxCount
		, uint32_t instCount
		, uint32_t firstVertex
		, uint32_t firstInstance )const
	{
		m_commands.emplace_back( std::make_unique< DrawCommand >( vtxCount
			, instCount
			, firstVertex
			, firstInstance
			, m_currentPipeline->getInputAssemblyState().getTopology() ) );
	}

	void CommandBuffer::drawIndexed( uint32_t indexCount
		, uint32_t instCount
		, uint32_t firstIndex
		, uint32_t vertexOffset
		, uint32_t firstInstance )const
	{
		m_commands.emplace_back( std::make_unique< DrawIndexedCommand >( indexCount
			, instCount
			, firstIndex
			, vertexOffset
			, firstInstance
			, m_currentPipeline->getInputAssemblyState().getTopology()
			, m_indexType ) );
	}

	void CommandBuffer::copyToImage( renderer::BufferImageCopyArray const & copyInfo
		, renderer::BufferBase const & src
		, renderer::Texture const & dst )const
	{
		m_commands.emplace_back( std::make_unique< CopyBufferToImageCommand >( copyInfo
			, src
			, dst ) );
	}

	void CommandBuffer::copyToBuffer( renderer::BufferImageCopyArray const & copyInfo
		, renderer::Texture const & src
		, renderer::BufferBase const & dst )const
	{
		m_commands.emplace_back( std::make_unique< CopyImageToBufferCommand >( copyInfo
			, src
			, dst ) );
	}

	void CommandBuffer::copyBuffer( renderer::BufferCopy const & copyInfo
		, renderer::BufferBase const & src
		, renderer::BufferBase const & dst )const
	{
		m_commands.emplace_back( std::make_unique< CopyBufferCommand >( copyInfo
			, src
			, dst ) );
	}

	void CommandBuffer::copyImage( renderer::ImageCopy const & copyInfo
		, renderer::TextureView const & src
		, renderer::TextureView const & dst )const
	{
		m_commands.emplace_back( std::make_unique< CopyImageCommand >( copyInfo
			, src
			, dst ) );
	}

	void CommandBuffer::blitImage( renderer::ImageBlit const & blitInfo
		, renderer::FrameBufferAttachment const & src
		, renderer::FrameBufferAttachment const & dst
		, renderer::Filter filter )const
	{
		m_commands.emplace_back( std::make_unique< BlitImageCommand >( blitInfo
			, src
			, dst
			, filter ) );
	}

	void CommandBuffer::resetQueryPool( renderer::QueryPool const & pool
		, uint32_t firstQuery
		, uint32_t queryCount )const
	{
		m_commands.emplace_back( std::make_unique< ResetQueryPoolCommand >( pool
			, firstQuery
			, queryCount ) );
	}

	void CommandBuffer::beginQuery( renderer::QueryPool const & pool
		, uint32_t query
		, renderer::QueryControlFlags flags )const
	{
		m_commands.emplace_back( std::make_unique< BeginQueryCommand >( pool
			, query
			, flags ) );
	}

	void CommandBuffer::endQuery( renderer::QueryPool const & pool
		, uint32_t query )const
	{
		m_commands.emplace_back( std::make_unique< EndQueryCommand >( pool
			, query ) );
	}

	void CommandBuffer::writeTimestamp( renderer::PipelineStageFlag pipelineStage
		, renderer::QueryPool const & pool
		, uint32_t query )const
	{
		m_commands.emplace_back( std::make_unique< WriteTimestampCommand >( pipelineStage
			, pool
			, query ) );
	}

	void CommandBuffer::pushConstants( renderer::PipelineLayout const & layout
		, renderer::PushConstantsBufferBase const & pcb )const
	{
		if ( m_currentPipeline || m_currentComputePipeline )
		{
			m_commands.emplace_back( std::make_unique< PushConstantsCommand >( layout
				, pcb ) );
		}
		else
		{
			m_pushConstantBuffers.emplace_back( &layout, &pcb );
		}
	}

	void CommandBuffer::dispatch( uint32_t groupCountX
		, uint32_t groupCountY
		, uint32_t groupCountZ )const
	{
		m_commands.emplace_back( std::make_unique< DispatchCommand >( groupCountX
			, groupCountY 
			, groupCountZ ) );
	}

	void CommandBuffer::setLineWidth( float width )const
	{
		m_commands.emplace_back( std::make_unique< SetLineWidthCommand >( width ) );
	}
}

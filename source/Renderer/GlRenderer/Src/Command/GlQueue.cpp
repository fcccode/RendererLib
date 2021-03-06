/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Command/GlQueue.hpp"

#include "Command/GlCommandBuffer.hpp"
#include "Core/GlDevice.hpp"
#include "Sync/GlFence.hpp"
#include "Sync/GlSemaphore.hpp"
#include "Core/GlSwapChain.hpp"
#include "Commands/GlCommandBase.hpp"

namespace gl_renderer
{
	Queue::Queue( Device const & device )
		: renderer::Queue{ device }
	{
	}

	void Queue::submit( renderer::CommandBufferCRefArray const & commandBuffers
		, renderer::SemaphoreCRefArray const & semaphoresToWait
		, renderer::PipelineStageFlagsArray const & semaphoresStage
		, renderer::SemaphoreCRefArray const & semaphoresToSignal
		, renderer::Fence const * fence )const
	{
		for ( auto & commandBuffer : commandBuffers )
		{
			auto & glCommandBuffer = static_cast< CommandBuffer const & >( commandBuffer.get() );
			glCommandBuffer.initialiseGeometryBuffers();

			for ( auto & command : glCommandBuffer.getCommands() )
			{
				command->apply();
			}

			glCommandBuffer.applyPostSubmitActions();
		}
	}

	void Queue::present( renderer::SwapChainCRefArray const & swapChains
		, renderer::UInt32Array const & imagesIndex
		, renderer::SemaphoreCRefArray const & semaphoresToWait )const
	{
	}

	void Queue::waitIdle()const
	{
		glLogCall( gl::Finish );
	}
}

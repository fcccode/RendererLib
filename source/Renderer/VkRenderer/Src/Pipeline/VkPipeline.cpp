#include "Pipeline/VkPipeline.hpp"

#include "Core/VkDevice.hpp"
#include "Core/VkRenderingResources.hpp"
#include "Pipeline/VkPipelineLayout.hpp"
#include "Pipeline/VkSpecialisationInfo.hpp"
#include "Pipeline/VkSpecialisationMapEntry.hpp"
#include "Pipeline/VkVertexLayout.hpp"
#include "RenderPass/VkRenderPass.hpp"
#include "Shader/VkAttribute.hpp"
#include "Shader/VkShaderProgram.hpp"

namespace vk_renderer
{
	namespace
	{
		VertexLayoutCRefArray convert( renderer::VertexLayoutCRefArray const & layouts )
		{
			VertexLayoutCRefArray result;
			result.reserve( layouts.size() );

			for ( auto & layout : layouts )
			{
				result.emplace_back( static_cast< VertexLayout const & >( layout.get() ) );
			}

			return result;
		}
	}

	Pipeline::Pipeline( Device const & device
		, renderer::PipelineLayout const & layout
		, renderer::GraphicsPipelineCreateInfo const & createInfo )
		: renderer::Pipeline{ device
			, layout
			, createInfo }
		, m_device{ device }
		, m_layout{ static_cast< PipelineLayout const & >( layout ) }
		, m_shader{ static_cast< ShaderProgram const & >( m_createInfo.program.get() ) }
		, m_vertexLayouts{ convert( m_createInfo.vertexLayouts ) }
		, m_renderPass{ static_cast< RenderPass const & >( m_createInfo.renderPass.get() ) }
		, m_inputAssemblyState{ convert( m_createInfo.inputAssemblyState ) }
		, m_rasterisationState{ convert( m_createInfo.rasterisationState ) }
		, m_colourBlendStateAttachments{ convert< VkPipelineColorBlendAttachmentState >( m_createInfo.colourBlendState.begin(), m_createInfo.colourBlendState.end() ) }
		, m_colourBlendState{ convert( m_createInfo.colourBlendState, m_colourBlendStateAttachments ) }
		, m_multisampleState{ convert( m_createInfo.multisampleState ) }
		, m_lineWidth{ m_createInfo.rasterisationState.hasLineWidth() }
	{
		if ( m_createInfo.depthStencilState )
		{
			m_depthStencilState = convert( m_createInfo.depthStencilState.value() );
		}

		if ( m_createInfo.scissor )
		{
			m_scissor = convert( m_createInfo.scissor.value() );
		}

		if ( m_createInfo.viewport )
		{
			m_viewport = convert( m_createInfo.viewport.value() );
		}

		if ( m_createInfo.tessellationState )
		{
			m_tessellationState = convert( m_createInfo.tessellationState.value() );
		}

		if ( !m_createInfo.specialisationInfo.empty() )
		{
			m_specialisationEntries.resize( m_createInfo.specialisationInfo.size() );
			uint32_t index = 0;

			for ( auto & info : m_createInfo.specialisationInfo )
			{
				auto stage = convert( info.first );
				m_specialisationEntries[index] = convert< VkSpecializationMapEntry >( info.second.mapEntries.begin(), info.second.mapEntries.end() );
				m_specialisationInfos[stage] = convert( info.second, m_specialisationEntries[index] );
				++index;
			}
		}

		// Les informations liées aux shaders utilisés.
		std::vector< VkPipelineShaderStageCreateInfo > shaderStage;

		for ( auto & module : static_cast< ShaderProgram const & >( m_createInfo.program.get() ) )
		{
			auto it = m_specialisationInfos.find( module.getStage() );

			if ( it != m_specialisationInfos.end() )
			{
				shaderStage.emplace_back( VkPipelineShaderStageCreateInfo
				{
					VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					nullptr,
					0,                                                        // flags
					module.getStage(),                                        // stage
					VkShaderModule( module ),                                 // module
					"main",                                                   // pName
					&it->second,                                              // pSpecializationInfo
				} );
			}
			else
			{
				shaderStage.emplace_back( VkPipelineShaderStageCreateInfo
				{
					VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					nullptr,
					0,                                                        // flags
					module.getStage(),                                        // stage
					VkShaderModule( module ),                                 // module
					"main",                                                   // pName
					nullptr,                                                  // pSpecializationInfo
				} );
			}
		}

		// Les informations des données contenues dans les tampons de sommets.
		std::vector< VkVertexInputBindingDescription > vertexBindingDescriptions;
		std::vector< VkVertexInputAttributeDescription > vertexAttributeDescriptions;
		vertexBindingDescriptions.reserve( m_vertexLayouts.size() );

		for ( auto const & vb : m_vertexLayouts )
		{
			vertexBindingDescriptions.push_back( vb.get().getDescription() );

			for ( auto const & attribute : vb.get() )
			{
				if ( attribute.getFormat() == renderer::AttributeFormat::eMat4f )
				{
					uint32_t offset = attribute.getOffset();
					uint32_t location = attribute.getLocation();

					for ( auto i = 0u; i < 4u; ++i )
					{
						auto attrib = renderer::Attribute{ vb.get()
							, renderer::AttributeFormat::eVec4f
							, location
							, offset };
						vertexAttributeDescriptions.emplace_back( convert( attrib ) );
						++location;
						offset += 16u;
					}
				}
				else
				{
					vertexAttributeDescriptions.emplace_back( convert( attribute ) );
				}
			}
		}

		VkPipelineVertexInputStateCreateInfo vertexInputState
		{
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			nullptr,
			0,                                                            // flags
			static_cast< uint32_t >( vertexBindingDescriptions.size() ),  // vertexBindingDescriptionCount
			vertexBindingDescriptions.data(),                             // pVertexBindingDescriptions
			static_cast< uint32_t >( vertexAttributeDescriptions.size() ),// vertexAttributeDescriptionCount
			vertexAttributeDescriptions.data()                            // pVertexAttributeDescriptions
		};

		// Le viewport.
		VkPipelineViewportStateCreateInfo viewportState
		{
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			nullptr,
			0,                                                                      // flags
			1u,                                                                     // viewportCount
			m_viewport                                                              // pViewports
				? &m_viewport.value()
				: nullptr,
			1u,                                                                     // scissorCount
			m_scissor                                                               // pScissors
				? &m_scissor.value()
				: nullptr,
		};

		// Les états dynamiques, le cas échéant
		std::vector< VkDynamicState > dynamicStates;

		if ( !m_viewport )
		{
			dynamicStates.push_back( VK_DYNAMIC_STATE_VIEWPORT );
		}

		if ( !m_scissor )
		{
			dynamicStates.push_back( VK_DYNAMIC_STATE_SCISSOR );
		}

		if ( !m_lineWidth )
		{
			dynamicStates.push_back( VK_DYNAMIC_STATE_LINE_WIDTH );
		}

		VkPipelineDynamicStateCreateInfo dynamicState
		{
			VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,         // sType
			nullptr,                                                      // pNext
			0,                                                            // flags
			static_cast< uint32_t >( dynamicStates.size() ),              // dynamicStateCount
			dynamicStates.data()                                          // pDynamicStates
		};

		// Enfin, on crée le pipeline !!
		VkGraphicsPipelineCreateInfo pipeline
		{
			VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			nullptr,
			0,                                                            // flags
			static_cast< uint32_t >( shaderStage.size() ),                // stageCount
			shaderStage.data(),                                           // pStages
			&vertexInputState,                                            // pVertexInputState;
			&m_inputAssemblyState,                                        // pInputAssemblyState
			m_tessellationState                                           // pTessellationState
				? &m_tessellationState.value()
				: nullptr,
			&viewportState,                                               // pViewportState
			&m_rasterisationState,                                        // pRasterizationState
			&m_multisampleState,                                          // pMultisampleState
			m_depthStencilState                                           // pDepthStencilState
				? &m_depthStencilState.value()
				: nullptr,
			&m_colourBlendState,                                          // pColorBlendState
			dynamicStates.empty() ? nullptr : &dynamicState,              // pDynamicState
			m_layout,                                                     // layout
			m_renderPass,                                                 // renderPass
			0,                                                            // subpass
			VK_NULL_HANDLE,                                               // basePipelineHandle
			-1                                                            // basePipelineIndex
		};
		DEBUG_DUMP( pipeline );
		DEBUG_WRITE( "pipeline.log" );
		auto res = m_device.vkCreateGraphicsPipelines( m_device
			, VK_NULL_HANDLE
			, 1
			, &pipeline
			, nullptr
			, &m_pipeline );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Pipeline creation failed: " + getLastError() };
		}
	}

	Pipeline::~Pipeline()
	{
		m_device.vkDestroyPipeline( m_device, m_pipeline, nullptr );
	}
}

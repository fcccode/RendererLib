/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Renderer/Device.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Classe contenant les informations liées au GPU logique.
	*/
	class Device
		: public renderer::Device
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] renderer
		*	L'instance de Renderer.
		*\param[in] connection
		*	La connection à l'application.
		*/
		Device( renderer::Renderer const & renderer
			, renderer::ConnectionPtr && connection );
		/**
		*\brief
		*	Crée une passe de rendu.
		*\param[in] formats
		*	Les formats des attaches voulues pour la passe.
		*\param[in] subpasses
		*	Les sous passes (au moins 1 nécéssaire).
		*\param[in] initialState
		*	L'état voulu en début de passe.
		*\param[in] finalState
		*	L'état voulu en fin de passe.
		*\param[in] clear
		*	Dit si l'on veut vider le contenu des images au chargement de la passe.
		*\param[in] samplesCount
		*	Le nombre d'échantillons (pour le multisampling).
		*\return
		*	La passe créée.
		*/
		renderer::RenderPassPtr createRenderPass( std::vector< utils::PixelFormat > const & formats
			, renderer::RenderSubpassPtrArray const & subpasses
			, renderer::RenderPassState const & initialState
			, renderer::RenderPassState const & finalState
			, bool clear
			, renderer::SampleCountFlag samplesCount )const override;
		/**
		*\brief
		*	Crée une sous-passe de rendu.
		*\param[in] formats
		*	Les formats des attaches voulues pour la passe.
		*\param[in] neededState
		*	L'état voulu pour l'exécution de cette sous passe.
		*\return
		*	La sous-passe créée.
		*/
		renderer::RenderSubpassPtr createRenderSubpass( std::vector< utils::PixelFormat > const & formats
			, renderer::RenderSubpassState const & neededState )const override;
		/**
		*\brief
		*	Crée un layout de sommets.
		*\param[in] bindingSlot
		*	Le point d'attache du tampon associé.
		*\param[in] stride
		*	La taille en octets séparant un élément du suivant, dans le tampon.
		*\return
		*	Le layout créé.
		*/
		renderer::VertexLayoutPtr createVertexLayout( uint32_t bindingSlot
			, uint32_t stride )const override;
		/**
		*\brief
		*	Crée un layout de pipeline.
		*\param[in] layout
		*	Le layout des descripteurs utilisés par le pipeline.
		*\return
		*	Le layout créé.
		*/
		renderer::PipelineLayoutPtr createPipelineLayout( renderer::DescriptorSetLayout const & layout )const override;
		/**
		*\brief
		*	Crée un pipeline.
		*\param[in] layout
		*	Le layout du pipeline.
		*\param[in] program
		*	Le programme shader.
		*\param[in] vertexBuffers
		*	Les tampons de sommets utilisés.
		*\param[in] renderPass
		*	La passe de rendu.
		*\param[in] topology
		*	La topologie d'affichage des sommets affichés via ce pipeline.
		*\return
		*	Le pipeline créé.
		*/
		renderer::PipelinePtr createPipeline( renderer::PipelineLayout const & layout
			, renderer::ShaderProgram const & program
			, renderer::VertexLayoutCRefArray const & vertexLayouts
			, renderer::RenderPass const & renderPass
			, renderer::PrimitiveTopology topology
			, renderer::RasterisationState const & rasterisationState
			, renderer::ColourBlendState const & colourBlendState )const override;
		/**
		*\brief
		*	Crée un layout de set de descripteurs.
		*\param[in] bindings
		*	Les attaches du layout.
		*\return
		*	Le layout créé.
		*/
		renderer::DescriptorSetLayoutPtr createDescriptorSetLayout( renderer::DescriptorSetLayoutBindingArray && bindings )const override;
		/**
		*\brief
		*	Crée une texture.
		*/
		renderer::TexturePtr createTexture()const override;
		/**
		*\brief
		*	Crée un échantillonneur.
		*\param[in] wrapS, wrapT, wrapR
		*	Les modes de wrap de texture.
		*\param[in] minFilter, magFilter
		*	Les filtres de minification et magnification.
		*\param[in] mipFilter
		*	Le filtre de mipmap.
		*/
		renderer::SamplerPtr createSampler( renderer::WrapMode wrapS
			, renderer::WrapMode wrapT
			, renderer::WrapMode wrapR
			, renderer::Filter minFilter
			, renderer::Filter magFilter
			, renderer::MipmapMode mipFilter )const override;
		/**
		*\brief
		*	Crée un tampon GPU.
		*\param[in] size
		*	La taille du tampon.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] flags
		*	Les indicateurs de mémoire du tampon.
		*/
		renderer::BufferBasePtr createBuffer( uint32_t size
			, renderer::BufferTargets target
			, renderer::MemoryPropertyFlags memoryFlags )const override;
		/**
		*\brief
		*	Crée un tampon de transfert.
		*\param[in] size
		*	La taille du tampon.
		*/
		renderer::StagingBufferPtr createStagingBuffer( uint32_t size )const override;
		/**
		*\brief
		*	Crée un tampon d'uniformes.
		*\param[in] size
		*	La taille du tampon.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] flags
		*	Les indicateurs de mémoire du tampon.
		*/
		renderer::UniformBufferBasePtr createUniformBuffer( uint32_t count
			, uint32_t size
			, renderer::BufferTargets target
			, renderer::MemoryPropertyFlags memoryFlags )const override;
		/**
		*\brief
		*	Crée une swap chain.
		*\param[in] size
		*	Les dimensions souhaitées.
		*/
		renderer::SwapChainPtr createSwapChain( utils::IVec2 const & size )const override;
		/**
		*\brief
		*	Crée un sémaphore.
		*/
		renderer::SemaphorePtr createSemaphore()const override;
		/**
		*\brief
		*	Crée un pool de tampons de commandes.
		*\param[in] queueFamilyIndex
		*	L'index de la famille à laquelle appartient le pool.
		*\param[in] flags
		*	Combinaison binaire de CommandPoolCreateFlag.
		*/
		renderer::CommandPoolPtr createCommandPool( uint32_t queueFamilyIndex
			, renderer::CommandPoolCreateFlags const & flags )const override;
		/**
		*\brief
		*	Crée un programme shader.
		*/
		virtual renderer::ShaderProgramPtr createShaderProgram()const override;
		/**
		*\brief
		*	Attend que le périphérique soit inactif.
		*/
		void waitIdle()const;
		/**
		*\brief
		*	Echange les tampons.
		*/
		void swapBuffers()const;
		/**
		*\brief
		*	Le numéro de version.
		*/
		inline std::string const & getVersion()const
		{
			return m_version;
		}

	private:
		ContextPtr m_context;
		std::string m_version;
	};
}
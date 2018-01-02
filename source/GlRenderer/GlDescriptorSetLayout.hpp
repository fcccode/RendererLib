/**
*\file
*	VertexBuffer.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_DescriptorSetLayout_HPP___
#define ___VkRenderer_DescriptorSetLayout_HPP___
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <Renderer/DescriptorSetLayout.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Classe wrappant un vk::DescriptorLayout.
	*/
	class DescriptorSetLayout
		: public renderer::DescriptorSetLayout
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le p�riph�rique logique.
		*/
		DescriptorSetLayout( renderer::Device const & device
			, renderer::DescriptorSetLayoutBindingArray && bindings );
		/**
		*\brief
		*	Cr�e un pool pour les descripteurs qui utiliseront ce layout.
		*\param[in] maxSets
		*	Le nombre maximum de sets que le pool peut cr�er.
		*\return
		*	Le pool.
		*/
		renderer::DescriptorSetPoolPtr createPool( uint32_t maxSets )const override;
	};
}

#endif
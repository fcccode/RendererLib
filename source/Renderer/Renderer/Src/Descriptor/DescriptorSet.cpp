/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Descriptor/DescriptorSet.hpp"

#include "Descriptor/DescriptorSetLayoutBinding.hpp"
#include "Descriptor/DescriptorSetPool.hpp"
#include "Image/Sampler.hpp"
#include "Image/Texture.hpp"

namespace renderer
{
	DescriptorSet::DescriptorSet( DescriptorSetPool const & pool, uint32_t bindingPoint )
		: m_bindingPoint{ bindingPoint }
	{
		pool.allocate( 1u );
	}
}

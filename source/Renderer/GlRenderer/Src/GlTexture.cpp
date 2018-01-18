#include "GlTexture.hpp"

#include "GlCommandBuffer.hpp"
#include "GlDevice.hpp"
#include "GlImageMemoryBarrier.hpp"
#include "GlRenderingResources.hpp"
#include "GlTextureView.hpp"

namespace gl_renderer
{
	namespace
	{
		renderer::ImageAspectFlags getAspectFlag( renderer::PixelFormat format )
		{
			switch ( format )
			{
			case renderer::PixelFormat::eL8:
			case renderer::PixelFormat::eL8A8:
			case renderer::PixelFormat::eR8G8B8:
			case renderer::PixelFormat::eRGB565:
			case renderer::PixelFormat::eR8G8B8A8:
			case renderer::PixelFormat::eB8G8R8A8:
			case renderer::PixelFormat::eRGBA5551:
			case renderer::PixelFormat::eRGBA4444:
				return renderer::ImageAspectFlag::eColour;

			case renderer::PixelFormat::eD16:
				return renderer::ImageAspectFlag::eDepth;

			case renderer::PixelFormat::eD24S8:
				return renderer::ImageAspectFlag::eDepth | renderer::ImageAspectFlag::eStencil;

			case renderer::PixelFormat::eS8:
				return renderer::ImageAspectFlag::eStencil;

			default:
				assert( false && "Unsupported pixel format." );
				return renderer::ImageAspectFlag::eColour;
			}
		}
	}

	Texture::Texture( Device const & device )
		: renderer::Texture{ device }
		, m_device{ device }
	{
		glGenTextures( 1, &m_texture );
	}

	Texture::~Texture()
	{
		glDeleteTextures( 1, &m_texture );
	}

	void Texture::doSetImage1D( renderer::ImageUsageFlags usageFlags
		, renderer::ImageTiling tiling
		, renderer::MemoryPropertyFlags memoryFlags )
	{
		m_glType = convert( m_type );
		glBindTexture( m_glType, m_texture );

		if ( m_layerCount )
		{
			glTexImage2D( m_glType
				, 0
				, gl_renderer::getInternal( m_format )
				, m_size[0]
				, m_layerCount
				, 0
				, gl_renderer::getFormat( m_format )
				, gl_renderer::getType( m_format )
				, nullptr );
		}
		else
		{
			glTexImage1D( m_glType
				, 0
				, gl_renderer::getInternal( m_format )
				, m_size[0]
				, 0
				, gl_renderer::getFormat( m_format )
				, gl_renderer::getType( m_format )
				, nullptr );
		}

		glBindTexture( m_glType, 0 );
	}

	void Texture::doSetImage2D( renderer::ImageUsageFlags usageFlags
		, renderer::ImageTiling tiling
		, renderer::MemoryPropertyFlags memoryFlags )
	{
		m_glType = convert( m_type );
		glBindTexture( m_glType, m_texture );

		if ( m_layerCount )
		{
			glTexImage3D( m_glType
				, 0
				, gl_renderer::getInternal( m_format )
				, m_size[0]
				, m_size[1]
				, m_layerCount
				, 0
				, gl_renderer::getFormat( m_format )
				, gl_renderer::getType( m_format )
				, nullptr );
		}
		else if ( m_samples != renderer::SampleCountFlag::e1 )
		{
			glTexImage2DMultisample( m_glType
				, GLsizei( m_samples )
				, gl_renderer::getInternal( m_format )
				, m_size[0]
				, m_size[1]
				, GL_TRUE );
		}
		else
		{
			glTexImage2D( m_glType
				, 0
				, gl_renderer::getInternal( m_format )
				, m_size[0]
				, m_size[1]
				, 0
				, gl_renderer::getFormat( m_format )
				, gl_renderer::getType( m_format )
				, nullptr );
		}

		glBindTexture( m_glType, 0 );
	}

	void Texture::doSetImage3D( renderer::ImageUsageFlags usageFlags
		, renderer::ImageTiling tiling
		, renderer::MemoryPropertyFlags memoryFlags )
	{
		m_glType = convert( m_type );
		glBindTexture( m_glType, m_texture );

		if ( m_samples != renderer::SampleCountFlag::e1 )
		{
			glTexImage3DMultisample( m_glType
				, GLsizei( m_samples )
				, gl_renderer::getInternal( m_format )
				, m_size[0]
				, m_size[1]
				, m_size[2]
				, GL_TRUE );
		}
		else
		{
			glTexImage3D( m_glType
				, 0
				, gl_renderer::getInternal( m_format )
				, m_size[0]
				, m_size[1]
				, m_size[2]
				, 0
				, gl_renderer::getFormat( m_format )
				, gl_renderer::getType( m_format )
				, nullptr );
		}

		glBindTexture( m_glType, 0 );
	}

	void Texture::generateMipmaps()const
	{
		glBindTexture( m_glType, m_texture );
		glGenerateMipmap( m_glType );
		glBindTexture( m_glType, 0 );
	}

	renderer::ImageMemoryBarrier Texture::makeGeneralLayout( renderer::AccessFlags accessFlags )const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eGeneral
			, ~( 0u )
			, accessFlags );
	}

	renderer::ImageMemoryBarrier Texture::makeTransferDestination()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eTransferDstOptimal
			, ~( 0u )
			, renderer::AccessFlag::eTransferWrite );
	}

	renderer::ImageMemoryBarrier Texture::makeTransferSource()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eTransferSrcOptimal
			, ~( 0u )
			, renderer::AccessFlag::eShaderRead );
	}

	renderer::ImageMemoryBarrier Texture::makeShaderInputResource()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eShaderReadOnlyOptimal
			, ~( 0u )
			, renderer::AccessFlag::eTransferRead );
	}

	renderer::ImageMemoryBarrier Texture::makeDepthStencilReadOnly()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eDepthStencilReadOnlyOptimal
			, ~( 0u )
			, renderer::AccessFlag::eShaderRead );
	}

	renderer::ImageMemoryBarrier Texture::makeColourAttachment()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eColourAttachmentOptimal
			, ~( 0u )
			, renderer::AccessFlag::eColourAttachmentWrite );
	}

	renderer::ImageMemoryBarrier Texture::makeDepthStencilAttachment()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eDepthStencilAttachmentOptimal
			, ~( 0u )
			, renderer::AccessFlag::eColourAttachmentWrite );
	}

	renderer::ImageMemoryBarrier Texture::makePresentSource()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::ePresentSrc
			, ~( 0u )
			, renderer::AccessFlag::eMemoryRead );
	}

	renderer::ImageMemoryBarrier Texture::doMakeLayoutTransition( renderer::ImageLayout layout
		, uint32_t queueFamily
		, renderer::AccessFlags dstAccessMask )const
	{
		// On fait passer le layout de l'image � un autre layout, via une barri�re.
		return renderer::ImageMemoryBarrier
		{
			0u,                                      // srcAccessMask
			dstAccessMask,                           // dstAccessMask
			renderer::ImageLayout::eUndefined,       // oldLayout
			layout,                                  // newLayout
			~( 0u ),                                 // srcQueueFamilyIndex
			queueFamily,                             // dstQueueFamilyIndex
			*this,                                   // image
			renderer::ImageSubresourceRange          // subresourceRange
			{
				getAspectFlag( m_format ),
				0u,
				1u,
				0u,
				1u
			}
		};
	}
}
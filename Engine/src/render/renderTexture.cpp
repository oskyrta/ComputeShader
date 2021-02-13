#include "renderTexture.h"
#include "drawble.h"
#include <iostream> 

namespace ce {

	RenderTexture::~RenderTexture() {
		glDeleteFramebuffers(1, &framebuffer);
		glDeleteRenderbuffers(1, &depthbuffer);
	}

	void RenderTexture::load(int width, int height) {
		RenderTarget::load(width, height);

		texture.load(width, height);

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		glGenRenderbuffers(1, &depthbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.getID(), 0);

		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);

		if (logging && glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "ERROR::RENDERTEXTURE::RENDERTEXTURE_LOADING_FAILED" << std::endl;
		}
	}
	
}
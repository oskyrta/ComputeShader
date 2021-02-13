#pragma once
#include "renderTarget.h"
#include "texture.h"

namespace ce {

	struct RenderTexture : public RenderTarget {
		virtual ~RenderTexture();

		void load(int width, int height);
		void clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

		const Texture* getTexture() { return &texture; }

	private:
		GLuint depthbuffer;
		Texture texture;
	};

}
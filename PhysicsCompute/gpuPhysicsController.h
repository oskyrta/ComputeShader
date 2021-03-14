#pragma once
#include "polygon.h"
#include "render/renderTarget.h"

namespace pc::gpu {

	void startup();
	void init(const std::vector<Polygon>& polygons);
	void draw(ce::RenderTarget* renderTarget);
	void updatePhysics(float deltatime);

}
#pragma once
#include "transform.h"
#include "program.h"

namespace ce {

	struct Drawable {
		virtual void draw() const = 0;

		//Transform* getTransform() { return &transform; }
		//const Transform* getTransform() const { return &transform; }

		void setProgram(const Program* program) { this->program = program; }
		const Program* getProgram() const { return program; }

	protected:
		const Program* program;
	};

}
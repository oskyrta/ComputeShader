#pragma once
#include <math/mat3.h>
#include <math/vec2.h>

namespace ce {

	struct Transform {
		void setDepth(float depth) { m_depth = depth; }
		float getDepth() const { return ((m_parent ? m_parent->getDepth() : 0) + m_depth); }

		void rotate(float degrees) { m_rotation += degrees; }
		void setRotation(float degrees) { m_rotation = degrees; }
		float getRotation() const { return (m_parent ? m_parent->getRotation() + m_rotation : m_rotation); }

		void scale(vec2f scale) { m_scale *= scale; }
		void setScale(vec2f scale) { m_scale = scale; }
		vec2f getScale() const { return (m_parent ? m_scale * m_parent->getScale() : m_scale); };

		void move(vec2f movement) { m_position += movement; }
		void setPosition(vec2f position) { m_position = position; }
		vec2f getPosition() const { return (m_parent ? m_parent->getPosition() + m_position : m_position); }

		void setOrigin(vec2f origin) { m_origin = origin; }
		vec2f getOrigin() const { return (m_parent ? m_parent->getOrigin() + m_origin : m_origin); }

		void setOffset(vec2f offset) { this->m_offset = offset; }
		vec2f getOffset() const { return (m_parent ? m_parent->getOffset() + m_offset : m_offset); }

		void setSize(vec2f size) { m_size = size; }
		vec2f getSize() const { return m_size; }

		void setParentTransform(Transform* parent) { m_parent = parent; }

		mat3f getTranslate() const;
		mat3f getInvertedTranslate() const;

	protected:
		Transform* m_parent  = nullptr;
							 
		float m_depth		 = 0;
		float m_rotation	 = 0;
		vec2f m_scale	 = vec2f(1, 1);
		vec2f m_position = vec2f(0, 0);
		vec2f m_origin	 = vec2f(0, 0);
		vec2f m_offset	 = vec2f(0, 0);
		vec2f m_size	 = vec2f(0, 0);
	};

}
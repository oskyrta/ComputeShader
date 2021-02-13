#include "transform.h"

namespace ce {
		   
	mat3f Transform::getTranslate() const {
		mat3f result(1);

		result = mat3f::translate(result, -getOrigin());
		result = mat3f::scale(result, getScale());
		result = mat3f::rotate(result, getRotation());
		result = mat3f::translate(result, getPosition() + getOffset());

		return result;
	}

	mat3f Transform::getInvertedTranslate() const {
		mat3f result(1);

		result = mat3f::translate(result, getOrigin());
		result = mat3f::scale(result, vec2f(1, 1) / getScale());
		result = mat3f::rotate(result, -getRotation());
		result = mat3f::translate(result, -getPosition() - getOffset());

		return result;
	}

}
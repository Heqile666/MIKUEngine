#pragma once
#include "Miku/Core/CoreDefinitions.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/matrix_major_storage.hpp>

#define M_PI                     (3.1415926535897932f)
#define M_INV_PI                 (0.3183098861837067f)
#define M_HALF_PI                (1.5707963267948966f)
#define M_TWO_PI                 (6.2831853071795864f)
#define M_PI_SQUARED             (9.8696044010893580f)
#define M_SQRT_PI                (1.4142135623730950f)
#define SMALL_NUMBER             (1.e-8f)
#define KINDA_SMALL_NUMBER       (1.e-4f)
#define BIG_NUMBER               (3.4e+38f)
#define DELTA                    (0.00001f)


namespace MIKU
{
	using Vector2i = glm::ivec2;
	using Vector3i = glm::ivec3;
	using Vector4i = glm::ivec4;

	using Vector2u = glm::uvec2;
	using Vector3u = glm::uvec3;
	using Vector4u = glm::uvec4;

	using Vector2f = glm::fvec2;
	using Vector3f = glm::fvec3;
	using Vector4f = glm::fvec4;

	using Vector2d = glm::dvec2;
	using Vector3d = glm::dvec3;
	using Vector4d = glm::dvec4;


}

namespace MIKU::Math
{

	template<typename T>
	FORCEINLINE T Max(const T& a, const T& b)
	{
		return (a >= b) ? a : b;
	}

	template<typename T>
	FORCEINLINE T Min(const T& a, const T& b)
	{
		return (a <= b) ? a : b;
	}

}
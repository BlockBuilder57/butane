// any headers needed to use opengl
#pragma once
#include <glad/glad.h>
#include <magic_enum.hpp>

namespace butane::core::gfx {

	// dummy enums for loading config w/ magic_enum
	enum class OGLTextureWrap : GLenum {
		CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
		CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
		MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
		REPEAT = GL_REPEAT,
		MIRROR_CLAMP_TO_EDGE = GL_MIRROR_CLAMP_TO_EDGE
	};
	enum class OGLTextureFilter : GLenum {
		NEAREST = GL_NEAREST,
		LINEAR = GL_LINEAR,
		NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
		LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
	};
	enum class OGLType : GLenum {
		Unknown = 0,
		Byte = GL_BYTE,
		UByte = GL_UNSIGNED_BYTE,
		Short = GL_SHORT,
		UShort = GL_UNSIGNED_SHORT,
		Int = GL_INT,
		UInt = GL_UNSIGNED_INT,
		Float = GL_FLOAT,
		Color = GL_COLOR,
		Vec2 = GL_FLOAT_VEC2,
		Vec3 = GL_FLOAT_VEC3,
		Vec4 = GL_FLOAT_VEC4,
		Vec2_Int = GL_INT_VEC2,
		Vec3_Int = GL_INT_VEC3,
		Vec4_Int = GL_INT_VEC4,
		Bool = GL_BOOL,
		Vec2_Bool = GL_BOOL_VEC2,
		Vec3_Bool = GL_BOOL_VEC3,
		Vec4_Bool = GL_BOOL_VEC4,
		Mat2 = GL_FLOAT_MAT2,
		Mat3 = GL_FLOAT_MAT3,
		Mat4 = GL_FLOAT_MAT4,
		Texture1D = GL_SAMPLER_1D,
		Texture2D = GL_SAMPLER_2D,
		Texture3D = GL_SAMPLER_3D,
		TextureCube = GL_SAMPLER_CUBE,
		Texture1DShadow = GL_SAMPLER_1D_SHADOW,
		Texture2DShadow = GL_SAMPLER_2D_SHADOW,
	};

}


// I WAS HERE FOR THREE HOURS
// (max - min) must be less than UINT16_MAX.
template <>
struct magic_enum::customize::enum_range<butane::core::gfx::OGLTextureWrap> {
	static constexpr int min = 0x2900;
	static constexpr int max = 0x8750;

};
template <>
struct magic_enum::customize::enum_range<butane::core::gfx::OGLTextureFilter> {
	static constexpr int min = 0x2600;
	static constexpr int max = 0x2704;
};
template <>
struct magic_enum::customize::enum_range<butane::core::gfx::OGLType> {
	static constexpr int min = 0x1400;
	static constexpr int max = 0x8B62;
};
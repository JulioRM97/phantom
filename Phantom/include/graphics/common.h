#pragma once


#define MAX_UNIFORM_BLOCKS 10

enum class LightType
{
  k_None = 0,
  k_Dir,
  k_Spot,
  k_Point
};

enum class ShaderType
{
  k_None,
  k_Fragment,
  k_Vertex,
  k_Geometry,
  k_Compute
};

enum class BlendType
{
  k_None = 0,
  k_Add,
  k_Subtract,
  k_Reverse_Subtract,
  k_Min,
  k_Max
};

enum class BufferType
{
  k_None = 0,
  k_Index,
  k_Vertex,
  k_Uniform,
  k_Storage
};

enum class BlendFunc
{
  k_Zero = 0,
  k_One = 1,
  k_Src_Color,
  k_One_Minus_Src_Color,
  k_Src_Alpha,
  k_One_Minus_Src_Alpha,
  k_Dst_Alpha,
  k_One_Minus_Dst_Alpha,
  k_Dst_Color,
  k_One_Minus_Dst_Color,
  k_Src_Alpha_Saturate
};

enum class ZPrePassMode
{
  k_Normal,
  k_OnlyZ,
  k_ZEqual
};

enum class DrawMode
{
  k_Point = 0,
  k_Line,
  k_LineStrip,
  k_Triangle,
  k_TriangleStrip,
  k_TriangleFan
};

enum class PolygonMode
{
  k_Fill = 0,
  k_Line,
  k_Point
};

enum class Format
{
  k_UNDEFINED = 0,
  k_R32_UINT = 98,
  k_R32_SINT = 99,
  k_R32_SFLOAT = 100,
  k_R32G32_UINT = 101,
  k_R32G32_SINT = 102,
  k_R32G32_SFLOAT = 103,
  k_R32G32B32_UINT = 104,
  k_R32G32B32_SINT = 105,
  k_R32G32B32_SFLOAT = 106,
  k_R32G32B32A32_UINT = 107,
  k_R32G32B32A32_SINT = 108,
  k_R32G32B32A32_SFLOAT = 109,
  k_R64_UINT = 110,
  k_R64_SINT = 111,
  k_R64_SFLOAT = 112,
  k_R64G64_UINT = 113,
  k_R64G64_SINT = 114,
  k_R64G64_SFLOAT = 115,
  k_R64G64B64_UINT = 116,
  k_R64G64B64_SINT = 117,
  k_R64G64B64_SFLOAT = 118,
  k_R64G64B64A64_UINT = 119,
  k_R64G64B64A64_SINT = 120,
  k_R64G64B64A64_SFLOAT = 121,
  k_B10G11R11_UFLOAT_PACK32 = 122
};

enum class InputVertexRate
{
  k_None     = -1,
  k_Vertex   = 0,
  k_Instance = 1
};

enum class AttributeType
{
  k_None     = -1,
  k_Position = 0,
  k_Normal   = 1,
  k_Color    = 2,
  k_Tangent  = 3
};

enum class TextureFilter
{
  k_NEAREST = 0,
  k_LINEAR = 1,
};

enum class TextureMode
{
  k_MODE_REPEAT = 0,
  k_MODE_MIRRORED_REPEAT = 1,
  k_MODE_CLAMP_TO_EDGE = 2,
  k_MODE_CLAMP_TO_BORDER = 3,
  k_MODE_MIRROR_CLAMP_TO_EDGE = 4,
};



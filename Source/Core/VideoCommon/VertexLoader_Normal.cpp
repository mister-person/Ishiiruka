// Copyright 2013 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#include "VideoCommon/VertexLoader_Normal.h"
#include "VideoCommon/VertexLoader_NormalFuncs.h"

VertexLoader_Normal::Set VertexLoader_Normal::m_Table[NUM_NRM_TYPE][NUM_NRM_INDICES][NUM_NRM_ELEMENTS][NUM_NRM_FORMAT];
bool VertexLoader_Normal::Initialized = false;
namespace
{

template <typename T, int N>
struct Normal_Direct
{
	static void LOADERDECL function()
	{
		_Normal_Direct<T, N>();
	}
	static const int size = sizeof(T) * N * 3;
};

template <typename I, typename T, int N>
struct Normal_Index
{
	static void LOADERDECL function()
	{
		_Normal_Index_Offset<I, T, N, 0>();
	}

	static const int size = sizeof(I);
};

template <typename I, typename T>
struct Normal_Index_Indices3
{
	static void LOADERDECL function()
	{
		_Normal_Index_Offset<I, T, 1, 0>();
		_Normal_Index_Offset<I, T, 1, 1>();
		_Normal_Index_Offset<I, T, 1, 2>();
	}

	static const int size = sizeof(I) * 3;
};

#if _M_SSE >= 0x301
template <int N>
struct Normal_Direct_FLOAT_SSSE3
{
	static void LOADERDECL function()
	{
		_Normal_Direct_FLOAT_SSSE3<N>();
	}
	static const int size = sizeof(float) * N * 3;
};

template <typename I, int N>
struct Normal_Index_FLOAT_SSSE3
{
	static void LOADERDECL function()
	{
		_Normal_Index_FLOAT_SSSE3<I, N>();
	}

	static const int size = sizeof(I);
};

template <typename I>
struct Normal_Index3_FLOAT_SSSE3
{
	static void LOADERDECL function()
	{
		_Normal_Index3_FLOAT_SSSE3<I>();
	}
	static const int size = sizeof(I) * 3;
};
#endif

#if _M_SSE >= 0x401

template <int N>
struct Normal_Direct_S16_SSSE4
{
	static void LOADERDECL function()
	{
		_Normal_Direct_S16_SSSE4<N>();
	}

	static const int size = sizeof(s16) * N * 3;
};

template <int N>
struct Normal_Direct_U16_SSSE4
{
	static void LOADERDECL function()
	{
		_Normal_Direct_U16_SSSE4<N>();
	}

	static const int size = sizeof(u16) * N * 3;
};

template <typename I, int N>
struct Normal_Index_S16_SSE4
{
	static void LOADERDECL function()
	{
		_Normal_Index_S16_SSE4<I, N>();
	}

	static const int size = sizeof(I);
};

template <typename I, int N>
struct Normal_Index_U16_SSE4
{
	static void LOADERDECL function()
	{
		_Normal_Index_U16_SSE4<I, N>();
	}

	static const int size = sizeof(I);
};

template <typename I>
struct Normal_Index3_S16_SSE4
{
	static void LOADERDECL function()
	{
		_Normal_Index3_S16_SSE4<I>();
	}

	static const int size = sizeof(I) * 3;
};

template <typename I>
struct Normal_Index3_U16_SSE4
{
	static void LOADERDECL function()
	{
		_Normal_Index3_U16_SSE4<I>();
	}

	static const int size = sizeof(I) * 3;
};
#endif


}

void VertexLoader_Normal::Init(void)
{
	if (Initialized)
	{
		return;
	}
	Initialized = true;
	m_Table[NRM_DIRECT] [NRM_INDICES1][NRM_NBT] [FORMAT_UBYTE] 	= Normal_Direct<u8, 1>();
	m_Table[NRM_DIRECT] [NRM_INDICES1][NRM_NBT] [FORMAT_BYTE]   = Normal_Direct<s8, 1>();
	m_Table[NRM_DIRECT] [NRM_INDICES1][NRM_NBT] [FORMAT_USHORT]	= Normal_Direct<u16, 1>();
	m_Table[NRM_DIRECT] [NRM_INDICES1][NRM_NBT] [FORMAT_SHORT] 	= Normal_Direct<s16, 1>();
	m_Table[NRM_DIRECT] [NRM_INDICES1][NRM_NBT] [FORMAT_FLOAT] 	= Normal_Direct<float, 1>();
	m_Table[NRM_DIRECT] [NRM_INDICES1][NRM_NBT3][FORMAT_UBYTE] 	= Normal_Direct<u8, 3>();
	m_Table[NRM_DIRECT] [NRM_INDICES1][NRM_NBT3][FORMAT_BYTE]  	= Normal_Direct<s8, 3>();
	m_Table[NRM_DIRECT] [NRM_INDICES1][NRM_NBT3][FORMAT_USHORT]	= Normal_Direct<u16, 3>();
	m_Table[NRM_DIRECT] [NRM_INDICES1][NRM_NBT3][FORMAT_SHORT] 	= Normal_Direct<s16, 3>();
	m_Table[NRM_DIRECT][NRM_INDICES1][NRM_NBT3][FORMAT_FLOAT] = Normal_Direct<float, 3>();

	// Same as above
	m_Table[NRM_DIRECT] [NRM_INDICES3][NRM_NBT] [FORMAT_UBYTE] 	= Normal_Direct<u8, 1>();
	m_Table[NRM_DIRECT] [NRM_INDICES3][NRM_NBT] [FORMAT_BYTE]  	= Normal_Direct<s8, 1>();
	m_Table[NRM_DIRECT] [NRM_INDICES3][NRM_NBT] [FORMAT_USHORT]	= Normal_Direct<u16, 1>();
	m_Table[NRM_DIRECT] [NRM_INDICES3][NRM_NBT] [FORMAT_SHORT] 	= Normal_Direct<s16, 1>();
	m_Table[NRM_DIRECT] [NRM_INDICES3][NRM_NBT] [FORMAT_FLOAT] 	= Normal_Direct<float, 1>();
	m_Table[NRM_DIRECT] [NRM_INDICES3][NRM_NBT3][FORMAT_UBYTE] 	= Normal_Direct<u8, 3>();
	m_Table[NRM_DIRECT] [NRM_INDICES3][NRM_NBT3][FORMAT_BYTE]  	= Normal_Direct<s8, 3>();
	m_Table[NRM_DIRECT] [NRM_INDICES3][NRM_NBT3][FORMAT_USHORT]	= Normal_Direct<u16, 3>();
	m_Table[NRM_DIRECT] [NRM_INDICES3][NRM_NBT3][FORMAT_SHORT] 	= Normal_Direct<s16, 3>();
	m_Table[NRM_DIRECT] [NRM_INDICES3][NRM_NBT3][FORMAT_FLOAT] 	= Normal_Direct<float, 3>();

	m_Table[NRM_INDEX8] [NRM_INDICES1][NRM_NBT] [FORMAT_UBYTE] 	= Normal_Index<u8, u8, 1>();
	m_Table[NRM_INDEX8] [NRM_INDICES1][NRM_NBT] [FORMAT_BYTE]  	= Normal_Index<u8, s8, 1>();
	m_Table[NRM_INDEX8] [NRM_INDICES1][NRM_NBT] [FORMAT_USHORT]	= Normal_Index<u8, u16, 1>();
	m_Table[NRM_INDEX8] [NRM_INDICES1][NRM_NBT] [FORMAT_SHORT] 	= Normal_Index<u8, s16, 1>();
	m_Table[NRM_INDEX8] [NRM_INDICES1][NRM_NBT] [FORMAT_FLOAT] 	= Normal_Index<u8, float, 1>();
	m_Table[NRM_INDEX8] [NRM_INDICES1][NRM_NBT3][FORMAT_UBYTE] 	= Normal_Index<u8, u8, 3>();
	m_Table[NRM_INDEX8] [NRM_INDICES1][NRM_NBT3][FORMAT_BYTE]  	= Normal_Index<u8, s8, 3>();
	m_Table[NRM_INDEX8] [NRM_INDICES1][NRM_NBT3][FORMAT_USHORT]	= Normal_Index<u8, u16, 3>();
	m_Table[NRM_INDEX8] [NRM_INDICES1][NRM_NBT3][FORMAT_SHORT] 	= Normal_Index<u8, s16, 3>();
	m_Table[NRM_INDEX8] [NRM_INDICES1][NRM_NBT3][FORMAT_FLOAT] 	= Normal_Index<u8, float, 3>();

	// Same as above for NRM_NBT
	m_Table[NRM_INDEX8] [NRM_INDICES3][NRM_NBT] [FORMAT_UBYTE] 	= Normal_Index<u8, u8, 1>();
	m_Table[NRM_INDEX8] [NRM_INDICES3][NRM_NBT] [FORMAT_BYTE]  	= Normal_Index<u8, s8, 1>();
	m_Table[NRM_INDEX8] [NRM_INDICES3][NRM_NBT] [FORMAT_USHORT]	= Normal_Index<u8, u16, 1>();
	m_Table[NRM_INDEX8] [NRM_INDICES3][NRM_NBT] [FORMAT_SHORT] 	= Normal_Index<u8, s16, 1>();
	m_Table[NRM_INDEX8] [NRM_INDICES3][NRM_NBT] [FORMAT_FLOAT] 	= Normal_Index<u8, float, 1>();
	m_Table[NRM_INDEX8] [NRM_INDICES3][NRM_NBT3][FORMAT_UBYTE] 	= Normal_Index_Indices3<u8, u8>();
	m_Table[NRM_INDEX8] [NRM_INDICES3][NRM_NBT3][FORMAT_BYTE]  	= Normal_Index_Indices3<u8, s8>();
	m_Table[NRM_INDEX8] [NRM_INDICES3][NRM_NBT3][FORMAT_USHORT]	= Normal_Index_Indices3<u8, u16>();
	m_Table[NRM_INDEX8] [NRM_INDICES3][NRM_NBT3][FORMAT_SHORT] 	= Normal_Index_Indices3<u8, s16>();
	m_Table[NRM_INDEX8] [NRM_INDICES3][NRM_NBT3][FORMAT_FLOAT] 	= Normal_Index_Indices3<u8, float>();

	m_Table[NRM_INDEX16][NRM_INDICES1][NRM_NBT] [FORMAT_UBYTE] 	= Normal_Index<u16, u8, 1>();
	m_Table[NRM_INDEX16][NRM_INDICES1][NRM_NBT] [FORMAT_BYTE]  	= Normal_Index<u16, s8, 1>();
	m_Table[NRM_INDEX16][NRM_INDICES1][NRM_NBT] [FORMAT_USHORT]	= Normal_Index<u16, u16, 1>();
	m_Table[NRM_INDEX16][NRM_INDICES1][NRM_NBT] [FORMAT_SHORT] 	= Normal_Index<u16, s16, 1>();
	m_Table[NRM_INDEX16][NRM_INDICES1][NRM_NBT] [FORMAT_FLOAT] 	= Normal_Index<u16, float, 1>();
	m_Table[NRM_INDEX16][NRM_INDICES1][NRM_NBT3][FORMAT_UBYTE] 	= Normal_Index<u16, u8, 3>();
	m_Table[NRM_INDEX16][NRM_INDICES1][NRM_NBT3][FORMAT_BYTE]  	= Normal_Index<u16, s8, 3>();
	m_Table[NRM_INDEX16][NRM_INDICES1][NRM_NBT3][FORMAT_USHORT]	= Normal_Index<u16, u16, 3>();
	m_Table[NRM_INDEX16][NRM_INDICES1][NRM_NBT3][FORMAT_SHORT] 	= Normal_Index<u16, s16, 3>();
	m_Table[NRM_INDEX16][NRM_INDICES1][NRM_NBT3][FORMAT_FLOAT] 	= Normal_Index<u16, float, 3>();

	// Same as above for NRM_NBT
	m_Table[NRM_INDEX16][NRM_INDICES3][NRM_NBT] [FORMAT_UBYTE] 	= Normal_Index<u16, u8, 1>();
	m_Table[NRM_INDEX16][NRM_INDICES3][NRM_NBT] [FORMAT_BYTE]  	= Normal_Index<u16, s8, 1>();
	m_Table[NRM_INDEX16][NRM_INDICES3][NRM_NBT] [FORMAT_USHORT]	= Normal_Index<u16, u16, 1>();
	m_Table[NRM_INDEX16][NRM_INDICES3][NRM_NBT] [FORMAT_SHORT] 	= Normal_Index<u16, s16, 1>();
	m_Table[NRM_INDEX16][NRM_INDICES3][NRM_NBT] [FORMAT_FLOAT] 	= Normal_Index<u16, float, 1>();
	m_Table[NRM_INDEX16][NRM_INDICES3][NRM_NBT3][FORMAT_UBYTE] 	= Normal_Index_Indices3<u16, u8>();
	m_Table[NRM_INDEX16][NRM_INDICES3][NRM_NBT3][FORMAT_BYTE]  	= Normal_Index_Indices3<u16, s8>();
	m_Table[NRM_INDEX16][NRM_INDICES3][NRM_NBT3][FORMAT_USHORT]	= Normal_Index_Indices3<u16, u16>();
	m_Table[NRM_INDEX16][NRM_INDICES3][NRM_NBT3][FORMAT_SHORT] 	= Normal_Index_Indices3<u16, s16>();
	m_Table[NRM_INDEX16][NRM_INDICES3][NRM_NBT3][FORMAT_FLOAT] 	= Normal_Index_Indices3<u16, float>();


#if _M_SSE >= 0x301
	if (cpu_info.bSSSE3)
	{
		m_Table[NRM_DIRECT][NRM_INDICES1][NRM_NBT][FORMAT_FLOAT] = Normal_Direct_FLOAT_SSSE3<1>();		
		m_Table[NRM_DIRECT][NRM_INDICES1][NRM_NBT3][FORMAT_FLOAT] = Normal_Direct_FLOAT_SSSE3<3>();
		
		// Same as above
		m_Table[NRM_DIRECT][NRM_INDICES3][NRM_NBT][FORMAT_FLOAT] = Normal_Direct_FLOAT_SSSE3<1>();
		m_Table[NRM_DIRECT][NRM_INDICES3][NRM_NBT3][FORMAT_FLOAT] = Normal_Direct_FLOAT_SSSE3<3>();

		m_Table[NRM_INDEX8][NRM_INDICES1][NRM_NBT][FORMAT_FLOAT] = Normal_Index_FLOAT_SSSE3<u8, 1>();
		m_Table[NRM_INDEX8][NRM_INDICES1][NRM_NBT3][FORMAT_FLOAT] = Normal_Index_FLOAT_SSSE3<u8, 3>();

		// Same as above for NRM_NBT
		m_Table[NRM_INDEX8][NRM_INDICES3][NRM_NBT][FORMAT_FLOAT] = Normal_Index_FLOAT_SSSE3<u8, 1>();
		m_Table[NRM_INDEX8][NRM_INDICES3][NRM_NBT3][FORMAT_FLOAT] = Normal_Index3_FLOAT_SSSE3<u8>();

		m_Table[NRM_INDEX16][NRM_INDICES1][NRM_NBT][FORMAT_FLOAT] = Normal_Index_FLOAT_SSSE3<u16, 1>();
		m_Table[NRM_INDEX16][NRM_INDICES1][NRM_NBT3][FORMAT_FLOAT] = Normal_Index_FLOAT_SSSE3<u16, 3>();

		// Same as above for NRM_NBT
		m_Table[NRM_INDEX16][NRM_INDICES3][NRM_NBT][FORMAT_FLOAT] = Normal_Index_FLOAT_SSSE3<u16, 1>();
		m_Table[NRM_INDEX16][NRM_INDICES3][NRM_NBT3][FORMAT_FLOAT] = Normal_Index3_FLOAT_SSSE3<u16>();
	}
#endif
#if _M_SSE >= 0x401
	if (cpu_info.bSSE4_1)
	{
		m_Table[NRM_DIRECT][NRM_INDICES1][NRM_NBT][FORMAT_USHORT] = Normal_Direct_U16_SSSE4<1>();
		m_Table[NRM_DIRECT][NRM_INDICES1][NRM_NBT][FORMAT_SHORT] = Normal_Direct_S16_SSSE4<1>();
		m_Table[NRM_DIRECT][NRM_INDICES1][NRM_NBT3][FORMAT_USHORT] = Normal_Direct_U16_SSSE4<3>();
		m_Table[NRM_DIRECT][NRM_INDICES1][NRM_NBT3][FORMAT_SHORT] = Normal_Direct_S16_SSSE4<3>();

		// Same as above
		m_Table[NRM_DIRECT][NRM_INDICES3][NRM_NBT][FORMAT_USHORT] = Normal_Direct_U16_SSSE4<1>();
		m_Table[NRM_DIRECT][NRM_INDICES3][NRM_NBT][FORMAT_SHORT] = Normal_Direct_S16_SSSE4<1>();
		m_Table[NRM_DIRECT][NRM_INDICES3][NRM_NBT3][FORMAT_USHORT] = Normal_Direct_U16_SSSE4<3>();
		m_Table[NRM_DIRECT][NRM_INDICES3][NRM_NBT3][FORMAT_SHORT] = Normal_Direct_S16_SSSE4<3>();

		m_Table[NRM_INDEX8][NRM_INDICES1][NRM_NBT][FORMAT_USHORT] = Normal_Index_U16_SSE4<u8, 1>();
		m_Table[NRM_INDEX8][NRM_INDICES1][NRM_NBT][FORMAT_SHORT] = Normal_Index_S16_SSE4<u8, 1>();
		m_Table[NRM_INDEX8][NRM_INDICES1][NRM_NBT3][FORMAT_USHORT] = Normal_Index_U16_SSE4<u8, 3>();
		m_Table[NRM_INDEX8][NRM_INDICES1][NRM_NBT3][FORMAT_SHORT] = Normal_Index_S16_SSE4<u8, 3>();

		// Same as above for NRM_NBT
		m_Table[NRM_INDEX8][NRM_INDICES3][NRM_NBT][FORMAT_USHORT] = Normal_Index_U16_SSE4<u8, 1>();
		m_Table[NRM_INDEX8][NRM_INDICES3][NRM_NBT][FORMAT_SHORT] = Normal_Index_S16_SSE4<u8, 1>();
		m_Table[NRM_INDEX8][NRM_INDICES3][NRM_NBT3][FORMAT_USHORT] = Normal_Index3_U16_SSE4<u8>();
		m_Table[NRM_INDEX8][NRM_INDICES3][NRM_NBT3][FORMAT_SHORT] = Normal_Index3_S16_SSE4<u8>();

		m_Table[NRM_INDEX16][NRM_INDICES1][NRM_NBT][FORMAT_USHORT] = Normal_Index_U16_SSE4<u16, 1>();
		m_Table[NRM_INDEX16][NRM_INDICES1][NRM_NBT][FORMAT_SHORT] = Normal_Index_S16_SSE4<u16, 1>();
		m_Table[NRM_INDEX16][NRM_INDICES1][NRM_NBT3][FORMAT_USHORT] = Normal_Index_U16_SSE4<u16, 3>();
		m_Table[NRM_INDEX16][NRM_INDICES1][NRM_NBT3][FORMAT_SHORT] = Normal_Index_S16_SSE4<u16, 3>();

		// Same as above for NRM_NBT
		m_Table[NRM_INDEX16][NRM_INDICES3][NRM_NBT][FORMAT_USHORT] = Normal_Index_U16_SSE4<u16, 1>();
		m_Table[NRM_INDEX16][NRM_INDICES3][NRM_NBT][FORMAT_SHORT] = Normal_Index_S16_SSE4<u16, 1>();
		m_Table[NRM_INDEX16][NRM_INDICES3][NRM_NBT3][FORMAT_USHORT] = Normal_Index3_U16_SSE4<u16>();
		m_Table[NRM_INDEX16][NRM_INDICES3][NRM_NBT3][FORMAT_SHORT] = Normal_Index3_S16_SSE4<u16>();
	}
#endif

}

unsigned int VertexLoader_Normal::GetSize(unsigned int _type,
	unsigned int _format, unsigned int _elements, unsigned int _index3)
{
	return m_Table[_type][_index3][_elements][_format].gc_size;
}

TPipelineFunction VertexLoader_Normal::GetFunction(unsigned int _type,
	unsigned int _format, unsigned int _elements, unsigned int _index3)
{
	TPipelineFunction pFunc = m_Table[_type][_index3][_elements][_format].function;
	return pFunc;
}

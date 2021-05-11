#pragma once

#include "../config.hpp"

namespace Mesh {

	class ReusableBuffer {

		private:

			byte* buffer;
			size_t pos;
			size_t length;

		public:
		
			ReusableBuffer( size_t length );
			ReusableBuffer( ReusableBuffer&& );
			~ReusableBuffer();
		
			inline void push( byte );
			void clear();
			void assert_size( int count );

			byte* copy();
			byte* data();
			size_t size();

	};

	class StaticBuffer {

		public:

			const byte* data;
			const size_t size;
	
			StaticBuffer( ReusableBuffer* buffer );
			~StaticBuffer();


	};

	void buildIndice( ReusableBuffer& vec, byte x, byte y, byte z, const byte* color );
	void buildQuad( ReusableBuffer& vec, byte x1, byte y1, byte z1, byte x2, byte y2, byte z2, byte x3, byte y3, byte z3, byte x4, byte y4, byte z4, const byte* color );
	void buildVoxel( ReusableBuffer& vec, const byte* rgb, byte x, byte y, byte z, byte flags );
}

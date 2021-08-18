#pragma once

#include "../config.hpp"

// this must be a simple, C-like structure
struct OctreeVoxel {
	byte r; // red color
	byte g; // green color
	byte b; // blue color
	byte a; // translucency 
//	byte i; // light intensity
};

template< typename T >
void octree_callback_set(const byte mask, const int offset, T* ptr) {
	if( mask != 1 ) ptr[offset] = { 255, 255, 255, 255 }; // FIXME: generify this ugly trash
}

template< typename T >
void octree_callback_get(const byte mask, const int offset, T* ptr) {

}

template< typename T >
class Octree {

	private:

		bool modified = false;

		int mask; // iterator mask, based on tree depth

		T* buffer;

	protected:

		/// branch iterator callback
		using Iterator = void (*) (const byte, const int, T*);
		
		/// get a leaf pointer
		template<Iterator Func>
		T* accessor( const int x, const int y, const int z ) {

			// the offset into the tree
			int offset = 0;

			// copy iterator mask
			byte mask = this->mask;	

			// FIXME fix it in some better, less hacky, way
			Func(0, 0, this->buffer);

			// pointer to the tree data buffer
			T* pointer = this->buffer + 1;

			// iterate until the mask is shifted to target (leaf) layer
			while( mask ) {

				// shift the offset so that it aligns to the next layer
				offset <<= 3;

				// calculate the offset by decomposing the xyz to its binary form
				offset += (
					!!(x & mask) * 1 + 
					!!(y & mask) * 2 +
					!!(z & mask) * 4
				);

				// call iterator handle (this is inlined by the compiler)
				Func(mask, offset, pointer);

				offset ++;

				// shift the mask
				mask >>= 1;

			}

			// the last level don't have headers, we need to un-skip it
			return pointer + offset; 

		}
	
	public:
	
		Octree( int depth ) {

			// length of the buffer used to store the tree
			int length = sizeOf(depth);

			// iterator mask, based on tree depth (skips tree root)
			this->mask = 1 << (depth - 1);
	
			// allocate the tree buffer
			this->buffer = new T[length];

			// clear allocated tree buffer
			memset(this->buffer, 0, length * sizeof(T));

		}
	
		~Octree() {
			delete[] buffer;
		}

		/// get raw entry from the tree
		T get( const int x, const int y, const int z ) {
			return *accessor<octree_callback_get<T>>(x, y, z);
		}

		/// set raw entry in a tree
		void set( const int x, const int y, const int z, T leaf ) {
			this->modified = true;
			*accessor<octree_callback_set<T>>(x, y, z) = leaf;
		}

	public:

		/// get tree buffer pointer
		byte* data() {
			return (byte*) this->buffer;
		}

		/// TODO make it less ugly
		bool dirty() {
			bool flag = this->modified;
			this->modified = false;
			return flag;
		}

		// length of the buffer used to store the tree of a given depth
		static int sizeOf( int depth ) {
			// derived from: `Sn = (1 - q^n) / (1 - q)`
			return ((1 - pow(8, depth + 1)) / -7);
		}

};



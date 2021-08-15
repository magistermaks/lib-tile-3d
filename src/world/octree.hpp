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
void octree_callback_set(byte mask, T* ptr) {
	if( mask != 1 ) *ptr = { 255, 255, 255, 255 };
}

template< typename T >
void octree_callback_get(byte mask, T* ptr) {

}

template< typename T >
class Octree {

	private:

		bool modified = false;

		int mask; // iterator mask, based on tree depth
		int size; // size of the second branch

		T* buffer;

	protected:

		/// branch iterator callback
		using Iterator = void (*) (byte, T*);
		
		/// get a leaf pointer
		template<Iterator Func>
		T* accessor( const int x, const int y, const int z ) {

			// copy iterator mask
			byte mask = this->mask;

			// copy branch size
			int size = this->size;

			// pointer to the tree data buffer
			T* offset = this->buffer;

			// iterate until the mask is shifted to target (leaf) layer
			while( mask ) {

				// calculate the offset by decomposing xyz to its binary form
				offset += (
					!!(x & mask) * 1 + 
					!!(y & mask) * 2 +
					!!(z & mask) * 4
				) * size;

				// call iterator handle (this is inlined by the compiler)
				Func(mask, offset);

				// skip branch header
				offset ++;

				// get size of the lower level
				size = (size - 1) >> 3;

				// shift the mask
				mask >>= 1;

			}

			// the last level doesn't have headers, we need to un-skip it
			return offset - 1; 

		}
	
	public:
	
		Octree( int depth ) {

			// length of the buffer used to store the tree
			int length = sizeOf(depth);

			// iterator mask, based on tree depth (skips tree root)
			this->mask = 1 << (depth - 1);

			// the value we use in set and get, the size of the second branch
			this->size = (length - 1) >> 3;
	
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

		/// get size (in bytes) of the buffer 
		int length() {
			return ((length << 3) + 1) * sizeof(T);
		}

		/// TODO
		bool dirty() {
			bool flag = this->modified;
			this->modified = false;
			return flag;
		}

		// length of the buffer used to store the tree of a given depth
		static int sizeOf( int depth ) {
			// derived from: `Sn = (1 - q^n) / (1 - q)`
			return ((1 - pow(8, depth + 1)) / -7) * sizeof(T);
		}

};



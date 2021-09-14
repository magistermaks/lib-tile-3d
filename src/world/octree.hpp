#pragma once

#include <core.hpp>

struct OctreeVoxel {
	byte r; // red color
	byte g; // green color
	byte b; // blue color
	byte a; // translucency 
//	byte i; // light intensity

	bool empty() {
		return this->a == 0;
	}

	static void set( const int octant, const int offset, OctreeVoxel* ptr ) {
		ptr[offset].a |= 1 << octant;
	}

	static void get( const int octant, const int offset, OctreeVoxel* ptr ) {
		// noop
	}

	static void erase( const int octant, const int offset, OctreeVoxel* ptr ) {
		// noop
	}

};

template< typename T >
class Octree {

	private:

		int mask; // iterator mask, based on tree depth

		T* buffer;

	protected:

		/// branch iterator callback
		using Iterator = void (*) (const int, const int, T*);
		
		/// get a leaf pointer
		template<Iterator Func>
		T* accessor( const int x, const int y, const int z ) {

			// the offset into the tree
			int offset = 0;

			// copy iterator mask
			byte mask = this->mask;

			// iterate until the mask is shifted to target (leaf) layer
			while( mask ) {

				// calculate the octant by decomposing the xyz to its binary form
				const int octant = (
					!!(x & mask) * 1 + 
					!!(y & mask) * 2 +
					!!(z & mask) * 4
				);

				// call iterator handle (this SHOULD be inlined by the compiler)
				Func(octant, offset, this->buffer);

				// shift the offset so that it aligns to the next layer
				offset <<= 3;

				// add octant id to the shifted offset, keep the octant id in range 1-8, not 0-7
				offset += octant + 1;

				// shift the mask
				mask >>= 1;

			}

			// return pointer to leaf
			return this->buffer + offset;

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
			delete[] this->buffer;
		}

		/// get raw entry from the tree
		T get( const int x, const int y, const int z ) {
			return *accessor<T::get>(x, y, z);
		}

		/// set raw entry in a tree
		void set( const int x, const int y, const int z, T leaf ) {
			if( leaf.empty() ) {
				*accessor<T::erase>(x, y, z) = leaf;
			} else {
				*accessor<T::set>(x, y, z) = leaf;
			}
		}

	public:

		/// get tree buffer pointer
		byte* data() {
			return (byte*) this->buffer;
		}

		// length of the buffer used to store the tree of a given depth
		static int sizeOf( int depth ) {
			// derived from: `Sn = (1 - q^n) / (1 - q)`
			return ((1 - pow(8, depth + 1)) / -7);
		}

};



#pragma once

#include "trait.hpp"

template< typename T >
class ReusableBuffer {

	private:

		T* buffer;
		size_t pos;
		size_t length;

	public:

		ReusableBuffer( size_t );
		ReusableBuffer( ReusableBuffer&& );
		~ReusableBuffer();
		
		inline void push( T );
		inline void push( T*, size_t );
		void clear();
		void assert_size( int );

		T* copy();
		T* data();
		size_t size();
		bool empty();

		template< class... Args, class = trait::are_types_equal< T, Args... > >
		void push( T first, Args... args ) {
			this->assert_size( 1 + sizeof...(args) );
			this->push(first);
			this->push(args...);
		}

};

template< typename T >
ReusableBuffer<T>::ReusableBuffer( size_t length ) {
	this->pos = 0;
	this->length = length;
	this->buffer = (T*) malloc(length * sizeof(T));
}

template< typename T >
ReusableBuffer<T>::ReusableBuffer( ReusableBuffer<T>&& buffer ) {
	this->pos = buffer.pos;
	this->length = buffer.length;
	this->buffer = buffer.buffer;

	buffer.buffer = nullptr;
}

template< typename T >
ReusableBuffer<T>::~ReusableBuffer() {
	if( this->buffer != nullptr ) {
		free(this->buffer);
	}
}

template< typename T >
inline void ReusableBuffer<T>::push( T data ) {
	this->buffer[ this->pos ++ ] = data;
}

template< typename T >
inline void ReusableBuffer<T>::push( T* buffor, size_t length ) {
	this->assert_size(length);
	memcpy(this->buffer, buffor, length * sizeof(T));

	this->pos += length;
}

template< typename T >
void ReusableBuffer<T>::clear() {
	this->pos = 0;
}

template< typename T >
void ReusableBuffer<T>::assert_size( int count ) {
	if( this->pos + count > this->length ) {
		size_t new_size = this->length * 2;

		//if( new_size > MAX_BUFFER_LENGTH * sizeof(T) ) {
		//	throw std::runtime_error( "Maximum buffer size excedded!" );
		//}

		this->buffer = (T*) realloc(this->buffer, new_size * sizeof(T));
		this->length = new_size;
	}
}

template< typename T >
size_t ReusableBuffer<T>::size() {
	return this->pos;
}

template< typename T >
bool ReusableBuffer<T>::empty() {
	return this->pos == 0;
}


template< typename T >
T* ReusableBuffer<T>::copy() {
	size_t size = this->pos + 1;
	T* buf = new T[size];
	memcpy(buf, this->buffer, size * sizeof(T));

	return buf;
}

template< typename T >
T* ReusableBuffer<T>::data() {
	return this->buffer;
}


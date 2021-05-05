
// currently unused //

#include "pallet.hpp"

Pallet::Pallet( std::vector<byte> pallet ) {
	glGenTextures(1, &(this->id));  

	this->bind();
	this->size = pallet.size() / 4;

	glTextureParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, this->size, 0, GL_RGBA, GL_UNSIGNED_BYTE, pallet.data());
}

void Pallet::bind() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, this->id);
}

size_t Pallet::size() {
	return this->size;
}

byte Pallet::bytes() {
	if( this->size <= 0xFF ) return 1;
	if( this->size <= 0xFFFF ) return 2;
	if( this->size <= 0xFFFFFF ) return 3;
	return 4;
}


#require shader/opencl/math.cl

void kernel render( const int spp, const int width, global byte* imgb, global float* scnf, global byte* voxsoct ) {

	// Epic ChadRayFrameworkX
	//   pixel x : get_global_id(0)
	//   pixel y : get_global_id(1)
	//   width   : width
	//   texture : imgb
	//   scene   : scnf
	//   samples : spp
	//   voxels  : voxsoct

	int off = get_global_id(0) * 3 + get_global_id(1) * width * 3;

	imgb[off + 0] = get_global_id(0);
	imgb[off + 1] = 100;
	imgb[off + 2] = 100;

}

	

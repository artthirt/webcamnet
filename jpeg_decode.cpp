#include "jpeg_decode.h"

#include <jpeglib.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

jpeg_decode::jpeg_decode()
{

}

//////////////////////////

struct bufstream{
	bufstream(const bytearray* d): data(d), offset(0){

	}
    uint8_t *ptr(){
        return (uint8_t*)&(*data)[offset];
	}

	void skip(size_t nb){
		offset += nb;
	}

	const bytearray* data;
	size_t offset;
};

void init_src(j_decompress_ptr cinfo)
{
	bufstream *bs = (bufstream*)cinfo->client_data;
}

void skip_src(j_decompress_ptr cinfo, long num_bytes)
{
	bufstream *bs = (bufstream*)cinfo->client_data;
	bs->skip(num_bytes);
	cinfo->src->next_input_byte = bs->ptr();
}

void term_src(j_decompress_ptr cinfo)
{
	bufstream *bs = (bufstream*)cinfo->client_data;
}

boolean fill_buffer(j_decompress_ptr cinfo)
{
	return true;
}

boolean resync_to_restart(j_decompress_ptr cinfo, int desired)
{
	return true;
}

void exitFn(j_common_ptr cinfo)
{
    jpeg_decode *j = static_cast<jpeg_decode*>(cinfo->client_data);
    if(j){
        j->setError();
    }
}

///////////////////////////

void jpeg_decode::decode(const bytearray &data, cv::Mat &mat)
{
    if(data.empty()){
        return;
    }

    if(data[0] == 0 && data[1] == 0){
        return;
    }

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	int row_stride;

	bufstream bufstr(&data);
	jpeg_source_mgr src;
	src.bytes_in_buffer = data.size();
    src.next_input_byte = (uint8_t*)&data[0];
	src.fill_input_buffer = fill_buffer;
	src.init_source = init_src;
	src.skip_input_data = skip_src;
	src.term_source = term_src;
	src.resync_to_restart = resync_to_restart;

	jpeg_create_decompress(&cinfo);

    cinfo.client_data = this;
    cinfo.err = jpeg_std_error(&jerr);

    jerr.error_exit = exitFn;

	cinfo.client_data = &bufstr;
	cinfo.src = &src;
	//jpeg_mem_src(&cinfo, (uchar*)&data[0], data.size());

	jpeg_read_header(&cinfo, TRUE);

	jpeg_start_decompress(&cinfo);

	row_stride = cinfo.output_width * cinfo.output_components;

	int type = CV_8UC1;
	int offset = 0;
	switch (cinfo.output_components) {
		case 3:
			type = CV_8UC3;
			break;
		case 4:
			type = CV_8UC4;
		default:
			break;
	}

	mat = cv::Mat(cinfo.output_height, cinfo.output_width, type);
    while(cinfo.output_scanline < cinfo.output_height && !error()){
		unsigned char *buffer_array[1];
		buffer_array[0] = &mat.data[offset];
		jpeg_read_scanlines(&cinfo, buffer_array, 1);
		offset += row_stride;
	}

	jpeg_finish_decompress(&cinfo);

	jpeg_destroy_decompress(&cinfo);

//	mat = imdecode(data, IMREAD_COLOR);
//	cvtColor(mat, mat, CV_RGB2BGR);
}


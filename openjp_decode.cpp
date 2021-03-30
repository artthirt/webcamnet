#include "openjp_decode.h"

#include <openjpeg-2.3/openjpeg.h>

using namespace std;
using namespace cv;

void image2mat(const opj_image* image, Mat& mat)
{
	if(!image)
		return;

	int val = CV_8UC3;
	if(image->numcomps == 4)
		val = CV_8UC4;

	mat = Mat(image->y1 - image->y0, image->x1 - image->x0, val);

	for(int i = 0; i < mat.rows; i++){
		int index = i * mat.cols;
		if(image->numcomps == 3){
			for(int j = 0; j < mat.cols; j++){
				Vec3b vb;
				vb[0] = image->comps[0].data[index];
				vb[1] = image->comps[1].data[index];
				vb[2] = image->comps[2].data[index];
				index++;
				mat.at<Vec3b>(i, j) = vb;
			}
		}else if(image->numcomps == 4){
			for(int j = 0; j < mat.cols; j++){
				Vec4b vb;
				vb[0] = image->comps[0].data[index];
				vb[1] = image->comps[1].data[index];
				vb[2] = image->comps[2].data[index];
				vb[3] = image->comps[3].data[index];
				index++;
				mat.at<Vec4b>(i, j) = vb;
			}
		}
	}
}


void error_callback(const char *msg, void *v) {
	(void)msg;
	(void)v;
	puts(msg);
//	assert(0);
}
void warning_callback(const char *msg, void *v) {
	(void)msg;
	(void)v;
	puts(msg);
}
void info_callback(const char *msg, void *v) {
	(void)msg;
	(void)v;
//	puts(msg);
}

////////////////////////////////////////////////

size_t openjp_decode::read_stream(void * p_buffer, size_t p_nb_bytes, void * p_user_data)
{
	openjp_decode* slf = (openjp_decode*)p_user_data;
	const bytearray& fbuf = *slf->m_array;
	if(slf->m_offset + p_nb_bytes > fbuf.size())
		p_nb_bytes = (int64)max((int64)0, (int64)(fbuf.size() - slf->m_offset));
	memcpy(p_buffer, &fbuf.data()[slf->m_offset], p_nb_bytes);
	slf->m_offset += p_nb_bytes;

	return p_nb_bytes;
}

////////////////////////////////////////////////

openjp_decode::openjp_decode()
{

}

void openjp_decode::decode(const bytearray &data, cv::Mat &mat)
{
	if(!data.size())
		return;

	bool result = false;

	opj_dparameters_t parameters;			/* decompression parameters */
	opj_image_t* image = NULL;
	opj_stream_t *l_stream = NULL;				/* Stream */
	opj_codec_t* l_codec = NULL;				/* Handle to a decompressor */

	try
	{
		/* set decoding parameters to default values */

		opj_set_default_decoder_parameters(&parameters);

		l_stream = opj_stream_create(data.size(), true);
		l_codec = opj_create_decompress(OPJ_CODEC_J2K);

		opj_stream_set_read_function(l_stream, &openjp_decode::read_stream);
		opj_stream_set_user_data(l_stream, (void*)this, NULL);
		opj_stream_set_user_data_length(l_stream, data.size());

		opj_set_info_handler(l_codec, info_callback,00);
		opj_set_warning_handler(l_codec, warning_callback,00);
		opj_set_error_handler(l_codec, error_callback,00);

		if (!opj_setup_decoder(l_codec, &parameters) ){
			goto _Exit;
		}

		/* Read the main header of the codestream and if necessary the JP2 boxes*/
		if(! opj_read_header(l_stream, l_codec, &image)){
			goto _Exit;
		}

		if (!opj_set_decode_area(l_codec, image, (OPJ_INT32)parameters.DA_x0,
			(OPJ_INT32)parameters.DA_y0, (OPJ_INT32)parameters.DA_x1, (OPJ_INT32)parameters.DA_y1)){
				fprintf(stderr,	"ERROR -> opj_decompress: failed to set the decoded area\n");
				opj_stream_destroy(l_stream);
				opj_destroy_codec(l_codec);
				opj_image_destroy(image);
				return;
		}

		if ( !(opj_decode(l_codec, l_stream, image) && opj_end_decompress(l_codec, l_stream))) {
			goto _Exit;
		}

		image2mat(image, mat);
		result = true;
	}
	catch (char*)
	{
		cout << "undefined error" << endl;
		// 		QCoreApplication::postEvent(m_owner, new FrameNotExpandedEvent(e.get_message()));
	}
_Exit:
	if(l_stream)
		opj_stream_destroy(l_stream);
	if(l_codec)
		opj_destroy_codec(l_codec);
	if(image)
		opj_image_destroy(image);
	// 	VideoChannel::releaseBank(this);
}


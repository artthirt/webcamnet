#include "openjp_decode.h"

#include <openjpeg.h>

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

void error_handler(const char *msg, void *client_data)
{
	cout << msg << endl;
}

void info_handler(const char *msg, void *client_data)
{
	//cout << msg << endl;
}

void warning_handler(const char *msg, void *client_data)
{
	cout << msg << endl;
}


openjp_decode::openjp_decode()
{

}

void openjp_decode::decode(const bytearray &data, cv::Mat &mat)
{
	if(!data.size())
		return;

	opj_dparameters_t parameters;
	opj_image_t *image = 0;
	opj_dinfo_t *dinfo = 0;
	opj_cio_t *cio = 0;

	opj_event_mgr_t event_mgr;
	CLEAR(event_mgr);
	event_mgr.error_handler = error_handler;
	event_mgr.info_handler = info_handler;
	event_mgr.warning_handler = warning_handler;

	dinfo = opj_create_decompress(CODEC_J2K);
	if(!dinfo)
		return;

	opj_set_default_decoder_parameters(&parameters);
	opj_set_event_mgr((opj_common_ptr)dinfo, &event_mgr, 0);

	opj_setup_decoder(dinfo, &parameters);

	cio = opj_cio_open((opj_common_ptr)dinfo, (uchar*)&data[0], data.size());

	if(!cio){
		opj_image_destroy(image);
		opj_destroy_decompress(dinfo);
		return;
	}

	image = opj_decode(dinfo, cio);
	image2mat(image, mat);

	opj_cio_close(cio);
	opj_image_destroy(image);
	opj_destroy_decompress(dinfo);
}


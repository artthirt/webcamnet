#ifndef JPEG_DECODE_H
#define JPEG_DECODE_H

#include <opencv2/core/core.hpp>
#include "common.h"

class jpeg_decode
{
public:
	jpeg_decode();

	void decode(const bytearray& data, cv::Mat& mat);
};

#endif // JPEG_DECODE_H

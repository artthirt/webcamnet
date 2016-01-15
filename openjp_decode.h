#ifndef OPENJP_DECODE_H
#define OPENJP_DECODE_H

#include <opencv2/core/core.hpp>
#include <vector>

#include "common.h"

class openjp_decode
{
public:
	openjp_decode();

	void decode(const bytearray& data, cv::Mat& mat);
};

#endif // OPENJP_DECODE_H

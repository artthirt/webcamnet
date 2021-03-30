#ifndef JPEG_DECODE_H
#define JPEG_DECODE_H

#include <opencv2/core/core.hpp>
#include "common.h"

class jpeg_decode
{
public:
	jpeg_decode();

	void decode(const bytearray& data, cv::Mat& mat);

    void setError()    { mError = true; }
    void resetError()  { mError = false; }
    bool error() const { return mError; }

private:
    bool mError = false;
};

#endif // JPEG_DECODE_H

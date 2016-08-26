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

private:
	bytearray* m_array;
	uint m_offset;

	static size_t read_stream(void * p_buffer, size_t p_nb_bytes, void * p_user_data);
};

#endif // OPENJP_DECODE_H

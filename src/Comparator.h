#pragma once

#include <thread>
#include <opencv2/core.hpp>

class Comparator {

public:
	Comparator(const cv::Mat& imgA, const cv::Mat& imgB, const uint8_t& parts);

	virtual ~Comparator() = default;

	void setImages(const cv::Mat& imgA, const cv::Mat& imgB);

	long difference() const;
	double fitness() const;

private:

	static void sumPartitionThread(
		uint8_t* aPtr,
		uint8_t* bPtr,
		int start,
		int end,
		int index,
		int* sums
	);

private:

	cv::Mat m_imgA;
	cv::Mat m_imgB;

	long m_pixelCount;
	uint8_t m_parts;
};

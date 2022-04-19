#include "Comparator.h"

Comparator::Comparator(const cv::Mat& imgA, const cv::Mat& imgB, const uint8_t& parts) {

	setImages(imgA, imgB);
	m_parts = parts;
}

void Comparator::setImages(const cv::Mat& imgA, const cv::Mat& imgB) {

	m_imgA = imgA;
	if (imgA.size() != imgB.size()) {
		m_imgB = cv::Mat::zeros(imgA.size(), CV_8UC3);
		imgA.copyTo(m_imgB);
	}
	else {
		m_imgB = imgB;
	}

	m_pixelCount = m_imgA.rows * m_imgA.cols * 3;
}

long Comparator::difference() const {

	long diff = 0;
	uint8_t* bPtr = (uint8_t*)m_imgB.ptr<uint8_t>();
	uint8_t* aPtr = (uint8_t*)m_imgA.ptr<uint8_t>();

	int partSize = m_pixelCount / m_parts;
	int* partSums = new int[m_parts];

	std::thread* threads = new std::thread[m_parts];

	for (int p = 0; p < m_parts; p++) {
		int start = partSize * p;
		int end = start + partSize;

		threads[p] = std::thread(
			Comparator::sumPartitionThread,
			aPtr, bPtr, start, end, p, partSums
		);
	}

	for (int p = 0; p < m_parts; p++) {
		threads[p].join();
	}

	for (int p = 0; p < m_parts; p++) {
		diff += partSums[p];
	}

	delete[] threads;
	delete[] partSums;

	return diff;
}

double Comparator::fitness() const {

	long diff = difference();
	return 1.0 - (diff / (255.0 * 3.0 * m_pixelCount));
}

void Comparator::sumPartitionThread(
	uint8_t* aPtr,
	uint8_t* bPtr,
	int start,
	int end,
	int index,
	int* sums
) {

	long diff = 0;
	uint8_t* a = aPtr + start;
	uint8_t* b = bPtr + start;
	for (int i = start; i < end; i++) {
		diff += abs(*a - *b);
		a++;
		b++;
	}
	sums[index] = diff;
}

#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "Random.h"

class Ellipse {

public:

	Ellipse(
		cv::Point center,
		cv::Size size,
		double rotation,
		cv::Scalar color,
		double opacity
	);
	Ellipse(const Ellipse& that);
	Ellipse& operator=(const Ellipse& that);

	virtual ~Ellipse() = default;

public:

	static Ellipse randomizedInBounds(
		cv::Size bounds,
		uint16_t minSize,
		uint16_t maxSize
	);

public:

	Ellipse mutated() const;

	void apply(cv::Mat& dest) const;

private:

	static Random m_random;

	cv::Point m_center;
	cv::Size m_size;
	double m_rotation;
	cv::Scalar m_color;
	double m_opacity;
};

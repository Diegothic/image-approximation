#include "Ellipse.h"

Random Ellipse::m_random{};

Ellipse::Ellipse(
	cv::Point center,
	cv::Size size,
	double rotation,
	cv::Scalar color,
	double opacity
) {

	size.width = abs(size.width);
	size.height = abs(size.height);

	while (rotation > 360.0)
		rotation -= 360.0;
	while (rotation < 0.0)
		rotation += 360.0;

	for (int colInd = 0; colInd < 3; colInd++)
		color.val[colInd] = std::clamp(color.val[colInd], 0.0, 255.0);

	opacity = std::clamp(opacity, 0.5, 1.0);

	m_center = center;
	m_size = size;
	m_rotation = rotation;
	m_color = color;
	m_opacity = opacity;
}

Ellipse::Ellipse(const Ellipse& that) {

	*this = that;
}

Ellipse& Ellipse::operator=(const Ellipse& that) {

	m_center = that.m_center;
	m_size = that.m_size;
	m_rotation = that.m_rotation;
	m_color = that.m_color;
	m_opacity = that.m_opacity;

	return *this;
}

Ellipse Ellipse::randomizedInBounds(
	cv::Size bounds,
	uint16_t minSize,
	uint16_t maxSize
) {

	cv::Point center(
		m_random.rangedInt(0, bounds.width),
		m_random.rangedInt(0, bounds.height)
	);
	cv::Size size(
		m_random.rangedInt(minSize, maxSize),
		m_random.rangedInt(minSize, maxSize)
	);
	double rotation = m_random.rangedDouble(0.0, 360.0);
	cv::Scalar color(
		m_random.rangedInt(0, 255),
		m_random.rangedInt(0, 255),
		m_random.rangedInt(0, 255)
	);
	double opacity = m_random.rangedDouble(0.5, 1.0);

	return Ellipse(center, size, rotation, color, opacity);

}

Ellipse Ellipse::mutated() const {

	cv::Point centerOffset(
		m_random.rangedInt(-5, 5),
		m_random.rangedInt(-5, 5)
	);
	cv::Size sizeOffset(
		m_random.rangedInt(-5, 5),
		m_random.rangedInt(-5, 5)
	);
	double rotationOffset(m_random.rangedDouble(-5.0, 5.0));
	cv::Scalar colorOffset(
		m_random.rangedInt(-10, 10),
		m_random.rangedInt(-10, 10),
		m_random.rangedInt(-10, 10)
	);
	double opacityOffset = m_random.rangedDouble(-0.05, 0.05);

	auto center = m_center + centerOffset;
	auto size = m_size + sizeOffset;
	auto rotation = m_rotation + rotationOffset;
	auto color = m_color + colorOffset;
	auto opacity = m_opacity + opacityOffset;

	return Ellipse(center, size, rotation, color, opacity);
}

void Ellipse::apply(cv::Mat& dest) const {

	auto alpha = m_opacity;
	auto beta = 1.0 - alpha;

	cv::Mat overlay = cv::Mat::zeros(dest.size(), CV_8UC3);
	dest.copyTo(overlay);

	cv::ellipse(overlay, m_center, m_size, m_rotation, 0.0, 360.0, m_color, cv::FILLED);
	cv::addWeighted(overlay, alpha, dest, beta, 0.0, dest);
}

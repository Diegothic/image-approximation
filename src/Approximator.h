#pragma once

#include <iostream>
#include <filesystem>
#include <regex>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "Ellipse.h"
#include "Comparator.h"

class Approximator {

public:

	Approximator();
	Approximator(
		std::string ref,
		std::string out,
		uint16_t ellipses,
		uint16_t progressStep
	);

	virtual ~Approximator() = default;

	Approximator& selectReference();
	Approximator& selectDestination();
	Approximator& selectConfiguration();

	void start();

private:

	void initCanvas();
	void recreate();
	Ellipse findNewEllipse() const;
	Ellipse findBestMutation(const Ellipse& candidate) const;
	void outputProgress(int num) const;

	bool validateReference() const;
	bool validateOutputDir() const;

private:

	cv::Mat m_canvas;
	cv::Mat m_reference;

	std::string m_referencePath;
	std::string m_outputDir;

	uint16_t m_ellipses;
	uint16_t m_progressStep;

	uint16_t m_brushSize;
};

#include "Approximator.h"

Approximator::Approximator() {

	std::string workingDir = std::filesystem::current_path().string();

	m_referencePath = workingDir + "\\reference.jpg";
	m_reference = cv::imread(m_referencePath, cv::IMREAD_COLOR);

	m_outputDir = workingDir + "\\output";
	if (!std::filesystem::exists(m_outputDir)) {
		std::filesystem::create_directories(m_outputDir);
	}

	m_ellipses = 100;
	m_progressStep = 5;
	m_brushSize = 5;
}

Approximator::Approximator(
	std::string ref,
	std::string out,
	uint16_t ellipses,
	uint16_t progressStep
) {

	m_referencePath = std::move(ref);
	m_reference = cv::imread(m_referencePath, cv::IMREAD_COLOR);
	m_outputDir = std::move(out);

	m_ellipses = ellipses;
	m_progressStep = progressStep;
	m_brushSize = 5;
}

Approximator& Approximator::selectReference() {

	std::string input;
	const std::string defPath = m_referencePath;

	while (true) {
		std::cout << "Path to reference image: " << std::endl;
		std::getline(std::cin, input);


		if (input.empty()) {
			std::cout << "Default reference { "
				<< defPath
				<< " } will be chosen"
				<< std::endl;
			m_referencePath = defPath;
			m_reference = cv::imread(m_referencePath, cv::IMREAD_COLOR);
			break;
		}

		m_referencePath = input;
		m_reference = cv::imread(input, cv::IMREAD_COLOR);

		if (validateReference()) {
			break;
		}
	}

	return *this;
}

Approximator& Approximator::selectDestination() {

	std::string input;
	const std::string defPath = m_outputDir;

	while (true) {
		std::cout << "Output directory: " << std::endl;
		std::getline(std::cin, input);

		if (input.empty()) {
			std::cout << "Default directory { "
				<< defPath
				<< " } will be chosen"
				<< std::endl;
			m_outputDir = defPath;
			break;
		}

		m_outputDir = input;

		if (validateOutputDir()) {
			break;
		}
	}

	return *this;
}

Approximator& Approximator::selectConfiguration() {

	const std::regex numberRegex("[0-9]+");
	std::string input;

	while (true) {
		std::cout << "Ellipses count: " << std::endl;
		std::getline(std::cin, input);

		if (std::regex_match(input, numberRegex)) {
			m_ellipses = std::stoi(input);
			break;
		}
	}

	while (true) {
		std::cout << "Progress output step: " << std::endl;
		std::getline(std::cin, input);

		if (std::regex_match(input, numberRegex)) {
			m_progressStep = std::stoi(input);
			break;
		}
	}

	return *this;
}

void Approximator::start() {

	if (m_reference.empty()) {
		std::cout << "Cannot start without a reference image" << std::endl;
		return;
	}

	initCanvas();
	recreate();
}

void Approximator::initCanvas() {

	m_canvas = cv::Mat::zeros(m_reference.size(), CV_8UC3);
	m_canvas.setTo(cv::Scalar(255, 255, 255));
}

void Approximator::recreate() {

	m_brushSize = (uint16_t)((m_reference.size().width + m_reference.size().height) / 2.0);
	const uint8_t brushStep = std::max(m_ellipses / 25, 1);

	for (int ellipseNum = 0; ellipseNum < m_ellipses; ellipseNum++) {
		Ellipse candidate = findNewEllipse();
		candidate = findBestMutation(candidate);

		candidate.apply(m_canvas);

		if ((ellipseNum + 1) % m_progressStep == 0 || ellipseNum == m_ellipses - 1) {
			outputProgress(ellipseNum + 1);
		}

		if (ellipseNum != 0 && ellipseNum % brushStep == 0) {
			m_brushSize = std::max((int)(m_brushSize * 0.8), 5);
		}
	}

	std::cout << "Finished" << std::endl;
	getchar();
}

Ellipse Approximator::findNewEllipse() const {

	Comparator comp(m_reference, m_canvas, 2);
	const double currentFit = comp.fitness();

	Ellipse candidate = Ellipse::randomizedInBounds(m_reference.size(), 1, m_brushSize);
	cv::Mat copy = cv::Mat::zeros(m_reference.size(), CV_8UC3);
	m_canvas.copyTo(copy);
	candidate.apply(copy);

	comp.setImages(m_reference, copy);
	double newFit = comp.fitness();
	while (newFit < currentFit) {
		candidate = Ellipse::randomizedInBounds(m_reference.size(), 1, m_brushSize);

		m_canvas.copyTo(copy);
		candidate.apply(copy);

		comp.setImages(m_reference, copy);
		newFit = comp.fitness();
	}

	return candidate;
}

Ellipse Approximator::findBestMutation(const Ellipse& candidate) const {

	Ellipse best = candidate;

	Comparator comp(m_reference, m_canvas, 2);
	double currentFit = comp.fitness();

	Ellipse mutated = candidate;
	cv::Mat copy = cv::Mat::zeros(m_reference.size(), CV_8UC3);
	double mutatedFit;

	for (uint8_t i = 0; i < 100; i++) {
		mutated = candidate.mutated();

		m_canvas.copyTo(copy);
		mutated.apply(copy);

		comp.setImages(m_reference, copy);
		mutatedFit = comp.fitness();

		if (mutatedFit > currentFit) {
			currentFit = mutatedFit;
			best = mutated;
		}
	}

	return best;
}

void Approximator::outputProgress(int num) const {

	std::string output = m_outputDir + "\\" + std::to_string(num) + ".jpg";
	cv::imwrite(output, m_canvas);

	const Comparator comp(m_reference, m_canvas, 2);

	std::cout << "Progress saved with { "
		<< num
		<< " } ellipses and { "
		<< comp.fitness()
		<< " } fitness"
		<< std::endl;
}

bool Approximator::validateReference() const {

	if (m_reference.empty()) {
		std::cout << "Could not read image: "
			<< m_referencePath
			<< std::endl;
		return false;
	}
	return true;
}

bool Approximator::validateOutputDir() const {

	if (!std::filesystem::is_directory(m_outputDir)) {
		std::cout << "Is not a valid directory: "
			<< m_outputDir
			<< std::endl;
		return false;
	}
	return true;
}

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>


using namespace std;
using namespace cv;
using namespace cv::face;
namespace fs = boost::filesystem;


// Function to get the images and label data
void getImagesAndLabels(const string& path, vector<Mat>& faceSamples, vector<int>& ids, CascadeClassifier& detector) {
	for (const auto& entry : fs::directory_iterator(path)) {
		string imagePath = entry.path().string();
		Mat img = imread(imagePath, IMREAD_GRAYSCALE);

		if (img.empty()) {
			cerr << "Error: Could not read image: " << imagePath << endl;
			continue;
		}

		// Extract label from filename
		string filename = entry.path().filename().string();
		size_t first_dot = filename.find('.');
		size_t second_dot = filename.find('.', first_dot + 1);

		if (first_dot != string::npos && second_dot != string::npos) {
			string label_str = filename.substr(first_dot + 1, second_dot - first_dot - 1);
			int id = stoi(label_str);

			vector<Rect> faces;
			detector.detectMultiScale(img, faces);

			for (const auto& face : faces) {
				faceSamples.push_back(img(face));
				ids.push_back(id);

				// Print the label and image path
				cout << "Label: " << id << ", Image Path: " << imagePath << endl;
			}
		}
		else {
			cerr << "Warning: Filename does not match expected format: " << filename << endl;
		}
	}
}

int main() {
	string path = "/root/opencv-3.2.0/workspace/face_led/dataset";
    	string cascadePath = "/home/root/opencv-3.2.0/data/haarcascades/haarcascade_frontalface_default.xml";
    	string trainerPath = "trainer/trainer.yml";	// Initialize the face recognizer and detector

	// Initialize the face recognizer and detector
    	Ptr<FaceRecognizer> recognizer = createLBPHFaceRecognizer();
    	CascadeClassifier detector(cascadePath);

	if (detector.empty()) {
		cerr << "Error: Could not load cascade file. Exiting!" << endl;
		return -1;
	}

	vector<Mat> faceSamples;
	vector<int> ids;

	cout << "\n[INFO] Training faces. It will take a few secondas. Wait ..." << endl;
	getImagesAndLabels(path, faceSamples, ids, detector);
	recognizer->train(faceSamples, ids);

	// Ensure the trainer directory exists
	if (!fs::exists("trainer")) {
		fs::create_directories("trainer");
	}

	// Save the model into trainer/trainer.yml
   	 FileStorage fs(trainerPath, FileStorage::WRITE);
   	 recognizer->save(fs);
    	fs.release();

    	cout << "\n[INFO] " << ids.size() << " faces trained. Exiting Program" << endl;
   	 return 0;
}

#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <boost/filesystem.hpp>

using namespace cv;
using namespace std;
namespace fs = boost::filesystem;


int main() {
    // Initialize video capture
    VideoCapture cam(0);
    if (!cam.isOpened()) {
        cerr << "Error opening video capture. Exiting!" << endl;
        return -1;
    }
    cam.set(CAP_PROP_FRAME_WIDTH, 640); // Set video width
    cam.set(CAP_PROP_FRAME_HEIGHT, 480); // Set video height

    // Load the Haar cascade file for face detection
    string cascadePath = "/home/root/opencv-3.2.0/data/haarcascades/haarcascade_frontalface_default.xml";
    CascadeClassifier faceDetector;
    if (!faceDetector.load(cascadePath)) {
        cerr << "Error loading Haar cascade file. Exiting!" << endl;
        return -1;
    }

    // Ensure the dataset directory exists
    if (!fs::exists("dataset")) {
        fs::create_directory("dataset");
    }

    // Input user ID
    int faceID;
    cout << "\nEnter user ID and press <return> ==>  ";
    cin >> faceID;

    if (cin.fail()) {
        cerr << "Invalid input. Face ID must be a number." << endl;
        return -1;
    }

    cout << "\n [INFO] Initializing face capture. Look at the camera and wait ..." << endl;

    // Initialize face capture count
    int count = 0;
    while (true) {
        Mat frame, gray;
        cam >> frame;
        if (frame.empty()) {
            cerr << "No captured frame. Exiting!" << endl;
            break;
        }

        cvtColor(frame, gray, COLOR_BGR2GRAY);
        vector<Rect> faces;
        faceDetector.detectMultiScale(gray, faces, 1.1, 10);

        for (const auto& face : faces) {
            rectangle(frame, face, Scalar(255, 0, 0), 2);
            count++;

            // Save the captured image into the dataset folder
            string filename = "dataset/User." + to_string(faceID) + "." + to_string(count) + ".jpg";
            imwrite(filename, gray(face));

            imshow("Image", frame);
        }

        int key = waitKey(10); // Press 'ESC' for exiting video
        if (key == 27) {
            break;
        } else if (count >= 100) { // Take 30 face samples and stop video
            break;
        }
    }

    // Cleanup
    cout << "\n [INFO] Exiting Program and cleanup stuff" << endl;
    cam.release();
    destroyAllWindows();

    return 0;
}

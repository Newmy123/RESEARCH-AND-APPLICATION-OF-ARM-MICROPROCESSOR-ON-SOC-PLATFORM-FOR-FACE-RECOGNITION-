#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>

// Define missing macros
#define ALT_LWFPGASLVS_OFST 0xFF2000000ULL 
#define LED_PIO_BASE        0x3000
#define LED_PIO_SPAN        0x10
// Define LED CONTROL VALUES
#define LED_ON              0x3FF
#define LED_OFF             0x00

using namespace std;
using namespace cv;
using namespace cv::face;

int main() {
    void *led_pio;
    int fd;
    volatile int *led_register;

    // Open device file and map memory region
    fd = open("/dev/mem", (O_RDWR | O_SYNC));
    if (fd == -1) {
        cerr << "ERROR: could not open \"/dev/mem\"..." << endl;
        perror("open");
        return 1;
    }

    // Map LED PIO physical address to virtual address space
    led_pio = mmap(NULL, LED_PIO_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, static_cast<off_t>(ALT_LWFPGASLVS_OFST + LED_PIO_BASE));
    if (led_pio == MAP_FAILED) {
        cerr << "ERROR: mmap() failed..." << endl;
        perror("mmap");
        close(fd);
        return 1;
    }
    led_register = (volatile int *)led_pio;

    // Create face recognizer
    Ptr<LBPHFaceRecognizer> recognizer = createLBPHFaceRecognizer();
    recognizer->load("trainer/trainer.yml");

    // Load Haar Cascade
    string cascadePath = "/home/root/opencv-3.2.0/data/haarcascades/haarcascade_frontalface_default.xml";
    CascadeClassifier faceCascade;
    if (!faceCascade.load(cascadePath)) {
        cerr << "Error loading face cascade file. Exiting!" << endl;
        return -1;
    }

    // Define names
    vector<string> names = { "", "Giang", "Hieu", "Khanh"};

    // Initialize video capture
    VideoCapture cam(0);
    if (!cam.isOpened()) {
        cerr << "Error opening video capture. Exiting!" << endl;
        return -1;
    }

    cam.set(CAP_PROP_FRAME_WIDTH, 640);
    cam.set(CAP_PROP_FRAME_HEIGHT, 480);

    double minW = 0.1 * cam.get(CAP_PROP_FRAME_WIDTH);
    double minH = 0.1 * cam.get(CAP_PROP_FRAME_HEIGHT);

    int font = FONT_HERSHEY_SIMPLEX;
    auto start = chrono::steady_clock::now();
    int frameCount = 0;
    double fps = 0.0;

    while (true) {
        Mat frame, gray;
        cam >> frame;
        if (frame.empty()) {
            cerr << "No captured frame. Exiting!" << endl;
            break;
        }

        cvtColor(frame, gray, COLOR_BGR2GRAY);

        vector<Rect> faces;
        faceCascade.detectMultiScale(
            gray,
            faces,
            1.1,
            10,
            0 | CASCADE_SCALE_IMAGE,
            Size(minW, minH)
        );

        for (const auto& face : faces) {
            rectangle(frame, face, Scalar(0, 255, 0), 2);

            Mat faceROI = gray(face);
            int id = -1;
            double confidence = 0.0;
            recognizer->predict(faceROI, id, confidence);

            string label;
                if (confidence < 100 ) {
                    label = names[id];
		    confidence = 100 - confidence;

                } else {
                    label = "unknown";
		    confidence = 100 - confidence;

                }
 		 cout << "Detected Face - ID: " << id << " Confidence: " << confidence << endl;

            putText(frame, label, Point(face.x + 5, face.y - 5), font, 1, Scalar(255, 255, 255), 2);
            putText(frame, format("%.2f%%", confidence), Point(face.x + 5, face.y + face.height - 5), font, 1, Scalar(255, 255, 0), 1);
        }

        frameCount++;
        auto end = chrono::steady_clock::now();
        chrono::duration<double> elapsed = end - start;
        if (elapsed.count() >= 1.0) {
            fps = frameCount / elapsed.count();
            frameCount = 0;
            start = chrono::steady_clock::now();
        }

        putText(frame, format("FPS: %.2f", fps), Point(10, 30), font, 1, Scalar(255, 0, 0), 2);

        imshow("camera", frame);

        if (waitKey(1) == 27) {  // Press 'ESC' to exit
            break;
        }
    }

    // Cleanup
    cout << "\n [INFO] Exiting Program and cleanup stuff" << endl;
    if (munmap(led_pio, LED_PIO_SPAN) != 0) {
        cerr << "ERROR: munmap() failed..." << endl;
        perror("munmap");
    }
    close(fd);
    cam.release();
    destroyAllWindows();

    return 0;
}

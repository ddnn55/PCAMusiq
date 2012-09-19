#include "PCAPlayer.h"

#include <QApplication>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QImage>

#include "PCA.h"

#include "Config.h"

PCAPlayer::PCAPlayer()
{

}

void PCAPlayer::run()
{
    // load original video
    QString videofilename = QApplication::instance()->arguments()[1];
    qDebug() << "Opening " << videofilename;
    CvCapture* capture = cvCaptureFromFile(videofilename.toStdString().c_str());
    cvQueryFrame(capture);
    int frameH    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
    int frameW    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
    int fps       = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
    int numFrames = (int) cvGetCaptureProperty(capture,  CV_CAP_PROP_FRAME_COUNT);

    // load PCA
    QString pcaPath = QApplication::instance()->arguments()[2];
    PCA pca(pcaPath);


    while(1)
    {
        IplImage* img = 0;
        if(!cvGrabFrame(capture)){              // capture a frame
          printf("Could not grab a frame\n\7");
          //exit(0);
        }
        img = cvRetrieveFrame(capture);           // retrieve the captured frame

        std::vector<float> pcaProjection = pca.project(img);
        QVector<float> coefficients(pcaProjection.size());
        for(int c = 0; c < pcaProjection.size(); c++)
            coefficients[c] = pcaProjection[c];
        newCoefficients(coefficients);


        QImage newImage = this->IplImage2QImage(img);
        QImage reconstructedImage = this->IplImage2QImage(reconstructImage(coefficients));
        newFrame(newImage);
        newReconstructedFrame(cvMat2QImage(pca.backProject(pcaProjection), pca.getEVImageWidth(), pca.getEVImageHeight()));
    }
}

IplImage* PCAPlayer::reconstructImage(TimeSeriesSamples coefficients)
{

}

QImage PCAPlayer::cvMat2QImage(cv::Mat m, int width, int height)
{
    QImage image = QImage(width, height, QImage::Format_RGB888);
    QImage* qtImage = &image;

    // Assume matrix has 3 channels interleaved (RGB)

    //int channels = iplImage->nChannels;
    uchar *data = m.data;
    char r, g, b, a = 0;

    /*for (int y=0; y < qtImage->height(); y++)

            for (int x=0; x < qtImage->width(); x++, data += 1 * width * 3) {

                    b = data[x * 3 + 0];
                    g = data[x * 3 + 1];
                    r = data[x * 3 + 2];

                    qtImage->setPixel(x, y, qRgb(r,g,b));

            }*/

    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            int pixelIndex = y * width + x;
            float b = m.at<float>(pixelIndex * 3 + 0);
            float g = m.at<float>(pixelIndex * 3 + 1);
            float r = m.at<float>(pixelIndex * 3 + 2);

            image.setPixel(x, y, qRgb(r,g,b));
        }
    }

    return image;
}

QImage PCAPlayer::IplImage2QImage(IplImage *iplImage) {



    QImage image = QImage(iplImage->width, iplImage->height, QImage::Format_RGB888);
    QImage* qtImage = &image;

        // Assume IplImage having 3 channels (RGB)

        //int channels = iplImage->nChannels;
        char *data = iplImage->imageData;
        char r, g, b, a = 0;

        for (int y=0; y < qtImage->height(); y++, data += iplImage->widthStep)

                for (int x=0; x < qtImage->width(); x++) {

                        b = data[x * 3 + 0];
                        g = data[x * 3 + 1];
                        r = data[x * 3 + 2];

                        qtImage->setPixel(x, y, qRgb(r,g,b));

                }
        return image;
}

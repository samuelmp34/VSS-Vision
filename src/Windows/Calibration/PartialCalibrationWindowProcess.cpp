/*
 * This file is part of the VSS-Vision project.
 *
 * This Source Code Form is subject to the terms of the GNU GENERAL PUBLIC LICENSE,
 * v. 3.0. If a copy of the GPL was not distributed with this
 * file, You can obtain one at http://www.gnu.org/licenses/gpl-3.0/.
 */

#include <Repositories/CalibrationRepository.h>
#include <CameraReader.h>
#include <Windows/Calibration/CalibrationWindow.h>
#include <Core/FrameHelper.h>

void CalibrationWindow::setNewFrame(){
  processFrame();
  screenImage->set_image(frame);
}

void CalibrationWindow::receiveNewFrame(cv::Mat _frame){
  frame = _frame;
  dispatcher_frame.emit();
}

void CalibrationWindow::processFrame() {
  cv::warpAffine(frame, frame, cv::getRotationMatrix2D(cv::Point2f(frame.cols/2, frame.rows/2), calibration.rotation, 1.0), frame.size());

  colorRecognizer->processImage(frame);
  //cv::imshow("asd", colorRecognizer->getBinaryImage());
}
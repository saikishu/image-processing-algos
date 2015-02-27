#LUCAS KANADE 
Implementation of the Lucas-Kanade (LK) version of optical flow to estimate motion between two images. Lucas-Kanade is widely used method for optical flow estimation.

[Wikipedia: Lucas-Kanade Method](http://en.wikipedia.org/wiki/Lucas%E2%80%93Kanade_method)
##Implementation Details
Filter2D and GaussianBlur functions from OpenCV are used to take care of the derivative filters.

Window size of 9x9 pixels is used.

To keep it simple, first and last four columns and rows of the image are ignored. This ensures window doesn't go outside the bounds of the image.

Only images with less than 1px motion are considered. If it is more than 1px then program has to be modified to consider pyramid of images at different resolutions. 


###Notes
* image0.png and image1.png are present in Images dir contain motion less than one pixel.
* Avoids matrix inverse operations.

###Instructions to Run
1. Install and configure OpenCV.
2. Compile and run.
3. output.png will be generated in the same dir as the executable.

###Attributions
Some of the code to draw motion vectors on an image is inspired by [David Stavens] (http://robots.stanford.edu/cs223b05/notes/CS%20223-B%20T1%20stavens_opencv_optical_flow.pdf) lecture.



  

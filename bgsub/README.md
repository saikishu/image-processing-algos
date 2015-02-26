#Background Subtraction
Also known as Foreground Detection is a widely known technique to extract image's foreground

[Wikipedia: Background Subtraction](http://en.wikipedia.org/wiki/Background_subtraction)

##Implementation Details
This projects implements the 1-D Gaussian background subtraction algorithm. Each individual pixel is modeled using a Gaussian distribution.

###Notes
* Binary thresholding is applied by setting foreground to 255 (white) and background to 0 (black).
* Initial threshold (k) and temporal window (𝜌) values are set based on trail and error. 

###Instructions to Run
1. Download image sequence from here: [Office Dataset](http://wordpress-jodoin.dmi.usherb.ca/static/dataset/baseline/office.zip)
2. Extract images into Images directory. (Image file names format: inxxxxxx.jpg
3. First 619 images are used for generating binary threshold version of image in000620.jpg. Adjust variables as need be.

  

# Image-Processing
C/C++ application designed to seamlessly blend two images into a single, visually stunning composition. This software is specifically tailored to work with 24-bit per pixel BMP files, the industry-standard image format.

**Usage:**

[program_name] [imagefile1] [imagefile2] [ratio] [outputfile]

**e.g.**
"blendimages face.bmp butterfly.bmp 0.3 merged.bmp"

The program catches wrong or missing parameters and prints a manual page.

The ratio determines how much of imagefile1 and imagefile2 will be in the result. A ratio of 0.5 means the outputfile will be a 50:50 mixture of both. 0.3 means 30% imagefile1 and 70% imagefile2.

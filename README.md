# project_CA

The application applies two filters on a set of images in BMP format. The result will be a new image with the same format, after applying the mentioned filters.
For every image the following filters will be applied:
  - Gaussian blur.
  - Sobel operator.
  
Two different versions of the program will be developed:
  - image-seq: Sequential version.
  - image-par: Parallel version.

Application (image-seq or image-par) will take 3 parameters:
  - Execution mode.
  - Directory with input images.
  - Directory were all output images will be written.

Execution mode will be one of the following:
  - copy: No transformation is performed. It only copies files.
  - gauss: Applies the Gaussian blur filter.
  - sobel: Applies the Gaussian blur filter and then the Sobel operator.


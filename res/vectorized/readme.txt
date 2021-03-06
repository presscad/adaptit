README for vectorizing / embedding .png files into Adapt It / WX
Author: Erik Brommers
Date: 08 June 2012
--------

Background: wxWidgets supports several graphics formats, but at least with the
2.8.x and 2.9.3 library versions only supports alpha transparencies with the .png format. 
This is great for using freely-available image libraries like the Tango icon library, but 
there is a hitch: embedding .png images in a cross-platform manner requires that we vectorize 
the image in a C vector.

The wxwidgets wiki comes to the rescue here. We're following the instructions posted here:

http://wiki.wxwidgets.org/Embedding_PNG_Images

First, there's source code for a utility called bin2c that I've built for Linux. 
This utility can be run from the command line to convert a .png file into a .cpp file 
containing the vectorized version. Here's the license from the source:

// bin2c.c
//
// convert a binary file into a C source vector
//
// THE "BEER-WARE LICENSE" (Revision 3.1415):
// sandro AT sigala DOT it wrote this file. As long as you retain this notice you can do
// whatever you want with this stuff.  If we meet some day, and you think this stuff is
// worth it, you can buy me a beer in return.  Sandro Sigala
//
// syntax:  bin2c [-c] [-z] <input_file> <output_file>
//
//          -c    add the "const" keyword to definition
//          -z    terminate the array with a zero (useful for embedded C strings)
//
// examples:
//     bin2c -c myimage.png myimage_png.cpp
//     bin2c -z sometext.txt sometext_txt.cpp

I suppose we Sandro a drink if we ever meet him. :-)


Here are the basic steps to bring a new image into Adapt It:
------------
1. Create or copy the image into the source tree and add it to SVN.
   - If the image is created in-house, make sure to describe the proper copyrights
     in the .svg file. In Inkscape, this is found in the File/Document Metadata
     menu command. I've put the ones I created under CC-BY-SA copyrights.
   - If the image is copied from somewhere else, make sure the rights are such that
     we can use it.
2. Run bin2c on the image to create the vectorized version. From the linux command
   line, type:
       .\bin2c -c -z <path/to/image.png> <image>_<size>.cpp
   Where <size> is the size (32px, 48px, etc.) of the image. That should create
   a .cpp file in this directory that can be pulled into the AI project.
   whm 29Apr2020 Note: Need to tweak the internal name of the image in the .cpp file
       and also tweak the #ifdef...<header_name>...#define <header_name>...endif lines
       to be unique for the Adapt It project.
3. To use the image in Adapt It:
   a. #include the image. For example: #include "../res/vectorized/pw_48.cpp"
   b. Where you would normally work with a wxImage, replace it with the following:
          wxGetBitmapFromMemory(<id>)
      Where <id> is the id generated by the bin2c utility, found inside the .cpp
      file. For example, in the pw_48.cpp file you'll note the <id> generated is
      pw48_png.


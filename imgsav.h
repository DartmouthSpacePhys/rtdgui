// Convert fltk shared image to PNG.

#ifndef _IMAGE_H
#define _IMAGE_H 1

#include "map"
#include "string"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FL/Fl_Shared_Image.H>
#include <png.hpp>

using namespace std;

class ImageHandler
{
public:
  
  // save image as a png
  static bool save(Fl_Shared_Image *image, const char *filename) 
  {
    if (image->count() > 1) 
      return save_indexed(image, filename);
    else 
      return save_inline(image, filename);
  }

protected:
    
  // save indexed images (gif... no xpm support yet)
  static bool save_indexed(Fl_Shared_Image *image, const char *filename) // indexed images
  {
    png::image< png::rgba_pixel > png(image->w(), image->h());

    const char * const *data = image->data();
    
    // printf("Converting indexed image: %s\n",data[0]);
    
    //  fltk compressed palette length is stored as negative number - make sure it exists
    std::string id(data[0]);  // string containing image data
    if (id.find_first_of('-') == std::string::npos)
      {
	fprintf(stderr,"XPM to PNG not currently supported.\n");
	return false;
      }
    
    // extract palette size from image data string
    int palsize = atoi(id.substr( id.find_first_of('-') +1, id.size() - id.find_last_of(' ') ).c_str());
    
    // printf("Palette size: %i\n",palsize);
    // for (int i = 2; i<image->count(); i++) printf("%s\n", data[i]);
    
    // map fltk palette index characters to their position in the palette data array
    uchar *pd = (uchar *)data[1]; // palette data array
    std::map<uchar, int> palette; 
    for (uchar c=0; c<palsize*4; c+=4) 
      {
	palette[pd[c]] = c;
	//printf("%c: %02x%02x%02x \n", pd[c],pd[c+1],pd[c+2],pd[c+3]);
      }
    
    for ( int y=0; y<image->h(); y++ )  // Y loop
      {                              
	for ( int x=0; x<image->w(); x++ ) // X loop
	  {                           
	    const char *buf = image->data()[y+2];

	    char i = *(buf+x);  // character we are on representing a palette index
        
	    char r,g,b,a;
	    r=g=b=0;  // color defaults to black
	    a=255;  // alpha defaults to fully visible
        
	    // set alpha to 0 for transparent pixels
	    if (i == ' ')
	      {
		a = 0;
		continue;
	      }
        
	    // set the pixel in the pngpp image
	    r = pd[palette[i]+1];
	    g = pd[palette[i]+2];
	    b = pd[palette[i]+3];
	    png.set_pixel(x,y,png::rgba_pixel(r,g,b,a));
        
	  } // end Y loop
      } // end X loop
  
    png.write(filename);  // write the pngpp image to disk
    
    // printf("depth: %i  /  width: %i  /  height: %i / count: %i \n", image->d(), image->w(), image->h(), image->count());
    
    return true;
  } // end save_indexed
  
  // save non-indexed images (bmp, jpg, pnm) (see Erco's cheat page)
  static bool save_inline(Fl_Shared_Image *image, const char *filename)  // non-indexed images
  {
    // printf("Converting color image\n");
    png::image< png::rgba_pixel > png(image->w(), image->h());
    const char *buf = image->data()[0];

    
    for ( int y=0; y<image->h(); y++ )  // Y loop
      {                              
	for ( int x=0; x<image->w(); x++ ) // X loop
	  {                           
	    long index = (y * image->w() * image->d()) + (x * image->d()); // X/Y -> buf index
        
	    char r,g,b,a;
	    r=g=b=0;
	    a=0xFF;
        
	    switch ( image->d() ) // check image depth
	      {          
	      case 4: // 24bit + alpha
		{                                    
		  a = *(buf+index+3);
		}
	      case 3:  // 24bit                                    
		{
		  r = *(buf+index+0);
		  g = *(buf+index+1);
		  b = *(buf+index+2);
		  break;
		case 2:  // 8bit + alpha
		  {                                   
		    a = *(buf+index+1);
		  }
		case 1:  // 8bit
		  {                                    
		    r = g = b = *(buf+index);
		    break;
		  }
		}
		// fl reports depth 0 for xbm... add support here
	      default:                                     // ??
		{
		  fprintf(stderr, "Not supported: %d channels\n", image->d());
		  return false;
		}
	      }
        
	    png.set_pixel(x,y,png::rgba_pixel(r,g,b,a));
	  } // end Y loop
      } // end X loop
  
    png.write(filename);
    return true;
  } // end save_inline

    
};  // end class

#endif

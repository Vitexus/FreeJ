/*  FreeJ
 *  (c) Copyright 2001 Denis Roio aka jaromil <jaromil@dyne.org>
 *
 * This source code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License as published 
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * Please refer to the GNU Public License for more details.
 *
 * You should have received a copy of the GNU Public License along with
 * this source code; if not, write to:
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * "$Id$"
 *
 */

/**
   @file layer.h
   @brief FreeJ generic Layer interface
*/

#ifndef __LAYER_H__
#define __LAYER_H__

#include <inttypes.h>
#include <freej.h>
#include <filter.h>
#include <screen.h>
#include <jsync.h>


class Context;
class AudioCollector;
class Iterator;
class Blitter;
class Blit;

class JSClass;
class JSContext;
class JSObject;


/* function for type detection of implemented layers */
extern const char *layers_description;
///< list of implemented layers to print in help
extern Layer *create_layer(Context *env, char *file);
///< create the propriate Layer type from a file

/**
   This class describes methods and properties common to all Layers in
   FreeJ: it is the main interface for functionalities like blit
   changes, filter management and position changes.

   The public methods hereby described are matching the javascript API
   which is made available by the internal parser.

   Methods implemented to create and destroy a layer:
   - Layer::open
   - Layer::init
   - Layer::close
   
   Miscellaneus operations made available for the layer:
   - Layer::set_position
   - Layer::set_blit
   - Layer::get_blit
   - Layer::set_alpha
   - Layer::get_name

   LinkList of filters used on the layer:
   - Layer::filters

   Pointer to the initialized Context where the layer is used:
   - Layer::freej

   Geometrical informations about the layer:
   - Layer::geo
   
   @brief Layer parent abstract class
*/
class Layer: public Entry, public JSyncThread {
  friend class Blitter;
  friend class Context;
  friend class JSyncThread;
  friend class ViewPort;

 public:

  enum Type {
	  UNKNOWN,
	  TEXT,
	  F0R_GENERATOR,
#if defined HAVE_DARWIN && defined WITH_COCOA
	  GL_COCOA
#endif
  };

  Layer(); ///< Layer constructor
  virtual ~Layer(); ///< Layer destructor

  /* wrap JSyncThread::start() so we don't export JSyncThread on SWIG */

  /**
     Start the layer thread
  */
  int start() { return JSyncThread::start(); }

  Type type; ///< type of the layer

  /* these must be defined in layer implementations */

  /**
     Open a file or resource for the layer
     @param file string describing the path to the file, can be also an url
  */
  virtual bool open(const char *file) =0; ///< open the file (first called)
  /**
     Initialize the layer
     @param freej freej context where the layer will be used
   */
  virtual bool init(Context *freej) =0; ///< initialize the layer (second called)
  virtual bool init(Context *freej, int w, int h) { return this->init(freej); }; ///< overload with size
  virtual void close() =0; ///< close the layer (ready to open a new one)

  Context * context();

  bool set_parameter(int idx); ///< activate the setting on parameter pointed by idx index number

  char *get_name() { return name; };
  char *get_filename() { return filename; };
  ///< Get Layer's filename

  /**
     Move the layer to absolute position,
     coordinates refer to the upper left corner
     @param x horizontal coordinate
     @param y vertical coordinate
  */
  void set_position(int x, int y);
  ///< Set Layer's position on screen

  void slide_position(int x, int y, int speed);
  ///< Slide the Layer to a position on screen

  /**
     Set the zoom rate (magnification) for a layer
     the coordinates are floats, original size is 1.0
     @param x horizontal zoom float coefficient (default 1.0)
     @param y vertical zoom float coefficient (default 1.0)
  */
  bool set_zoom(double x, double y); ///< Zoom (resize) a Layer
  /**
     Degrees of rotation
     @param angle from 0 to 360 degrees rotation
   */
  bool set_rotate(double angle); ///< Rotate a Layer
  bool set_spin(double rot, double z);
  ///< continously zoom and rotate a Layer with a certain increment
  bool antialias;
  bool zooming;
  bool rotating;
  double zoom_x;
  double zoom_y;
  double rotate;
  double spin_rotation;
  double spin_zoom;
  
  void fit(bool maintain_aspect_ratio = true);



  /**
     If the Layer is in another blit mode then it is switched
     to alpha with zero opacity and pulsed (fade in->out)
     @param step fade value change for every frame (affects speed of fade, the higher the slower)
     @param value ceiling of the pulse, fade until there and back
  */
  void pulse_alpha(int step, int value);
  ///< Pulse the Layer in alpha blending (in->out) 

  Linklist<Parameter> *parameters;
  ///< Parameter list for the layer

  Linklist<FilterInstance> filters;
  ///< Filter list of effects applied on the Layer
  void *do_filters(void *tmp_buf); ///< process all filters on a buffer

  ScreenGeometry geo;
  ///< Geometrical information about the Layer
  ScreenGeometry geo_rotozoom;
  ///< Geometrical information about the Rotozoom

  Linklist<Iterator> iterators;
  ///< Iterator list of value modifiers
  int do_iterators(); ///< process all registered iterators
  

  bool active; ///< is active? (read-only)
  bool hidden; ///< is hidden (read-only by the blit)
  bool fade; ///< layer is deactivated at the end of current iterations (read-write internal)
  bool use_audio; ///< layer makes use of audio input
  bool opened; /// set by the layer (ex: image file has been opened)
  bool need_crop; /// tell the screen that the layer need a crop (r/w internal)
  int bgcolor; ///< matte background color
  int null_feeds; ///< counter of how many sequencial feed() returned null
  int max_null_feeds; ///< maximum null feeds tolerated

  //////////////////////// BLIT operations
  Blitter *blitter;
  Blit *current_blit;
  char *get_blit(); ///< return the name of the currently seleted blit
  bool set_blit(const char *bname); ///< select the current blit 
  void blit(); ///< operates the current blit

  ViewPort *screen;  


  AudioCollector *audio; ///< registered audio collector

  /** physical buffers */
  void *buffer; ///< feed buffer returned by layer implementation
  void *offset; ///< pointer to pixel plane


  JSClass *jsclass; ///< pointer to the javascript class

  void *js_constructor(Context *env, JSContext *cx,
		       JSObject *obj, int argc, void *aargv, char *err_msg);
  ///< javascript layer constructor
  void layer_gc(JSContext *cx, JSObject *obj);

  unsigned int textureID; ///< opengl texture id

  FPS fps; ///< FPS class
  int frame_rate; ///< value set by implemented layer type

 protected:

  Closing *deferred_calls;

  void _init(int wdt, int hgt);
  ///< Layer abstract initialization

  

  void set_filename(const char *f);
  char filename[256];


  bool is_native_sdl_surface;

  Context *env; ///< private pointer to the environment filled at _init()


 private:

  void _fit(bool maintain_aspect_ratio);

  char alphastr[5];

  void run(); ///< Main Layer thread loop

  virtual void *feed() = 0; ///< feeds in the image source

  bool cafudda(); ///< cafudda is called by the Context

  void *bgmatte;

  // working variables
  int res;
  Iterator *iter;
  Iterator *itertmp;
  // colorkey point
  uint8_t colorkey_r;
  uint8_t colorkey_g;
  uint8_t colorkey_b;

  // slide_position values
  float slide_x;
  float slide_y;


};


#endif

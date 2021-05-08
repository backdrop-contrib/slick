Slick Carousel
===============

Slick is a powerful and performant slideshow/carousel solution that works with
fields and Views, and supports enhancements for image, video, audio, and more
complex layouts.

Powerful: Slick is one of the sliders [1], as of 9/15, the only one [2], which
supports nested sliders and a mix of lazy-loaded image/video/audio with
image-to-iframe or multimedia lightbox switchers.
See below for the supported media.

Performant: Slick is stored as plain HTML the first time it is requested, and
then reused on subsequent requests. Carousels with cacheability and lazyload
are lighter and faster than those without.

Slick has gazillion options, please start with the very basic working samples
from slick_example [3] only if trouble to build slicks. Be sure to read
its README.txt. Spending 5 minutes or so will save you hours in building more
complex slideshows.

[1] https://groups.drupal.org/node/20384
[2] https://www.drupal.org/node/418616
[3] http://dgo.to/slick_extras

Slick has a lot of options, please start with the very basic working samples
from the [Slick Extras](https://www.drupal.org/project/slick_extras) module.

* [Samples](https://www.drupal.org/project/slick_extras).
* [Demo](http://kenwheeler.github.io/slick/)

Features

* Fully responsive. Scales with its container.
* Uses CSS3 when available. Fully functional when not.
* Swipe enabled. Or disabled, if you prefer.
* Desktop mouse dragging.
* Fully accessible with arrow key navigation.
* Built-in lazyLoad, and multiple breakpoint options.
* Random, autoplay, pagers, etc...
* Works with Views, Image, Media or Field collection, or none of them.
* Supports text, responsive image/ picture, responsive iframe, video, and audio
  carousels with aspect ratio, see samples. No extra jQuery plugin FitVids is
  required. Just CSS.
* Exportable via CTools, or Features.
* Modular and extensible skins, e.g.: Fullscreen, Fullwidth, Split, Grid, or
  multiple-row carousel.
* Various slide layouts are built with CSS goodness.
* Nested sliders/ overlays, or multiple carousels, within a single Slick.
* Multimedia lightboxes, or inline multimedia.
* Media switcher: linked to content, iframe, lightboxes: Colorbox, Photobox,
  PhotoSwipe.
* Cacheability + lazyload = light + fast.
* Navigation/ pager options:
  - thumbnails
  - arrows
  - dots, comes with different flavors: circle dots, dots as static grid
    thumbnails, and dots with hoverable thumbnails
  - text/tabs, provide Thumbnail caption, and leave Thumbnail style/image empty


Requirements
------------

This module requires that the following modules are also enabled:

 * [Slick library](https://github.com/kenwheeler/slick/releases) >= 1.5 and <= 1.8.1
   - Extract it as is, rename "slick-master" to "slick", so the assets are at:
        sites/../libraries/slick/slick/slick.css
        sites/../libraries/slick/slick/slick-theme.css (optional if a skin chosen)
        sites/../libraries/slick/slick/slick.min.js
 * [CTools module](https://www.drupal.org/project/ctools) >=2.x
 * [Libraries module](https://www.drupal.org/project/libraries)
 * Optional: [jqeasing](https://github.com/gdsmith/jquery.easing)
 * jQuery >= 1.8


Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules.

- Enable Slick UI sub-module to create and manage Slick Optionsets (see below).

- (Optional) Enable the Slick Fields sub-module. supports Image, Media, and
  Field collection fields.

- (Optional) Enable additional Slick modules. See a more complete list below.

- Visit the configuration page under Administration > Configuration > Media >
  Slick (admin/config/media/slick) to create a Slick optionset.

- Optionsets will be available


Additional Slick Modules
-------------------------

- [slick_views](http://dgo.to/slick_views): to get more complex slides.

- [slick_extras](http://dgo.to/slick_extras): Includes slick_devel, if you
    want to help testing and developing the Slick, and slick_example to get up
    and running quickly.

Other Recommended Modules
-------------------------

- Block reference to have more complex slide content for Fullscreen/width skins.
- Field formatter settings, to modify field formatter settings and summaries.
- Colorbox, to have grids/slides that open up image/video/audio in overlay.
- Photobox, idem ditto.
- Picture, to get truly responsive image.
- Media, including media_youtube, media_vimeo, and media_soundcloud, to have
  fairly variant slides: image, video, audio, or a mix of em.
- Field Collection, to add Overlay image/audio/video over the main image stage,
  with additional basic Scald integration for the image/video/audio overlay.
- Color field module within Field Collection to colorize the slide individually.
- Mousewheel, download from https://github.com/brandonaaron/jquery-mousewheel,
  so it is available at:
  sites/.../libraries/mousewheel/jquery.mousewheel.min.js

Documentation
-------------

Additional documentation is located in the Wiki:
https://github.com/backdrop-contrib/slick/wiki/Documentation.

Slick documentation also available:
- http://kenwheeler.github.io/slick/
- https://github.com/kenwheeler/slick/

Issues
------

Bugs and Feature requests should be reported in the Issue Queue:
https://github.com/backdrop-contrib/slick/issues.

Current Maintainers
-------------------

- [Jen Lampton](https://github.com/jenlampton).
- Seeking additional maintainers.

Credits
-------

- Ported to Backdrop CMS by [Jen Lampton](https://github.com/jenlampton)..
- Originally created for Drupal by [Arshad](https://www.drupal.org/u/arshadcn).
- Maintined and Updated for Drupal by [Gaus Surahman](https://www.drupal.org/u/gausarts)
- Maintined for Drupal by [Thalles Ferreira](https://www.drupal.org/u/thalles)
- Based on the [Slick project](http://kenwheeler.github.io/slick) by [Ken Wheeler](https://github.com/kenwheeler).


Licenses
--------

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.

Slick Carousel is Licensed under the MIT license.
Copyright (c) 2017 Ken Wheeler

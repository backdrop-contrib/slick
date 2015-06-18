Slick Carousel Module
================================================================================

Drupal module for Ken Wheeler's Slick carousel.
See http://kenwheeler.github.io/slick.

o Fully responsive. Scales with its container.
o Uses CSS3 when available. Fully functional when not.
o Swipe enabled. Or disabled, if you prefer.
o Desktop mouse dragging.
o Fully accessible with arrow key navigation.
o Autoplay, pagers, arrows, text+/thumbnail pagers etc...
o Exportable via CTools.
o Works with Views, core and contrib fields: Image, Media or Field collection.
o Optional and modular skins, e.g.: Carousel, Classic, Fullscreen, Fullwidth,
  Grid, Split. Nothing loaded unless configured so.
o Various slide layouts are built with pure CSS goodness.
o Nested slicks, slide overlays or multiple slicks within a single Slick using
  Field collection, or Views.
o Some useful hooks and drupal_alters for advanced works.
o Modular integration with various contribs via optional sub-modules. You can
  build slicks without sub-modules by simply passing markups to theme_slick().



VERSIONS:
--------------------------------------------------------------------------------
7.x-2.x supports exportable optionsets via CTools.
Be sure to run update, when upgrading from 7.x-1.x to 7.x-2.x to allow creating
database table to store/ manage option sets.
Any module that provides settings in the UI needs to store them in a table.
With Bulk exporter, or Features, optionsets may be stored in codes to avoid
database lookup. It is analog to Drupal 8 CMI.
See slick_example.slick_default_preset.inc for the stored-in-code sample.

7.x-2.x supports Slick 1.5 above, and dropped supporting Slick 1.4.x and below.
Be sure to read the project home page for more info before updating your module.


INSTALLATION:
--------------------------------------------------------------------------------
Install the module as usual, more info can be found on:
http://drupal.org/documentation/install/modules-themes/modules-7

The Slick module has several sub-modules:
- slick_ui, to manage optionsets, can be uninstalled at production.
- slick_fields, supports Image, Media file, and Field collection fields.
- slick_views, a separate project as of 2015-5-29, > beta1:
  http://dgo.to/slick_views
- slick_devel, if you want to help testing and developing the Slick.
- slick_example, if you want to get up and running quickly.
  The last two are separate projects as of 2015-5-31, > beta1:
  http://dgo.to/slick_extras

See README.txt on each sub-module for their relevant information.


REQUIREMENTS:
--------------------------------------------------------------------------------
- Slick library:
  o Download Slick archive >= 1.5 from https://github.com/kenwheeler/slick/
  o Extract it as is, rename "slick-master" to "slick", so the needed assets are
    available at:
    sites/../libraries/slick/slick/slick.css
    sites/../libraries/slick/slick/slick-theme.css (optional if a skin chosen)
    sites/../libraries/slick/slick/slick.min.js

- CTools, for exportable optionsets -- only the main "Chaos tools" is needed.
  If you have Views installed, CTools is already enabled.
  D8 in core: CMI.

- libraries (>=2.x)
  D8: dropped.

- jquery_update with jQuery > 1.7, perhaps 1.8 if trouble with the latest Slick.
  D8: dropped.

- Download jqeasing from http://gsgd.co.uk/sandbox/jquery/easing
  Rename jquery.easing.1.3.js to jquery.easing.min.js, so available at:
  sites/../libraries/easing/jquery.easing.min.js
  This is a fallback for non-supporting browsers.



OPTIONAL INTEGRATION:
--------------------------------------------------------------------------------
Slick supports enhancements and more complex layouts.
- Colorbox
- Photobox
- Picture, to get truly responsive image using art direction technique.
  D8 in core: Responsive image.
- Media, including media_youtube, media_vimeo, and media_soundcloud.
  D8: Media entity, or isfield.
- Field Collection, to add Overlay image/audio/video over the main image stage,
  with additional basic Scald integration for the image/video/audio overlay.
  D8: ?
- Color field module within Field Collection to colorize the slide individually.
  D8 in core: Color field.
- Mousewheel, download from https://github.com/brandonaaron/jquery-mousewheel,
  so it is available at:
  sites/.../libraries/mousewheel/jquery.mousewheel.min.js


OPTIONSETS:
--------------------------------------------------------------------------------
To create your optionsets, go to:
"admin/config/media/slick"
These will be available at Manage display field format, and Views UI.

To store optionsets in code for versioning and performance, use CTools Bulk
exporter, or Features. And revert it via UI to Default to avoid database lookup.



VIEWS AND FIELDS:
--------------------------------------------------------------------------------
Slick works with Views and as field display formatters.
Slick Views is available as a style plugin included at slick_views.module.
Slick Fields is available as a display formatter included at slick_fields.module
which supports core and contrib fields: Image, Media, Field collection.



PROGRAMATICALLY:
--------------------------------------------------------------------------------
See slick_fields.module for advanced sample, or slick.api.php for a simple one.


NESTED SLICKS
--------------------------------------------------------------------------------
Nested slick is a parent Slick containing slides which contain individual child
slick per slide. The child slicks are basically regular slide overlays like
a single video over the large background image, only with nested slicks it can
be many videos displayed as a slideshow.
Use Field collection, or Views to build one.
Supported multi-value fields for nested slicks: Image, Media, Atom reference.


SKINS:
--------------------------------------------------------------------------------
Skins allow swappable layouts like next/prev links, split image or caption, etc.
Be sure to enable slick_fields.module and provide a dedicated slide layout
per field to get more control over caption placements. However a combination of
skins and options may lead to unpredictable layouts, get dirty yourself.

Some default complex layout skins applied to desktop only, adjust for the mobile
accordingly. The provided skins are very basic to support the necessary layouts.
It is not the module job to match your design requirements.

Optional skins:
--------------
- None
  It is all about DIY.
  Doesn't load any extra CSS other than the basic styles required by slick.
  Skins defined by sub-modules fallback to those defined at the optionset.
  Be sure to empty the Optionset skin to disable the skin at all.
  If you are using individual slide layout, do the layouts yourself.

- 3d back
  Adds 3d view with focal point at back, works best with 3 slidesToShow,
  centerMode, and caption below the slide.

- Classic
  Adds dark background color over white caption, only good for slider (single
  slide visible), not carousel (multiple slides visible), where small captions
  are placed over images, and animated based on their placement.

- Full screen
  Works best with 1 slidesToShow. Use z-index layering > 8 to position elements
  over the slides, and place it at large regions. Currently only works with
  Slick fields, use Views to make it a block. Use block_reference inside FC to
  have more complex contents inside individual slide, and assign it to Slide
  caption fields.

- Full width
  Adds additional wrapper to wrap overlay audio/video and captions properly.
  This is designated for large slider in the header or spanning width to window
  edges at least 1170px width for large monitor.

- Boxed
  Added a 0 60px margin to slick-list container and hide neighboring slides.
  An alternative to centerPadding which still reveals neighboring slides.

- Split
  Caption and image/media are split half, and placed side by side.

- Box carousel
  Added box-shadow to the carousel slides, multiple visible slides. Use
  slidesToShow option > 2.

- Boxed split
  Caption and image/media are split half, and have edge margin 0 60px.

- Grid, to create the last grid carousel. Use slidesToShow > 1 to have more grid
  combination, only if you have considerable amount of grids, otherwise 1.
  Avoid variableWidth and adaptiveHeight. Use consistent dimensions.
  Choose skin "Grid" for starter.
  Uses the Foundation 5.5 block-grid, and disabled if you choose your own skin
  not name Grid. Otherwise overrides skin Grid accordingly.

- Rounded, should be named circle
  This will circle the main image display, reasonable for small carousels, maybe
  with a small caption below to make it nice. Use slidesToShow option > 2.
  Expecting square images.

If you want to attach extra 3rd libraries, e.g.: image reflection, image zoomer,
more advanced 3d carousels, etc, simply put them into js array of the target
skin. Be sure to add proper weight, if you are acting on existing slick events,
normally < 0 (slick.load.min.js) is the one.

See slick.slick.inc for more info on skins.


HTML structure:
--------------------------------------------------------------------------------
Note, non-BEM classes are added by JS.
Before Slick 1.4:
-----------------
<div class="slick slick-processed slick-initialized slick-slider">
  <div class="slick__slide"></div>
  <nav class="slick__arrow"></nav>
</div>


After Slick 1.4:
-----------------
<div class="slick slick-processed">
  <div class="slick__slider slick-initialized slick-slider">
    <div class="slick__slide"></div>
  </div>
  <nav class="slick__arrow"></nav>
</div>

At both cases, asNavFor should target slick-initialized class/ID attributes.


RECOMMENDED MODULES
--------------------------------------------------------------------------------
The following modules are supported, but optional.
- Colorbox, to have grids/slides that open up image/video/audio in overlay.
- Photobox, idem ditto.
- Media, to have fairly variant slides: image, video, audio.
- Field collection, to have more complex layout along with Media file.
- Color field, to colorize slide background individually via Field collection.
- Block reference to have more complex slide content for Fullscreen/width skins.
- Entity translation, to have translated file and translate links with Media.
- Field formatter settings, to modify field formatter settings and summaries.


HOW CAN YOU HELP?
--------------------------------------------------------------------------------
Please consider helping in the issue queue, provide improvement, or helping with
documentation.


TROUBLESHOOTING:
--------------------------------------------------------------------------------
- When upgrading from Slick v1.3.6 to later version, try to resave options at:
  o admin/config/media/slick
  o admin/structure/types/manage/CONTENT_TYPE/display
  o admin/structure/views/view/VIEW
  only if trouble to see the new options, or when options don't apply properly.
  This is most likely true when the library adds/changes options, or the module
  does something new.

- Always clear the cache, and re-generate JS (if aggregation is on) when
  updating the module to ensure things are picked up:
  o admin/config/development/performance

- Frontend type juggling is removed [#2497945]:
  - Please re-save and re-export optionsets if you are a pre-alpha user who
    stored optionsets in codes before alpha release on 2015-3-31, or precisely
    before 2015-3-2 commit:
    http://cgit.drupalcode.org/slick/commit/?id=f08c3b4

  - Please ignore if you:
    o are a (pre-)alpha user who stored optionsets in codes after alpha.
    o never stored/exported optionsets in codes.

- If switching from beta1 to the latest via Drush fails, try the good old UI.
  Be sure to clear cache first, then run /update.php, if broken slick.

- If you are customizing templates or theme funtions be sure to re-check them.

More info relevant to each option is available at their form display by hovering
over them, and click a dark question mark.


KNOWN ISSUES
--------------------------------------------------------------------------------
- The Slick lazyLoad is not supported with picture-enabled images. Slick only
  facilitates Picture to get in. The image formatting is taken over by Picture.
  If you want advanced lazyload, but not willing to use Picture, do preprocess
  with theme_image_lazy() and use lazy 'advanced' to override it and DIY.
  Please see theme_image_lazy() for more info.
- Photobox is not compatible with infinite true + slidesToShow > 1, since slicks
  will have clones which are filtered out by Photobox loader, otherwise dup
  thumbnails. It works best for:
  - infinite true + slidesToShow 1
  - infinite false + slidesToShow N
  If "infinite true + slidesToShow > 1" is a must, simply override the JS to
  remove ":not(.slick-cloned)", and disable 'thumbs' option.
- The following is not module related, but worth a note:
  o lazyLoad ondemand has issue with dummy image excessive height.
    Added fixes to suppress it via CSS.
  o If the total < slidesToShow, Slick behaves. Previously added a workaround to
    fix this, but later dropped and handed over to the core instead.
  o Fade option with slideToShow > 1 will screw up.
  o variableWidth ignores slidesToShow.
  o Too much centerPadding at small device affects slidesToShow.


UNKNOWN ISSUES
--------------------------------------------------------------------------------
- Anything I am not aware of.
  Please report if you find one. Your report and help is any module QA. Thanks.



CURRENT DEVELOPMENT STATUS
--------------------------------------------------------------------------------
A full release should be reasonable after proper feedbacks from the community,
some code cleanup, and optimization where needed. Patches are very much welcome.



ROADMAP
--------------------------------------------------------------------------------
- Bug fixes, code cleanup, optimization, and full release.
- Get 1.x out of dev.
- Drupal 8 port.



AUTHOR/MAINTAINER/CREDITS
--------------------------------------------------------------------------------
Slick 7.x-2.x-dev by gausarts, inspired by Flexslider with CTools integration.
Slick 7.x-1.x-dev by arshadcn, the original author.

With the help from the community:
- https://www.drupal.org/node/2232779/committers
- CHANGELOG.txt for helpful souls with their patches, suggestions and reports.


READ MORE
--------------------------------------------------------------------------------
See the project page on drupal.org: http://drupal.org/project/slick.
See the Slick docs at:
- http://kenwheeler.github.io/slick/
- https://github.com/kenwheeler/slick/

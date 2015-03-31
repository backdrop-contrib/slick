Slick Carousel Module
================================================================================

Drupal module for Ken Wheeler's Slick carousel.
See http://kenwheeler.github.io/slick.

* Fully responsive. Scales with its container.
* Uses CSS3 when available. Fully functional when not.
* Swipe enabled. Or disabled, if you prefer.
* Desktop mouse dragging.
* Fully accessible with arrow key navigation.
* Autoplay, pagers, arrows, etc...
* Exportable via CTools.
* Works with Views, core and contrib fields: Image, Media or Field collection.
* Optional and modular skins, e.g.: Carousel, Classic, Fullscreen, Fullwidth,
  Grid, Split. Nothing loaded unless required.
* Nested slicks, slide overlays or multiple slicks within a single Slick using
  Field collection, or Views.
* Some useful hooks and drupal_alters for advanced works.



VERSIONS:
--------------------------------------------------------------------------------
7.x-2.x supports exportable optionsets via CTools.
Be sure to run update, when upgrading from 7.x-1.x to 7.x-2.x to allow creating
database table to store/ manage option sets.
Any module that provides settings in the UI needs to store them in a table.
With Bulk exporter, or Features, optionsets may be stored in codes to avoid
database lookup. It is analog to Drupal 8 CMI.



REQUIREMENTS:
--------------------------------------------------------------------------------
- Slick library:
  * Download Slick archive >= 1.4 from https://github.com/kenwheeler/slick/,
  * Extract it as is, so the needed assets available at:
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
- jqeasing from http://gsgd.co.uk/sandbox/jquery/easing, so available at:
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

See README.txt on slick_fields.module for more info on slide layouts and fields
integration.



OPTIONSETS:
--------------------------------------------------------------------------------
To create your optionsets, go to:
"admin/config/media/slick"
These will be available at Manage display field format, and Views UI.

To store optionsets in code for versioning and performance, use CTools Bulk
exporter or Features. And revert it via UI to Default to avoid database lookup.



VIEWS AND FIELDS:
--------------------------------------------------------------------------------
Slick works with Views and as field display formatters.
Slick Views is available as a style plugin included at slick_views.module.
Slick Fields is available as a display formatter included at slick_fields.module
which supports core and contrib fields: Image, Media, Field collection.

See README.txt on slick_views.module for more info on Views integration.



PROGRAMATICALLY:
--------------------------------------------------------------------------------
Use renderable arrays, see slick_fields.module.



NESTED SLICKS
--------------------------------------------------------------------------------
Nested slick is a parent Slick containing slides which contain individual child
slick per slide. The child slicks are basically regular slide overlays like
a single video over the large background image, only with nested slicks it can
be many videos displayed as a slideshow.
Use Field collection, or Views to build one.
Supported multi-value fields for nested slicks: Image, Media, Atom reference.

Usage example:
- A home slideshow containing multiple videos per slide for quick overview.
- A large product/ portfolio slideshow containing a grid of slides.
- A news slideshow containing latest related news items per slide.



SKINS:
--------------------------------------------------------------------------------
Skins allow swappable layouts like next/prev links, split image or caption, etc.
Make sure to enable slick_fields.module and provide a dedicated slide layout
per field to get more control over caption placements. However a combination of
skins and options may lead to unpredictable layouts, get dirty yourself.

Some default complex layout skins applied to desktop only, adjust for the mobile
accordingly. The provided skins are very basic to support the necessary layouts.
It is not the module job to match your design requirements.

Optional skins:
--------------
- None
  Doesn't load any extra CSS other than the basic styles required by slick.
  Skins defined by sub-modules fallback to those defined at the optionset.
  Re-save existing Optionset to disable the skin at all.
  If you are using individual slide layout, you may have to do the layouts
  yourself.

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

See slick.slick.inc for more info on skins.

Tips:
----
- Use the Slick API hook_slick_skins_info() to add your own skins.
- Use the provided Wrapper class option at Optionset manager to have a unique
  context as needed, useful to build asNavFor aka thumbnail navigation.
- For nested slicks, set the parent slick to non-draggable, the child slick is,
  or vice versa, to allow proper dragging with the child slicks, otherwise both
  are dragged.



TROUBLESHOOTING:
--------------------------------------------------------------------------------
When upgrading from Slick v1.3.6 to later version, try to resave options at:
- admin/config/media/slick
- admin/structure/types/manage/CONTENT_TYPE/display
- admin/structure/views/view/VIEW
only if trouble to see the new options, or when options don't apply properly.
This is most likely true when the library adds/changes options, or the module
does something new.

Always clear the cache when updating the module to ensure things are picked up:
- admin/config/development/performance

If having JS error with jQuery v1.7, you may need to upgrade it to v1.8.

Dropped workaround for "on demand" lazyLoad. The issue is no longer valid.
However if the issue persists at your end, please try two possible fixes below.
Related old info:
Default lazyLoad "ondemand" may have issue to generate newly created images,
causing 403.
Maybe related to itok: https://www.drupal.org/drupal-7.20-release-notes
Possible fixes without compromising security:
- https://www.drupal.org/project/imageinfo_cache, to have derivatives in place.
- Use lazyLoad "progressive" instead.

lazyLoad ondemand also has issue with dummy image excessive height, so use it
with care. It is never encouraged to use by the Slick author due to bad UX.
It may be useful to cycle a large number of slides randomly like ads though.
Dummy image is for valid HTML5. Added fix for this via CSS.

More info relevant to each option is available at their form display by hovering
over them, and click a dark question mark.



SLICK > 1.4:
--------------------------------------------------------------------------------
If you just start using Slick, or never override templates, you may ignore this.
See breaking changes for more info at:
- https://github.com/kenwheeler/slick/releases/tag/1.4.0
- CHANGELOG.txt dated 2015-1-30.

Added direct child container within .slick to hold the slides, i.e.:
  .slick__slider. Previously slides are direct children of .slick container.

Since 1.4, Slick is initialized at this .slick__slider, not .slick, to allow
placing arrows within the .slick container, otherwise arrows are part of the
slides. Previous workaround by specifiying specific class (.slick__slide) no go.

Slick 1.4 will break existing asNavFor, so be sure to update the asNavFor
selectors accordingly at Field formatter and Views pages, e.g.:

  Before: #slick-nodes targetting .slick container.
  After: #slick-nodes-slider (note "-slider") targetting .slick__slider.

  Or adding " .slick__slider" to your current selector should resolve, e.g.:
  Before: #slick-nodes
  After: #slick-nodes .slick__slider
  Apply it to both asNavFor Main and asNavFor Thumbnail.

  Or if you are using sub-modules, and unsure, simply check the new option:
  "asNavFor auto selector" at:
  - admin/structure/types/manage/CONTENT_TYPE/display
  - admin/structure/views/view/VIEW
  This will auto generate the proper asNavFor selectors accordingly instead.



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
- Colorbox, to have small grids/slides that open up images/videos in overlay.
- Photobox, idem ditto.
- Media, to have fairly variant slides: image, video, audio.
- Field collection, to have more complex layout with Media.
- Color field, to colorize slide background individually.
- Block reference to have more complex slide content for Fullscreen/width skins.
- Entity translation, to have translated file and translate links with Media.
- Field formatter settings, to modify field formatter settings and summaries.



NICE TO HAVE
--------------------------------------------------------------------------------
These should not block the proper release, but nice to have before then.
- Multi resolution fullscreen background image sizes.
  FIXED - 2015-3-31, added slick_fields_inline_css_output_info_alter() to modify
  the inline CSS outputs at your own edge-case leasure.
- Photobox integration.
  FIXED - 2015-3-9.



HOW CAN YOU HELP?
--------------------------------------------------------------------------------
Please consider helping in the issue queue, provide improvement, or helping with
documentation.


KNOWN ISSUES
--------------------------------------------------------------------------------
- It currently supports Slick 1.4 above (< 1.5), and dropped Slick 1.3 below.
- Fullscreen admin preview with Views is not working, and intentionally disabled
  to avoid interfering/ covering admin pages.
- The following is not module related, but worth a note:
  * lazyLoad ondemand has issue with dummy image excessive height. See above for
    the recommended fixes. Added fixes to suppress it via CSS.
  * If the total < slidesToShow, Slick behaves. Previously added a workaround to
    fix this, but later dropped and handed over to the core instead.


UNKNOWN ISSUES
--------------------------------------------------------------------------------
- Anything I am not aware of.
  Please report if you find one. Your report and help is any module QA. Thanks.



CURRENT DEVELOPMENT STATUS
--------------------------------------------------------------------------------
It is currently feature complete, I think. I only have few left TODOs related to
the above nice to have, but that should not stop the proper release.
A proper release should be reasonable after proper feedbacks from the community,
some code cleanup, and optimization where needed. Patches are very much welcome.



ROADMAP
--------------------------------------------------------------------------------
- Bug fixes, code cleanup, optimization, and proper release.
- Drupal 8 port, see if you can help/ provide suggestion:
  https://www.drupal.org/node/2432711



AUTHOR/MAINTAINER/CREDITS
--------------------------------------------------------------------------------
Slick 7.x-2.x-dev by gausarts, inspired by Flexslider with CTools integration.
Slick 7.x-1.x-dev by arshadcn, the original author.

With the help from the community:
- https://www.drupal.org/node/2232779/committers
- CHANGELOG.txt for helpful souls with their suggestions and reports.


READ MORE
--------------------------------------------------------------------------------
See the project page on drupal.org: http://drupal.org/project/slick.
See the Slick docs at:
- http://kenwheeler.github.io/slick/
- https://github.com/kenwheeler/slick/

Slick Example Module
================================================================================
You may not need this module if you are a developer, or site builder.

No fields installed by default, unless there is enough interest which can be
done without using Features.


REQUIREMENTS
--------------------------------------------------------------------------------
- field_image, as available with Standard install.
- field_images, must be created before seeing this example useful immediately.
- node/3 containing field_images.

The last two requirements may be adjusted to available instances, see below.

To have various slick displays, recommended to put both "field_image" and
"field_images" at the same content type. This allows building nested slick or
asNavFor at its very basic usage. You can later use the pattern to build more
complex nested slick with video/audio via Media file fields or SCALD atom
reference when using with Field collection.

We don't want to install unneeded fields, nor depend on Features for something
you will not actually use. I have seen at least a client site which doesn't
actually use Features, but they enable it just to learn Flexslider example,
and they forgot to uninstall both the example and Features, and we don't want
that to happen for Slick.

All slick extra supported fields are optional unless explicitly required.

Shortly, you have to add, or adjust the fields manually if you need to learn
from this example.

The samples depend on existence of "field_image", normally available at Article
at Standard install. And "field_images" which you should create manually, or
adjust the example references to images accordingly at the Views edit page.

See "admin/reports/fields" for the list of your fields.

The Slick example is just providing basic samples of the Slick usage:
- Several optionsets prefixed with "X" available at "admin/config/media/slick".
  You can clone what is needed, and make them disabled, or uninstalled later.

- Several view blocks available at "admin/structure/views".
  You can clone it to make it yours, and ajust anything accordingly.

- Several slick image styles at "admin/config/media/image-styles".
  You can re-create your own styles, and adjust the sample Views accordingly
  after cloning them.

You may want to edit the Views before usage, adjust possible broken settings:
admin/structure/views/view/slick_x/edit

The first block depends on node ID 3 which is expected to have "field_images":
admin/structure/views/view/slick_x/edit/block

If you don't have such node ID, adjust the filter criteria to match your site
node ID containing images.
If you don't have "field_images", simply change the broken reference into yours.

Slick grid set to have at least 10 visible images per slide to a total of 40.
Be sure to have at least 12 visible images/ nodes with image, or so to see the
grid work which results in at least 2 visible slides.

See slick_example.module for more exploration on available hooks.

And don't forget to uninstall this module at production. This only serves as
examples, no real usage, nor intended for production.

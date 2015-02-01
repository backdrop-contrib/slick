Slick Carousel Module
================================================================================
You don't need this module if you are a developer, or site builder.

No fields installed by default, unless there is enough interest which can be
done without using Features.

We don't want to install unneeded fields, nor depend on Features for something
you will not actually use. I have seen at least a client site which doesn't
actually use Features, but they enable it just to learn Flexslider example,
and they forgot to uninstall both the example and Features, and we don't want
that to happen for Slick.

Shortly you still have to add or adjust the fields manually if you need to learn
from this example.

The samples depend on existence of "field_image", normally available at Article
at Standard install. And field_images which you should create manually, or
adjust the example references to images accordngly at the Views edit page.

See admin/reports/fields for the list of your fields.

The Slick example is just providing basic samples of the Slick usage:
- Several optionsets prefixed with "X" available at admin/config/media/slick
- Several view blocks available at admin/structure/views

You should edit them before usage, and adjust some possible broken settings at:
admin/structure/views/view/slick_x/edit

The first depends on node ID 3 which is expected to have "field_images":
admin/structure/views/view/slick_x/edit/block

If you don't have such node ID, adjust the filter criteria to match your site of
node ID containing images.
If you don't have "field_images", simply change the broken reference into yours.

Slick grid set to have at least 10 visible images per slide to a total of 40.
Be sure to have at least 12 visible images/ nodes with image, or so to see the
grid work which results in at least 2 visible slides.

And don't forget to uninstall this module at production. This only serves as
examples, no real usage, nor intended for production.

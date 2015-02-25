##
## This file is only needed for Compass/Sass integration. If you are not using
## Compass, you may safely ignore or delete this file.
##

environment = :production

# Location of the theme's resources.
css_dir = "css"
sass_dir = "sass"
images_dir = "images"
generated_images_dir = images_dir + "/generated"
javascripts_dir = "js"

# Require any additional compass plugins installed on your system.
require 'breakpoint'
require 'sass-globbing'
require 'autoprefixer-rails'

on_stylesheet_saved do |file|
  css = File.read(file)
  File.open(file, 'w') do |io|
    io << AutoprefixerRails.process(css)
  end
end

# Disable cache busting on image assets.
asset_cache_buster :none

# Increased decimal precision.
# 33.33333% instead of 33.333%
Sass::Script::Number.precision = 5

##
## You probably don't need to edit anything below this.
##

# You can select your preferred output style here (:expanded, :nested, :compact
# or :compressed).
output_style = (environment == :production) ? :expanded : :nested

# To enable relative paths to assets via compass helper functions.
relative_assets = true

# Conditionally enable line comments when in development mode.
line_comments = (environment == :production) ? false : true

# Output debugging info in development mode. :unix_newlines => true
# sass_options = (environment == :production) ? {} : {:debug_info => true}
# https://github.com/Compass/compass/issues/949
# no joy by 2/25/15
sass_options = {:unix_newlines => true}

# Add the 'sass' directory itself as an import path to ease imports.
add_import_path 'sass'

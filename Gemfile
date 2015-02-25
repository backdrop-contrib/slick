source 'https://rubygems.org'

group :development do

  # Sass, Compass and extensions.
  # Updated for Foundation 5.5, and unix_newlines issue, no joy.
  gem "sass", "~> 3.4.12"       # February 14, 2015
  gem "compass", "1.0.3"        # January 15, 2015 "1.0.0"

  gem 'sass-globbing', '1.1.0'  # Import Sass files based on globbing pattern.
  gem 'compass-validator'       # So you can `compass validate`.
  gem 'breakpoint'              # Manages CSS media queries.
  gem 'css_parser'              # Helps `compass stats` output statistics.

  # Guard
  gem 'guard'                   # Guard event handler.
  gem 'guard-compass'           # Compile on sass/scss change.
  gem 'guard-shell'             # Run shell commands.
  gem 'guard-livereload'        # Browser reload.
  gem 'yajl-ruby'               # Faster JSON with LiveReload in the browser.

  gem 'autoprefixer-rails'      # Adds browser prefixes.

  # Dependency to prevent polling. Setup for multiple OS environments.
  # Optionally remove the lines not specific to your OS.
  # https://github.com/guard/guard#efficient-filesystem-handling
  gem 'rb-inotify', '~> 0.9', :require => false      # Linux
  # gem 'rb-fsevent', :require => false                # Mac OSX
  gem 'rb-fchange', :require => false                # Windows

  # Windows Directory Monitor (WDM) is required dependency for livereload Windows.
  # http://stackoverflow.com/questions/16232960/guard-wont-load-wdm
  require 'rbconfig'
  # gem 'wdm', '>= 0.1.0' if RbConfig::CONFIG['target_os'] =~ /mswin|mingw/i
  gem "wdm", :platform => [:mswin, :mingw]           # Windows

end

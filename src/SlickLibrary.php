<?php

namespace Drupal\slick;

use Drupal\blazy\BlazyLibrary;

/**
 * Provides Slick library methods mainly for hooks.
 */
class SlickLibrary extends BlazyLibrary {

  /**
   * The slick manager service.
   *
   * @var \Drupal\slick\SlickManagerInterface
   */
  protected $manager;

  /**
   * Constructs a SlickHook instance.
   */
  public function __construct(SlickManagerInterface $manager) {
    $this->manager = $manager;
  }

  /**
   * Implements hook_library_alter().
   */
  public function libraryAlter(&$libraries, $extension) {
    if ($extension === 'slick') {
      $library_path = libraries_get_path('slick') ?: libraries_get_path('slick-carousel');
      if ($library_path) {
        $libraries['slick']['js'] = [$library_path . '/slick/slick.min.js' => ['group' => JS_LIBRARY]];
        $libraries['slick']['css']['base'] = [$library_path . '/slick/slick.css' => []];
        $libraries['css']['css']['theme'] = [$library_path . '/slick/slick-theme.css' => []];
      }

      $library_easing = libraries_get_path('easing') ?: libraries_get_path('jquery.easing');
      if ($library_easing) {
        $easing_path = $library_easing . '/jquery.easing.min.js';
        // Composer via bower-asset puts the library within `js` directory.
        if (!is_file($easing_path)) {
          $easing_path = $library_easing . '/js/jquery.easing.min.js';
        }
        $libraries['easing']['js'] = [$easing_path => ['group' => JS_LIBRARY]];
      }

      $library_mousewheel = libraries_get_path('mousewheel') ?: libraries_get_path('jquery-mousewheel');
      if ($library_mousewheel) {
        $libraries['mousewheel']['js'] = [$library_mousewheel . '/jquery.mousewheel.min.js' => ['group' => JS_LIBRARY]];
      }
    }
  }

  /**
   * Implements hook_library_info_build().
   */
  public function library() {
    if (!isset($this->libraries)) {
      $info    = system_get_info('module', 'slick');
      $library = libraries_get_path('slick') ?: libraries_get_path('slick-carousel');
      $path    = drupal_get_path('module', 'slick');
      $common  = [
        'website' => 'https://drupal.org/project/slick',
        'version' => empty($info['version']) ? '7.x-3.x' : $info['version'],
      ];

      foreach (['easing', 'mousewheel'] as $key) {
        $libraries[$key] = [
          'js' => [libraries_get_path($key) . '/jquery.' . $key . '.min.js' => ['group' => JS_LIBRARY]],
        ];
      }

      $libraries['slick'] = [
        'title' => 'Slick',
        'website' => 'https://kenwheeler.github.io/slick/',
        'js' => [$library . '/slick/slick.min.js' => ['group' => JS_LIBRARY]],
        'css' => [$library . '/slick/slick.css' => []],
        'version' => '1.x',
      ];

      $libraries['css'] = [
        'dependencies' => [['slick', 'slick']],
        'css' => [$library . '/slick/slick-theme.css' => []],
      ];

      $libraries['load'] = [
        'dependencies' => [['slick', 'slick']],
        'js' => [$path . '/js/slick.load.min.js' => ['group' => JS_DEFAULT, 'weight' => -0.01]],
      ];

      $libraries['theme'] = [
        'dependencies' => [['slick', 'load']],
        'css' => [
          $path . '/css/layout/slick.module.css' => [],
          $path . '/css/theme/slick.theme.css' => [],
        ],
      ];

      $items = ['arrow.down', 'thumbnail.grid', 'thumbnail.hover'];
      foreach ($items as $item) {
        $libraries[$item] = [
          'dependencies' => [['slick', 'theme']],
          'css' => [$path . '/css/components/slick.' . $item . '.css' => []],
        ];
      }

      $libraries['colorbox'] = [
        'dependencies' => [['blazy', 'colorbox']],
        'js' => [$path . '/js/slick.colorbox.min.js' => ['group' => JS_DEFAULT]],
      ];

      foreach (SlickManager::getConstantSkins() as $group) {
        if ($skins = $this->manager->getSkinsByGroup($group)) {
          foreach ($skins as $key => $skin) {
            $provider = isset($skin['provider']) ? $skin['provider'] : 'slick';
            $id = $provider . '.' . $group . '.' . $key;

            foreach (['css', 'js', 'dependencies'] as $property) {
              if (isset($skin[$property]) && is_array($skin[$property])) {
                $libraries[$id][$property] = $skin[$property];
              }
            }
          }
        }
      }

      foreach ($libraries as &$library) {
        $library += $common;
      }
      $this->libraries = $libraries;
    }
    return $this->libraries;
  }

  /**
   * Implements hook_libraries_info().
   */
  public function librariesInfo() {
    if (!isset($this->librariesInfo)) {
      $this->librariesInfo['slick'] = [
        'name' => 'Slick carousel',
        'vendor url' => 'http://kenwheeler.github.io/slick/',
        'download url' => 'https://github.com/kenwheeler/slick/releases',
        'version' => '1.x',
        'version arguments' => [
          'file' => 'slick/slick.js',
          'pattern' => '@Version:\s+([0-9a-zA-Z\.-]+)@',
          'lines' => 16,
        ],
        'files' => ['js' => ['slick/slick.min.js']],
        'variants' => [
          'minified' => ['files' => ['js' => ['slick/slick.min.js']]],
          'source' => ['files' => ['js' => ['slick/slick.js']]],
        ],
      ];
    }
    return $this->librariesInfo;
  }

}
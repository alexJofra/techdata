<?php

/**
 * @file
 * pageflow.theme.inc
 */

/**
 * Theme declaration for Pageflow.
 */
function theme_pageflow($variables) {
  $output = array(
    '#output' => array(
      'settings' => $variables['settings'],
      'pages' => $variables['content']
    ),
    '#pre_render' => array('pageflow_pre_render_cache'),
    // @todo, allow cache to be disabled during debug mode.
    /*'#cache' => array(
      'keys' => array('pageflow', $variables['pfid']),
      'bin' => 'cache_pageflow',
      'expire' => time() + 60,
      'granularity' => DRUPAL_CACHE_GLOBAL
    )*/
  );

  return drupal_render($output);
}

/**
 * Theme declaration for Pageflow wrapper.
 */
function theme_pageflow_wrapper($variables) {
  $attributes = array('class' => array('pageflow-wrapper'));

  return '<div' . drupal_attributes($attributes) . '>' . drupal_render($variables['output']) . '</div>';
}

/**
 * Pre render list of Pageflow pages.
 */
function pageflow_pre_render_cache($element) {
  $settings = $element['#output']['settings'];
  $pages = $element['#output']['pages'];

  $output = array(
    '#theme' => 'pageflow_page',
    '#content' => $pages
  );

  $element['#markup'] = pageflow_build($output);
  $element['#children'] = drupal_render($element['#markup']);
  $element['#attached'] = array(
    'js' => array(
      array(
        'data' => array('pageflow' => $settings),
        'type' => 'setting'
      ),
      array(
        'data' => drupal_get_path('module', 'pageflow') . '/includes/js/pageflow.components.js',
        'type' => 'file'
      )
    ),
    'css' => array(
      array(
        'data' => drupal_get_path('module', 'pageflow') . '/includes/css/pageflow.css',
        'type' => 'file'
      )
    )
  );

  return $element;
}

/**
 * Build output with wrapper.
 */
function pageflow_build($output) {
  return array(
    '#output' => $output,
    '#theme_wrappers' => array('pageflow_wrapper'),
  );
}

/**
 * Theme pageflow page.
 */
function theme_pageflow_page($variables) {
  $pages = $variables['content'];

  $output = '';
  foreach ($pages as $key => $page) {
    // Format page id; convert slash to dash.
    $page_id = str_replace('/', '-', $page['data']);

    // @todo, use drupal_attributes.
    $page['#prefix'] = '<div id="' . drupal_html_id($page_id) . '" class="pageflow-page pageflow-' . $key . '" data-path="' . url($page['data']) . '"' . '>';
    $page['#suffix'] = '</div>';

    $output .= render($page);
  }

  return $output;
}

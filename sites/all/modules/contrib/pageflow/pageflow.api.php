<?php

/**
 * @file
 * API documentation for Pageflow.
 */

/**
 * Allows the altering of page regions before
 * render.
 *
 * @param array $page
 * @param array $pageflow
 */
function hook_pageflow_region_alter(&$page, $pageflow) {
  
}

/**
 * Allows altering of the final Pageflow array before 
 * render. Allows altering of Pageflow settings.
 *
 * @param array $output
 * @param int $id
 */
function hook_pageflow_pre_render_alter(&$output, $id) {

}

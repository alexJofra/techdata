<?php

/**
 * @file
 * pageflow_ui_theme.inc 
 * 
 * Pageflow administration theme inc.
 */

/**
 * Theme callback for the pageflow ui page form.
 */
function theme_pageflow_ui_page_form($variables) {
  $form = $variables['form'];

  $rows = array();
  foreach (element_children($form['pages']) as $id) {
    $form['pages'][$id]['weight']['#attributes']['class'] = array('item-weight');

    $rows[] = array(
      'data' => array(
        drupal_render($form['pages'][$id]['name']),
        drupal_render($form['pages'][$id]['page_path']),
        drupal_render($form['pages'][$id]['edit']),
        drupal_render($form['pages'][$id]['weight']),
      ),
      'class' => array('draggable'),
    );
  }

  $header = array(t('Name'), t('Path'), t('Edit'), t('Weight'));
  $table_id = drupal_html_id('pageflow-pages-table');

  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => $table_id),
  ));

  $output .= drupal_render_children($form);
  drupal_add_tabledrag($table_id, 'order', 'sibling', 'item-weight', NULL, NULL, FALSE);

  return $output;
}

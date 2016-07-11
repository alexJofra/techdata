<?php

/**
 * @file
 * pageflow_ui.admin.inc
 */

/**
 * Menu callback, admin dashboard.
 */
function pageflow_ui_dashboard() {
  $pageflow = FALSE;

  $build['main'] = array(
    'header' => array(
      '#type' => 'vertical_tabs',
      '#tree' => TRUE,
      'form' => array(
        '#tree' => TRUE,
        'id' => array(
          '#type' => 'value',
          '#value' => 'Manage',
        ),
      ),
    )
  );

  // @todo, Support filters.
  // @todo, Pager?
  $query = db_select('pageflow', 'p')->extend('PagerDefault');
  $pfids = $query
    ->fields('p', array('pfid'))
    ->limit(50)
    ->execute()
    ->fetchCol();
  $pageflows = pageflow_load_multiple($pfids);

  foreach ($pageflows as $key => $pageflow) {
    $form = drupal_get_form('pageflow_ui_page_form', $pageflow->pfid);

    $build['main']['header'][$key] = array(
      '#type' => 'fieldset',
      '#title' => $pageflow->title,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $build['main']['header'][$key]['form'] = array(
      '#type' => 'markup',
      '#markup' => render($form),
      '#tree' => TRUE,
      '#parents' => array('main', 'header', $key, 'form'),
    );
    $build['main']['header'][$key]['form']['id'] = array(
      '#type' => 'value',
      '#value' => $pageflow->title,
    );
  }

  if ($pageflow) {
    // Apparently this needs to happen.
    drupal_get_form('pageflow_ui_add_page_form');
  }

  return $build;
}

/**
 * Build pageflow inner table.
 */
function pageflow_ui_page_form($form, $form_state) {
  $form['#attributes']['class'][] = 'pageflow-ui-form';
  $id = drupal_html_id('pageflow-ui-page-form-wrapper');

  $form['#prefix'] = '<div id="' . $id . '">';
  $form['#suffix'] = '</div>';
  $form['pages'] = array(
    '#tree' => TRUE,
  );

  $pfid = isset($form_state['build_info']['args'][0]) ? $form_state['build_info']['args'][0] : FALSE;
  $form['pfid'] = array(
    '#type' => 'hidden',
    '#value' => $pfid,
  );
  $form['pd_wrapper'] = array(
    '#type' => 'hidden',
    '#value' => $id,
  );

  $results = pageflow_ui_load_page($pfid);
  foreach ($results as $result) {
    $form['pages'][$result['pfpid']] = array(
      'name' => array(
        '#type' => 'item',
        '#markup' => check_plain($result['title']),
      ),
      'path' => array(
        '#type' => 'hidden',
        '#value' => check_plain($result['path']),
      ),
      'page_path' => array(
        '#type' => 'item',
        '#markup' => check_plain($result['path']),
      ),
      'edit' => array(
        '#type' => 'submit',
        '#value' => t('Edit'),
        '#name' => $result['pfpid'],
        '#ajax' => array(
          'callback' => 'pageflow_ui_page_form_callback',
          'wrapper' => $id,
          'effect' => 'fade',
        ),
      ),
      'weight' => array(
        '#type' => 'weight',
        '#title' => t('Weight'),
        '#default_value' => $result['weight'],
        '#delta' => 10,
        '#title_display' => 'invisible',
      ),
    );
  }

  $form['actions'] = array(
    '#attributes' => array(
      'type' => 'actions'
    ),
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit', 
    '#value' => t('Submit')
  );
  $form['actions']['edit'] = array(
    '#type' => 'markup',
    '#markup' => l(t('Edit'), 'admin/structure/pageflow/manage/' .  $pfid, array('attributes' => array('class' => array('button positive-button')))),
  );
  $form['actions']['delete'] = array(
    '#type' => 'markup',
    '#markup' => l(t('Delete'), 'admin/structure/pageflow/manage/' .  $pfid . '/delete', array('attributes' => array('class' => array('button negative-button')))),
  );

  return $form;
}

/**
 * Edit ajax form callback.
 */
function pageflow_ui_page_form_callback($form, $form_state) {
  // Pass the page id to load default values.
  $pfpid = $form_state['triggering_element']['#name'];
  $page_form = drupal_get_form('pageflow_ui_add_page_form', $pfpid);

  return render($page_form);
}

/**
 * Submit callback page ui form.
 */
function pageflow_ui_page_form_submit($form, &$form_state) {
  foreach ($form_state['input']['pages'] as $id => $item) {
    db_query(
      "UPDATE {pageflow_page} SET weight = :weight WHERE path = :path",
      array(':weight' => $item['weight'], ':path' => $item['path'])
    );
  }
}

/**
 * Pageflow add new page form.
 */
function pageflow_ui_add_page_form($form, $form_state) {
  $defaults = FALSE;

  if (isset($form_state['build_info']['args'][0])) {
    $defaults = pageflow_ui_load_page(FALSE, $form_state['build_info']['args'][0]);
  }

  // Pass pageflow page id.
  $form['pfpid'] = array(
    '#type' => 'hidden',
    '#value' => $defaults ? $form_state['build_info']['args'][0] : FALSE,
  );

  $form['container'] = array(
    '#type' => 'container',
    '#title' => t('Add page'),
  );
  $form['container']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('Friendly page identifier.'),
    '#default_value' => $defaults ? $defaults[0]['title'] : '',
    '#required' => TRUE,
  );
  $form['container']['path'] = array(
    '#type' => 'textfield',
    '#title' => t('Path'),
    '#description' => t('Page url.'),
    '#default_value' => $defaults ? $defaults[0]['path'] : '',
    '#required' => TRUE,
  );

  $form['additional_settings'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => 99,
  );

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
  );
  $form['settings']['group'] = array(
    '#type' => 'select',
    '#title' => t('Pageflow Group'),
    '#options' => pageflow_ui_return_pageflow_list(),
    '#description' => t('The Pageflow group to join.'),
    '#default_value' => $defaults ? $defaults[0]['pfid'] : '',
  );
  $form['settings']['redirect'] = array(
    '#type' => 'select',
    '#title' => t('Redirect'),
    '#options' => array(
      0 => t('No'),
      1 => t('Yes'),
    ),
    '#description' => t('Redirect original path to parent Pageflow.'),
    '#default_value' => $defaults ? $defaults[0]['redirect'] : 1,
  );

  // Get active default theme.
  $theme = variable_get('theme_default');
  $regions = system_region_list($theme);

  $form['regions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Regions'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
    '#weight' => 11,
    '#group' => 'additional_settings',
  );
  $form['regions']['active'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Active Regions'),
    '#options' => $regions,
    '#description' => t('Allows regions to be suppressed from rendering.'),
    '#default_value' => $defaults ? unserialize($defaults[0]['regions']) : array_keys($regions),
  );

  $form['actions'] = array(
    '#type' => 'container',
    '#weight' => 101,
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#name' => 'submit',
  );
  /*$form['actions']['disable'] = array(
    '#type' => 'submit',
    '#value' => t('Disable'),
    '#name' => 'disable',
    '#limit_validation_errors' => array(),
    '#submit' => array('pageflow_ui_add_page_form_disable'),
  );*/
  $form['actions']['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
    '#name' => 'delete',
    '#limit_validation_errors' => array(),
    '#submit' => array('pageflow_ui_delete_pageflow_page_submit'),
  );

  return $form;
}

/**
 * Disable page submit hanlder.
 */
function pageflow_ui_add_page_form_disable($form, $form_state) {
  $pfpid = $form_state['input']['pfpid'];
  pageflow_ui_disable_page(array($pfpid));
}

/**
 * Implements hook_form_alter().
 */
function pageflow_ui_form_pageflow_ui_add_page_form_alter(&$form, $form_state) {
  if (isset($form_state['build_info']['args'][0])) {
    $form['#action'] = '';

    $form['actions']['submit']['#name'] = 'update';
    $form['actions']['submit']['#value'] = t('Update');
  }
  else {
    hide($form['actions']['disable']);
    hide($form['actions']['delete']);
  }
}

/**
 * Pageflow add new pageflow page form validation handler.
function pageflow_ui_add_page_form_validate($form, $form_state) {
  if (!array_key_exists('update', $form_state['input'])) {
    $url = db_query('SELECT p.pfid FROM {pageflow_page} AS p WHERE p.path = :path', array(
      ':path' => $form_state['values']['path'])
    )->fetchField();
    if ($url) {
      form_set_error('path', t('Path is already in use.'));
    }

    $title = db_query('SELECT p.pfid FROM {pageflow_page} AS p WHERE p.title = :title', array(
      ':title' => $form_state['values']['title'])
    )->fetchField();
    if ($title) {
      form_set_error('title', t('Title is already in use.'));
    }
  }
}*/

/**
 * Delete Pageflow page.
 */
function pageflow_ui_delete_pageflow_page_submit($form, $form_state) {
  $pfpid = $form_state['input']['pfpid'];
  pageflow_ui_delete_pageflow_page($pfpid);
}

/**
 * Pageflow page form submit handler.
 */
function pageflow_ui_add_page_form_submit($form, &$form_state) {
  if (!array_key_exists('update', $form_state['input'])) {  
    $data = array(
      'title' => $form_state['values']['title'], 
      'path' => $form_state['values']['path'],
      'pfid' => $form_state['values']['group'],
      'redirect' => $form_state['values']['redirect'],
      'regions' => $form_state['values']['regions']['active'],
    );
    $result = pageflow_ui_insert_page($data);

    if ($result) {
      $message = t('Page @title has been added.', array('@title' => $data['title']));
      $status = 'status';
    }
    else {
      $message = t('Page @title could not be added.', array('@title' => $data['title']));
      $status = 'warning';
    }
  }
  else {
    $data = array(
      'pfpid' => $form_state['input']['pfpid'],
      'title' => $form_state['values']['title'],
      'path' => $form_state['values']['path'],
      'pfid' => $form_state['values']['group'],
      'redirect' => $form_state['values']['redirect'],
      'regions' => $form_state['values']['regions']['active'],
    );
    pageflow_ui_update_page($data);

    $message = t('Page @title has been updated.', array('@title' => $data['title']));
    $status = 'status';
  }

  drupal_set_message($message, $status);
}

/**
 * Build pageflow orphan table form.
 */
function pageflow_ui_orphan_table_form($form, $form_state) {
  $pages = pageflow_ui_find_orphans();

  $header = array(
    'pfpid' => t('id'),
    'title' => t('Title'),
    'path' => t('Path'),
    'delete' => t('Delete'),
  );

  $rows = array();
  foreach ($pages as $page) {
    $delete = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#name' => $page['pfpid'],
    );

    $rows[$page['pfpid']] = array(
      'pfpid' => $page['pfpid'],
      'title' => $page['title'],
      'path' => $page['path'],
      'delete' => render($delete),
      '#attributes' => array('class' => array('page-row')),
    );
  }

  $form['table'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#empty' => t('No orphan pages found.'),
  );
  
  $form['pageflow'] = array(
    '#type' => 'select',
    '#title' => t('Pageflow'),
    '#options' => pageflow_ui_return_pageflow_list(),
  );

  $form['actions'] = array(
    '#type' => 'container',
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

/**
 * Orphan table submit handler.
 */
function pageflow_ui_orphan_table_form_submit($form, $form_state) {
  if (in_array('Delete', $form_state['input'])) {
    foreach ($form_state['input'] as $key => $value) {
      if ($value == 'Delete') {
        $pfpid = $key;
      }
    }

    pageflow_ui_delete_pageflow_page($pfpid);
  }
  else {
    $pfpids = $form_state['values']['table'];
    $pfid = $form_state['values']['pageflow'];

    pageflow_ui_enable_page($pfpids, $pfid);
  }
}

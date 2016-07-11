<?php

/**
 * @file
 * pageflow.admin.inc
 */

/**
 * Create new site entity form.
 */
function pageflow_form($form, $form_state, $pageflow = NULL) {
  $form = array();

  $form['title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#default_value' => isset($pageflow->title) ? $pageflow->title : '',
    '#required' => TRUE,
    '#maxlength' => 255,
  );

  field_attach_form('pageflow', $pageflow, $form, $form_state);

  // @todo, publishing.
  $form['additional_settings'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => 99,
  );

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Pageflow Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
    '#weight' => 11
  );
  $form['settings']['menu_selector'] = array(
    '#type' => 'textfield',
    '#title' => t('Selector'),
    '#description' => t("The main menu's most parent list item selector."),
    '#default_value' => isset($pageflow->menu_selector) ? $pageflow->menu_selector : '#block-system-main-menu .menu',
  );
  $form['settings']['orientation'] = array(
    '#type' => 'select',
    '#title' => t('Orientation'),
    '#options' => array(
      'vertical' => t('Vertical'),
      'horizontal' => t('Horizontal'),
    ),
    '#description' => t('The page scroll direction (orientation).'),
    '#default_value' => isset($pageflow->orientation) ? $pageflow->orientation : 'vertical',
  );
  $form['settings']['animation_speed'] = array(
    '#type' => 'textfield',
    '#title' => t('Animation Speed'),
    '#description' => t('The animation transition speed.'),
    '#default_value' => isset($pageflow->animation_speed) ? $pageflow->animation_speed : 550,
  );

  $form['actions'] = array(
    '#type' => 'actions',
    '#weight' => 101,
    'submit' => array(
      '#type' => 'submit',
      '#value' => isset($pageflow->sid) ? t('Update Pageflow') : t('Save Pageflow'),
    ),
    'delete_link' => array(
      '#markup' => isset($pageflow->sid) ? l(t('Delete'), 'admin/content/pageflow/manage/' . $pageflow->sid . '/delete', array('attributes' => array('id' => array('pageflow-delete-' . $pageflow->sid), 'class' => array('button remove')), 'query' => array('destination' => 'admin/content/pageflow'))) : '')
    );

  return $form;
}

/**
 * Implements hook_form_alter().
 */
function pageflow_form_pageflow_form_alter(&$form, $form_state) {
  $form['#entity'] = $form_state['pageflow'];

  $langcode = entity_language('pageflow', (object) $form['#entity']);
  $langcode = !empty($langcode) ? $langcode : LANGUAGE_NONE;

  $conditions = array();
  if (isset($form['#entity']->pfid)) {
    $conditions = array('source' => 'pageflow/' . $form['#entity']->pfid, 'language' => $langcode);
  }

  $path = (isset($form['#entity']->pfid) ? path_load($conditions) : array());
  if ($path === FALSE) {
    $path = array();
  }

  $path += array(
    'pid' => NULL,
    'source' => isset($form['#entity']->pfid) ? 'pageflow/' . $form['#entity']->pfid : NULL,
    'alias' => '',
    'language' => $langcode,
  );
  $form['path_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Path alias'),
    '#group' => 'additional_settings',
  );
  $form['path_settings']['path'] = array(
    '#access' => user_access('create url aliases') || user_access('administer url aliases'),
    '#tree' => TRUE,
    '#element_validate' => array('path_form_element_validate'),
  );
  $form['path_settings']['path']['alias'] = array(
    '#type' => 'textfield',
    '#title' => t('URL alias'),
    '#default_value' => $path['alias'],
    '#maxlength' => 255,
    '#weight' => 0,
    '#description' => t("Optionally specify an alternative URL by which this term can be accessed. Use a relative path and don't add a trailing slash or the URL alias won't work."),
  );
  $form['path_settings']['path']['pid'] = array('#type' => 'value', '#value' => $path['pid']);
  $form['path_settings']['path']['source'] = array('#type' => 'value', '#value' => $path['source']);
  $form['path_settings']['path']['language'] = array('#type' => 'value', '#value' => $path['language']);
}

/**
 * Implements hook_form_submit().
 */
function pageflow_form_submit($form, &$form_state) {
  $pageflow = entity_ui_form_submit_build_entity($form, $form_state);
  $pageflow->save();

  drupal_set_message(t('@title pageflow has been saved.', array('@title' => $pageflow->title)));

  // Additionally save the entity path alias.
  $path = FALSE;
  if (empty($pageflow->path['pid']) && !empty($pageflow->path['alias'])) {
    $path = array(
      'source' => 'pageflow/' . $pageflow->pfid,
      'alias' => $pageflow->path['alias']
    );
  }
  elseif (!empty($pageflow->path['alias'])) {
    $path = $pageflow->path;
  }

  if ($path) {
    path_save($path);
  }

  $form_state['redirect'] = 'admin/structure/pageflow';
}

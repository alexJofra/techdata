<?php
/**
* Implement hook_install().
*
* Perform actions to set up the site for this profile.
*/
function cerberus_installation_install() {
  include_once DRUPAL_ROOT . '/profiles/standard/standard.install';
  standard_install();
  // Any themes without keys here will get numeric keys and so will be enabled,
  // but not placed into variables.
  $enable = array(
    'theme_default' => 'cerberus_theme',
    'admin_theme' => 'adminimal_theme',
    'bootstrap'
  );
  theme_enable($enable);

  foreach ($enable as $var => $theme) {
    if (!is_numeric($var)) {
      variable_set($var, $theme);
    }
  }

  // Disable the default Bartik theme
  theme_disable(array('bartik'));

  // Enable clean urls on installation.
  variable_set('clean_url', 1);
}
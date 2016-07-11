/**
 * @file
 * pageflow.js
 *
 * Contains navigation handling & 
 * init stuff.
 */

(function ($) {

  Drupal.behaviors.pageflow = {
    attach: function(context, settings) {
      if (typeof Drupal.pageflow !== 'undefined') {
        Drupal.pageflow.init();
      }

      // Setup correct paths.
      if (typeof settings.pageflow.navigation !== 'undefined') {
        pageflowPaths(settings.pageflow.navigation, settings.pageflow.settings.paths);
      }
    }
  }

  /**
   * Handle path direction for single page urls. 
   */
  pageflowPaths = function (navigation, paths) {
    $nav = $(navigation.selector);

    $.each(paths, function(index, value) {
      var links = $nav.children().find('a');

      for (i = 0; i < links.length; ++i) {
        if ($(links[i]).attr('href') == Drupal.settings.basePath + index) {
          // Store original path.
          $(links[i]).attr('data-path', $(links[i]).attr('href'));

          // Normalize path for id match.
          path = index.replace(/\//g, '-');

          // @todo, update to prop?
          $(links[i]).attr('href', Drupal.settings.basePath + value + '#' + path);
        }
      }
    });
  }

  /**
   * Because $.fn. isn't pretty.
   * http://stackoverflow.com/questions/527089/is-it-possible-to-create-a-namespace-in-jquery
   */
  namespace = function() {
    var a = arguments, o = null, i, j, d;
    for (i = 0; i < a.length; i = i+1) {
      d = a[i].split(".");
      o = window;
      for (j = 0; j < d.length; j = j+1) {
        o[d[j]] = o[d[j]] || {};
        o = o[d[j]];
      }
    }

    return o;
  };

})(jQuery);

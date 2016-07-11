
/**
 * @file
 * pageflow.components.js
 */

(function ($) {
  namespace('Drupal.pageflow.navigation');

  /**
   * Pageflow init.
   */
  Drupal.pageflow.init = function (globalSettings) {
    var settings = Drupal.pageflow.defaults();
    settings = $.extend({}, settings, globalSettings);

    var reset = true;
    if (reset) {
      // @todo, Allow resetting of navigation.
      Drupal.pageflow.navigation.reset(settings.navigation.selector, 'first');
    }

    // Single page navigation.
    Drupal.pageflow.navigation.onScroll(settings.navigation);
    Drupal.pageflow.navigation.onClick(settings.navigation);

    // Single page orientation.
    Drupal.pageflow.orientation(settings.navigation.orientation);
  }

  /**
   * Pageflow defaults.
   */
  Drupal.pageflow.defaults = function () {
    return {
      navigation: {
        selector: Drupal.settings.pageflow.navigation.selector,
        orientation: Drupal.settings.pageflow.navigation.orientation,
        animationSpeed: Drupal.settings.pageflow.navigation.animation_speed
      }
    };
  }

  /**
   * Single page orientation.
   */
  Drupal.pageflow.orientation = function (orientation) {
    if (orientation == 'horizontal') {
      var total = 0;
      var windowWidth = $(window).width();

      $.each($('.pageflow-page'), function(index, value) {
        total += windowWidth;

        $(value).width(windowWidth);
        $(value).css('float', 'left');
      });

      $('.pageflow-wrapper').width(total);
    }
  }

  /**
   * Reset menu link attributes.
   */
  Drupal.pageflow.navigation.reset = function (navigation, direction) {
    $nav = $(navigation);
    $children = $nav.children();

    $.each($children, function(index, value) {
      var link = $(value).find('a');

      // Reset any active classes.
      link.removeClass('active');
      link.removeClass('active-trail');
    });

    if (direction == 'first') {
      $children.first().addClass('active-trail');
      $children.first().find('a').addClass('active');
    }
    else {
      $children.last().addClass('active-trail');
      $children.last().find('a').addClass('active');
    }
  }

  /**
   * Handles navigation click logic.
   */
  Drupal.pageflow.navigation.onClick = function (navigation) {
    $nav = $(navigation.selector);

    $.each(($nav).children(), function(index, value) {
      var link = $(value).find('a');

      $(link).click(function(e) {
        e.preventDefault();

        if ($('html, body').filter(':animated').length > 0) {
          return false;
        }

        var dataPath = $('.pageflow-page[data-path="' + $(this).attr('data-path')  + '"]');
        if (!dataPath.length) {
          return false;
        }

        if (navigation.orientation == 'horizontal') {
          $('html, body').animate({
            scrollLeft: dataPath.offset().left
          }, navigation.animationSpeed);
        }
        else {
          // Vertical.
          $('html, body').animate({
            scrollTop: dataPath.offset().top
          }, navigation.animationSpeed);
        }
      });
    });
  }

  /**
   * Update pageflow navigation on scroll.
   *
   * @todo, support horizontal scrolling.
   */
  Drupal.pageflow.navigation.onScroll = function (navigation) {
    var pageflowPage = $('.pageflow-page');

    $(window).bind('scroll', pageflowPage, function() {
      var pageHeight = [];
      var scrollPosition = window.pageYOffset;

      $.each(pageflowPage, function(index, value) {
        pageHeight[index] = ($(value).height());
        if (index != 0) {
          var i = index;
          pageHeight[index] += pageHeight[i - 1];
        }

        if (pageHeight[index] > scrollPosition) {
          Drupal.pageflow.navigation.activeMenuItem(navigation.selector, $(this).data());

          return false;
        }
      });
    });
  }

  /**
   * Set active menu item.
   */
  Drupal.pageflow.navigation.activeMenuItem = function (navigation, path) {
    $nav = $(navigation);

    $.each(($nav).children(), function(index, value) {
      var link = $(value).find('a');

      if ($(link).attr('data-path') == path.path) {
        link.addClass('active');
        link.parent().addClass('active-trail');
      }
      else {
        link.removeClass('active');
        link.parent().removeClass('active-trail');
      }
    });
  }

})(jQuery);

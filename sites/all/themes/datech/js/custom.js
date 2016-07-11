/**
 * Created by ujrmca on 24/02/2016.
 */
(function ($) {
    $(document).ready(function() {
        $('#fullpage').fullpage({
            anchors: ['start-info', 'about-info', 'service-info', 'actual-info', 'contact-info'],
            menu: '#menu',
            scrollingSpeed: 1000
        });
    });
})(jQuery);
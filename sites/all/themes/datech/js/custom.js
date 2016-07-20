/**
 * Created by ujrmca on 24/02/2016.
 */
(function ($) {
    $(document).ready(function() {

        $('.carousel').carousel({
            interval: false
        });
        $('#fullpage').fullpage({
            anchors: ['firstPage', 'secondPage', 'thirdPage', 'fourthPage'],
            menu: '#menu',
            scrollingSpeed: 1000,
            slideSelector: '.full-page-slide',
            loopHorizontal: false
        });
        $('.menu').click(function () {
           $('.menuc').slideToggle( "slow" );
        });
        $('.fp-controlArrow').click(function () {
           if($('.menuc').is(':visible')){
                $('.menuc').slideToggle( "slow" );
            }
        });
        $('.menuc').click(function () {
            if($('.menuc').is(':visible')) {
                $('.menuc').slideToggle("slow");
            }
        });
        $('#menu').click(function(event){
            event.stopPropagation();
        });

    });
})(jQuery);
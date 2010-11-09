// $Id: micro.js, v 1.0 2010/12/01 04:20:00 blup Exp $

(function ($) {

Drupal.behaviors.microFieldsetSummaries = {
  attach: function (context) {
    $('fieldset#edit-revision-information', context).drupalSetSummary(function (context) {
      return $('#edit-revision', context).is(':checked') ?
        Drupal.t('New revision') :
        Drupal.t('No revision');
    });

    $('fieldset#edit-author', context).drupalSetSummary(function (context) {
      var name = $('#edit-name').val() || Drupal.settings.anonymous,
        date = $('#edit-date').val();
      return date ?
        Drupal.t('By @name on @date', { '@name': name, '@date': date }) :
        Drupal.t('By @name', { '@name': name });
    });

    $('fieldset#edit-options', context).drupalSetSummary(function (context) {
      var vals = [];

      $('input:checked', context).parent().each(function () {
        vals.push(Drupal.checkPlain($.trim($(this).text())));
      });

      if (!$('#edit-status', context).is(':checked')) {
        vals.unshift(Drupal.t('Not published'));
      }
      return vals.join(', ');
    });
  }
};

})(jQuery);

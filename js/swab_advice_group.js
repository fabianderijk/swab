(function ($) {

  Drupal.behaviors.swabAdviceGroup = {
    attach: function(context, settings) {
      $('.advice_group_fieldset .advice-dialog').click(function() {
        Drupal.ReferencesDialog.open($(this).attr('href'), $(this).html());
        return false;
      }, context);
    }
  }

  $(document).ready(function() {
    var form = $('#swab-advice-group-page');
    if (form.length) {
      var ids = []; var names = [];
      form.find('.form-submit').click(function() {
        if (isInIframe()) {
          var items = form.find('.improvedselect_sel li');
          var row = form.find('input[name="row"]').val();
          $.each(items, function(index, val) {
            ids.push($(val).attr('so'));
            names.push($(val).text());
          });
          updateIndications(ids, names, row);
        }
        return false;
      });
    }
  })

  function isInIframe() {
    return (window.location != window.parent.location) ? true : false;
  }

  function updateIndications(ids, names, row) {
    var parentWindow = $(window.parent.document);
    var input = parentWindow.find('#edit-adv-fieldset .indication-field-' + row);
    var div = parentWindow.find('#edit-adv-fieldset .indication-placeholder-' + row);

    input.val(ids.join(','));
    div.find('.inner').empty().append(names.join(', '));

    parent.Drupal.ReferencesDialog.close();
  }

})(jQuery);
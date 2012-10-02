(function ($) {

  Drupal.behaviors.swabAdviceGroup = {
    attach: function(context, settings) {
      $('#edit-advices .add-advice-dialog').click(function() {
        Drupal.ReferencesDialog.open($(this).attr('href'), $(this).html());
        Drupal.ReferencesDialog.entityIdReceived = function(entity_type, entity_id, label) {
          var url = Drupal.settings.basePath + 'ajax/swab_get_advice';
          $.ajax({
            url: url,
            dataType: 'json',
            success: function(data) {
              $('#edit-advices #swab-advice-table').empty().append(data.return);
              swabAdviceGroupBindDelete();
            }
          })
        }
        return false;
      }, context);

      swabAdviceGroupBindDelete();
    }
  }

  function swabAdviceGroupBindDelete() {
    $('td.swab-advice-group-row-delete img').unbind('click');
    $('td.swab-advice-group-row-delete img').bind('click', function() {
      $.ajax({
        url: Drupal.settings.basePath + "ajax/swab_drop_advice_rows/" + $(this).attr('class'),
        success: function(data) {
          $('#edit-advices #swab-advice-table').empty().append(data);
        }
      })
    });
  }
})(jQuery);

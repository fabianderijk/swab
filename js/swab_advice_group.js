(function ($) {

  Drupal.behaviors.swabAdviceGroup = {
    attach: function(context, settings) {
      $('#edit-advices .add-advice-dialog').click(function() {
        Drupal.ReferencesDialog.open($(this).attr('href'), $(this).html());
        Drupal.ReferencesDialog.entityIdReceived = function(entity_type, entity_id, label) {
          var url = Drupal.settings.basePath + 'ajax/swab_get_advice/' + entity_id;
          $.getJSON(url, function(data) {
            $('#advices-table').append(data.table);
          });
        }
        return false;
      }, context);
    }
  }
})(jQuery);

(function ($) {

  Drupal.behaviors.swabAdviceGroup = {
    attach: function(context, settings) {
      $('#swab-advice-group-add-advice-form #edit-add-another').click(function() {
        var button = $(this);
        button.attr('disabled', 'disabled');
        var url = settings.basePath + 'ajax/swab_get_advice_row';
        $.getJSON(url, function(data) {
          $('#add-edit-advices-table').append(data.row);
          Drupal.tableDrag['add-edit-advices-table'].restripeTable();
          Drupal.tableDrag['add-edit-advices-table'].makeDraggable($('#add-edit-advices-table tr:last').get(0));
          button.removeAttr('disabled');
        });
        return false;
      }, context);
      $('#swab-advice-group-add-advice-form #edit-submit').unbind('click');
      $('#swab-advice-group-add-advice-form #edit-submit').bind('click', function() {
        var button = $(this);
        button.attr('disabled', 'disabled');
        var url = settings.basePath + 'ajax/swab_put_advice_rows';
        var data = swabAdviceGroupGetAdviceData();
        var ajaxCall = $.ajax({
          url: url,
          type: 'POST',
          data: data,
          dataType: 'JSON',
          success: function(data) {
            if (data.success == 'true' && isInIframe()) {
              parent.Drupal.ReferencesDialog.close(null, 0, null);
              ajaxCall.abort();
            }
          }
        });
        return false;
      }, context);
    }
  }

  function swabAdviceGroupGetAdviceData() {
    var data = {};
    var form = $('#swab-advice-group-add-advice-form');
    var els = form.find('select, input, textarea').not('.form-submit, input[type="hidden"]');
    $.each(els, function(index, element) {
      var el = $(element);
      data[el.attr('name')] = el.val();
    });
    return data;
  }

  function isInIframe() {
    return (window.location != window.parent.location) ? true : false;
  }

})(jQuery);

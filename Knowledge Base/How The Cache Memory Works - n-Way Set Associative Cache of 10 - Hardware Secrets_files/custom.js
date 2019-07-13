jQuery(document).ready(function () {
	//jQuery(".td-a-rec-id-content_bottom  ").insertBefore("#1");

	jQuery('.td-a-rec-id-content_bottom').each(function() {
		jQuery(this).parent().find('p:nth-last-child(3)').before(jQuery(this));
});
});
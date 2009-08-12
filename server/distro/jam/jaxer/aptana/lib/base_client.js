var ContentManager = 
{
	modal_window: null,
	
	fetchContent: function fetchContent(location, element, options)
	{
		if(!element)
		{
			element = 'main_content_cage';
		}
		
		new Ajax.Updater(element, location, Object.extend({ evalScripts: true }, options || {}));
	},
	
	fetchContentModal: function fetchContentModal(href)
	{
		new Ajax.Request('modal_window.html',
		{
			onComplete: function(response)
			{
				var windowHeight = document.viewport.getHeight() - 100;
				var windowWidth = document.viewport.getWidth() - 100;
				
				var content = new Template(response.responseText);
				
				var values =
				{
					href: href,
					width: windowWidth,
					height: windowHeight - 35
				}
				
				ContentManager.modal_window = new Control.Modal(content.evaluate(values),
				{
					className: 'modal',
					width: windowWidth,
					height: windowHeight,
					iframeshim: true,
					afterOpen: function()
					{
						$('control_overlay').style.position = 'absolute';
						$('modal_content_wrap').style.display = 'none';
						$('modal_window_iframe').style.display = 'none';
						
						var fixDisplay = function()
						{
							$('modal_content_wrap').style.display = '';
							$('modal_window_iframe').style.display = '';
						}
						
						setTimeout(fixDisplay, 0);
					},
					afterClose: function()
					{
						ContentManager.modal_window.destroy();
						ContentManager.modal_window = null;
					}
				});
				
				ContentManager.modal_window.open();
			}
		});
	},
	
	closeModalWindow: function closeModalWindow()
	{
		ContentManager.modal_window.close();
		
		try 
		{
			ContentManager.modal_window.destroy();
		}
		catch(e)
		{
			// nothing here...
		}
	},
	
	checkForNewVersion: function checkForNewVersion()
	{
        fetchCurrentBuildNumber.async(function(response)
		{
			if (response != false) 
			{
				$('upgrade').insert({ bottom: response }).show();
				
				// fix safari quirkiness...
				$('upgrade').setStyle({ height: $('upgrade').getHeight() + 'px' });
			}
        });
	}
}

window.onload = function()
{
	ContentManager.checkForNewVersion();
	ContentManager.fetchContent('jaxer_about.html');
}

/*
 * Simple DOM access via $ convenience method.
 */function $(id)
{
    return document.getElementById(id);
}

$.runat = 'both';

/*
 * remove the HTML icon when inside Aptana Studio preview.
 */
function serverInit()
{
	if (Jaxer.System.executableFolder.match('com.aptana.ide.framework.jaxer')) 
	{
		document.getElementById('sampleSource').innerHTML = "";
		document.getElementById('sourceButton').innerHTML = "";
	}
		
}

function clientInit()
{
	
}
clientInit.runat='client';

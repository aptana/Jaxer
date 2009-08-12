/**
 * @author Kevin Lindsey
 */

// globals
var FILE_PROTOCOL = "file://";

/**
 * getFileContext
 * 
 * @return {FileContext}
 */
function getFileContext()
{
	var result = null;
	
	try
	{
		result = editors.activeEditor.textEditor.getFileContext();
	}
	catch(e)
	{
	}
	
	return result;
}

/**
 * getFilename
 * 
 * @return {String}
 */
function getFilename()
{
	var result = null;
	var sourceProvider = getSourceProvider();
	
	if (sourceProvider !== null && sourceProvider !== undefined)
	{
		result = sourceProvider.getSourceURI() + "";
		
		if (result.indexOf(FILE_PROTOCOL) == 0)
		{
			result = result.substring(FILE_PROTOCOL.length);
		}
	}
	
	return result;
}

/**
 * getLanguageColorizer
 * 
 * @param {String} language
 * @return {LanguageColorizer}
 */
function getLanguageColorizer(language)
{
	var registry = getLanguageRegistry();
	var result = null;
	
	if (registry)
	{
		result = registry.getLanguageColorizer(language);
	}
	
	return result;
}

/**
 * getLanguageRegistry
 * 
 * @return {LanguageRegistry}
 */
function getLanguageRegistry()
{
	loadBundle("com.aptana.ide.editors");
	
	return Packages.com.aptana.ide.editors.unified.LanguageRegistry;
}

/**
 * getLanguageTokenList
 * 
 * @param {String} language
 * @return {TokenList}
 */
function getLanguageTokenList(language)
{
	var registry = getLanguageRegistry();
	var result = null;
	
	if (registry)
	{
		result = registry.getTokenList(language);
	}
	
	return result;
}

/**
 * getLexemeList
 *
 * @return {LexemeList}
 */
function getLexemeList()
{
	var result = null;
	var fileContext = getFileContext();
	
	if (fileContext !== null && fileContext !== undefined)
	{
		result = fileContext.getLexemeList();
	}
	
	return result;
}

/**
 * getParseResults
 * 
 * @return {IParseNode}
 */
function getParseResults()
{
	var fileContext = this.getFileContext();
	var parseResults = null;
	
	if (fileContext != null)
	{
		var parseState = fileContext.getParseState();
		
		if (parseState != null)
		{
			parseResults = parseState.getParseResults();
		}
	}
	
	return parseResults;
}

/**
 * getSourceProvider
 * 
 * @return {SourceProvider}
 */
function getSourceProvider()
{
	var result = null;
	var fileContext = getFileContext();
		
	if (fileContext !== null && fileContext !== undefined)
	{
		try
		{
			result = fileContext.getSourceProvider();
		}
		catch (e)
		{
		}
	}
	
	return result;
}

/**
 * getFiles
 * 
 * @param {String} filename
 */
function getFiles(filename)
{
	var ipath = Packages.org.eclipse.core.runtime.Path.fromOSString(filename);
	var workspace = Packages.org.eclipse.core.resources.ResourcesPlugin.getWorkspace();
	var root = workspace.getRoot();
	
	return root.findFilesForLocation(ipath);
}

/**
 * open
 * 
 * @param {String} filename
 */
function open(filename)
{
	loadBundle("com.aptana.ide.scripting");
	loadBundle("org.eclipse.ui.ide");

	var plugin = Packages.com.aptana.ide.scripting.ScriptingPlugin.getDefault();
	var workbench = plugin.getWorkbench();
	var activeWindow = workbench.getActiveWorkbenchWindow();
	var page = activeWindow.getActivePage();
	var files = this.getFiles(filename);
	
	if (files.length > 0)
	{
		Packages.org.eclipse.ui.ide.IDE.openEditor(page, files[0]);
	}
}

/**
 * refresh any folders that contain the specified filename
 * 
 * @param {String} filename
 */
function refresh(filename)
{
	var files = this.getFiles(filename);
	
	for (var i = 0; i < files.length; i++)
	{
		files[i].getParent().refreshLocal(1, null);
	}
}

/**
 * writeAndShow
 * 
 * @param {String} filename
 * @param {String} text
 */
function writeAndShow(filename, text)
{
	var targetFile = new File(filename);
	
	if (targetFile.exists == false)
	{
		if (targetFile.createNewFile() == false)
		{
			err.println("Unable to create" + targetFile.absolutePath);
		}
	}
	
	if (targetFile.exists)
	{
		if (targetFile.canWrite)
		{
			targetFile.write(text);
			refresh(targetFile.absolutePath);
			open(targetFile.absolutePath);
		}
		else
		{
			err.println(targetFile.absolutePath + " is read-only");
		}
	}
}

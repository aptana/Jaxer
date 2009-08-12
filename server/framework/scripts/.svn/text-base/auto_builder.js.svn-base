/* ***** BEGIN LICENSE BLOCK *****
 * Version: GPL 3
 *
 * This program is Copyright (C) 2007-2008 Aptana, Inc. All Rights Reserved
 * This program is licensed under the GNU General Public license, version 3 (GPL).
 *
 * This program is distributed in the hope that it will be useful, but
 * AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 * NONINFRINGEMENT. Redistribution, except as permitted by the GPL,
 * is prohibited.
 *
 * You can redistribute and/or modify this program under the terms of the GPL, 
 * as published by the Free Software Foundation.  You should
 * have received a copy of the GNU General Public License, Version 3 along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 * Aptana provides a special exception to allow redistribution of this file
 * with certain other code and certain additional terms
 * pursuant to Section 7 of the GPL. You may view the exception and these
 * terms on the web at http://www.aptana.com/legal/gpl/.
 * 
 * You may view the GPL, and Aptana's exception and additional terms in the file
 * titled license-jaxer.html in the main distribution folder of this program.
 * 
 * Any modifications to this file must keep this entire header intact.
 *
 * Menu: Jaxer > Auto Builder
 * Kudos: Aptana Dev Team
 * License: GPL v3
 * DOM: http://download.eclipse.org/technology/dash/update/org.eclipse.eclipsemonkey.lang.javascript
 * DOM: http://localhost/com.aptana.ide.scripting
 * OnLoad: main()
 *
 * ***** END LICENSE BLOCK ***** */

// includes
if (!window["server"])
{
	include("IDE_Utils.js");
}
include("packer2.js");

// from org.eclipse.core.resources.IResourceChangeEvent
var POST_CHANGE = 1;

// from org.eclipse.core.resources.IResourceDelta
var ADDED = 1;
var REMOVED = 2;
var CHANGED = 4;
var CONTENT = 256;
var MOVED_FROM = 4096;
var MOVED_TO = 8192;
var REPLACED = 262144;

// delays in ms
var INITIAL_BUILD_DELAY = 2000;
var BUILD_DELAY = 500;

// event-related globals
var registered = false;
var listener;
var deltaVisitor;

// packer related globals
var encoding = 62;			// 0 = None, 10 = Numeric, 62 = Normal, 95 = High ASCII
var fast_decode = true;
var special_char = false;

// build-related globals
var CONFIG_FILE = "buildConfig.js"; // No path information should be here!
var TEST_FOLDER = "" // set in processProjects for each project processed
var LOCAL_TEST_FOLDER = null; // set in buildConfig.js so you can send tests to a live instance

var firstRun = true;
var configLoaded = false;
var BUILD_CONFIG = "buildConfig.js";
var BUILD_FILE = "build.json";
var BUILD_FILE_PATTERN = /build\.json/i;
var JSON_EXTENSION_PATTERN = /\.json$/i;
var JS_EXTENSION_PATTERN = /\.js$/i;
var CLIENT_TARGET = /client/i;
var LANGUAGE = "text/json";
var id = -1;
var registeredFiles = {};
var orderedFiles = [];
var emitLoadingTrace = false;
var buildByInclude = false;

var LICENSE_TEXT = "";

/**
 * property handler object
 */
var propertyHandlers = {
    /**
     * build other targets within this build file
     *
     * @param {File} file
     * @param {Object} targets
     * @param {String} targetName
     * @param {Object} data
     */
    build: function(file, targets, targetName, data)
    {
		// determine project's test folder location
        TEST_FOLDER = file.parentFile.absolutePath + File.separator + "tests";
		
		// NOTE: using java.io.File since built-in file doesn't support deletion
		var testFolderFile = new Packages.java.io.File(TEST_FOLDER);
		
		// make sure folder does not exist for a clean build
		recursiveDelete(testFolderFile);
		
		// refresh project
		protectedRefresh(file.parentFile.absolutePath);

		// grab target list for the given target
        var targetList = targets[targetName].build;

		// process each target
        for (var i = 0; i < targetList.length; i++) 
        {
            var targetName = targetList[i];
            
            trace("Building " + targetName);
            
            runTop(file, targets, targetName);
        }
    },
	
	/**
     * skip buildMode property, but implement here so we don't get warnings about an unsupported property
     *
     * @param {File} file
     * @param {Object} targets
     * @param {String} targetName
     * @param {Object} data
     */
    buildMode: function(file, targets, targetName, data)
    {
        // no-op
    },
    
	/**
     * skip compress property, but implement here so we don't get warnings about an unsupported property
     *
     * @param {File} file
     * @param {Object} targets
     * @param {String} targetName
     * @param {Object} data
     */
    compress: function(file, targets, targetName, data)
    {
        // no-op
    },
	
	/**
     * skip compress_prepend property, but implement here so we don't get warnings about an unsupported property
     *
     * @param {File} file
     * @param {Object} targets
     * @param {String} targetName
     * @param {Object} data
     */
    compress_prepend: function(file, targets, targetName, data)
    {
        // no-op
    },
	
    /**
     * Add files to the current output
     *
     * @param {File} file
     * @param {Object} targets
     * @param {String} targetName
     * @param {Object} data
     */
    files: function(file, targets, targetName, data)
    {
        var currentPath = file.parentFile.absolutePath;
        var files = targets[targetName].files;
        		
        for (var i = 0; i < files.length; i++) 
        {
            var filename = files[i];
            var nestedFilename = currentPath + File.separator + filename;
            var nestedFile = new File(nestedFilename);
            
            // normalize path
            nestedFilename = nestedFile.absolutePath;
            
            if (nestedFile.exists) 
            {
                // prevent cycles
                if (data.files.hasOwnProperty(nestedFilename) == false) 
                {
                    data.files[nestedFilename] = true;
                    
                    if (filename.match(JSON_EXTENSION_PATTERN)) 
                    {
                        var nestedTargets = collectTargets(nestedFilename);
                        
                        // run the "default" target
                        if (nestedTargets.hasOwnProperty("default")) 
                        {
                            run(nestedFile, nestedTargets, "default", data);
                        }
                        else 
                        {
                            throw new Error("run: No 'default' target defined");
                        }
                    }
                    else 
                    {
						// prefix javascript file with a prefix comment to indicate the original source.
                        if (filename.match(JS_EXTENSION_PATTERN)) 
                        {
							data.lines.push("");
                            data.lines.push("/*");
							data.lines.push(" * fragment : " + currentPath.replace(/^.*\\/, "") + " > " + filename);
							data.lines.push(" */");
							
							if (emitLoadingTrace)
							{
								data.lines.push("coreTraceMethods.TRACE('Loading fragment: " + filename + "');");
								data.lines.push("Jaxer.lastLoadedFragment = '" + filename + "';");
							}
                        }
						if (filename.match(JS_EXTENSION_PATTERN) 
						&& buildByInclude
						&& (!filename.match('framework'))
						)
						{
							var outputName = currentPath + File.separator+'..'+ File.separator+'src'+File.separator  +filename;
							var outputFile = new File(outputName);
							
							var outputDirectory = outputFile.parentFile;

	                        data.lines = data.lines.concat([ "include('resource:///framework/src/"+filename+"')"]);	
														
							trace("Writing Src "+ outputName);
							
							if (outputDirectory.exists == false)
							{
								if (outputDirectory.createDirectory(true) == false)
								{
									throw new Error("src: Unable to create directory: " + outputDirectory.absolutePath);
								}
							}
							
				            if (outputFile.exists == false) 
				            {
				                if (outputFile.createNewFile() == false) 
				                {
				                    throw new Error("src: Unable to create file: " + outputFile.absolutePath);
				                }
				            }
				            
				            if (outputFile.exists) 
				            {
				                if (outputFile.canWrite) 
				                {							
				                    outputFile.write(LICENSE_TEXT+"\n"+removeLicenseBlocks(nestedFile.readLines()).join('\n'));
				                }
				                else 
				                {
				                    throw new Error("src: File is read-only: " + outputName);
				                }
				            }																															
						} 
						else 
						{
	                        data.lines = data.lines.concat(nestedFile.readLines());							
						}
                    }
                }
            }
            else 
            {
                throw new Error("run: File does not exist: " + nestedFile.absolutePath);
            }
        }
    },
	
    /**
     * skip names, but implement here so we don't get warnings about an unsupported property
     *
     * @param {File} file
     * @param {Object} targets
     * @param {String} targetName
     * @param {Object} data
     */
    name: function(file, targets, targetName, data)
    {
        // no-op
    },
    
    /**
     * add an output file to be written to once all files have been concatenated
     *
     * @param {File} file
     * @param {Object} targets
     * @param {String} targetName
     * @param {Object} data
     */
    output: function(file, targets, targetName, data)
    {
        data.outputs.push(targets[targetName].output);
    },
	
	/**
	 * copy unit tests into the tests directory
	 * 
	 * @param {File} file
     * @param {Object} targets
     * @param {String} targetName
     * @param {Object} data
	 */
	tests: function(file, targets, targetName, data)
	{
		var currentPath = file.parentFile.absolutePath;
        var files = targets[targetName].tests;
        		
        for (var i = 0; i < files.length; i++)
		{
			var filename = files[i];
			var nestedFilename = currentPath + File.separator + filename;
			var nestedFile = new File(nestedFilename);
			
			if (nestedFile.exists == false)
			{
				warning("Test file does not exist: " + nestedFilename);
				continue;
			}
			
            var text = nestedFile.readLines().join("\r\n");
            
            // calculate target file
			
			var TARGET_FOLDER = (LOCAL_TEST_FOLDER) ? LOCAL_TEST_FOLDER : TEST_FOLDER ;
			
			var index = findDiffIndex(TEST_FOLDER, nestedFilename);
			var outputName = TARGET_FOLDER + File.separator + nestedFilename.substring(index);

			// munge outputName to remove unit_tests_jaxer from path
			re = new RegExp("\\bunit_tests_jaxer\\"+ File.separator + "\\b");
			outputName = outputName.replace(re,"");
			
            var outputFile = new File(outputName);
			var outputDirectory = outputFile.parentFile;
			
			trace("Copying "+ outputName);
			
			if (outputDirectory.exists == false)
			{
				if (outputDirectory.createDirectory(true) == false)
				{
					throw new Error("runTop: Unable to create directory: " + outputDirectory.absolutePath);
				}
			}
			
            if (outputFile.exists == false) 
            {
                if (outputFile.createNewFile() == false) 
                {
                    throw new Error("runTop: Unable to create file: " + outputFile.absolutePath);
                }
            }
            
            if (outputFile.exists) 
            {
                if (outputFile.canWrite) 
                {
                    outputFile.write(text);
                    protectedRefresh(outputName);
                }
                else 
                {
                    throw new Error("runTop: File is read-only: " + outputName);
                }
            }
        }
	},
	
	/**
     * skip trace, but implement here so we don't get warnings about an unsupported property
     *
     * @param {File} file
     * @param {Object} targets
     * @param {String} targetName
     * @param {Object} data
     */
    trace: function(file, targets, targetName, data)
    {
        // no-op
    },
}
/**
 * main
 */
function main()
{
	var autoRun = true;
	
	if (firstRun)
	{
		// clear first run flag
		firstRun = false;
		
		// set default value
		AUTO_BUILD = false;
		
		// load buildConfig.js
		var thisFile = new File(location);
		var projectPath = thisFile.parentFile.parentFile.absolutePath;
		var buildConfig = new File(projectPath + File.separator + BUILD_CONFIG);
		
		if (buildConfig.exists)
		{
			loadConfig(buildConfig);
		}
		else
		{
			trace("buildConfig doesn't exist: " + buildConfig.absolutePath);
		}

		// set flag from config (or use default)
		autoRun = AUTO_BUILD;
		
		//trace("autoRun = " + autoRun);
	}
	
	if (autoRun) 
	{
		loadBundle("org.eclipse.core.resources");
		
		var plugin = Packages.org.eclipse.core.resources.ResourcesPlugin;
		var workspace = plugin.getWorkspace();
		
		if (registered == false) 
		{
			createInterfaces();
			
			// register resource change listener
			workspace.addResourceChangeListener(listener, POST_CHANGE);
			// tag as being registered
			registered = true;
			
			// clear registry
			registeredFiles = {};
			orderedFiles = [];
			
			trace("Auto Builder activated");
			
			id = setTimeout(processProjects, INITIAL_BUILD_DELAY);
		}
		else 
		{
			// unregister resource change listener
			workspace.removeResourceChangeListener(listener);
			
			// tag as being unregistered
			registered = false;
			
			trace("Auto Builder deactivated");
		}
	}
	else
	{
		trace("auto_builder not auto-started");
	}
}

/**
 * unwatchFiles
 *
 * @param {String} lst
 */
function unwatchFiles(lst, target)
{
    for (var source in registeredFiles) 
    {
        var infos = registeredFiles[source];
        
        for (var i = 0; i < infos.length; i++) 
        {
            var info = infos[i];
            
            if (info.buildFile == lst && info.target == target) 
            {
                infos.splice(i, 1);
            }
        }
        
        if (infos.length == 0) 
        {
            delete registeredFiles[source];
        }
    }
}

/**
 * watchFile
 *
 * @param {String} source
 * @param {String} lst
 * @param {String} target
 */
function watchFile(source, lst, target)
{
    if (registeredFiles.hasOwnProperty(source) == false) 
    {
        registeredFiles[source] = [{
            buildFile: lst,
            target: target
        }];
        
        if (target == "server framework") 
        {
            orderedFiles.push(source);
        }
    }
    else 
    {
        var items = registeredFiles[source];
        var match = false;
        
        for (var i = 0; i < items.length; i++) 
        {
            var item = items[i];
            
            if (item.buildFile == lst && item.target == target) 
            {
                match = true;
                break;
            }
        }
        
        if (match == false) 
        {
            items.push({
                buildFile: lst,
                target: target
            });
        }
    }
}

/**
 * collectTargets
 *
 * @param {IParseNode} tree
 * @return {Object}
 */
function collectTargets(filename)
{
    var file = new File(filename);
    var result = {};
    
    if (file.exists) 
    {
        if (configLoaded == false) // load the config (if not already loaded when processing the project)
        {
            var currentPath = file.parentFile.absolutePath;
            var configAbsoluteName = currentPath + File.separator + CONFIG_FILE;
            var configFile = new File(configAbsoluteName);
            loadConfig(configFile);
        }
        var source = file.readLines().join("\n");
        var data;
        
        // probably should be a bit more secure here
        data = eval('(' + source + ')');
        
        // turn single target defs into an array of defs
        if (data && data.constructor === Object) 
        {
            data = [data];
        }
        
        // process all targets in the array
        if (data && data.constructor === Array) 
        {
            for (var i = 0; i < data.length; i++) 
            {
                var item = data[i];
                var name = item.name || "default";
                
                result[name] = item;
            }
        }
    }
    else 
    {
        throw new Error("collectTargets: File does not exist: " + file.absolutePath);
    }
    
    return result;
}

/**
 * createInterfaces
 */
function createInterfaces()
{
    if (deltaVisitor == null) 
    {
        deltaVisitor = new Packages.org.eclipse.core.resources.IResourceDeltaVisitor({
            visit: function(delta)
            {
                var resourceFile = delta.getResource().getLocation().toFile();
                var name = resourceFile.getAbsolutePath();
                var result = true;
                var files = [];
                
                if (name.match(BUILD_FILE_PATTERN)) 
                {
                    files.push(new File(name));
                }
                
                if (registeredFiles.hasOwnProperty(name)) 
                {
                    var names = {};
                    var infos = registeredFiles[name];
                    
                    for (var i = 0; i < infos.length; i++) 
                    {
                        names[infos[i].buildFile] = true;
                    }
                    
                    for (var p in names) 
                    {
                        files.push(new File(p));
                    }
                }
                
                if (files.length > 0 && delta.getKind() == CHANGED && (delta.getFlags() & CONTENT) == CONTENT) 
                {
                    if (id == -1) 
                    {
                        setTimeout(function()
                        {
                            processFiles(files)
                        }, BUILD_DELAY);
                    }
                    else 
                    {
                        trace("Build already in progress");
                    }
                }
                
                return result;
            }
        });
    }
    
    if (listener == null) 
    {
        listener = new Packages.org.eclipse.core.resources.IResourceChangeListener({
            resourceChanged: function(e)
            {
                e.getDelta().accept(deltaVisitor);
            }
        });
    }
}

/**
 * createFrameworkProfile
 */
function createFrameworkProfile()
{
    try 
    {
        var uris = [];
        
        for (var i = 0; i < orderedFiles.length; i++) 
        {
            var file = orderedFiles[i];
            
            if (file.match(JSON_EXTENSION_PATTERN) === null) 
            {
                uris.push("file:///" + file);
            }
        }
        
        loadBundle("com.aptana.ide.editors");
        var profile = new Packages.com.aptana.ide.editors.profiles.Profile("Framework", "jaxer", true);
        profile.addURIs(uris);
        profiles.addProfile(profile);
    } 
    catch (e) 
    {
        trace("error while adding files to profile: " + e);
    }
}

/**
 * Find the first index where the two strings differ
 * 
 * @param {String} a
 * @param {String} b
 * @return {Number}
 */
function findDiffIndex(a, b)
{
	var minLength = Math.min(a.length, b.length);
	var result;
	
	for (result = 0; result < minLength; result++)
	{
		if (a[result] != b[result])
		{
			break;
		}
	}
	
	return result;
}

/**
 * getLexemes
 *
 * @param {File} file
 * @return {Lexeme[]}
 */
function getLexemes(file)
{
    var result = [];
    
    // associate source with lexer
    var source = file.readLines().join("\n");
    
    lexer.setSource(source);
    
    // initialize lexer language and group
    lexer.setLanguageAndGroup(LANGUAGE, "default");
    
    // accumulate lexemes until there are no more
    var lexeme = lexer.getNextLexeme();
    
    // NOTE: not handling errors
    while (lexeme !== null) 
    {
        var category = lexeme.getCategory() + "";
        
        if (category != "WHITESPACE") 
        {
            result.push(lexeme);
        }
        
        lexeme = lexer.getNextLexeme();
    }
    
    return result;
}

/**
 * processFiles
 *
 * @param {File[]} files
 */
function processFiles(files)
{
    var result = 0;
    
    // process all files
    for (var i = 0; i < files.length; i++) 
    {
        var temp = processFile(files[i].absolutePath);
        
        if (result == 0) 
        {
            result = temp;
        }
    }
    
    // create a profile of all the JS files that were processed
    //createFrameworkProfile();
    
    if (result == 0) 
    {
        trace("Build complete");
    }
    else 
    {
        trace("Error encountered during build");
    }
    
    return result;
}

/**
 * processFile
 *
 * @param {String} filename
 */
function processFile(filename)
{
    var result = 0;
    
    try 
    {
        trace("Processing " + filename);
        
        // get this class's class loader
        var oldLoader = setClassLoader(classLoader);
        
        // get all the top-level targets
        var targets = collectTargets(filename);
		
        // run the "default" target
        if (targets.hasOwnProperty("default")) 
        {
            var file = new File(filename);
            
            runTop(file, targets, "default");
        }
        else 
        {
            result = -1;
        }
    } 
    catch (e) 
    {
        error(e.message);
        result = -1;
    }
    finally 
    {
        setClassLoader(oldLoader);
    }
    
    // reset timeout flag to allow another build 
    id = -1;
    
    // return status code
    return result;
}

/**
 * processProjects
 */
function processProjects()
{
    try 
    {
        // get this class's class loader
        var oldLoader = setClassLoader(classLoader);
        
        var plugin = Packages.org.eclipse.core.resources.ResourcesPlugin;
        var workspace = plugin.getWorkspace();
        var projects = workspace.getRoot().getProjects();
        
        for (var i = 0; i < projects.length; i++) 
        {
            var project = projects[i];
            
            // NOTE: project.getFile will always return a non-null object even if the
            // resource does not exist
            var ifile 			= project.getFile(BUILD_FILE);
            var resourceFile 	= ifile.getLocation().toFile();
            var name			= resourceFile.getAbsolutePath();
            var file 			= new File(name);
            
            var iconfigfile 		= project.getFile(CONFIG_FILE);
            var configResourceFile 	= iconfigfile.getLocation().toFile();
            var configName 			= configResourceFile.getAbsolutePath();
            var configFile 			= new File(configName);
			
            if (file.exists) 
            {
                if (configFile.exists) 
                {
                    loadConfig(configFile);
                    
                    // build
                    processFiles([file]);
                    
                    // include configFile as a watch file for all targets in the build file
                    var targets = collectTargets(name);
                    
                    for (var target in targets) 
                    {
                        watchFile(configName, name, target);
                    }
                }
                else 
                {
                    warning("Found " + name + " without a corresponding " + CONFIG_FILE + " file.");
                }
            }
        }
    } 
    catch (e) 
    {
        error("processProjects: " + e.message);
    }
    finally 
    {
        setClassLoader(oldLoader);
    }
}

/**
 * removes individual License Block during concatenation.
 * @param {String} source
 *  The source code to be prepped.
 */
function removeLicenseBlocks(sourceArray){

	var source =[];
	var inLicenseBlock = false;

	for (x=0;x<sourceArray.length;x++){
		if (sourceArray[x].match("BEGIN LICENSE BLOCK")) 
		{
			inLicenseBlock = true;
		}

		if (!inLicenseBlock){
			source.push(sourceArray[x]);
		}

		if (sourceArray[x].match("END LICENSE BLOCK")) 
		{
			inLicenseBlock = false;
		}

	}

	return source;

}

/**
 * runTop
 *
 * @param {File} currentPath
 * @param {Object} targets
 * @param {String} targetName
 */
function runTop(file, targets, targetName)
{
    var data = {
        outputs: [],
        lines: [],
        files: {}
    };
    var currentPath = file.parentFile.absolutePath;
    
    run(file, targets, targetName, data);
    
    if (data.lines.length > 0 && data.outputs.length > 0) 
    {
	
		// read through data.lines to remove individual license blocks
		
        var text = removeLicenseBlocks(data.lines).join("\r\n");
        var outputs = data.outputs;
		var target = targets[targetName];
		var compress = target.hasOwnProperty("compress") && target.compress;
		var compress_prepend = target.hasOwnProperty("compress_prepend") && target.compress_prepend;
        
        for (var i = 0; i < outputs.length; i++) 
        {
            var outputName = currentPath + File.separator + outputs[i];
            var outputFile = new File(outputName);
			
			writeToFile(outputFile, text);
			
			// check if we need a compressed version
			var target = targets[targetName];
			
			if (compress)
			{
				// bit of a hack to calculate compressed file name
				outputName = currentPath + File.separator + outputs[i];
				outputName = outputName.replace(/\.js/, "_compressed.js");
				
				// create compressed text
				var compressedText = pack(text, encoding, fast_decode, special_char);
				
				// create file
            	outputFile = new File(outputName);
				
				// May need to prepend some uncompressed text
				var textToWrite;
				if (compress_prepend)
				{
					var prependName = currentPath + File.separator + compress_prepend;
					prependFile = new File(prependName);
					textToWrite = prependFile.readLines() + compressedText;
				}
				else
				{
					textToWrite = compressedText;
				}
				
				// write compressed text
				writeToFile(outputFile, textToWrite);
			}
        }
        
        var buildFilename = file.absolutePath;
        
        // remove files associated with this build file
        unwatchFiles(buildFilename, targetName);
        
        // add processed files
        for (var filename in data.files) 
        {
            watchFile(filename, buildFilename, targetName);
        }
    }
}

/**
 * Write text to given the filename
 * 
 * @param {File} filename
 * @param {String} text
 */
function writeToFile(outputFile, text)
{
	var outputName = outputFile.absolutePath;
	
	if (outputFile.exists == false) 
    {
        if (outputFile.createNewFile() == false) 
        {
            throw new Error("writeToFile: Unable to create " + outputName);
        }
    }
    
    if (outputFile.exists) 
    {
        if (outputFile.canWrite) 
        {
            trace("Writing to " + outputName);
            
            outputFile.write(text);
            protectedRefresh(outputName);
        }
        else 
        {
            throw new Error("writeToFile: File is read-only: " + outputName);
        }
    }
}

/**
 * Refresh this specified file, if refresh is available
 * 
 * @param {String} file
 */
function protectedRefresh(file)
{
	// NOTE: refresh doesn't exist when this script runs on the build server
	if (typeof refresh != "undefined") 
	{
		refresh(file);
	}
}

/**
 * Delete file. If the file is a directory, then recurse to delete all descendants
 * 
 * @param {java.io.File} file
 */
function recursiveDelete(file)
{
	if (file.exists()) 
	{
		if (file.isDirectory()) 
		{
			var files = file.listFiles();
			
			for (var i = 0; i < files.length; i++) 
			{
				recursiveDelete(files[i]);
			}
		}
		
		// NOTE: we use a property index since "delete" is a keyword
		if (file["delete"]() == false) 
		{
			warning("Unable to delete file: " + file.getAbsolutePath());
		}
	}
}

/**
 * run
 *
 * @param {File} currentPath
 * @param {Object} target
 */
function run(file, targets, targetName, data)
{
    var target = targets[targetName];
    
	// small hack to support tracing of each module as it loads
	if (target.hasOwnProperty("output"))
	{
		if (target.hasOwnProperty("trace"))
		{
			// use the user-defined trace value
			emitLoadingTrace = target.trace;
		}
		else
		{
			// turn off any previous tracing
			emitLoadingTrace = false;
		}
		// use the user-defined buildMode value
		buildByInclude = target.hasOwnProperty("buildMode") && target.buildMode == "include";
	}
	
    for (var p in target) 
    {
        if (propertyHandlers.hasOwnProperty(p)) 
        {
            propertyHandlers[p](file, targets, targetName, data);
        }
        else 
        {
            trace("Unrecognized property: " + p);
        }
    }
}

/**
 * loadConfig
 * 
 * @param {File} configFile
 */
function loadConfig(configFile)
{
    var source = configFile.readLines().join("\n");
    // probably should be a bit more secure here
    eval(source);
    configLoaded = true;
	
	// get the license header contents to be prepended to generated files
	var baseDir 	= new File(location).parentFile.parentFile.absolutePath
	var licenseFile = new File(baseDir+File.separator+LICENSE_HEADER);
	
	LICENSE_TEXT = licenseFile.readLines().join("\n");

}

/**
 * error
 *
 * @param {String} message
 */
function error(message)
{
    trace("ERROR: " + message);
}

/**
 * warning
 *
 * @param {String} message
 */
function warning(message)
{
    trace("WARNING: " + message);
}

/**
 * trace
 *
 * @param {String} message
 */
function trace(message)
{
    function pad(num)
    {
        if (num < 10) 
        {
            return "0" + num;
        }
        else 
        {
            return num + "";
        }
    }
    
    var current = new Date();
    var hours = pad(current.getHours());
    var mins = pad(current.getMinutes());
    var secs = pad(current.getSeconds());
    var timeText = [hours, mins, secs].join(":");
    
    out.println("[" + timeText + "] " + message);
}

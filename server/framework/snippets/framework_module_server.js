/*
	category: Jaxer
	name: Framework Module - Server
	toolTip: Create a new framework module for use on the server side
	prompt(module_name): The name of the module
*/

/**
 * Copyright (c) 2005-2008 Aptana, Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html. If redistributing this code,
 * this entire header must remain intact.
 * 
 * @author <YOUR NAME>
 */

(function(){

var log = Log.forModule("${module_name}"); // Only if Log itself is defined at this point of the includes

/////////////////////////////////////////////////////
// For an instantiable (that is, non-static) class:

function ${module_name}()
{
	// Constructor code here
}

${module_name}.prototype.METHOD_NAME = function METHOD_NAME()
{
	// Method code here
}

/////////////////////////////////////////////////////
// For a static (or singleton) class:

var ${module_name} = {};

${module_name}.METHOD_NAME = function METHOD_NAME()
{
	// Method code here
}

${selection}

/////////////////////////////////////////////////////
// Finally, refer to the globally-accessible objects:

frameworkGlobal.${module_name} = Jaxer.${module_name} = ${module_name};

Log.trace("*** ${module_name}.js loaded");  // Only if Log itself is defined at this point of the includes

})();


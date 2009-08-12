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
 * ***** END LICENSE BLOCK ***** */

(function(){

var log = Log.forModule("Process");

var PROCESS_CLASS_ID = "@aptana.com/utils/jxProcess;1"
var PROCESS_INTERFACE = Components.interfaces.jxIProcess;

/**
 * @classDescription {Jaxer.Process} 
 * 		Used to execute operating system processes
 */

/**
 * The constructor for an object that wraps an operating system process. This also provides
 * static functions for more easily executing operating system processes.
 * 
 * @constructor
 * @alias Jaxer.Process
 * @param {String} [path]
 * 		The absolute path to the executable file to execute. This must be specified 
 * 		before execution begins.
 * @param {Boolean} [async]
 * 		By default, the process executes synchronously: it blocks until complete.
 * 		If async is true, it will execute asynchronously, not blocking before completion.
 * 		This can be specified at any time until execution begins.
 * @return {Jaxer.Process}
 * 		The wrapper around the operating system process
 */
function Process(path, async)
{
	/**
	 * The absolute path to the executable file to execute
	 * @alias Jaxer.Process.prototype.path
	 * @property {String}
	 */
	this.path = path;

	/**
	 * If true, the process will be executed asynchronously (without blocking)
	 * @alias Jaxer.Process.prototype.async
	 * @property {Boolean}
	 */
	this.async = async;

	/**
	 * The array of arguments to this process.
	 * @alias Jaxer.Process.prototype.args
	 * @property {Array}
	 */
	this.args = null;

	/**
	 * The internal process handle
	 * @advanced
	 * @alias Jaxer.Process.prototype._proc
	 * @property {Object}
	 */
	this._proc = Components.classes[PROCESS_CLASS_ID].createInstance(PROCESS_INTERFACE);

	/**
	 * How to handle the standard input stream (STDIN) to the process. 
	 * If this is a string, it is written to STDIN as a character stream;
	 * if this is an array, it is written to STDIN as a binary (byte array) stream;
	 * otherwise no writing to STDIN is performed.
	 * Note that this property is only used by methods like exec and execAsync;
	 * you can always just explicitly write to STDIN using writeString or writeBinary.
	 * @alias Jaxer.Process.prototype.stdin
	 * @property {String, Number[]}
	 */
	this.stdin = null;

	/**
	 * The size in bytes of the buffer to use when reading from STDOUT and STDERR.
	 * By default this is 1024 bytes.
	 * @advanced
	 * @alias Jaxer.Process.prototype.readBufferSize
	 * @property {Number}
	 */
	this.readBufferSize = 1024;

	/**
	 * How to handle the standard output stream (STDOUT) from the process. 
	 * If this is a string, it read from STDOUT as a character stream;
	 * if this is an array, it is read from STDOUT as a binary (byte array) stream;
	 * otherwise no reading from STDOUT is performed.
	 * Note that this property is only used by methods like exec and execAsync;
	 * you can always just explicitly read from STDOUT using readString or readBinary.
	 * @alias Jaxer.Process.prototype.stdout
	 * @property {String, Number[]}
	 */
	this.stdout = null;

	/**
	 * How to handle the standard error stream (STDERR) from the process. 
	 * If this is a string, it read from STDERR as a character stream;
	 * otherwise no reading from STDERR is performed.
	 * Note that this property is only used by methods like exec and execAsync;
	 * you can always just explicitly read from STDERR using readErrString.
	 * @alias Jaxer.Process.prototype.stderr
	 * @property {String}
	 */
	this.stderr = null;

//	this.timeout = null;

	/**
	 * Whether to detach the process (if it's asynchronous) after any writing to STDIN is complete. 
	 * This is true by default
	 * Note that this property is only used by methods like exec and execAsync;
	 * you can always just explicitly detach by calling endExec.
	 * @alias Jaxer.Process.prototype.autoDetachIfAsync
	 * @property {String}
	 */
	this.autoDetachIfAsync = true;

	/**
	 * The internal status (state) of the process: one of Jaxer.Process.CREATED,
	 * Jaxer.Process.EXEC_BEGUN, Jaxer.Process.WRITE_ENED, and Jaxer.Process.EXEC_ENDED.
	 * @advanced
	 * @alias Jaxer.Process.prototype._status
	 * @property {Number}
	 */
	this._status = 0;

	/**
	 * The exit status (i.e. return code) of the process, if any.
	 * For an async process this is null.
	 * @alias Jaxer.Process.prototype.exitStatus
	 * @property {Number}
	 */
	this.exitStatus = null;
}

/**
 * The internal status indicating the process wrapper has been created but no execution has begun
 * @alias Jaxer.Process.CREATED
 * @property {Number}
 */
Process.CREATED = 0;
/**
 * The internal status indicating the process has begun executing, but any writing to STDIN is not necessarily finished
 * @alias Jaxer.Process.EXEC_BEGUN
 * @property {Number}
 */
Process.EXEC_BEGUN = 1;
/**
 * The internal status indicating the process has begun executing and any writing to STDIN is finished
 * @alias Jaxer.Process.WRITE_ENDED
 * @property {Number}
 */
Process.WRITE_ENDED = 2;
/**
 * The internal status indicating the process has finished executing or has been detached (if async)
 * @alias Jaxer.Process.EXEC_ENDED
 * @property {Number}
 */
Process.EXEC_ENDED = 3;

/**
 * Begins execution of the process. Any arguments to this method are treated as
 * arguments to the process; if no arguments are given, and this.args has been
 * set, it will be used instead.
 * You can start to write to STDIN and read from STDOUT and STDERR after you call this method.
 * 
 * @alias Jaxer.Process.prototype.beginExec
 */
Process.prototype.beginExec = function beginExec()
{
	
	// Verify path is reasonable:
	var file = new File(this.path);
	if (!file.exists)
	{
		throw new Exception("The path '" + this.path + "' to the process was not found (make sure an absolute path is given)");
	}
	if (!file.isFile)
	{
		throw new Exception("The path '" + this.path + "' to the process is not a file");
	}
	if (!file.isExec)
	{
		throw new Exception("The path '" + this.path + "' to the process is not an executable file");
	}
	
	if (this._status > Process.CREATED) throw new Exception("Cannot begin executing again a process that has previously begun executing");
	if ((this.args == null) || (arguments.length > 0)) this.args = Array.slice(arguments);
	log.trace("Starting execution of " + this.path + " with arguments: " + this.args);
	var argv = [this.path].concat(this.args);
	this._proc.run(!this.async, argv, argv.length);
	this._status = Process.EXEC_BEGUN;
	this.exitStatus = null;
}

/**
 * Writes the given string as a character stream to the process's STDIN.
 * 
 * @alias Jaxer.Process.prototype.writeString
 * @param {String} str
 * 		The string to write
 */
Process.prototype.writeString = function writeString(str)
{
	if (this._status < Process.EXEC_BEGUN) throw new Exception("Cannot write to the STDIN of a process that has not begun executing");
	if (this._status >= Process.WRITE_ENDED) throw new Exception("Cannot write to the STDIN of a process that has ended its write phase (e.g. because readString() was called)");
	log.trace("Writing a string of length " + str.length + " to STDIN of " + this.path);
	this._proc.writeString(str);
}

/**
 * Writes the given byte array as a binary stream to the process's STDIN
 * 
 * @alias Jaxer.Process.prototype.writeBinary
 * @param {Number[]} data
 * 		The byte array (array of integers)
 */
Process.prototype.writeBinary = function writeBinary(data)
{
	if (this._status < Process.EXEC_BEGUN) throw new Exception("Cannot end the writing phase of a process that has not begun executing");
	if (this._status >= Process.WRITE_ENDED) throw new Exception("Cannot write to the STDIN of a process that has ended its write phase (e.g. because readString() was called)");
	log.trace("Writing a byte array of length " + data.length + " to STDIN of " + this.path);
	this._proc.writeBytes(data, data.length);
}

/**
 * Closes STDIN for further writing, which may be needed by the process before it can proceed.
 * This is automatically called by reading anything from STDOUT and STDERR and by endExec.
 * 
 * @alias Jaxer.Process.prototype.endWrite
 */
Process.prototype.endWrite = function endWrite()
{
	if (this._status < Process.EXEC_BEGUN) throw new Exception("Cannot write to the STDIN of a process that has not begun executing");
	if (this._status >= Process.WRITE_ENDED) return; // already ended writing so nothing to do
	log.trace("Closing STDIN of " + this.path);
	this._proc.endStdin();
	this._status = Process.WRITE_ENDED;
}

/**
 * Reads the process's STDOUT stream as a character string.
 * 
 * @alias Jaxer.Process.prototype.readString
 * @param {Number} [maxLength]
 * 		If specified, limits reading of STDOUT to maxLength characters
 * @return {String}
 * 		The string value of STDOUT
 */
Process.prototype.readString = function readString(maxLength)
{
	if (this._status < Process.EXEC_BEGUN) throw new Exception("Cannot read from the STDOUT of a process that has not begun executing");
	if (this._status >= Process.EXEC_ENDED) throw new Exception("Cannot read from the STDOUT of a process whose execution has ended or that has been detached");
	this.endWrite();
	var hasMaxLength = Util.Math.isInteger(maxLength);
	var stdout = this._proc.readStdoutString();
	var output = '';
	while (stdout.length > 0)
	{
		if (!hasMaxLength || (output.length < maxLength))
		{
			output += stdout;
		}
		stdout = this._proc.readStdoutString(); // keep reading even if past maxLength, to flush out buffer
	}
	if (hasMaxLength)
	{
		output = output.substr(0, maxLength);
	}
	log.trace("Returning a string of length " + output.length + " from STDOUT of " + this.path);
	return output;
}

/**
 * Reads the process's STDOUT stream as a byte array.
 * 
 * @alias Jaxer.Process.prototype.readBinary
 * @param {Number} [maxLength]
 * 		If specified, limits reading of STDOUT to maxLength bytes
 * @return {Number[]}
 * 		The binary value of STDOUT as an array of integers
 */
Process.prototype.readBinary = function readBinary(maxLength)
{
	if (this._status < Process.EXEC_BEGUN) throw new Exception("Cannot read from the STDOUT of a process that has not begun executing");
	if (this._status >= Process.EXEC_ENDED) throw new Exception("Cannot read from the STDOUT of a process whose execution has ended or that has been detached");
	this.endWrite();
	var hasMaxLength = Util.Math.isInteger(maxLength);
	var stdoutDataHolder = {};
	this._proc.readStdoutBytes(stdoutDataHolder, {});
	var stdout = stdoutDataHolder.value;
	var output = [];
	while (stdout.length > 0)
	{
		if (!hasMaxLength || (output.length < maxLength))
		{
			output.push.apply(output, stdout);
		}
		this._proc.readStdoutBytes(stdoutDataHolder, {}); // keep reading even if past maxLength, to flush out buffer
		stdout = stdoutDataHolder.value;
	}
	if (hasMaxLength && (output.length > maxLength))
	{
		output.splice(maxLength, output.length - maxLength);
	}
	log.trace("Returning a byte array of length " + output.length + " from STDOUT of " + this.path);
	return output;
}

/**
 * Reads the process's STDERR stream as a character string.
 * 
 * @alias Jaxer.Process.prototype.readErrString
 * @param {Number} [maxLength]
 * 		If specified, limits reading of STDERR to maxLength characters
 * @return {String}
 * 		The string value of STDERR
 */
Process.prototype.readErrString = function readErrString(maxLength)
{
	if (this._status < Process.EXEC_BEGUN) throw new Exception("Cannot read from the STDERR of a process that has not begun executing");
	if (this._status >= Process.EXEC_ENDED) throw new Exception("Cannot read from the STDERR of a process whose execution has ended or that has been detached");
	this.endWrite();
	var hasMaxLength = Util.Math.isInteger(maxLength);
	var stderr = this._proc.readStderrString();
	var output = '';
	while (stderr.length > 0)
	{
		if (!hasMaxLength || (output.length < maxLength))
		{
			output += stderr;
		}
		stderr = this._proc.readStderrString(); // keep reading even if past maxLength, to flush out buffer
	}
	if (hasMaxLength)
	{
		output = output.substr(0, maxLength);
	}
	log.trace("Returning a string of length " + output.length + " from STDERR of " + this.path);
	return output;
}

/**
 * Completes the execution of the process (if synchronous) or detaches it (if asynchronous).
 * A running process may be stopped via kill() anytime before endExec() is called.
 * 
 * @alias Jaxer.Process.prototype.endExec
 * @return {Number}
 * 		If the process is synchronous, its exit status (i.e. return code) is returned.
 */
Process.prototype.endExec = function endExec()
{
	if (this._status < Process.EXEC_BEGUN) throw new Exception("Cannot finish execution of a process that has not begun executing");
	if (this._status >= Process.EXEC_ENDED) return; // already ended -- nothing to do
	if (this._status < Process.WRITE_ENDED) 
	{
		this.endWrite();
	}
	log.trace("Ending the execution of (or detaching) " + this.path);
	this._proc.wait();
	this.exitStatus = this.async ? null : this._proc.exitValue;
	log.trace("Ended the execution of (or detached) " + this.path + " with exitStatus: " + this.exitStatus);
	this._status = Process.EXEC_ENDED;
	return this.exitStatus;
}

/**
 * Kills a running process. This can only be called before endExec() has been called.
 * 
 * @alias Jaxer.Process.prototype.kill
 */
Process.prototype.kill = function kill()
{
	if (this._status < Process.EXEC_BEGUN) throw new Exception("Cannot kill a process that has not begun executing");
	if (this._status >= Process.EXEC_ENDED) return; // the process is finished or detached when endExec is called so there's nothing to kill
	log.trace("Killing process " + this.path);
	this._proc.kill();
	log.trace("Killed process " + this.path);
	this.exitStatus = null;
	this._status = Process.EXEC_ENDED;
}

/**
 * Executes the process. The path to the executable file should already have been
 * set when creating the Process or via the path property. The arguments to this
 * method are used as the arguments to the process.
 * By default, the process is executed synchronously (i.e. it blocks), 
 * and nothing is written to STDIN nor read from STDOUT or STDERR. 
 * To override these defaults, set any of the following properties before calling this: 
 * async, stdin, stdout, stderr, and autoDetachIfAsync.
 * If any of these are set they will be used; for stdout and stderr they will
 * be used only if the process is synchronous, in which case they'll be set to STDOUT/STDERR.  
 * If the process is asycnhronous and autoDetachIfAsync is false, it will not be detached
 * until it falls out of scope, so you can still read STDOUT or STDERR and kill it until
 * it falls out of scope, at which point it will be detached; 
 * otherwise it will be detached immediately (or after any STDIN is written).
 * 
 * @alias Jaxer.Process.prototype.exec
 * @return {Number}
 * 		If the process is synchronous, its exit status (i.e. return code) is returned.
 */
Process.prototype.exec = function exec()
{
	this.beginExec.apply(this, arguments);
	if (typeof this.stdin == "string")
	{
		this.writeString(this.stdin);
	}
	else if (this.stdin && (this.stdin.constructor == this.stdin.__parent__.Array))
	{
		this.writeBinary(this.stdin);
	}
	this.endWrite();
	if (!this.async) // auto read from STDOUT/STDERR for blocking process
	{
		if (typeof this.stdout == "string") 
		{
			this.stdout = this.readString();
		}
		else if (this.stdout && (this.stdout.constructor == this.stdout.__parent__.Array)) 
		{
			this.stdout = this.readBinary();
		}
		if (typeof this.stderr == "string") 
		{
			this.stderr = this.readErrString();
		}
	}
	if (!this.async || this.autoDetachIfAsync) // finish execution or detach
	{
		this.endExec();
	}
	if (!this.async)
	{
		return this.exitStatus;
	}
}

/**
 * Executes the process asynchronously (i.e. without blocking). 
 * The path to the executable file should already have been
 * set when creating the Process or via the path property. The arguments to this
 * method are used as the arguments to the process.
 * By default, nothing is written to STDIN, and the process
 * is immediately detached after it is launched. 
 * To override these defaults, set stdin and/or autoDetachIfAsync on the process
 * before calling this.
 * If stdin is set, it will be used. 
 * If autoDetachIfAsync is not set (or set to the default value of true), 
 * the process will be detached immediately after any STDIN is written.
 * If autoDetachIfAsync is set to false, the process will not be detached,
 * so you can still read STDOUT or STDERR and kill it until
 * it falls out of scope, at which point it will be detached.
 * 
 * @alias Jaxer.Process.prototype.execAsync
 */
Process.prototype.execAsync = function execAsync()
{
	this.async = true;
	this.exec.apply(this, arguments);
}

////////// STATIC METHODS ////////////

/**
 * Executes the process specified by the given absolute path. 
 * Any remaining arguments to this function are used as the arguments to the process, 
 * except possibly for the last argument if it is an object, 
 * in which case it's removed and used to set options.
 * By default, the process is executed synchronously (i.e. it blocks), 
 * and nothing is written to STDIN. 
 * To override these defaults, pass as a final argument an object containing 
 * the properties to be overridden: async, stdin, stdout, stderr, autoDetachIfAsync.
 * If any of these are set they will be used; for stdout and stderr they will
 * be used only if the process is synchronous, in which case 
 * their values in your object will be set to STDOUT/STDERR. 
 * For a synchronous process, the return value will be the STDOUT of the process,
 * unless you specify stdout in your options object, in which case the return value
 * is the exitStatus; and unless
 * you specify stderr, an error will be thrown if the process's exitStatus is non-zero
 * or if it writes to STDERR.
 * If the process is executed asynchronously and autoDetachIfAsync is false, 
 * it will not be detached until it falls out of scope; 
 * otherwise it will be detached immediately (or after any STDIN is written).
 * 
 * @alias Jaxer.Process.exec
 * @param {String} path
 * 		The absolute path of the executable file to execute
 * @return {String}
 * 		For a synchronous process (without having a stdout property in an options
 * 		object), returns the STDOUT of the process; otherwise does not return anything.
 * @example
 *		<pre>
 *			document.write("The folder contains:\n" + Jaxer.Process.exec("/bin/ls"));
 *
 *			var opts = {stdin: 'Hello world', stdout: '', stderr: ''};
 *			var exitStatus = Jaxer.Process.exec("/bin/cat", "-", opts);
 *			document.write("Finished /bin/cat with status " + exitStatus + ": opts = " + uneval(opts));
 * 		</pre>
 */
Process.exec = function exec(path)
{
	var proc = new Process(path);
	var options = preProcessOptions(proc, Array.slice(arguments, 1)); 
	var procArgs = options.procArgs;
	var userOpts = options.userOpts;
	var exitStatus = proc.exec.apply(proc, procArgs);
	if (!proc.async)
	{
		if ('stdout' in userOpts) 
		{
			userOpts.stdout = proc.stdout;
		}
		if ('stderr' in userOpts)
		{
			userOpts.stderr = proc.stderr;
		}
		else if (exitStatus || proc.stderr)
		{
			var err = new Exception("Process " + proc.path + " returned an exit status of " + exitStatus + (proc.stderr ? (" and STDERR of '" + proc.stderr + "'") : " and no STDERR"));
			err.exitStatus = exitStatus;
			err.stderr = proc.stderr;
			throw err;
		}
		if ('stdout' in userOpts)
		{
			return exitStatus;
		} 
		else
		{ 
			return proc.stdout;
		}
	}
}

/**
 * Asynchronously executes the process specified by the given absolute path (so it does not block). 
 * Any remaining arguments to this function are used as the arguments to the process, 
 * except possibly for the last argument if it is an object, 
 * in which case it's removed and used to set options.
 * By default, nothing is written to STDIN nor read from STDOUT or STDERR,
 * and the process is immediately detached after it is launched. 
 * To override these defaults, pass as a final argument an object containing 
 * the properties to be overridden: stdin and/or autoDetachIfAsync.
 * If stdin is set, it will be used. 
 * If autoDetachIfAsync is not set (or set to the default value of true), 
 * the process will be detached immediately after any STDIN is written.
 * If autoDetachIfAsync is set to false, the process will not be detached until
 * it falls out of scope, at which point it will be detached.
 * 
 * @alias Jaxer.Process.execAsync
 * @param {String} path
 * 		The absolute path of the executable file to execute
 * @example
 *		<pre>
 *			Jaxer.Process.execAsync("/bin/sleep", 1, {autoDetachIfAsync: false});
 *			var opts = {stdout: ''};
 *			Jaxer.Process.exec('/bin/ps', '-ax', opts);
 *			print("Sleep is running? " + (Jaxer.Util.String.grep(opts.stdout, "/bin/sleep").length > 0));
 *			Jaxer.Util.sleep(2000);
 *			Jaxer.Process.exec('/bin/ps', '-ax', opts);
 *			print("Sleep is running? " + (Jaxer.Util.String.grep(opts.stdout, "/bin/sleep").length > 0));
 * 		</pre>
 */
Process.execAsync = function execAsync(path)
{
	var proc = new Process(path, true);
	var options = preProcessOptions(proc, Array.slice(arguments, 1)); 
	var procArgs = options.procArgs;
	var userOpts = options.userOpts;
	proc.execAsync.apply(proc, procArgs);
}

function preProcessOptions(proc, args)
{
	var userOpts = {};
	if (args && args.length)
	{
		var candidateOpts = args[args.length - 1]; // the last argument is the options -- if it's of type "object"
		if (candidateOpts && (typeof candidateOpts == "object"))
		{
			userOpts = candidateOpts;
			args = args.slice(0, args.length - 1);
		}
	}
	var procOpts = (proc.async || userOpts.async) ? userOpts : Util.extend({stdout: '', stderr: ''}, userOpts);
	log.trace("Applying options: " + uneval(procOpts));
	['async', 'stdin', 'stdout', 'stderr', 'path', 'autoDetachIfAsync' /*, 'timeout'*/].forEach(function(name) 
	{ 
		if (name in procOpts) proc[name] = procOpts[name]; 
	});
	return {
		procArgs: args,
		userOpts: userOpts
	};
}

Jaxer.Process = Process;

Log.trace("*** Process.js loaded");

})();


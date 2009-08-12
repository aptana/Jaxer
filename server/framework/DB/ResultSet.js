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

var log = Log.forModule("ResultSet"); // Only if Log itself is defined at this point of the includes

/**
 * @classDescription {Jaxer.DB.ResultSet} Contains the returned value of a SQL
 * query.
 */

/**
 * Returned value of a SQL query, containing the named rows and columns of the
 * result and to be used as the return value of execute(). 
 * The rowsAsArray array is similar to the rows array but each row in it
 * is an array itself: it consists of the cells in that row, in
 * the same order as the columns array. 
 * 
 * To see if there is any data, check whether hasData is true or whether
 * rows.length or rowsAsArrays.length exceed 0. To get a single result (the
 * first column of the first row) grab singleResult.
 * 
 * @example
 * If the third column is named "account" you can get to a given cell value
 * either through
 * resultSet.rows[10].account or through
 * resultSet.rowsAsArrays[10][2]
 * 
 * @constructor
 * @alias Jaxer.DB.ResultSet
 * @return {Jaxer.DB.ResultSet}
 * 		Returns an instance of ResultSet.
 */
var ResultSet = function ResultSet()
{
	/**
	 * The array of rows in the resultSet in the order retrieved from the database.
	 * Each row has properties corresponding to the column names in the returned data.
	 * @alias Jaxer.DB.ResultSet.prototype.rows
	 * @property {Array}
	 */
	this.rows = [];
	
	/**
	 * An alternate representation of the rows of the resultSet:
	 * each row is itself an array, containing the values (cells) in that row
	 * in the same order as the columns array.
	 * @alias Jaxer.DB.ResultSet.prototype.rowsAsArrays
	 * @property {Array}
	 */
	this.rowsAsArrays = [];
	
	/**
	 * An array of column names for all rows in this resultSet.
	 * TODO - Note that (depending on the DB "driver" implementation used)
	 * the columns array may be empty if there are no rows of data;
	 * this will be fixed in the future.
	 * @alias Jaxer.DB.ResultSet.prototype.columns
	 * @property {Array}
	 */
	this.columns = [];
	
	/**
	 * This contains the first value (cell) in the first row
	 * in the resultSet, if any, or else it contains null.
	 * (You can disambiguate DB nulls from no data by checking the
	 * hasData property or rows.length). This is convenient
	 * for queries (e.g. "SELECT COUNT(*) FROM mytable") that
	 * are known to return a single value.
	 * @alias Jaxer.DB.ResultSet.prototype.singleResult
	 * @property {Object}
	 */
	this.singleResult = null;
	
	/**
	 * True if this resultSet contains any data, false otherwise.
	 * @alias Jaxer.DB.ResultSet.prototype.hasData
	 * @property {Boolean}
	 */
	this.hasData = false;
	
	var currentRowIndex = 0;
	
	/**
	 * True if this resultSet's cursor is positioned on a valid row,
	 * which means you can access the row's data.
	 * @alias Jaxer.DB.ResultSet.prototype.isValidRow
	 * @return {Boolean}
	 * 		true if positioned on a valid row, false otherwise
	 */
	this.isValidRow = function isValidRow()
	{
		return currentRowIndex < this.rows.length;
	}
	
	/**
	 * Advances this resultSet's cursor to the next row of results.
	 * Use isValidRow() to determine if you've reached the end 
	 * of the rows.
	 * @alias Jaxer.DB.ResultSet.prototype.next
	 */
	this.next = function next()
	{
		currentRowIndex++;
	}
	
	/**
	 * Closes this resultSet and empties its internal data structures.
	 * This is optional.
	 * @alias Jaxer.DB.ResultSet.prototype.close
	 */
	this.close = function close()
	{
		this.rows = [];
		this.rowsAsArrays = [];
		this.columns = [];
		this.singleResult = null;
		this.hasData = false;
	}
	
	/**
	 * Returns the number of fields (columns) in this resultSet,
	 * the same as this.columns.length.
	 * @alias Jaxer.DB.ResultSet.prototype.fieldCount
	 * @return {Number}
	 * 		The number of fields (columns)
	 */
	this.fieldCount = function fieldCount()
	{
		return this.columns.length;
	}
	
	/**
	 * Returns the name of the column at the given (0-based) index,
	 * the same as this.columns[fieldIndex].
	 * Throws a Jaxer.Exception if fieldIndex is out of range.
	 * @alias Jaxer.DB.ResultSet.prototype.fieldName
	 * @param {Number} fieldIndex
	 * 		The 0-based index of the desired field (column),
	 * 		bounded by this.columns.length.
	 * @return {String}
	 * 		The name of the field (column)
	 */
	this.fieldName = function fieldName(fieldIndex)
	{
		if (fieldIndex < 0 || (fieldIndex >= this.columns.length))
		{
			throw new Exception("fieldIndex out of range: " + fieldIndex + " is not between 0 and " + this.columns.length);
		}
		return this.columns[fieldIndex];
	}

	/**
	 * For the current row pointed to by the cursor,
	 * returns the value of the field (column) at the given (0-based) index,
	 * the same as this.rowsAsArrays[currentRowIndex][fieldIndex]
	 * Throws a Jaxer.Exception if fieldIndex is out of range.
	 * @alias Jaxer.DB.ResultSet.prototype.field
	 * @param {Number} fieldIndex
	 * 		The 0-based index of the desired field (column),
	 * 		bounded by this.columns.length.
	 * @return {Object}
	 * 		The value of the field (column)
	 */
	this.field = function field(fieldIndex)
	{
		if (fieldIndex < 0 || (fieldIndex >= this.columns.length))
		{
			throw new Exception("fieldIndex out of range: " + fieldIndex + " is not between 0 and " + this.columns.length);
		}
		var row = this.rowsAsArrays[currentRowIndex];
		return row[fieldIndex];
	}

	/**
	 * For the current row pointed to by the cursor,
	 * returns the value of the field (column) at the given fieldName (column name),
	 * the same as this.rows[currentRowIndex][fieldName]
	 * Throws a Jaxer.Exception if fieldName is not the name of a field (column).
	 * @alias Jaxer.DB.ResultSet.prototype.fieldByName
	 * @param {String} fieldByName
	 * 		The name of the desired field (column), which should be an
	 * 		element of the Array this.columns.
	 * @return {Object}
	 * 		The value of the field (column)
	 */
	this.fieldByName = function fieldByName(fieldName)
	{
		var row = this.rows[currentRowIndex];
		if (!row.hasOwnProperty(fieldName))
		{
			throw new Exception("No such fieldName in row: '" + fieldName + "'");
		}
		return row[fieldName];
	}

};

/**
 * Adds a row, where its structure is assumed to be the same as
 * all the other rows.
 * 
 * @alias Jaxer.DB.ResultSet.prototype.addRow
 * @param {Object} row
 * 		The row to add 
 */
ResultSet.prototype.addRow = function addRow(row)
{
	this.rows.push(row);
	var values = this.columns.map(function(col) { return row[col]; });
	this.rowsAsArrays.push(values);
	if (!this.hasData) 
	{
		this.hasData = true;
		this.singleResult = row[this.columns[0]];
	}
};

/**
 * Returns the index of the column with the given name
 * 
 * @alias Jaxer.DB.ResultSet.prototype.indexOfColumn
 * @param {String} columnName
 * 		The textual name of the database column 
 * @return {Number}
 * 		The 0-based index in the columns array (and in each row in the rows
 * 		array)
 */
ResultSet.prototype.indexOfColumn = function indexOfColumn(columnName)
{
	return this.columns.indexOf(columnName);
};

/**
 * Returns an array of objects, one per row, that only have properties
 * corresponding to the given columns.
 * 
 * @alias Jaxer.DB.ResultSet.prototype.extractColumns
 * @param {Array} columns
 * 		The names of the columns that should be made available for each row.
 * @return {Array}
 * 		An array of simple objects, each with the requested properties.
 */
ResultSet.prototype.extractColumns = function extractColumns(columns)
{
	var result = [];
	if (columns == "*") columns = this.columns;
	for (var i=0; i<this.rows.length; i++)
	{
		var obj = {};
		for (var ip=0; ip<columns.length; ip++)
		{
			var column = columns[ip];
			obj[column] = this.rows[i][column];
		}
		result.push(obj);
	}
	return result;
};

/**
 * Returns a string representation of the resultset object
 * 
 * @alias Jaxer.DB.ResultSet.prototype.toString
 * @return {String}
 * 		A string representation of the resultset object
 */
ResultSet.prototype.toString = function toString()
{
	if (!this.hasData) return "(empty)";
	var columnSeparator = " | ";
	var lineSeparator = "\n";
	var lines = [];
	lines.push("Columns: " + this.columns.join(columnSeparator));
	for (var i=0; i<this.rowsAsArrays.length; i++)
	{
		lines.push("Row " + i + ": " + this.rowsAsArrays[i].join(columnSeparator));
	}
	return lines.join(lineSeparator);
};

/**
 * Returns a HTML table snippet containing the resultset items
 * 
 * @alias Jaxer.DB.ResultSet.prototype.toHTML
 * @param {Object} [tableAttributes]
 * 		An optional hashmap which will be turned into attribute-value pairs on the
 * 		table tag
 * @param {Object} [headingAttributes]
 * 		An optional hashmap which will be turned into attribute-value pairs on the
 * 		thead tag
 * @param {Object} [bodyAttributes]
 * 		An optional hashmap which will be turned into attribute-value pairs on the 
 * 		tbody tag
 * @return {String}
 * 		HTML table snippet containing the resultset items
 */
ResultSet.prototype.toHTML = function toHTML(tableAttributes, headingAttributes, bodyAttributes)
{
	var lines = [];
	lines.push("<table " + Util.DOM.hashToAttributesString(tableAttributes) + ">");
	lines.push("\t<thead " + Util.DOM.hashToAttributesString(headingAttributes) + ">");
	lines.push("\t\t<tr>");
	for (var i=0; i<this.columns.length; i++)
	{
		lines.push("\t\t\t<td>" + this.columns[i] + "</td>");
	}
	lines.push("\t\t</tr>");
	lines.push("\t</thead>");
	if (this.hasData)
	{
		lines.push("\t<tbody " + Util.DOM.hashToAttributesString(bodyAttributes) + ">");
		for (var i=0; i<this.rowsAsArrays.length; i++)
		{
			var values = this.rowsAsArrays[i];
			lines.push("\t\t<tr>");
			for (var j=0; j<values.length; j++)
			{
				lines.push("\t\t\t<td>" + values[j] + "</td>");
			}
			lines.push("\t\t</tr>");
		}
		lines.push("\t</tbody>");
	}
	lines.push("</table>");
	return lines.join("\n");
};

/**
 * This contains the data in the first row of the resultSet as an object,
 * or else it contains an empty object.
 * 
 * (You can disambiguate DB nulls from no data by checking the
 * hasData property or rows.length). This is convenient
 * for queries (e.g. "SELECT * FROM mytable where key = 'value'") that
 * are known to return a single value.
 *
 * @alias Jaxer.DB.ResultSet.prototype.singleRow
 * @property {Array}
 *         A javascript array containing the  column values of first row from the current resultset
 */
ResultSet.prototype.__defineGetter__('singleRow', function singleRow()
{
    return (this.rows.length>0) ? this.rows[0] : {} ;
//    return (this.rows.length>0) ? this.rowsAsArrays[0] : [] ;
});

/**
 * An alias of the Jaxer.DB.ResultSet.prototype.singleRow method
 *
 * @alias Jaxer.DB.ResultSet.prototype.firstRow
 * @see Jaxer.DB.ResultSet.prototype.singleRow
 * @property {Array}
 *         A javascript array containing the  column values of first row from the current resultset
 */
ResultSet.prototype.__defineGetter__('firstRow', ResultSet.prototype.__lookupGetter__('singleRow'));

/**
 * This returns the data in the last row of the resultSet as an object,
 * or else it contains an empty object.
 * 
 * (You can disambiguate DB nulls from no data by checking the
 * hasData property or rows.length).
 *
 * @alias Jaxer.DB.ResultSet.prototype.finalRow
 * @property {Array}
 *         A javascript array containing the column values of last row from the current resultset
 */
ResultSet.prototype.__defineGetter__('finalRow', function finalRow()
{
    return (this.hasData) ? this.rows[this.rows.length - 1] : {} ;
//    return (this.hasData) ? this.rowsAsArrays[this.rows.length - 1] : [] ;
});

/**
 * This returns the data in each of the first columns of the resultSet as an array,
 * or else it contains an empty array.
 *
 * (You can disambiguate DB nulls from no data by checking the
 * hasData property or rows.length). This is convenient
 * for queries that are used to populate dropdowns.
 *
 * @alias Jaxer.DB.ResultSet.prototype.singleColumn
 * @property {Array}
 *         A javascript array containing each of the first column values from the current resultset
 */
ResultSet.prototype.__defineGetter__('singleColumn', function singleColumn()
{
    return this.columnsAsArray(0);
});

/**
 * This return the data in the provided columns of the resultSet as an array,
 * or else it contains null.
 *
 * @alias Jaxer.DB.ResultSet.prototype.columnsAsArray
 * @method
 * @param {Number|String}
 *         identifies the columns to return, either as an array index value,
 *         or a column label value, you can specify 1 or more columns to be returned
 * @return {Array}
 *         A javascript array of arrays containg each of the values of the requested column from the current resultset
 */
ResultSet.prototype.columnsAsArray = function columnsAsArray()
{
    var cols = Array.slice(arguments);
	
	if (cols == "*") 
	{
		return this.rowsAsArrays;
	}
	
    for (var col in cols)
    {
        if (typeof cols[col] != 'number')
        {
            cols[col] = this.columns.indexOf(cols[col]);
        }
    }

	var results = [];	
	for (var i=0; i<this.rowsAsArrays.length; i++)
	{
		var row = [];
		for (var col in cols)
		{
			row.push(this.rowsAsArrays[i][cols[col]]);
		}
		results.push(row);
	}
	
	// return flattened array for single column results
	return ( (results.length) > 1 && (results[0].length) > 1) ? results : results.reduce(function(a,b) {  return a.concat(b);}, []);

};

/**
 * This contains the requested columns from the first row of the resultSet as an array,
 * or else it contains an empty array.
 *
 * @alias Jaxer.DB.ResultSet.prototype.singleRowAsArray
 * @method
 * @param {Number|String}
 *         identifies the columns to return, either as an array index value,
 *         or a column label value, you can specify 1 or more columns to be returned
 * @return {Array}
 *         A javascript array the values of the requested columns from the first row of the current resultset
 */
ResultSet.prototype.singleRowAsArray = function singleRowAsArray()
{
    var cols = Array.slice(arguments);
    for (var col in cols)
    {
    	if (typeof cols[col] != 'number')
        {
            cols[col] = this.columns.indexOf(cols[col]);
        }
    }
    var rows = this.rowsAsArrays;

    return  (this.hasData) ? cols.map(function(col) { return rows[0][col] }) : [] ;
};

/**
 * An alias of the Jaxer.DB.ResultSet.prototype.singleRowAsArray method
 *
 * @alias Jaxer.DB.ResultSet.prototype.singleRowAsArray
 * @method
 * @see Jaxer.DB.ResultSet.prototype.singleRowAsArray
 */
ResultSet.prototype.firstRowAsArray = ResultSet.prototype.singleRowAsArray;

/**
 * This contains the requested columns from the last row of the resultSet as an array,
 * or else it contains an empty array.
 *
 * @alias Jaxer.DB.ResultSet.prototype.finalRowAsArray
 * @method
 * @param {Number|String}
 *         identifies the columns to return, either as an array index value,
 *         or a column label value, you can specify 1 or more columns to be returned
 * @return {Array}
 *         A javascript array the values of the requested columns from the last row of the current resultset
 */
ResultSet.prototype.finalRowAsArray = function finalRowAsArray()
{
    var cols = Array.slice(arguments);
    for (var col in cols)
    {
        if (typeof cols[col] != 'number')
        {
            cols[col] = this.columns.indexOf(cols[col]);
        }
    }
    var rows = this.rowsAsArrays;

    return cols.map(function(col) /*! +this !*/ 
	{
        return (this.hasData0) ? rows[rows.length - 1][col] : [] ;
    });
};

/**
 * An alias of the Jaxer.DB.ResultSet.prototype.finalRowAsArray method
 *
 * @alias Jaxer.DB.ResultSet.prototype.lastRowAsArray
 * @method
 * @see Jaxer.DB.ResultSet.prototype.finalRowAsArray
 */
ResultSet.prototype.lastRowAsArray = ResultSet.prototype.finalRowAsArray;

/**
 * indexOf compares the value of the requested column in each row of the results using strict
 * equality (the same method used by the ===, or triple-equals, operator). it returns either the
 * index of the first matched item or -1 if no items match
 *
 * @alias Jaxer.DB.ResultSet.prototype.indexOf
 * @method
 * @param {Number|String} column
 * @param {Object} value
 * @param {Number} fromIndex
 * @return {Number}
 */
ResultSet.prototype.indexOf = function indexOf(column, value, fromIndex)
{
	if (typeof column != 'number')
    {
        column = this.columns.indexOf(column);
    }
    return this.columnsAsArray(column).indexOf(value, fromIndex || 0);
};

/**
 * indexOf compares the value of the requested column in each row of the results using strict
 * equality (the same method used by the ===, or triple-equals, operator). it returns either the
 * index of the last matched item or -1 if no items match
 *
 * @alias Jaxer.DB.ResultSet.prototype.lastIndexOf
 * @method
 * @param {Number|String} column
 * @param {Object} value
 * @param {Number} fromIndex
 * @return {Number}
 */
ResultSet.prototype.lastIndexOf = function lastIndexOf(column, value, fromIndex)
{
	if (typeof column != 'number')
    {
        column = this.columns.lastIndexOf(column);
    }
    return this.columnsAsArray(column).lastIndexOf(value, fromIndex || Number.MAX_VALUE );
};

/**
 * map runs a function on every row in the resultset and returns the results in an array.
 * The row and index of the current item are passed as parameters to the current function.
 *
 * @alias Jaxer.DB.ResultSet.prototype.map
 * @method
 * @param {Function} fn
 * @return {Array}
 */
ResultSet.prototype.map = function map(fn)
{
    return this.rowsAsArrays.map(function(row, index) 
	{
        return fn(row, index);
    }, this);
};

/**
 * reduce runs a function on every item in the resultset and collects the results returned.
 * reduce executes the callback function once for each element present in the resultset,
 * receiving four arguments:
 * - the initial value (or value from the previous callback call),
 * - the value of the current row of the resultset,
 * - the current index,
 * - an array representing the resultset over which iteration is occurring.
 *
 * @alias Jaxer.DB.ResultSet.prototype.reduce
 * @method
 * @param {Function} fn
 * @param {Object} initialValue
 * @return {Object}
 */
ResultSet.prototype.reduce = function reduce(fn, initialValue)
{
    return this.rowsAsArrays.reduce(function(previousValue, currentValue, index, array) 
	{
        return fn(previousValue, currentValue, index, array)
    }, initialValue || null);
};

/**
 * reduceRight runs a function on every item in the resultset and collects the results
 * returned, but in reverse order starting with the last element of the recordset.
 *
 * reduceRight executes the callback function once for each element present in the resultset,
 * excluding holes in the array, receiving four arguments:
 * - the initial value (or value from the previous callback call),
 * - the value of the current row of the resultset,
 * - the current index,
 * - an array representing the resultset over which iteration is occurring.
 *
 * @alias Jaxer.DB.ResultSet.prototype.reduceRight
 * @method
 * @param {Function} fn
 * @param {Object} initialValue
 * @return {Object}
 */
ResultSet.prototype.reduceRight = function reduce(fn, initialValue)
{
    return this.rowsAsArrays.reduceRight(function(previousValue, currentValue, index, array) 
	{
        return fn(previousValue, currentValue, index, array)
    }, initialValue || null);
};

/**
 * forEach executes the provided function (callback) once for each element present in the recordset.
 *
 * The callback is invoked with three arguments:
 * - the value of the current row in the resultset,
 * - the index of the current row in the resultset,
 * - an array representing the resultset being traversed.
 *
 * @alias Jaxer.DB.ResultSet.prototype.forEach
 * @method
 * @param {Function} fn
 *         Function to execute against each row of the recordset
 */
ResultSet.prototype.forEach = function forEach(fn)
{
    this.rowsAsArrays.forEach(function(element, index, array) 
	{
        return fn(element, index, array);
    }, this);
};

/**
 * filter returns a new array with all of the elements of this array for which the 
 * provided filtering function returns true.
 * 
 * The function is invoked with three arguments:
 * - the value of the current row in the resultset,
 * - the index of the current row in the resultset,
 * - an array representing the resultset being traversed.
 * 
 * @alias Jaxer.DB.ResultSet.prototype.filter
 * @method
 * @param {Function} fn
 *         Function to test each row of the recordset
 * @return {Array}
 */
ResultSet.prototype.filter = function filter(fn) 
{
    return this.rowsAsArrays.filter(function(element, index, array) 
	{
        return fn(element, index, array);
    }, this);
};

/**
 * every runs a function against each row in the recordset while that function is returning true.
 * returns true if the function returns true for every row it could visit
 * 
 * The function is invoked with three arguments:
 * - the value of the current row in the resultset,
 * - the index of the current row in the resultset,
 * - an array representing the resultset being traversed.
 * 
 * @alias Jaxer.DB.ResultSet.prototype.every
 * @method
 * @param {Function} fn
 * @return {Boolean}
 */
ResultSet.prototype.every = function every(fn) 
{
    return this.rowsAsArrays.every(function(element, index, array) 
	{
        return fn(element, index, array);
    }, this);
};

/**
 * some runs a function against each row in the recordset while that function until returning true.
 *
 * returns true if the function returns true for at least one row it could visit
 *
 * The function is invoked with three arguments:
 * - the value of the current row in the resultset,
 * - the index of the current row in the resultset,
 * - an array representing the resultset being traversed.
 * 
 * @alias Jaxer.DB.ResultSet.prototype.some
 * @method
 * @param {Function} fn
 * @return {Boolean}
 */
ResultSet.prototype.some = function some(fn)
{
    return this.rowsAsArrays.some(function(element, index, array) 
	{
        return fn(element, index, array);
    }, this);
};

/**
 * split runs a function against each row in the recordset.
 *
 * returns A two element array containing the recorset items for
 * which the function returned true as the first element and the
 * items which returned false as the 2nd element.
 *
 * The function is invoked with three arguments:
 * - the value of the current row in the resultset,
 * - the index of the current row in the resultset,
 * - an array representing the resultset being traversed.

 * @alias Jaxer.DB.ResultSet.prototype.split
 * @method
 * @param {Function} fn
 * @return {Array[]}
 */
ResultSet.prototype.split = function split(fn)
{
    var isTrue = this.rowsAsArrays.filter(function(element, index, array) 
	{
        return fn(element, index, array);
    }, this);
    var isFalse = this.rowsAsArrays.filter(function(element, index, array) 
	{
        return !fn(element, index, array);
    }, this);
	
    return [isTrue, isFalse];
};

DB.ResultSet = ResultSet;

Log.trace("*** ResultSet.js loaded");

})();

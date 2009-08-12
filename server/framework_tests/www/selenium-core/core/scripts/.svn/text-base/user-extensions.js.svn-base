/**
 * @author Kevin Lindsey
 */

/**
 * assertAllPassed
 * 
 * This is an Aptana-specific extension. This assertion looks for an output table
 * with an id equal to "output". All rows in the table are visited. The 3rd column
 * of each row is tested against the string "passed". If any rows do not contain
 * that value, this assertion fails.
 */
Selenium.prototype.assertAllPassed = function() {
	var tbody = this.page().findElement("//table[@id='output']/tbody");
	var count = tbody.childNodes.length;
	
	for (var i = 1; i <= count; i++) {
		var name = this.page().findElement("//table[@id='output']/tbody/tr[" + i + "]/td[2]").innerHTML;
		var element = this.page().findElement("//table[@id='output']/tbody/tr[" + i + "]/td[3]");
		var message = this.page().findElement("//table[@id='output']/tbody/tr[" + i + "]/td[4]").innerHTML;
		var lines = [
			"[" + i + "]",
			name,
			message || "---"
		];
		
		Assert.matches(lines.join(" : "), "passed", element.innerHTML);
	}
};

/**
 * assertAttributePresent
 * 
 * This tests if the specified element contains the named attribute
 * 
 * @param {String} locator
 * @param {String} name
 */
Selenium.prototype.assertAttributePresent = function(locator, name) {
	var element = this.page().findElement(locator);
	
	Assert.equals(true, element.hasAttribute(name));
};

/**
 * assertAttributeNotPresent
 * 
 * This tests if the specified element does not contain the named attribute
 * 
 * @param {String} locator
 * @param {String} name
 */
Selenium.prototype.assertAttributeNotPresent = function(locator, name) {
	var element = this.page().findElement(locator);
	
	Assert.equals(false, element.hasAttribute(name));
};

/**
 * doFrameworkCommand
 * 
 * @param {String} command
 */
Selenium.prototype.doFrameworkCommand = function(command)
{
	this.browserbot.getCurrentWindow().frameworkCommand(command);
}

/**
 * assertQueryText
 * 
 * @param {String} query
 * @param {Object} text
 */
Selenium.prototype.assertQueryText = function(query, text) {
	var result = this.browserbot.getCurrentWindow().frameworkQuery(query);
	
	Assert.matches(text, result);
}
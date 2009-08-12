<?php

// create files
write_xml_file();
write_properties_file();

/**
 * write_properties_file
 */
function write_properties_file() {
	// grab data
	$dataSource = ($_SERVER["REQUEST_METHOD"] == "GET") ? $_GET : $_POST;
	
	// create output file
	$handle = fopen("results.properties", "w");
	
	// emit key/value pairs in java properties format
	foreach ($dataSource as $key => $value) {
		if ($key == "result") {
	    	fwrite($handle, "$key=$value\r\n");
		}
	}
	
	// we're done, so close the file
	fclose($handle);
}

/**
 * write_xml_file
 */
function write_xml_file() {
	// grab data
	$dataSource = ($_SERVER["REQUEST_METHOD"] == "GET") ? $_GET : $_POST;
	
	// init attribute values
	$errors = 0;
	$failures = 0;
	$tests = 0;
	$time = 0;
	$tables = array();
	
	// grab key values of interest
	foreach ($dataSource as $key => $value) {
	    switch ($key) {
	    	case "numCommandErrors":
				$errors = $value;
				break;
				
			case "numTestFailures":
				$failures = $value;
				break;
				
			case "numTestTotal":
				$tests = $value;
				break;
				
			case "totalTime":
				$time = $value;
				break;
				
			default:
				// Unescape double-quotes and replace nbsp entities with spaces
				if (preg_match('/^testTable_\d+$/', $key)) {
					$no_quotes = str_replace('\"', '"', $value);
					$no_entities = str_replace('&nbsp;', " ", $no_quotes);
					
					array_push($tables, $no_entities);
				}
				break;
	    }
	}
	
	// emit JUnit-style XML file
	$handle = fopen("test-result.xml", "w");
	
	// build XML text
	array_unshift($tables, "<testsuite errors=\"$errors\" failures=\"$failures\" tests=\"$tests\" time=\"$time\">");
	array_push($tables, "</testsuite>");
	$xml = join($tables, "\r\n");
	
	// load XML
	$doc = new DOMDocument();
	$doc->loadXML($xml);
	
	// load XSLT
	$xsl = new DOMDocument();
	$xsl->load("results-to-junit.xslt");
	
	// create XSLT processor
	$proc = new XSLTProcessor();
	$proc->importStyleSheet($xsl);
	
	// transform XML
	$result = $proc->transformToXML($doc);
	
	// write result
	fwrite($handle, $result);
	
	// we're done, so close the file
	fclose($handle);
}
?>

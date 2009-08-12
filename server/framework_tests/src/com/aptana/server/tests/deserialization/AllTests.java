/**
 * Copyright (c) 2005-2007 Aptana, Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html. If redistributing this code,
 * this entire header must remain intact.
 */
package com.aptana.server.tests.deserialization;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
 
@RunWith(Suite.class)
@Suite.SuiteClasses({
	TestArrayDeserialization.class,
	TestBooleanDeserialization.class,
	TestDateDeserialization.class,
	TestNumberDeserialization.class,
	TestObjectDeserialization.class,
	TestReferenceDeserialization.class,
	TestSpecialValueDeserialization.class,
	TestStringDeserialization.class
})

public class AllTests
{
}

/**
 * Copyright (c) 2005-2007 Aptana, Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html. If redistributing this code,
 * this entire header must remain intact.
 */
package com.aptana.server.tests.serialization;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
 
@RunWith(Suite.class)
@Suite.SuiteClasses({
	TestArraySerialization.class,
	TestBooleanSerialization.class,
	TestDateSerialization.class,
	TestNumberSerialization.class,
	TestObjectSerialization.class,
	TestReferenceSerialization.class,
	TestSpecialValueSerialization.class,
	TestStringSerialization.class
})

public class AllTests
{
}

/**
 * Copyright (c) 2005-2007 Aptana, Inc.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html. If redistributing this code,
 * this entire header must remain intact.
 */
package com.aptana.server.tests.runat;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
 
@RunWith(Suite.class)
@Suite.SuiteClasses({
	TestNoAttribute.class,
	TestClientAttribute.class,
	TestServerAttribute.class,
	TestBothAttribute.class,
	
	TestServerAndCacheAttribute.class,
	TestBothAndCacheAttribute.class,
	TestServerAndProxyAttribute.class,
	TestBothAndProxyAttribute.class,
	
//	TestExternalNoAttribute.class,
//	TestExternalClientAttribute.class,
//	TestExternalServerAttribute.class,
//	TestExternalBothAttribute.class,
	
	TestGlobalFunction.class
})

public class AllTests
{
}

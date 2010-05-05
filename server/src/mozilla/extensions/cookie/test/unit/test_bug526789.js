const Cc = Components.classes;
const Ci = Components.interfaces;
const Cr = Components.results;

function do_check_throws(f, result, stack)
{
  if (!stack)
    stack = Components.stack.caller;

  try {
    f();
  } catch (exc) {
    if (exc.result == result)
      return;
    do_throw("expected result " + result + ", caught " + exc, stack);
  }
  do_throw("expected result " + result + ", none thrown", stack);
}

function run_test() {
  var cs = Cc["@mozilla.org/cookieService;1"].getService(Ci.nsICookieService);
  var cm = Cc["@mozilla.org/cookiemanager;1"].getService(Ci.nsICookieManager2);
  var ios = Cc["@mozilla.org/network/io-service;1"].getService(Ci.nsIIOService);

  cm.removeAll();

  // test that an empty or '.' http:// host results in a no-op
  var uri = ios.newURI("http://baz.com/", null, null);
  var emptyuri = ios.newURI("http:///", null, null);
  var doturi = ios.newURI("http://./", null, null);
  do_check_eq(uri.asciiHost, "baz.com");
  do_check_eq(emptyuri.asciiHost, "");
  do_check_eq(doturi.asciiHost, ".");
  cs.setCookieString(emptyuri, null, "foo2=bar", null);
  do_check_eq(getCookieCount(), 0);
  cs.setCookieString(doturi, null, "foo3=bar", null);
  do_check_eq(getCookieCount(), 0);
  cs.setCookieString(uri, null, "foo=bar", null);
  do_check_eq(getCookieCount(), 1);

  do_check_eq(cs.getCookieString(uri, null), "foo=bar");
  do_check_eq(cs.getCookieString(emptyuri, null), null);
  do_check_eq(cs.getCookieString(doturi, null), null);

  do_check_eq(cm.countCookiesFromHost("baz.com"), 1);
  do_check_eq(cm.countCookiesFromHost(""), 0);
  do_check_eq(cm.countCookiesFromHost("."), 0);

  var e = cm.getCookiesFromHost("baz.com");
  do_check_true(e.hasMoreElements());
  do_check_eq(e.getNext().QueryInterface(Ci.nsICookie2).name, "foo");
  do_check_false(e.hasMoreElements());
  e = cm.getCookiesFromHost("");
  do_check_false(e.hasMoreElements());
  e = cm.getCookiesFromHost(".");
  do_check_false(e.hasMoreElements());

  cm.removeAll();

  // test that an empty file:// host works
  var emptyuri = ios.newURI("file:///", null, null);
  do_check_eq(emptyuri.asciiHost, "");
  do_check_eq(ios.newURI("file://./", null, null).asciiHost, "");
  do_check_eq(ios.newURI("file://foo.bar/", null, null).asciiHost, "");
  cs.setCookieString(emptyuri, null, "foo2=bar", null);
  do_check_eq(getCookieCount(), 1);
  cs.setCookieString(emptyuri, null, "foo3=bar; domain=", null);
  do_check_eq(getCookieCount(), 2);
  cs.setCookieString(emptyuri, null, "foo4=bar; domain=.", null);
  do_check_eq(getCookieCount(), 2);
  cs.setCookieString(emptyuri, null, "foo5=bar; domain=bar.com", null);
  do_check_eq(getCookieCount(), 2);

  do_check_eq(cs.getCookieString(emptyuri, null), "foo2=bar; foo3=bar");

  do_check_eq(cm.countCookiesFromHost("baz.com"), 0);
  do_check_eq(cm.countCookiesFromHost(""), 2);
  do_check_eq(cm.countCookiesFromHost("."), 0);

  var e = cm.getCookiesFromHost("baz.com");
  do_check_false(e.hasMoreElements());
  e = cm.getCookiesFromHost("");
  do_check_true(e.hasMoreElements());
  e.getNext();
  do_check_true(e.hasMoreElements());
  e.getNext();
  do_check_false(e.hasMoreElements());
  e = cm.getCookiesFromHost(".");
  do_check_false(e.hasMoreElements());

  cm.removeAll();

  // test that an empty host to add() or remove() works,
  // but a host of '.' or ending with a '.' doesn't
  var expiry = (Date.now() + 1000) * 1000;
  cm.add("", "/", "foo2", "bar", false, false, true, expiry);
  do_check_eq(getCookieCount(), 1);
  do_check_throws(function() {
    cm.add(".", "/", "foo3", "bar", false, false, true, expiry);
  }, Cr.NS_ERROR_ILLEGAL_VALUE);
  do_check_eq(getCookieCount(), 1);
  cm.add("test.com", "/", "foo", "bar", false, false, true, expiry);
  do_check_eq(getCookieCount(), 2);
  do_check_throws(function() {
    cm.add("test.com.", "/", "foo4", "bar", false, false, true, expiry);
  }, Cr.NS_ERROR_ILLEGAL_VALUE);
  do_check_eq(getCookieCount(), 2);

  cm.remove("", "foo2", "/", false);
  do_check_eq(getCookieCount(), 1);
  do_check_throws(function() {
    cm.remove(".", "foo3", "/", false);
  }, Cr.NS_ERROR_ILLEGAL_VALUE);
  do_check_eq(getCookieCount(), 1);
  do_check_throws(function() {
    cm.remove("test.com.", "foo4", "/", false);
  }, Cr.NS_ERROR_ILLEGAL_VALUE);
  do_check_eq(getCookieCount(), 1);
  cm.remove("test.com", "foo", "/", false);
  do_check_eq(getCookieCount(), 0);

  cm.removeAll();
}

function getCookieCount() {
  var count = 0;
  var cm = Cc["@mozilla.org/cookiemanager;1"].getService(Ci.nsICookieManager2);
  var enumerator = cm.enumerator;
  while (enumerator.hasMoreElements()) {
    if (!(enumerator.getNext() instanceof Ci.nsICookie2))
      throw new Error("not a cookie");
    ++count;
  }
  return count;
}


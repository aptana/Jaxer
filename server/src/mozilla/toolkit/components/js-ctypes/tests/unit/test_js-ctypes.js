/* -*-  Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2; -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is js-ctypes.
 *
 * The Initial Developer of the Original Code is
 * Fredrik Larsson <nossralf@gmail.com>
 * Portions created by the Initial Developer are Copyright (C) 2008
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *  Mark Finkle <mark.finkle@gmai.com>, <mfinkle@mozilla.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

const Cc = Components.classes;
const Ci = Components.interfaces;

const Types = Ci.nsINativeTypes;

function POINT(x, y) {
  this.x = x; this.y = y;
}

POINT.prototype = {
  _fields_ : [{"x" : Types.INT32}, {"y" : Types.INT32}]
}

function RECT(x, y, w, h) {
  this.x = x; this.y = y; this.w = w; this.h = h;
}

RECT.prototype = {
  _fields_ : [{"x" : Types.INT32}, {"y" : Types.INT32}, {"w" : Types.INT32}, {"h" : Types.INT32}]
}

var gOS;

function run_test()
{
  var dist_bin = Cc["@mozilla.org/file/directory_service;1"]
                     .getService(Ci.nsIProperties)
                     .get("CurProcD", Ci.nsIFile);
  var testlib = dist_bin.path;

  var xulRuntime = Cc["@mozilla.org/xre/app-info;1"]
                   .getService(Ci.nsIXULRuntime);

  gOS = xulRuntime.OS.toLowerCase();
  if (gOS == "winnt")
    testlib += "\\js-ctypes-test.dll";
  else if (gOS == "linux")
    testlib += "/libjs-ctypes-test.so";
  else if (gOS == "darwin")
    testlib += "/libjs-ctypes-test.dylib";

  var library = Cc["@developer.mozilla.org/js-ctypes;1"]
                .createInstance(Ci.nsINativeTypes);
  var loaded = library.open(testlib);
  do_check_true(loaded);

  run_void_tests(library);
  run_short_tests(library);
  run_int_tests(library);
  run_float_tests(library);
  run_double_tests(library);
  run_string_tests(library);
  run_mixed_tests(library);
  run_struct_tests(library);
}

function run_void_tests(library) {
  var test_v = library.declare("test_v", Types.SYSV, Types.VOID);
  do_check_eq(test_v(), null);
}
function run_short_tests(library) {
  var test_s = library.declare("test_s", Types.SYSV, Types.INT16);
  do_check_eq(test_s(), 12345);

  var test_s_s = library.declare("test_s_s", Types.SYSV, Types.INT16, Types.INT16);
  do_check_eq(test_s_s(5), 5);

  var test_s_ss = library.declare("test_s_ss", Types.SYSV, Types.INT16, Types.INT16, Types.INT16);
  do_check_eq(test_s_ss(5, 5), 10);

  var test_s_echo = library.declare("test_s_echo", Types.SYSV, Types.INT16_P, Types.INT16_P);
  var p = 50;
  do_check_eq(test_s_echo(p), 50);
}

function run_int_tests(library) {
  var test_i = library.declare("test_i", Types.SYSV, Types.INT32);
  do_check_eq(test_i(), 123456789);

  var test_i_i = library.declare("test_i_i", Types.SYSV, Types.INT32, Types.INT32);
  do_check_eq(test_i_i(5), 5);

  var test_i_ii = library.declare("test_i_ii", Types.SYSV, Types.INT32, Types.INT32, Types.INT32);
  do_check_eq(test_i_ii(5, 5), 10);

  var test_i_echo = library.declare("test_i_echo", Types.SYSV, Types.INT32_P, Types.INT32_P);
  var p = 550;
  do_check_eq(test_i_echo(p), 550);
}

function run_float_tests(library) {
  var test_f = library.declare("test_f", Types.SYSV, Types.FLOAT);
  do_check_eq(test_f(), 123456.5);

  var test_f_f = library.declare("test_f_f", Types.SYSV, Types.FLOAT, Types.FLOAT);
  do_check_eq(test_f_f(5), 5);
  do_check_eq(test_f_f(5.25), 5.25);

  var test_f_ff = library.declare("test_f_ff", Types.SYSV, Types.FLOAT, Types.FLOAT, Types.FLOAT);
  do_check_eq(test_f_ff(5, 5), 10);
  do_check_eq(test_f_ff(5.5, 5.5), 11);

  var test_f_echo = library.declare("test_f_echo", Types.SYSV, Types.FLOAT_P, Types.FLOAT_P);
  var p = 550.5;
  do_check_eq(test_f_echo(p), 550.5);
}

function run_double_tests(library) {
  var test_d = library.declare("test_d", Types.SYSV, Types.DOUBLE);
  do_check_eq(test_d(), 123456789.5);

  var test_d_d = library.declare("test_d_d", Types.SYSV, Types.DOUBLE, Types.DOUBLE);
  do_check_eq(test_d_d(5), 5);
  do_check_eq(test_d_d(5.25), 5.25);

  var test_d_dd = library.declare("test_d_dd", Types.SYSV, Types.DOUBLE, Types.DOUBLE, Types.DOUBLE);
  do_check_eq(test_d_dd(5, 5), 10);
  do_check_eq(test_d_dd(5.5, 5.5), 11);

  // NOT SUPPORTED (Windows doesn't like it)
  //var test_d_echo = library.declare("test_d_echo", Types.SYSV, Types.DOUBLE_P, Types.DOUBLE_P);
  //var p = 123450.25;
  //do_check_eq(test_d_echo(p), 123450.25);
}

function run_string_tests(library) {
  var test_ansi_len = library.declare("test_strlen", Types.SYSV, Types.INT32, Types.STRING);
  do_check_eq(test_ansi_len("hello world"), 11);

  var test_wide_len = library.declare("test_wcslen", Types.SYSV, Types.INT32, Types.WSTRING);
  if (gOS == "winnt")
    do_check_eq(test_wide_len("hello world"), 11);

  var test_ansi_ret = library.declare("test_strret", Types.SYSV, Types.STRING);
  do_check_eq(test_ansi_ret(), "success");

  var test_ansi_echo = library.declare("test_str_echo", Types.SYSV, Types.STRING, Types.STRING);
  do_check_eq(test_ansi_echo("anybody in there?"), "anybody in there?");
}

function run_mixed_tests(library) {
  var test_i_if_floor = library.declare("test_i_if_floor", Types.SYSV, Types.INT32, Types.INT32, Types.FLOAT);
  do_check_eq(test_i_if_floor(5, 5.5), 10);
}

function run_struct_tests(library) {
  var test_pt_in_rect = library.declare("test_pt_in_rect", Types.SYSV, Types.INT32, RECT, POINT);
  var rect = new RECT(5, 5, 10, 10);
  var pt1 = new POINT(6, 6);
  do_check_eq(test_pt_in_rect(rect, pt1), 1);
  var pt2 = new POINT(2, 2);
  do_check_eq(test_pt_in_rect(rect, pt2), 0);

  // NOT SUPPORTED
  /*
  var test_init_pt = library.declare("test_init_pt", Types.SYSV, Types.VOID, POINT, Types.INT32, Types.INT32);
  var foo = new POINT(0, 0);
  test_init_pt(foo, 10, 10)
  do_check_eq(foo.x, 10);
  */
}

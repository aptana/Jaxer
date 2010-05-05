var ev = eval;
var z;

function f() {
  var m = 22;

  return ev("(function g() { for (var i = 0; i < 5; ++i) m; })");
}

var g = f();
g();

/*
	JSON parsing and stringifying with support for id and path based referencing.
	Uses the "old" JSON JS API that does not break enumerability.
	Modified version of the Douglas Crockford's JSON library from http://www.json.org/js.html
    Public Domain? How are you supposed to license something when you modify public domain code?
    Usage: 
    JSON.stringify(value) turns a JS value into JSON string. When an object is encountered more
    than one (through circular or multiple referencing), a reference is made to it.
    JSON.parse(string) turns a JSON string into a JS object. When a reference is encountered it is resolved.
     References are in the form of:
    {"$ref":"$|id.path"}     
*/
(function() {
  var ids={};
  JSON= {
  	resolveRef: function(ref) {
		throw new Error("Reference to " + ref + " could not be resolved");  		
  	},
     stringify: function (v) {
	   var a = [];
	   var paths={};
	   var m = {
	       '\b': '\\b',
	       '\t': '\\t',
	       '\n': '\\n',
	       '\f': '\\f',
	       '\r': '\\r',
	       '"' : '\\"',
	       '\\': '\\\\'
	   };
	   function e(s) {
	       a[a.length] = s;
	   }
	   function s(x) {
            if (/["\\\x00-\x1f]/.test(x)) {
               return '"' + x.replace(/([\x00-\x1f\\"])/g, function(a, b) {
                   var c = m[b];
                   if (c) {
                       return c;
                   }
                   c = b.charCodeAt();
                   return '\\u00' +
                       Math.floor(c / 16).toString(16) +
                       (c % 16).toString(16);
               }) + '"';
           }
           return '"' + x + '"';
	   }
	   function g(x,p) {
	       var c, i, l, v;
	       switch (typeof x) {
	       case 'object':
	           if (x) {
	               if (x instanceof Date) {
				        function f(n) {
				            return n < 10 ? '0' + n : n;
				        }

				        return e('"' + this.getUTCFullYear() + '-' +
				                f(this.getUTCMonth() + 1) + '-' +
				                f(this.getUTCDate()) + 'T' +
				                f(this.getUTCHours()) + ':' +
				                f(this.getUTCMinutes()) + ':' +
				                f(this.getUTCSeconds()) + 'Z"');
                    }
                    // ref ->
                   if (x.id) {
                    	if (ids[x.id] || paths[x.id]) 
                    		return g({$ref:x.id});
                    	else
                    		ids[p = x.id] = x;
                   	}
                   	else {
                   		x.id = p;
                   		paths[p] = x;
                  	}
                   // <- ref
	               if (x instanceof Array) {
	                   e('[');
	                   l = a.length;
	                   for (i = 0; i < x.length; i += 1) {
	                       v = x[i];
	                       if (typeof v != 'undefined' &&
	                               typeof v != 'function') {
	                           if (l < a.length) {
	                               e(',');
	                           }
	                           g(v,p +'['+i+']');
	                       }
	                   }
	                   e(']');
	                   return;
	               } 
	                    else if (typeof x.valueOf == 'function') {
	                        e('{');
	                        l = a.length;
	                        for (i in x) {
	                            v = x[i];
	                            if (typeof v != 'undefined' && !(i == 'id' && paths[v]) &&
	                                    typeof v != 'function' &&
	                                    (!v || typeof v != 'object' ||
	                                        typeof v.valueOf == 'function')) {
	                                if (l < a.length) {
	                                    e(',');
	                                }
	                                e(s(i));
	                                e(':');
	                                // -> ref
	                                g(v,p+(i.match(/^[a-zA-Z]\w*$/) ? ('.' + i) : ('[' + s(i) + ']')));
	                                // <- ref
	                            }
	                        }
	                        return e('}');
	                    }
	                }
	                e('null');
	                return;
	            case 'number':
	                e(isFinite(x) ? +x : 'null');
	                return;
	            case 'string':
	            	e(s(x));
	                return;
	            case 'boolean':
	                e(String(x));
	                return;
	            default:
	                e('null');
	                return;
	            }
	        }
	        g(v,'$');
	        for (i in paths) 
	        	delete paths[i].id;   
	        return a.join('');
	    },
	parse : function(str,stop) { 
			var j,ref,rw=[];
            function walk(v,stop) {
           	    // ref ->
            	if (v.id)
					ids[v.id] = v;
            	for (var i in v) {
					if (v.hasOwnProperty(i) && (typeof (val=v[i]) =='object') && val) {
						if (ref=val.$ref) {
							var stripped = ref.replace(/\\./g, '@').replace(/"[^"\\\n\r]*"/g, '');
							if(/[\w\[\]\.\$ \/\r\n\t]/.test(stripped) && !/=|((^|\W)new\W)/.test(stripped)) {
								var path = ref.match(/(^[\$\w]+)(\W.*)?/);
								(ref=path[1]=='$'?j:ids[path[1]]) ?
									(v[i] = path[2] ? eval('ref' + path[2]) : ref) : (stop ? JSON.resolveRef(val.$ref) : rw.push(v)); 
							}
						}
						if (!stop) // <- ref
							walk(val,rw==v);
					}
				}
            }
            if (/^[,:{}\[\]0-9.\-+Eaeflnr-u \n\r\t]*$/.test(str.
                    replace(/\\./g, '@').
                    replace(/"[^"\\\n\r]*"/g, ''))) {
                j = eval('(' + str + ')');
                walk(j);
                walk(rw);
                return j;
            }
            throw new SyntaxError('parseJSON');
        }
	}
})();

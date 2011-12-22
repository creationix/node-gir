var gir = require('./build/Release/girepository.node'),
    EventEmitter = require("events").EventEmitter;

gir.init();

var oldLoad = gir.load;
gir.load = function(namespace, version) {
    if(!version) {
        var namespaces = gir.loadedNamespaces();
        for(var i=0; i<namespaces.length; i++) {
            if(namespace == namespaces[i]) {
                version = gir.getVersion(namespaces[i]);
                break;
            }
        }
        
    }
    var ns = oldLoad(namespace, version);
    
    for(var i=0; i<ns.__objects__.length; i++) {
        var obj = ns[ns.__objects__[i]];
        if(!obj) {
            continue;
        }
        for(var j in obj.__methods__) {
            var cname = obj.__methods__[j];
            obj.prototype[camelcase(cname)] = (function(fname) {
                return function() {
                    var arg = Array.prototype.slice.call(arguments);
                    arg.unshift(fname);
                    this.__call__.apply(this, arg);
                };
            })(cname);
        }
        for(var j in obj.__properties__) {
            var cname = obj.__properties__[j];
            (function(propname) {
                obj.prototype.__defineGetter__(camelcase(cname), function() {
                    return this.__get_property__.apply(this, [propname]);
                });
                obj.prototype.__defineSetter__(camelcase(cname), function(x) {
                    return this.__set_property__.apply(this, [propname, x]);
                });
            })(cname);
        }
    }
    
    for(var i=0; i<ns.__roots__.length; i++) {
        extend(true, ns[ns.__roots__[i]].prototype, EventEmitter.prototype);
    }
    
    return ns;
};

module.exports = gir;



function camelcase(x) {
    return x.replace(/\_[a-z]/g, function(h) {
        return h.substr(1).toUpperCase();
    });
}

/**
 * Adopted from jquery's extend method. Under the terms of MIT License.
 *
 * http://code.jquery.com/jquery-1.4.2.js
 *
 * Modified by Brian White to use Array.isArray instead of the custom isArray
 * method
 */
function extend() {
  // copy reference to target object
  var target = arguments[0] || {},
      i = 1,
      length = arguments.length,
      deep = false,
      options,
      name,
      src,
      copy;

  // Handle a deep copy situation
  if (typeof target === "boolean") {
    deep = target;
    target = arguments[1] || {};
    // skip the boolean and the target
    i = 2;
  }

  // Handle case when target is a string or something (possible in deep copy)
  if (typeof target !== "object" && !typeof target === 'function')
    target = {};

  var isPlainObject = function(obj) {
    // Must be an Object.
    // Because of IE, we also have to check the presence of the constructor
    // property.
    // Make sure that DOM nodes and window objects don't pass through, as well
    if (!obj || toString.call(obj) !== "[object Object]" || obj.nodeType
        || obj.setInterval)
      return false;
    
    var has_own_constructor = hasOwnProperty.call(obj, "constructor");
    var has_is_prop_of_method = hasOwnProperty.call(obj.constructor.prototype,
                                                    "isPrototypeOf");
    // Not own constructor property must be Object
    if (obj.constructor && !has_own_constructor && !has_is_prop_of_method)
      return false;
    
    // Own properties are enumerated firstly, so to speed up,
    // if last one is own, then all properties are own.

    var last_key;
    for (key in obj)
      last_key = key;
    
    return typeof last_key === "undefined" || hasOwnProperty.call(obj, last_key);
  };


  for (; i < length; i++) {
    // Only deal with non-null/undefined values
    if ((options = arguments[i]) !== null) {
      // Extend the base object
      for (name in options) {
        src = target[name];
        copy = options[name];

        // Prevent never-ending loop
        if (target === copy)
            continue;

        // Recurse if we're merging object literal values or arrays
        if (deep && copy && (isPlainObject(copy) || Array.isArray(copy))) {
          var clone = src && (isPlainObject(src) || Array.isArray(src)
                              ? src : (Array.isArray(copy) ? [] : {}));

          // Never move original objects, clone them
          target[name] = extend(deep, clone, copy);

        // Don't bring in undefined values
        } else if (typeof copy !== "undefined")
          target[name] = copy;
      }
    }
  }

  // Return the modified object
  return target;
};

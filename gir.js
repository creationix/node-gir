/**
 *  Example use of this module:
 *    var gir = require('./path/to/gir.js')
 *     ,  gtk = gir.import('Gtk', '3.0');
 **/

//define export object, import girepository and EventEmitter onto export object
var gir = module.exports = { '_girepository': require('./build/Release/girepository.node') }
 ,  EventEmitter = require('events').EventEmitter;

/******************************************************************************/

/* BEGIN EXTERNAL HELPERS */

/**
 * Adopted from jquery's extend method. Under the terms of MIT License.
 *
 * http://code.jquery.com/jquery-1.4.2.js
 *
 * Modified by Brian White to use Array.isArray instead of the custom isArray
 * method.
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

/**
 * Copied from jQuery. Under the terms of MIT or GPLv2 License.
 * http://code.jquery.com/jquery-1.7.1.js
 */
function merge(first, second) {
		var i = first.length,
			j = 0;

		if ( typeof second.length === "number" ) {
			for ( var l = second.length; j < l; j++ ) {
				first[ i++ ] = second[ j ];
			}

		} else {
			while ( second[j] !== undefined ) {
				first[ i++ ] = second[ j++ ];
			}
		}

		first.length = i;

		return first;
	}
	
/**
 * Copied from jQuery. Under the terms of MIT or GPLv2 License.
 * http://code.jquery.com/jquery-1.7.1.js
 *
 * Modified by David Ball to work outside the jQuery environment. Removed
 * reference to jQuery.isWindow() and jQuery.type(). Modified jQuery.merge() to
 * use local scope. Applied logic from jQuery.type() since there is no DOM.
 * Changed push to Array.prototype.push.
 */
function makeArray(array, results) {
  var ret = results || [];

  if ( array != null ) {
	  // The window, strings (and functions) also have 'length'
	  // Tweaked logic slightly to handle Blackberry 4.7 RegExp issues #6930
	  var type = array == null ? String( obj ) : "object";

	  if ( array.length == null || type === "string" || type === "function" || type === "regexp") {
		  Array.prototype.push.call( ret, array );
	  } else {
		  merge( ret, array );
	  }
  }

  return ret;
} 

/* END EXTERNAL HELPERS */

/******************************************************************************/

/* BEGIN INTERNAL HELPERS */

/**
 * Converts underscored_names or dashed-names to properCamelCasing.
 **/
function camelCase(x) {
  return x.replace(/[-]+/g, function (h) {
    return '_';
  }).replace(/\_[a-z]/g, function(h) {
    return h.substr(1).toUpperCase();
  });
}

/**
 * Creates a onCamelCasedEvent() on the instance object's prototype that
 * matches the specified regular_event_name. The method will be dispatched
 * to on('event_name', callback). e.g. onNotify, onClosed, onDestroy
 **/
function defineInstanceEvent(obj, event_name) {
  var camelEventName = camelCase('on_'+event_name);
  //add event handler to object if possible
  if (!obj.prototype[camelEventName])
    //converts method name to camelCasing on the object
    obj.prototype[camelCase('on_'+event_name)] = function(event_name) {
      var invocation = function() {
        var args = Array.prototype.slice.call(arguments);
        if (args == undefined) args = [function noCallback() { }];
        args.unshift(event_name);
        //debug:console.log('called',event_name,'in',obj,'with',args[1]);
        return this.on.apply(this, args);
      };
      return invocation;
    }(event_name);
}

/**
 * Creates a camelCasedMethod() on the instance object's prototype that
 * matches the specified regular_method_name. The method will be dispatched
 * to __call__('method_name', arg0, argx, ...).
 **/
function defineInstanceMethod(obj, method_name) {
  var camelMethodName = camelCase(method_name);
  //add method handler to object if possible
  if (!obj.prototype[camelMethodName])
    //converts method name to camelCasing on the object
    obj.prototype[camelMethodName] = function(method_name) {
      //the internal function does all the hard work
      var invocation = function() {
        var args = Array.prototype.slice.call(arguments);
        if (args == undefined) args = new Array();
        args.unshift(method_name);
        //this helped me to catch a recent problem with object.cc
        //debug:console.log('called',method_name,'in',obj);
        //debug:for (var x in obj) console.log(x);
        return this.__call__.apply(this, args);
      };
      return invocation;
    }(method_name);
  //else
    //debug:console.warn("[node-gir] " + subobj + " object provides it's own " + method_name + " method. Not replacing existing method. :-(");
    
  //Not sure if this would do anything positive:
  //Check for new(), and makes new() the default constructor if it's in the gir object.
  //Disabled until it would serve useful. It simply assigns the new() function as
  //the JS constructor for the object. There is a small problem: some objects have multiple
  //new() functions, probably due to object inheritence or overloading. :~ Not sure really,
  //but it seems to add no new functionality while simultaneously not taking anything away.
  //It may be thrown away in a future commit if it is decidedly useless.
  //DISABLED:
  /*if (method_name == 'new') {
    //debug:console.log('[node-gir] Using new() as the default constructor for', subobj, 'provided by gir.');
    obj[subobj].prototype.constructor = defineObjectMethod(obj[subobj], 'new');
  }*/
}

/**
 * Creates a camelCasedVFunc() on the instance object's prototype that
 * matches the specified regular_method_name. The method will be dispatched
 * to __call_v_func__('v_func_name', arg0, argx, ...).
 **/
function defineInstanceVFunc(obj, v_name) {
  var camelVName = camelCase(v_name);
  //add method handler to object if possible
  if (!obj.prototype[camelVName])
    //converts method name to camelCasing on the object
    obj.prototype[camelVName] = function(v_name) {
      //the internal function does all the hard work
      var invocation = function() {
        var args = Array.prototype.slice.call(arguments);
        if (args == undefined) args = new Array();
        args.unshift(v_name);
        //call the method on the gir provided object
        //debug:console.log('called',v_name,'in',obj);
        this.__call_v_func__.apply(this, args);
      };
      return invocation;
    }(v_name);
  //else
    //debug:console.warn("[node-gir] " + subobj + " object provides it's own " + method_name + " method. Not replacing existing method. :-(");
}

/**
 * Creates a camelCasedProperty on the instance object's prototype that
 * matches the specified regular-property-name. The getter is dispatched to
 * __get_property__('property-name'). The setter is dispatched to
 * __set_property__('property-name', value).
 **/
//added try{}catch{} block because of TypeError: Illegal invocation, in what appears
//to be accessing beyond object access boundaries, e.g. trying to access a private member, or
//setting a getter-only property, or something to that effect.
//perhaps someone can figure out why this version throws TypeError: Illegal invocation 
//across certain boundaries while the swick/node-gir does not :~ could be a diff in binaries
function defineInstanceProperty(obj, property_name) {
  var camelPropertyName = camelCase(property_name);
  //add property handler to object if possible
  if (!obj.prototype[camelPropertyName]) {
    obj.prototype.__defineGetter__(camelPropertyName, function() {
      try {
        return this.__get_property__.apply(this, [property_name]);
      } catch (err) {
        //debug:console.warn('[node-gir] Error trying to install getter ' + camelCase(property_name) + '.', err);
        return undefined;
      }
    });
    obj.prototype.__defineSetter__(camelPropertyName, function(newValue) {
      try {
        return this.__set_property__.apply(this, [property_name, newValue]);
      } catch (err) {
        //debug:console.warn('[node-gir] Error trying to install setter ' + camelCase(property_name) + '.', err);
        return newValue;
      }
    });
  }
  //else
    //debug:console.warn("[node-gir] " + subobj + " object provides it's own " + property_name + " property. Not replacing existing property. :-(");
}

/* END INTERNAL HELPERS */

/******************************************************************************/

/* BEGIN LOGIC */

//save default module routines
gir._girepository.__init__ = gir._girepository.init;
gir._girepository.__import__ = gir._girepository.import;

//add init flag property
gir._girepository._has_init = false;

//override default init
gir._girepository.init = function() {
  //don't init twice, seems useless to do so
  if (!gir._girepository._has_init) {
    gir._girepository._has_init = true;
    return gir._girepository.__init__.apply(gir._girepository, Array.prototype.slice.call(arguments));
  }
};

//override default namespace loader
gir.load = gir.import = gir._girepository.import = function() {
  //auto-init if needed
  if (!gir._girepository._has_init) gir._girepository.init();
  
  //load gir module
  var obj = gir._girepository.__import__.apply(gir._girepository, Array.prototype.slice.call(arguments));
  
  //check for error
  if (!obj) return obj;
  
  //TODO: consider storing loaded module gir somewhere now so that it can be unloaded later ?
  
  //for each object within the loaded gir module:
  //  task 1: add EventEmitter as needed
  //  task 2: setup signals as onEventNames shortcut functions
  //  task 3: expose methods and make them callable
  //  task 4: expose vfuncs and make them callable
  //  task 5: expose properties and make them settable/gettable
  //  task 6: expose fields and make them settable/gettable
  for (var subobj in obj) {
    //task 1: add EventEmitter as needed
    //determine whether eventable
    var eventable = obj[subobj].__signals__ != undefined
                      && makeArray(obj[subobj].__signals__).length>0;
    if (eventable) {
      //combine EventEmitter logic with eventable gir objects
      extend(true, obj[subobj].prototype, EventEmitter.prototype);
      //check for prop __watch_signal__, if found, override EventEmitter.on()
      if (obj[subobj]['__signals__'] != undefined) {
        obj[subobj].prototype._EventEmitter_on = obj[subobj].prototype.on;
        obj[subobj].prototype.on = function () {
          var args = Array.prototype.slice.call(arguments);
          //tell gir loaded object to listen for the signal
          if (this['__watch_signal__'] != undefined)
            this.__watch_signal__(args[0]);
          //console.log('on hit with', args);
          //dispatch EventEmitter normally
          this._EventEmitter_on.apply(this, args);
        };
      }
    }

    //task 2: setup signals as onEventNames shortcut functions
    //task 3: expose methods and make them callable
    //task 4: expose vfuncs and make them callable
    //task 5: expose properties and make them settable/gettable
    //task 6: expose fields and make them settable/gettable
    for (var prop in obj[subobj]) {
      switch (prop) {
        case '__signals__':  //task 2
          for (var signal in obj[subobj][prop]) {
            var signal_name = obj[subobj][prop][signal];
            defineInstanceEvent(obj[subobj], signal_name);
          }
          break;
        case '__methods__':  //task 3
          for (var method in obj[subobj][prop]) {
            var method_name = obj[subobj][prop][method];
            defineInstanceMethod(obj[subobj], method_name);
          }
          break;
        case '__v_funcs__':  //task 4
          for (var v in obj[subobj][prop]) {
            var v_name = obj[subobj][prop][v];
            //i don't yet know the value of a v_func
            defineInstanceVFunc(obj[subobj], v_name);
          }
          break;
        case '__properties__':  //task 5
          for (var property in obj[subobj][prop]) {
            var property_name = obj[subobj][prop][property];
            defineInstanceProperty(obj[subobj], property_name);
          }
          break;
        case '__fields__':  //task 6
          for (var field in obj[subobj][prop]) {
            var field_name = obj[subobj][prop][field];
            //this requires changes to the C++ code
            //add field reference if possible
            //if (!obj[subobj].prototype[field_name])
              //defineInstanceField(obj[subobj], field_name);
            //debug:console.log('found field', field_name, 'in', subobj);
          }
          break;
      }
    }
    
    //keep the object name in the object for reflection
    if (obj[subobj].__name__ != undefined)
      console.warn("[node-gir]", arguments[0]+'.'+subobj, "provides it's own __name__. Not replacing __name__.");
    else
      obj[subobj].__name__ = subobj;
      
    //keep the namespace loader in the loaded object in case caller wants to reuse the loader
    if (obj[subobj].__gir__ != undefined)
      console.warn("[node-gir]", arguments[0], "provides it's own __gir__. Not replacing __gir__. Strange error? :-(");
    else
      obj[subobj].__gir__ = this;
  }

  //keep the namespace name in the loaded object for reflection
  if (obj.__name__ != undefined)
    console.warn("[node-gir]", arguments[0], "provides it's own __name__. Not replacing __name__.");
  else
    obj.__name__ = arguments[0];

  //keep the namespace version in the loaded object for reflection
  if (obj.__version__ != undefined)
    console.warn("[node-gir]", arguments[0], "provides it's own __version__. Not replacing __version__.");
  else
    obj.__version__ = arguments[1] ? arguments[1] : gir._girepository.getVersion(arguments[0]);
    
  //keep the namespace loader in the loaded namespace in case caller wants to reuse the loader
  if (obj.__gir__ != undefined)
    console.warn("[node-gir]", arguments[0], "provides it's own __gir__. Not replacing __gir__. Strange error? :-(");
  else
    obj.__gir__ = this;

  //return the brutally overridden object
  return obj;
};

/* END LOGIC */

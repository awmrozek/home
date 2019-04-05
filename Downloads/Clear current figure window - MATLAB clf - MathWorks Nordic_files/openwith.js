/******/ (function(modules) { // webpackBootstrap
/******/ 	// The module cache
/******/ 	var installedModules = {};
/******/
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/
/******/ 		// Check if module is in cache
/******/ 		if(installedModules[moduleId]) {
/******/ 			return installedModules[moduleId].exports;
/******/ 		}
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = installedModules[moduleId] = {
/******/ 			i: moduleId,
/******/ 			l: false,
/******/ 			exports: {}
/******/ 		};
/******/
/******/ 		// Execute the module function
/******/ 		modules[moduleId].call(module.exports, module, module.exports, __webpack_require__);
/******/
/******/ 		// Flag the module as loaded
/******/ 		module.l = true;
/******/
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/
/******/
/******/ 	// expose the modules object (__webpack_modules__)
/******/ 	__webpack_require__.m = modules;
/******/
/******/ 	// expose the module cache
/******/ 	__webpack_require__.c = installedModules;
/******/
/******/ 	// identity function for calling harmony imports with the correct context
/******/ 	__webpack_require__.i = function(value) { return value; };
/******/
/******/ 	// define getter function for harmony exports
/******/ 	__webpack_require__.d = function(exports, name, getter) {
/******/ 		if(!__webpack_require__.o(exports, name)) {
/******/ 			Object.defineProperty(exports, name, {
/******/ 				configurable: false,
/******/ 				enumerable: true,
/******/ 				get: getter
/******/ 			});
/******/ 		}
/******/ 	};
/******/
/******/ 	// getDefaultExport function for compatibility with non-harmony modules
/******/ 	__webpack_require__.n = function(module) {
/******/ 		var getter = module && module.__esModule ?
/******/ 			function getDefault() { return module['default']; } :
/******/ 			function getModuleExports() { return module; };
/******/ 		__webpack_require__.d(getter, 'a', getter);
/******/ 		return getter;
/******/ 	};
/******/
/******/ 	// Object.prototype.hasOwnProperty.call
/******/ 	__webpack_require__.o = function(object, property) { return Object.prototype.hasOwnProperty.call(object, property); };
/******/
/******/ 	// __webpack_public_path__
/******/ 	__webpack_require__.p = "";
/******/
/******/ 	// Load entry module and return exports
/******/ 	return __webpack_require__(__webpack_require__.s = 13);
/******/ })
/************************************************************************/
/******/ ([
/* 0 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


Object.defineProperty(exports, "__esModule", {
    value: true
});
exports.getOriginForIE = exports.addMessageListeners = exports.sendPostMessage = exports.prepareContainer = exports.sendManifest = undefined;

var _containerFactory = __webpack_require__(4);

function sendManifest(container, manifest) {
    return new Promise(function (resolve, reject) {
        if (container.ready) {
            container.onStatus = function (status) {
                resolve({
                    status: status
                });
            };
            sendPostMessage(container, manifest);
            if (container.handshakeRequired) {
                setTimeout(function () {
                    if (container.status !== true) {
                        reject();
                    }
                }, 100);
            }
        } else {
            reject();
        }
    });
}

function prepareContainer(container) {
    return new Promise(function (resolve, reject) {
        if (container.ready) {
            resolve();
        } else {
            container.onReady = function (status) {
                if (status) {
                    resolve();
                } else {
                    reject();
                }
            };
        }
    });
}

function reloadManifest(existingContainer, manifest) {
    var containerOpts = existingContainer.containerOpts;
    existingContainer.cleanup();
    existingContainer.destroy();

    var container = (0, _containerFactory.getOrCreateContainer)(containerOpts);
    container.show();
    prepareContainer(container).then(function () {
        sendManifest(container, manifest);
    });
}

function sendPostMessage(container, msg, msgType) {
    container.postContainerMessage(msg, msgType);
}

function listenMessage(fn) {
    var addMethod = window.addEventListener ? "addEventListener" : "attachEvent";
    var removeMethod = window.addEventListener ? "removeEventListener" : "detachEvent";
    var messageEvent = addMethod === "attachEvent" ? "onmessage" : "message";
    window[addMethod](messageEvent, fn, false);
    return {
        remove: function remove() {
            window[removeMethod](messageEvent, fn);
        }
    };
}

function createMessageHandler(container) {
    return function (data) {
        if (data.origin !== container.getOrigin()) {
            return;
        }

        var msg = {};
        try {
            msg = JSON.parse(data.data);
        } catch (e) {
            return;
        }

        switch (msg.message) {
            case "ready":
                container.ready = true;
                if (container.onReady) {
                    container.onReady(msg.data);
                    container.onReady = null;
                }
                break;
            case "status":
                if (container.onStatus) {
                    container.onStatus(msg.data);
                    container.onStatus = null;
                }
                break;
            case "close":
                container.hide();
                break;
            case "disconnect":
                container.cleanup();
                container.destroy();
                break;
            case "reload":
                reloadManifest(container, msg.data);
                break;
            default:
                break;
        }
    };
}

function addMessageListeners(container) {
    return listenMessage(createMessageHandler(container));
}

function getOriginForIE(urlElement) {
    if (urlElement.port && (urlElement.protocol === "https:" && urlElement.port !== "443" || urlElement.protocol === "http:" && urlElement.port !== "80")) {
        return urlElement.protocol + "//" + urlElement.hostname + ":" + urlElement.port;
    }
    return urlElement.protocol + "//" + urlElement.hostname;
}

exports.sendManifest = sendManifest;
exports.prepareContainer = prepareContainer;
exports.sendPostMessage = sendPostMessage;
exports.addMessageListeners = addMessageListeners;
exports.getOriginForIE = getOriginForIE;

/***/ }),
/* 1 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


Object.defineProperty(exports, "__esModule", {
    value: true
});
exports.getFileNameFromUrl = exports.getExampleContent = exports.checkPath = undefined;

var _promisePolyfill = __webpack_require__(3);

var _promisePolyfill2 = _interopRequireDefault(_promisePolyfill);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

if (!window.Promise) {
    window.Promise = _promisePolyfill2.default;
}

function filterOrigin(origin) {
    var obj = {
        coordinates: {},
        repository: ""
    };
    if (origin.repository === "MATLAB Examples") {
        obj.repository = origin.repository;
        obj.coordinates.component = origin.coordinates.component;
        obj.coordinates.exampleId = origin.coordinates.exampleId;
    } else if (origin.repository === "MLDO") {
        obj.repository = origin.repository;
        obj.coordinates.path = origin.coordinates.path;
        obj.coordinates.mssToken = origin.coordinates.mssToken;
        obj.coordinates.gdsSessionId = origin.coordinates.gdsSessionId;
        obj.coordinates.gdsUrl = origin.coordinates.gdsUrl;
    } else if (origin.repository === "Standalone Artifact") {
        obj.repository = origin.repository;
        obj.coordinates.artifactLocation = origin.coordinates.artifactLocation;
    }
    return obj;
}

function checkPath(serviceEndpoint, origin, cb) {
    origin = filterOrigin(origin);
    var requestPayload = {
        version: "1.0.0",
        properties: ["EXISTS"],
        artifacts: [{ origin: origin }]
    };
    var request = new XMLHttpRequest();
    request.open('POST', serviceEndpoint + "artifacts/information");
    request.setRequestHeader("Content-Type", "application/json");

    if (cb) {
        request.onload = function () {
            var response = JSON.parse(request.response);
            if (response && response.artifacts[0].responseProperties.exists.response) {
                cb("true");
            } else {
                cb("false");
            }
        };
        request.send(JSON.stringify(requestPayload));
    } else {
        return new _promisePolyfill2.default(function (resolve, reject) {
            request.onload = function () {
                var response = JSON.parse(request.response);
                if (response && response.artifacts[0].responseProperties.exists.response) {
                    resolve({
                        exists: true
                    });
                } else {
                    resolve({
                        exists: false
                    });
                }
            };
            request.onerror = function () {
                reject(request.statusText);
            };
            request.send(JSON.stringify(requestPayload));
        });
    }
}

function getExampleContent(serviceEndpoint, origin) {
    // Create the POST request payload

    origin = filterOrigin(origin);
    var requestPayload = {
        version: "1.0.0",
        properties: ["CONTENT_LIVE_EDITOR"],
        artifacts: [{ origin: origin }]
    };

    // Create the Request to be sent to Open With Service
    var request = new XMLHttpRequest();
    request.open('POST', serviceEndpoint + "artifacts/information");
    request.setRequestHeader("Content-Type", "application/json");

    return new _promisePolyfill2.default(function (resolve, reject) {
        request.onload = function () {
            var response = JSON.parse(request.response);
            if (response && response.artifacts[0].faults.length > 0 && response.artifacts[0].faults[0] !== null) {
                // Fault has occured, handle it later
                resolve({
                    fault: response.artifacts[0].faults[0].message
                });
            } else if (response && response.artifacts[0].responseProperties.content) {
                resolve({
                    opcPackage: response.artifacts[0].responseProperties.content.data
                });
            }
        };
        request.onerror = function () {
            reject({
                error: request.statusText
            });
        };
        request.send(JSON.stringify(requestPayload));
    });
}

function getFileNameFromUrl(url) {
    // Remove any trailing slashes
    url = url.replace(/\/$/, "");
    return url.split('/').pop().split('#')[0].split('?')[0];
}

exports.checkPath = checkPath;
exports.getExampleContent = getExampleContent;
exports.getFileNameFromUrl = getFileNameFromUrl;

/***/ }),
/* 2 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


Object.defineProperty(exports, "__esModule", {
    value: true
});
/*
* This is a custom Error thrown when mandatory properties
* are not provided as part of input
* */
function InvalidInputError(message) {
    this.name = "InvalidInputError";
    this.message = message;
}

// So we can call toString() and show a error message instead of Object
InvalidInputError.prototype.toString = function () {
    return this.name + ': "' + this.message + '"';
};

/*
* This is a custom Error thrown when untrusted request/repository
* is encountered
* */
function UnauthorizedSource() {
    this.name = "UnauthorizedSourceError";
    this.message = "Repository was not recognized";
}

// So we can call toString() and show a error message instead of Object
UnauthorizedSource.prototype.toString = function () {
    return this.name + ': "' + this.message + '"';
};

exports.InvalidInputError = InvalidInputError;
exports.UnauthorizedSource = UnauthorizedSource;

/***/ }),
/* 3 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";
/* WEBPACK VAR INJECTION */(function(setImmediate) {

var _typeof = typeof Symbol === "function" && typeof Symbol.iterator === "symbol" ? function (obj) { return typeof obj; } : function (obj) { return obj && typeof Symbol === "function" && obj.constructor === Symbol && obj !== Symbol.prototype ? "symbol" : typeof obj; };

(function (root) {

  // Store setTimeout reference so promise-polyfill will be unaffected by
  // other code modifying setTimeout (like sinon.useFakeTimers())
  var setTimeoutFunc = setTimeout;

  function noop() {}

  // Polyfill for Function.prototype.bind
  function bind(fn, thisArg) {
    return function () {
      fn.apply(thisArg, arguments);
    };
  }

  function Promise(fn) {
    if (_typeof(this) !== 'object') throw new TypeError('Promises must be constructed via new');
    if (typeof fn !== 'function') throw new TypeError('not a function');
    this._state = 0;
    this._handled = false;
    this._value = undefined;
    this._deferreds = [];

    doResolve(fn, this);
  }

  function handle(self, deferred) {
    while (self._state === 3) {
      self = self._value;
    }
    if (self._state === 0) {
      self._deferreds.push(deferred);
      return;
    }
    self._handled = true;
    Promise._immediateFn(function () {
      var cb = self._state === 1 ? deferred.onFulfilled : deferred.onRejected;
      if (cb === null) {
        (self._state === 1 ? resolve : reject)(deferred.promise, self._value);
        return;
      }
      var ret;
      try {
        ret = cb(self._value);
      } catch (e) {
        reject(deferred.promise, e);
        return;
      }
      resolve(deferred.promise, ret);
    });
  }

  function resolve(self, newValue) {
    try {
      // Promise Resolution Procedure: https://github.com/promises-aplus/promises-spec#the-promise-resolution-procedure
      if (newValue === self) throw new TypeError('A promise cannot be resolved with itself.');
      if (newValue && ((typeof newValue === 'undefined' ? 'undefined' : _typeof(newValue)) === 'object' || typeof newValue === 'function')) {
        var then = newValue.then;
        if (newValue instanceof Promise) {
          self._state = 3;
          self._value = newValue;
          finale(self);
          return;
        } else if (typeof then === 'function') {
          doResolve(bind(then, newValue), self);
          return;
        }
      }
      self._state = 1;
      self._value = newValue;
      finale(self);
    } catch (e) {
      reject(self, e);
    }
  }

  function reject(self, newValue) {
    self._state = 2;
    self._value = newValue;
    finale(self);
  }

  function finale(self) {
    if (self._state === 2 && self._deferreds.length === 0) {
      Promise._immediateFn(function () {
        if (!self._handled) {
          Promise._unhandledRejectionFn(self._value);
        }
      });
    }

    for (var i = 0, len = self._deferreds.length; i < len; i++) {
      handle(self, self._deferreds[i]);
    }
    self._deferreds = null;
  }

  function Handler(onFulfilled, onRejected, promise) {
    this.onFulfilled = typeof onFulfilled === 'function' ? onFulfilled : null;
    this.onRejected = typeof onRejected === 'function' ? onRejected : null;
    this.promise = promise;
  }

  /**
   * Take a potentially misbehaving resolver function and make sure
   * onFulfilled and onRejected are only called once.
   *
   * Makes no guarantees about asynchrony.
   */
  function doResolve(fn, self) {
    var done = false;
    try {
      fn(function (value) {
        if (done) return;
        done = true;
        resolve(self, value);
      }, function (reason) {
        if (done) return;
        done = true;
        reject(self, reason);
      });
    } catch (ex) {
      if (done) return;
      done = true;
      reject(self, ex);
    }
  }

  Promise.prototype['catch'] = function (onRejected) {
    return this.then(null, onRejected);
  };

  Promise.prototype.then = function (onFulfilled, onRejected) {
    var prom = new this.constructor(noop);

    handle(this, new Handler(onFulfilled, onRejected, prom));
    return prom;
  };

  Promise.all = function (arr) {
    var args = Array.prototype.slice.call(arr);

    return new Promise(function (resolve, reject) {
      if (args.length === 0) return resolve([]);
      var remaining = args.length;

      function res(i, val) {
        try {
          if (val && ((typeof val === 'undefined' ? 'undefined' : _typeof(val)) === 'object' || typeof val === 'function')) {
            var then = val.then;
            if (typeof then === 'function') {
              then.call(val, function (val) {
                res(i, val);
              }, reject);
              return;
            }
          }
          args[i] = val;
          if (--remaining === 0) {
            resolve(args);
          }
        } catch (ex) {
          reject(ex);
        }
      }

      for (var i = 0; i < args.length; i++) {
        res(i, args[i]);
      }
    });
  };

  Promise.resolve = function (value) {
    if (value && (typeof value === 'undefined' ? 'undefined' : _typeof(value)) === 'object' && value.constructor === Promise) {
      return value;
    }

    return new Promise(function (resolve) {
      resolve(value);
    });
  };

  Promise.reject = function (value) {
    return new Promise(function (resolve, reject) {
      reject(value);
    });
  };

  Promise.race = function (values) {
    return new Promise(function (resolve, reject) {
      for (var i = 0, len = values.length; i < len; i++) {
        values[i].then(resolve, reject);
      }
    });
  };

  // Use polyfill for setImmediate for performance gains
  Promise._immediateFn = typeof setImmediate === 'function' && function (fn) {
    setImmediate(fn);
  } || function (fn) {
    setTimeoutFunc(fn, 0);
  };

  Promise._unhandledRejectionFn = function _unhandledRejectionFn(err) {
    if (typeof console !== 'undefined' && console) {
      console.warn('Possible Unhandled Promise Rejection:', err); // eslint-disable-line no-console
    }
  };

  /**
   * Set the immediate function to execute callbacks
   * @param fn {function} Function to execute
   * @deprecated
   */
  Promise._setImmediateFn = function _setImmediateFn(fn) {
    Promise._immediateFn = fn;
  };

  /**
   * Change the function to execute on unhandled rejection
   * @param {function} fn Function to execute on unhandled rejection
   * @deprecated
   */
  Promise._setUnhandledRejectionFn = function _setUnhandledRejectionFn(fn) {
    Promise._unhandledRejectionFn = fn;
  };

  if (typeof module !== 'undefined' && module.exports) {
    module.exports = Promise;
  } else if (!root.Promise) {
    root.Promise = Promise;
  }
})(undefined);
/* WEBPACK VAR INJECTION */}.call(exports, __webpack_require__(20).setImmediate))

/***/ }),
/* 4 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


Object.defineProperty(exports, "__esModule", {
    value: true
});
exports.getOrCreateContainer = undefined;

var _embeddedContainer = __webpack_require__(16);

var _windowContainer = __webpack_require__(17);

var _containerUtility = __webpack_require__(0);

var containers = {};

function createContainer(containerOpts) {
    var container = void 0,
        messageListener = void 0;
    switch (containerOpts.containerType) {
        case "embedded":
            container = (0, _embeddedContainer.createEmbeddedContainer)(containerOpts);
            break;
        case "external":
            container = (0, _windowContainer.createExternalContainer)(containerOpts);
            container.handshakeRequired = true;
            break;
        default:
            container = (0, _embeddedContainer.createEmbeddedContainer)(containerOpts);
            break;
    }

    containers[containerOpts.source] = container;

    container.ready = false;
    container.containerOpts = containerOpts;
    messageListener = (0, _containerUtility.addMessageListeners)(container);
    container.cleanup = function () {
        messageListener.remove();
        delete containers[containerOpts.source];
    };
    return container;
}

function getOrCreateContainer(containerOpts) {
    var container = containers[containerOpts.source];
    if (containerOpts.forceContainerCreation || !container || container.isClosed()) {
        container = createContainer(containerOpts);
    }
    return container;
}

exports.getOrCreateContainer = getOrCreateContainer;

/***/ }),
/* 5 */
/***/ (function(module, exports) {

var g;

// This works in non-strict mode
g = (function() {
	return this;
})();

try {
	// This works if eval is allowed (see CSP)
	g = g || Function("return this")() || (1,eval)("this");
} catch(e) {
	// This works if the window reference is available
	if(typeof window === "object")
		g = window;
}

// g can still be undefined, but nothing to do about it...
// We return undefined, instead of nothing here, so it's
// easier to handle this case. if(!global) { ...}

module.exports = g;


/***/ }),
/* 6 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


Object.defineProperty(exports, "__esModule", {
    value: true
});
exports.originToManifest = undefined;

var _artifactsUtils = __webpack_require__(1);

var CLASSIFICATION_LEARNER = "classificationLearner";

function originToManifest(serviceEndpoint, origin, examplesEndpoint) {

    var defaultViewer = origin.targetViewer ? origin.targetViewer : "LIVE_EDITOR";
    var manifest = {
        targetInformation: {
            editor: {
                activeEditorPath: getActiveEditorPath(origin)
            },
            targetViewer: defaultViewer
        },
        workerConfiguration: {
            mwaToken: origin.coordinates.mwaToken
        },
        udcLogging: {
            artifactId: prepareArtifactIdForUDC(origin)
        }
    };

    // Check if the input type is present
    manifest.repository = origin.repository;

    // Get the dependent file.s to be downloaded
    manifest.filesToAdd = origin.files;

    // Check if special case file sneed to be added
    if (origin.coordinates && origin.coordinates.component === "marketing") {
        // get the examples endpoint
        var files = [];
        switch (origin.coordinates.exampleId) {
            case "ExploreEvents":
                files.push({
                    source: examplesEndpoint + "/" + origin.coordinates.component + "/" + origin.coordinates.exampleId + "/ExploreEvents_Dependencies.zip",
                    credentials: {}
                });
                break;
            case "ExploringExoplanets":
                files.push({
                    source: examplesEndpoint + "/" + origin.coordinates.component + "/" + origin.coordinates.exampleId + "/ExploringExoplanets_Dependencies.zip",
                    credentials: {}
                });
                break;
            case "SunriseSunset":
                files.push({
                    source: examplesEndpoint + "/" + origin.coordinates.component + "/" + origin.coordinates.exampleId + "/SunriseSunset_Dependencies.zip",
                    credentials: {}
                });
                break;
            default:
                break;
        }
        manifest.filesToAdd = files;
    }

    // Set the working directory
    manifest.workingDirectory = getWorkingDirectory(origin);

    // Set the initialization code
    manifest.initializationCode = prepareInitializationCode(origin);

    // Just return the manifest Object without OpcContent for test purposes
    if (origin.testMode) {
        return manifest;
    }
    var needToRetrieveContent = shouldRetrieveContent(origin);

    var retrieveContent = needToRetrieveContent ? (0, _artifactsUtils.getExampleContent)(serviceEndpoint, origin) : new Promise(function (resolve) {
        resolve(origin.coordinates);
    });

    return new Promise(function (resolve, reject) {
        retrieveContent.then(function (status) {
            if (status.fault) {
                reject({ fault: "errorLoadingContentMessage" });
            } else {
                manifest.targetInformation.bufferedEditorContent = status.opcPackage;
                resolve(manifest);
            }
        });
    });
}

function getWorkingDirectory(origin) {
    var destinationPath = origin.workingDirectory;
    var repository = origin.repository;
    var path = "";
    if (repository === "MLDO") {
        path = undefined;
    } else {
        if (origin.workingDirectory) {
            path = "/MATLAB Drive/Examples/" + origin.workingDirectory;
        } else if (repository === "MATLAB Examples") {
            if (origin.coordinates.exampleId === CLASSIFICATION_LEARNER) {
                path = "ClassificationLearner";
            } else {
                path = "/MATLAB Drive/Examples/" + origin.coordinates.component + "/" + origin.coordinates.exampleId;
            }
        } else if (repository === "Standalone Artifact") {
            path = "/MATLAB Drive/Examples/";
            if (destinationPath && destinationPath !== "") {
                path += destinationPath;
            } else {
                path += createUniqueTimeStamp();
            }
        }
    }

    return path;
}

function createUniqueTimeStamp() {
    return new Date().getTime();
}

function getActiveEditorPath(origin) {
    var relPath = "/Untitled.mlx";
    if (origin.repository === "MLDO" && origin.coordinates.path) {
        relPath = origin.coordinates.path;
    } else if (origin.repository === "MATLAB Examples") {
        relPath = "/" + origin.coordinates.exampleId + ".mlx";
    } else if (origin.repository === "Standalone Artifact") {
        var filename = origin.coordinates.opcPackage ? "Untitled.mlx" : (0, _artifactsUtils.getFileNameFromUrl)(origin.coordinates.artifactLocation);
        relPath = "/" + filename;
    }
    return relPath;
}

function prepareArtifactIdForUDC(origin) {
    if (origin.repository === "MATLAB Examples") {
        return origin.coordinates.component + "/" + origin.coordinates.exampleId;
    } else if (origin.repository === "MLDO") {
        return origin.coordinates.path;
    } else if (origin.repository === "Standalone Artifact") {
        return origin.coordinates.artifactLocation;
    }
}

function prepareInitializationCode(origin) {
    switch (origin.repository) {
        case "MATLAB Examples":
            if (origin.coordinates.component === "dlextra") {
                origin.initializationCode = "clear;alexnet = alexnet;";
            } else if (origin.coordinates.component === "marketing") {
                if (origin.coordinates.exampleId === CLASSIFICATION_LEARNER) {
                    origin.initializationCode = "connector.internal.warmupTasks.classificationLearner_4_MOPilot_Launcher;" + "drawnow;" + "load('fisheriris.mat'); " + "data = load('fisheriris.mat'); " + "t = table(data.meas(:,1),data.meas(:,2),data.meas(:,3),data.meas(:,4),data.species," + "'VariableNames',{'SepalLength','SepalWidth','PetalLength','PetalWidth','species'});";
                } else if (origin.coordinates.exampleId === "ExploreEvents") {
                    origin.initializationCode = "load('weatherEvents.mat')";
                } else if (origin.coordinates.exampleId === "SunriseSunset") {
                    origin.initializationCode = "load('timeZones.mat')";
                } else if (origin.coordinates.exampleId === "ExploringExoplanets") {
                    origin.initializationCode = "exoplanets = readtable('exoplanets.xlsx','TextType','string');";
                }
            } else {
                origin.initializationCode = "clear;[~, workDir] = setupExample('" + origin.coordinates.component + "/" + origin.coordinates.exampleId + "');cd(workDir);";
            }
            break;
        case "MLDO":
            var fullFilePath = "/MATLAB Drive" + origin.coordinates.path;
            origin.initializationCode = "edit('" + fullFilePath + "');";
            break;
        default:
            break;
    }
    // If the client had provided some initialization code add semicolon at the end
    return origin.initializationCode && origin.initializationCode.lastIndexOf(";") !== origin.initializationCode.length - 1 ? origin.initializationCode += ";" : origin.initializationCode;
}

function shouldRetrieveContent(origin) {
    if (origin.coordinates.opcPackage) {
        return false;
    }
    if (origin.repository === "MATLAB Examples" && origin.coordinates.exampleId !== CLASSIFICATION_LEARNER) {
        return true;
    }
    if (origin.repository === "Standalone Artifact") {
        return true;
    }
    return false;
}

exports.originToManifest = originToManifest;

/***/ }),
/* 7 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


Object.defineProperty(exports, "__esModule", {
    value: true
});
exports.inputToOrigin = undefined;

var _exampleUtils = __webpack_require__(12);

var _inputValidator = __webpack_require__(14);

var _errorHandling = __webpack_require__(2);

var _artifactsUtils = __webpack_require__(1);

function inputToOrigin(input) {
    // Check if the Client sent string
    input = readClientInput(input);

    if (!(0, _inputValidator.validateInput)(input)) {
        throw new _errorHandling.InvalidInputError("Mandatory fields missing in input");
    }

    var origin = {
        coordinates: {},
        // Add the repository field
        repository: input.repository,

        // Get the dependent files to be downloaded
        files: input.files || undefined,

        // Set the working directory
        workingDirectory: input.workingDirectory || undefined,

        // Set the initialization code
        initializationCode: input.initializationCode || undefined
    };

    // Check if client has passed opcPackage, then default to certain behaviors
    if (input.opcPackage) {
        // Add the OPC Package if the client provided it
        if (input.opcPackage && typeof input.opcPackage !== "string") {
            input.opcPackage = JSON.stringify(input.opcPackage);
        }
        origin.coordinates.opcPackage = input.opcPackage;
    } else {
        switch (input.repository) {
            case "MLDO":
                origin.errMsg = input.path;
                origin.coordinates = createCoordinatesForMLDO(input);
                origin.targetViewer = "MATLAB_ONLINE";
                break;
            case "MATLAB Examples":
                origin.errMsg = input.component + "/" + input.exampleId;
                origin.coordinates = createCoordinatesForMATLABExamples(input);
                break;
            case "Standalone Artifact":
                origin.errMsg = prepareErrorMessageForStandaloneArtifact(input);
                origin.coordinates = createCoordinatesForStandaloneArtifact(input);
                break;
        }
    }
    return origin;
}

function prepareErrorMessageForStandaloneArtifact(input) {
    var errMsg = "";
    if (input.opcPackage) {
        errMsg = "Error loading OPC Package";
    } else {
        errMsg = (0, _artifactsUtils.getFileNameFromUrl)(input.livescript.locationURI);
    }
    return errMsg;
}

function createCoordinatesForMLDO(input) {
    return {
        path: input.path,
        mssToken: input.mssToken ? input.mssToken : undefined,
        gdsSessionId: input.gdsSessionId,
        gdsUrl: input.gdsUrl,
        mwaToken: input.mwaToken
    };
}

function createCoordinatesForMATLABExamples(input) {
    return {
        component: input.component,
        exampleId: input.exampleId
    };
}

function createCoordinatesForStandaloneArtifact(input) {
    return {
        artifactLocation: input.livescript.locationURI,
        mwaToken: input.mwaToken ? input.mwaToken : undefined
    };
}

function readClientInput(config) {
    return (0, _exampleUtils.convertToConfigObject)(config);
}

exports.inputToOrigin = inputToOrigin;

/***/ }),
/* 8 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


Object.defineProperty(exports, "__esModule", {
    value: true
});
// This class contains all the mandatory properties
var MATLABDriveOnline = function MATLABDriveOnline() {
    this.path = undefined;
    this.mwaToken = undefined;
    this.gdsUrl = undefined;
    /*
    * Given we don't support LIVE_EDITOR mode for MLDO anymore,
    * gdsSessionId and gdsUrl properties are no longer mandatory
    * */
    //this.gdsSessionId = undefined;
    //this.gdsUrl = undefined;
};

exports.MATLABDriveOnline = MATLABDriveOnline;

/***/ }),
/* 9 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


Object.defineProperty(exports, "__esModule", {
    value: true
});
// This class contains all the mandatory properties
var MATLABExamples = function MATLABExamples() {

    this.component = undefined;
    this.exampleId = undefined;
};

exports.MATLABExamples = MATLABExamples;

/***/ }),
/* 10 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


Object.defineProperty(exports, "__esModule", {
    value: true
});
// This class contains all the mandatory properties
var StandaloneArtifact = function StandaloneArtifact() {

    this.livescript = {
        locationURI: undefined
    };
};

exports.StandaloneArtifact = StandaloneArtifact;

/***/ }),
/* 11 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


Object.defineProperty(exports, "__esModule", {
    value: true
});
// This class contains all the mandatory properties
var StandaloneOpcPackage = function StandaloneOpcPackage() {

    this.opcPackage = undefined;
};

exports.StandaloneOpcPackage = StandaloneOpcPackage;

/***/ }),
/* 12 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


Object.defineProperty(exports, "__esModule", {
    value: true
});
exports.convertToConfigObject = undefined;

var _errorHandling = __webpack_require__(2);

var SOURCETYPE = "MATLAB Examples";

function convertToConfigObject(input) {
    if (typeof input === "string") {
        var config = {};
        var result = input.split("/");
        if (result.length > 1) {
            config.component = result[0];
            config.exampleId = result[1];
            config.repository = SOURCETYPE;
            return config;
        } else {
            throw new _errorHandling.InvalidInputError("Incorrect string input");
        }
    }
    return input;
}

exports.convertToConfigObject = convertToConfigObject;

/***/ }),
/* 13 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


Object.defineProperty(exports, "__esModule", {
    value: true
});

var _artifactsUtils = __webpack_require__(1);

var _containerFactory = __webpack_require__(4);

var _containerUtility = __webpack_require__(0);

var _manifest = __webpack_require__(6);

var _origin = __webpack_require__(7);

var _promisePolyfill = __webpack_require__(3);

var _promisePolyfill2 = _interopRequireDefault(_promisePolyfill);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

var endpoints = {
    "open-with-ui": "https://openwith-content.mathworks.com/ui/1.14.8/matlab/index.html",
    "open-with-service": "https://openwith.mathworks.com/",
    "open-with-examples": "https://openwith-content.mathworks.com/examples/1.7.2"
};

var currentConfigStr = void 0;
var globalConfig = null;

function getIframeSrc() {
    return endpoints["open-with-ui"] + "?ows=" + encodeURIComponent(endpoints["open-with-service"]);
}

function doesExampleExist(config, callback) {
    var origin = (0, _origin.inputToOrigin)(config);
    if (typeof callback === "function") {
        (0, _artifactsUtils.checkPath)(endpoints["open-with-service"], origin, function (result) {
            callback(result);
        });
    } else {
        return (0, _artifactsUtils.checkPath)(endpoints["open-with-service"], origin);
    }
}

function getDefaultContainerOpts(config) {
    var defaultContainerOpts = {
        source: getIframeSrc(),
        containerType: "embedded",
        node: document.body,
        background: false,
        forceContainerCreation: false,
        testMode: config && config.testMode ? config.testMode : false
    };
    if (config) {
        switch (config.repository) {
            case "ClassificationLearner":
                defaultContainerOpts.source = defaultContainerOpts.source.replace("index.html", "index_mgg.html");
                break;
            case "MLDO":
                if (config.gdsUrl.indexOf("https://gds.mathworks.com") === 0) {
                    defaultContainerOpts.source = "https://matlab.mathworks.com";
                } else {
                    // any other env just default to nightly including gds-motw-integ
                    defaultContainerOpts.source = "https://nightly-matlab.mathworks.com";
                }
                defaultContainerOpts.containerType = "external";
                defaultContainerOpts.windowName = "mowindow";
                break;
            default:
                if (typeof config === "string" && config.indexOf("classificationLearner") > 0) {
                    defaultContainerOpts.source = defaultContainerOpts.source.replace("index.html", "index_mgg.html");
                    defaultContainerOpts.background = true;
                }
                break;
        }
    }
    return defaultContainerOpts;
}

function mixinOptions(o1, o2) {
    var result = {};
    for (var prop in o2) {
        if (o2.hasOwnProperty(prop)) {
            result[prop] = o1 && o1[prop] ? o1[prop] : o2[prop];
        }
    }
    return result;
}

function startOpenWith(containerOpts) {
    if (!containerOpts || typeof containerOpts === "string" || !containerOpts.containerType) {
        containerOpts = getDefaultContainerOpts();
    }
    globalConfig = containerOpts;
    return (0, _containerFactory.getOrCreateContainer)(containerOpts);
}

function loadExample(config, containerOpts) {
    config = config ? config : globalConfig;
    containerOpts = mixinOptions(containerOpts, getDefaultContainerOpts(config));

    // Replace classificationLEarner_show with classificationLearner
    if (typeof config === "string" && config.indexOf("classificationLearner_show") > 0) {
        containerOpts.background = false;
        config = config.replace("classificationLearner_show", "classificationLearner");
    }
    var origin = (0, _origin.inputToOrigin)(config);
    var container = startOpenWith(containerOpts);
    if (!containerOpts.testMode) {
        if (!(containerOpts && containerOpts.background)) {
            container.show();
        }

        var configStr = config;
        if (typeof config !== "string") {
            configStr = JSON.stringify(config);
        }
        if (configStr.indexOf("MLDO") < 0) {
            if (currentConfigStr === configStr && container) {
                return new _promisePolyfill2.default(function (resolve) {
                    resolve();
                });
            }
        }

        currentConfigStr = configStr;

        var manifestReady = (0, _manifest.originToManifest)(endpoints["open-with-service"], origin, endpoints["open-with-examples"]);
        var containerReady = (0, _containerUtility.prepareContainer)(container);

        return new _promisePolyfill2.default(function (resolve, reject) {
            containerReady.then(function () {
                (0, _containerUtility.sendPostMessage)(container, {}, "loadEmptyExampleMessage");
                manifestReady.then(function (manifest) {
                    waitAndThenRetrySendingManifest(container, manifest, 50, resolve, reject);
                }, function () {
                    (0, _containerUtility.sendPostMessage)(container, origin.errMsg, "errorLoadingContentMessage");
                    reject();
                });
            });
        });
    } else {
        return container;
    }
}

function waitAndThenRetrySendingManifest(container, manifest, ms, resolve, reject) {
    // In case the promise still gets rejected, we will retry
    setTimeout(function () {
        (0, _containerUtility.sendManifest)(container, manifest).then(function () {
            resolve();
        }, function () {
            if (ms > 300) {
                reject();
            } else {
                ms = ms * 2;
                waitAndThenRetrySendingManifest(container, manifest, ms, resolve, reject);
            }
        });
    }, ms);
}

function setEndpoint(name, loc) {
    endpoints[name] = loc;
}

var ow = {
    doesExampleExist: doesExampleExist, // will be deprecated
    startOpenWith: startOpenWith, // will be deprecated
    loadExample: loadExample, // will be deprecated
    load: loadExample,
    start: startOpenWith,
    createEmbeddedIframe: startOpenWith,
    describe: doesExampleExist,
    _setEndpoint: setEndpoint
};
window.ow = ow;
exports.default = ow;

/***/ }),
/* 14 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


Object.defineProperty(exports, "__esModule", {
    value: true
});
exports.validateInput = undefined;

var _typeof = typeof Symbol === "function" && typeof Symbol.iterator === "symbol" ? function (obj) { return typeof obj; } : function (obj) { return obj && typeof Symbol === "function" && obj.constructor === Symbol && obj !== Symbol.prototype ? "symbol" : typeof obj; };

var _StandaloneArtifact = __webpack_require__(10);

var _StandaloneOpcPackage = __webpack_require__(11);

var _MATLABExamples = __webpack_require__(9);

var _MATLABDriveOnline = __webpack_require__(8);

var _errorHandling = __webpack_require__(2);

var SUPPORTED_REPOSITORIES = ["MATLAB Examples", "Standalone Artifact", "MLDO"];

function validateInput(input) {
    var factoryProps = [];

    if (verifyTrustedRepository(input.repository)) {
        switch (input.repository) {
            case "MATLAB Examples":
                factoryProps = extractAllProperties(new _MATLABExamples.MATLABExamples(), factoryProps, false);
                //obj = Object.assign(new MATLABExamples, JSON.parse(JSON.stringify(input)));
                break;
            case "Standalone Artifact":
                /*
                * StandaloneArtifact repository has two operating modes i.e.
                * 1) Location of MLX file
                *  OR
                * 2) Directly provide an OPC Package
                * In order to maintain backwards compatibility and keep one repository name for both these
                * modes, we have to create instances from two different classes i.e.
                * StandaloneArtifact.js for location of MLX file
                * StandaloneOpcPackage.js for directly loading OPC Package
                * */
                if (input.opcPackage) {
                    factoryProps = extractAllProperties(new _StandaloneOpcPackage.StandaloneOpcPackage(), factoryProps, false);
                } else {
                    factoryProps = extractAllProperties(new _StandaloneArtifact.StandaloneArtifact(), factoryProps, false);
                }
                break;
            case "MLDO":
                factoryProps = extractAllProperties(new _MATLABDriveOnline.MATLABDriveOnline(), factoryProps, false);
                break;
        }
        // Check if all mandatory properties are present
        return checkMandatoryPropertiesExist(factoryProps, input);
    } else {
        throw new _errorHandling.UnauthorizedSource();
    }
}

function verifyTrustedRepository(repository) {
    return SUPPORTED_REPOSITORIES.indexOf(repository) >= 0;
}
/*
* This function goes over each attribute defined in a JavaScript Object
* inputObj: This is the original JavaScript object provided
* allProperties: This is an array which will contain all the properties
* excludeNullValues: This flag can either be true/false
*     a) true: This will exclude any attribute which is empty/undefined
*     b) false: This will include all attributes even if they are empty/undefined
* */
function extractAllProperties(inputObj, allProperties, excludeNullValues) {
    if ((typeof inputObj === "undefined" ? "undefined" : _typeof(inputObj)) === "object") {
        for (var key in inputObj) {
            if (excludeNullValues) {
                if (inputObj[key]) {
                    allProperties.push(key);
                }
            } else {
                allProperties.push(key);
            }
            extractAllProperties(inputObj[key], allProperties, excludeNullValues);
        }
    }
    return allProperties;
}

/*
* This function checks if all the mandatory properties are present or not
* factoryProps: This is a list of all mandatory properties based on Repository type
* input: This is JavaScript object passed by the client
* */

function checkMandatoryPropertiesExist(factoryProps, input) {
    var parsedProps = [];
    // Collect all the properties provided by the client
    parsedProps = extractAllProperties(input, parsedProps, true);
    // Check if provided properties include the mandatory properties
    return factoryProps.every(function (property) {
        return parsedProps.indexOf(property) >= 0;
    });
}

exports.validateInput = validateInput;

/***/ }),
/* 15 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


Object.defineProperty(exports, "__esModule", {
    value: true
});
var CONTROLLED_STYLES = ['width', 'height', 'minHeight', 'maxHeight', 'overflow'];

function createSizeProperties(width, height) {
    var widthText = width + 'px';
    var heightText = height + 'px';
    return {
        width: widthText,
        height: heightText,
        minHeight: heightText,
        maxHeight: heightText,
        overflow: 'hidden'
    };
}

function writeSizeProperties(node, props) {
    CONTROLLED_STYLES.forEach(function (prop) {
        node.style[prop] = props[prop];
    });
}

function readSizeProperties(node) {
    var result = {};
    CONTROLLED_STYLES.forEach(function (prop) {
        result[prop] = node.style[prop];
    });
    return result;
}

function saveSizeProperties(node) {
    var props = readSizeProperties(node);
    node.setAttribute("data-size-props", JSON.stringify(props));
}

function restoreSizeProperties(node) {
    var props = JSON.parse(node.getAttribute("data-size-props"));
    if (props) {
        writeSizeProperties(node, props);
    }
    node.removeAttribute("data-size-props");
}

function snapshotDocumentSize() {
    var html = document.documentElement;
    var body = document.body;
    saveSizeProperties(html);
    saveSizeProperties(body);
}

function constrainDocumentSize() {
    var html = document.documentElement;
    var body = document.body;
    var width = window.innerWidth > 0 ? window.innerWidth : screen.width;
    var height = window.innerHeight > 0 ? window.innerHeight : screen.height;
    var props = createSizeProperties(width, height);

    writeSizeProperties(html, props);
    writeSizeProperties(body, props);
}

function unconstrainDocumentSize() {
    var html = document.documentElement;
    var body = document.body;
    restoreSizeProperties(html);
    restoreSizeProperties(body);
}

exports.snapshotDocumentSize = snapshotDocumentSize;
exports.constrainDocumentSize = constrainDocumentSize;
exports.unconstrainDocumentSize = unconstrainDocumentSize;

/***/ }),
/* 16 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


Object.defineProperty(exports, "__esModule", {
    value: true
});
exports.createEmbeddedContainer = undefined;

var _documentSizeConstrainer = __webpack_require__(15);

var _containerUtility = __webpack_require__(0);

var flags = {
    ipad: /iPad/.test(navigator.userAgent) && !document.MSStream
};

var config = {
    NO_PADDING: flags.ipad,
    FLUID_CONTAINER_WIDTH: !flags.ipad,
    CONSTRAIN_OWNER_DOCUMENT: flags.ipad
};

function getPaddingBasedOnBrowserSize() {
    var w = window.innerWidth,
        h = window.innerHeight,
        paddingDivideByW = 1,
        paddingDivideByH = 1;

    if (config.NO_PADDING) {
        return {
            w: 0,
            h: 0
        };
    }

    if (w < 1000) {
        paddingDivideByW = 4;
    } else if (w < 1200) {
        paddingDivideByW = 2;
    }

    if (h < 1000) {
        paddingDivideByH = 4;
    } else if (h < 1200) {
        paddingDivideByH = 2;
    }

    return {
        w: Math.floor(10 / paddingDivideByW),
        h: Math.floor(10 / paddingDivideByH)
    };
}

function setBaseStyles(outerContainer, innerContainer, iframe, closeButton) {
    outerContainer.style.top = '0';
    outerContainer.style.left = '0';
    outerContainer.style.display = 'none';
    outerContainer.style.position = 'fixed';
    outerContainer.style.backgroundColor = 'rgba(0,0,0,0.75)';
    outerContainer.style.zIndex = '1036';

    innerContainer.style.display = 'none';
    innerContainer.style.position = 'absolute';
    innerContainer.style.backgroundColor = 'white';
    innerContainer.style.border = '2px solid white';
    innerContainer.style.zIndex = '1002';
    innerContainer.style.overflow = 'hidden';
    innerContainer.style.borderRadius = '5px';

    iframe.style.height = '100%';
    iframe.style.width = '100%';
    iframe.style.position = 'absolute';
    iframe.style.border = '0px';
    iframe.style.left = '0px';
    iframe.style.right = '0px';

    closeButton.style.top = "5px";
    closeButton.style.right = "6px";
    closeButton.style.position = 'absolute';
    closeButton.style.zIndex = "1";
    closeButton.style.margin = "0";
    closeButton.style.lineHeight = "14px";
    closeButton.style.padding = "0px 4px";

    updateContainerSizes(outerContainer, innerContainer);
}

function updateContainerSizes(outerContainer, innerContainer) {
    var width,
        height,
        padding = getPaddingBasedOnBrowserSize();

    innerContainer.style.top = padding.h + '%';
    innerContainer.style.left = padding.w + '%';

    if (config.FLUID_CONTAINER_WIDTH) {
        outerContainer.style.width = '100%';
        outerContainer.style.height = '100%';
        innerContainer.style.width = 100 - padding.w * 2 + '%';
        innerContainer.style.height = 100 - padding.h * 2 + '%';
    } else {
        width = window.innerWidth > 0 ? window.innerWidth : screen.width;
        height = window.innerHeight > 0 ? window.innerHeight : screen.height;

        outerContainer.style.width = width + 'px';
        outerContainer.style.height = height + 'px';
        innerContainer.style.width = width + 'px';
        innerContainer.style.height = (width > height ? height : 0.66 * height) + 'px';
    }
}

function createResizeHandler(outerContainer, innerContainer) {
    return function (evt) {
        if (outerContainer && outerContainer.style.display !== 'none') {
            updateContainerSizes(outerContainer, innerContainer);
            if (config.CONSTRAIN_OWNER_DOCUMENT) {
                (0, _documentSizeConstrainer.constrainDocumentSize)();
            }
        }
    };
}

function createDeviceOrientationListener(onRotate) {
    var isLandscape = window.innerWidth > window.innerHeight;
    return function () {
        var current = window.innerWidth > window.innerHeight;
        if (current !== isLandscape) {
            isLandscape = current;
            onRotate.call();
        }
    };
}

function createKeyDownListener(cb) {
    return function (evt) {
        evt = evt || window.event;

        var isEscape = false;
        if ("key" in evt) {
            isEscape = evt.key == "Escape" || evt.key == "Esc";
        } else {
            isEscape = evt.keyCode == 27;
        }

        if (isEscape) {
            cb();
        }
    };
}

function createEmbeddedContainer(containerConfig) {
    var src = containerConfig.source;
    var node = containerConfig.node;
    var docFragment = document.createDocumentFragment();
    var outerContainer = document.createElement("div");
    var innerContainer = document.createElement("div");
    var iframe = document.createElement("iframe");
    var closeButton = document.createElement("button");
    closeButton.innerText = "x";
    iframe.setAttribute('src', src);
    iframe.setAttribute('data-openwith-iframe', "openwithIframe");
    var config = containerConfig || {};

    innerContainer.appendChild(closeButton);
    innerContainer.appendChild(iframe);
    outerContainer.appendChild(innerContainer);
    docFragment.appendChild(outerContainer);

    var show = function show() {
        if (config.CONSTRAIN_OWNER_DOCUMENT) {
            (0, _documentSizeConstrainer.snapshotDocumentSize)();
            (0, _documentSizeConstrainer.constrainDocumentSize)();
        }
        updateContainerSizes(outerContainer, innerContainer);
        outerContainer.style.display = "block";
        innerContainer.style.display = "block";
    };

    var hide = function hide() {
        if (config.CONSTRAIN_OWNER_DOCUMENT) {
            (0, _documentSizeConstrainer.unconstrainDocumentSize)();
        }
        outerContainer.style.display = "none";
        innerContainer.style.display = "none";
    };

    var setSource = function setSource(newSource) {
        iframe.src = newSource;
    };

    var getSource = function getSource() {
        return iframe.src;
    };

    var postMessage = function postMessage(msg, msgType) {
        msgType = typeof msgType === "undefined" ? "openWithMessage" : msgType;
        iframe.contentWindow.postMessage(msgType + ":" + JSON.stringify(msg), '*');
    };

    var handleDrop = function handleDrop(e) {
        e.preventDefault();
        return false;
    };

    var keydownListener = createKeyDownListener(hide);
    var resizeHandler = createResizeHandler(outerContainer, innerContainer);
    var deviceOrientationListener = createDeviceOrientationListener(resizeHandler);

    var addListeners = function addListeners() {
        if (config.closeOnBackgroundClick !== false) {
            outerContainer.addEventListener("click", hide);
            innerContainer.addEventListener("click", hide);
        }
        closeButton.addEventListener("click", hide);
        document.addEventListener("keydown", keydownListener);
        if (!config.FLUID_CONTAINER_WIDTH) {
            if (window.DeviceOrientationEvent) {
                window.addEventListener('deviceorientation', deviceOrientationListener, false);
            } else {
                window.addEventListener("orientationchange", resizeHandler);
            }
        }
    };

    var addDropListener = function addDropListener() {
        outerContainer.addEventListener("dragover", handleDrop);
        outerContainer.addEventListener("drop", handleDrop);
    };

    var removeListeners = function removeListeners() {
        outerContainer.removeEventListener("click", hide);
        innerContainer.removeEventListener("click", hide);
        document.removeEventListener("keydown", keydownListener);
        if (!config.FLUID_CONTAINER_WIDTH) {
            if (window.DeviceOrientationEvent) {
                window.removeEventListener('deviceorientation', deviceOrientationListener);
            } else {
                window.removeEventListener("orientationchange", resizeHandler);
            }
        }
    };

    var destroy = function destroy() {
        //After being added to document, the doc fragment is empty, and so to remove the container from
        // the document, we must specify the container directly.
        node.removeChild(outerContainer);
        removeListeners();
        if (config.CONSTRAIN_OWNER_DOCUMENT) {
            (0, _documentSizeConstrainer.unconstrainDocumentSize)();
        }
    };

    var removeIframe = function removeIframe() {
        //After being added to document, the doc fragment is empty, and so to remove the container from
        // the document, we must specify the container directly.
        innerContainer.removeChild(iframe);
        innerContainer.removeChild(closeButton);
    };

    var insertIframe = function insertIframe(src) {
        iframe.setAttribute('src', src);
        iframe.setAttribute('data-openwith-iframe', "openwithIframe");
        innerContainer.appendChild(closeButton);
        innerContainer.appendChild(iframe);
        //setBaseStyles(outerContainer, innerContainer, iframe, closeButton);
        addListeners();
        addDropListener();
    };

    var getOrigin = function getOrigin(container) {
        var urlElement = document.createElement('a');
        urlElement.href = iframe.src;
        // IE does not add origin attribute when creating "a.href" tag
        return urlElement.origin ? urlElement.origin : (0, _containerUtility.getOriginForIE)(urlElement);
    };

    var isClosed = function isClosed() {
        innerContainer.getElementsByTagName("iframe").length > 0 ? false : true;
    };

    setBaseStyles(outerContainer, innerContainer, iframe, closeButton);
    addListeners();
    addDropListener();
    // check if running in test mode
    if (!containerConfig.testMode) {
        node.appendChild(docFragment);
    }

    return {
        show: show,
        hide: hide,
        destroy: destroy,
        setSource: setSource,
        getSource: getSource,
        postContainerMessage: postMessage,
        remove: removeIframe,
        insert: insertIframe,
        getOrigin: getOrigin,
        isClosed: isClosed
    };
}

exports.createEmbeddedContainer = createEmbeddedContainer;

/***/ }),
/* 17 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";


Object.defineProperty(exports, "__esModule", {
    value: true
});
exports.createExternalContainer = undefined;

var _containerUtility = __webpack_require__(0);

function createExternalContainer(containerConfig) {
    var url = void 0,
        windowName = void 0,
        windowContainer = void 0;
    url = containerConfig.source;
    windowName = containerConfig.windowName;

    // check if running in test mode
    if (!containerConfig.testMode) {
        windowContainer = window.open(url, windowName);
    }

    var show = function show() {
        if (windowContainer) {
            windowContainer.focus();
        }
    };

    var hide = function hide() {
        if (windowContainer) {
            windowContainer.close();
        }
    };

    var destroy = function destroy() {
        if (windowContainer) {
            windowContainer.close();
        }
    };

    var setSource = function setSource(newSource) {
        if (windowContainer) {
            windowContainer.location.href = newSource;
        }
    };

    var getSource = function getSource() {
        return url;
    };

    var isClosed = function isClosed() {
        if (windowContainer) {
            return windowContainer.closed;
        }
    };

    var postContainerMessage = function postContainerMessage(msg, msgType) {
        if (windowContainer) {
            msgType = typeof msgType === "undefined" ? "openWithMessage" : msgType;
            windowContainer.postMessage(msgType + ":" + JSON.stringify(msg), '*');
        }
    };

    var getOrigin = function getOrigin() {
        var urlElement = document.createElement('a');
        urlElement.href = url;
        // IE does not add origin attribute when creating "a.href" tag
        return urlElement.origin ? urlElement.origin : (0, _containerUtility.getOriginForIE)(urlElement);
    };

    return {
        show: show,
        hide: hide,
        destroy: destroy,
        setSource: setSource,
        getSource: getSource,
        postContainerMessage: postContainerMessage,
        getOrigin: getOrigin,
        isClosed: isClosed
    };
}

exports.createExternalContainer = createExternalContainer;

/***/ }),
/* 18 */
/***/ (function(module, exports) {

// shim for using process in browser
var process = module.exports = {};

// cached from whatever global is present so that test runners that stub it
// don't break things.  But we need to wrap it in a try catch in case it is
// wrapped in strict mode code which doesn't define any globals.  It's inside a
// function because try/catches deoptimize in certain engines.

var cachedSetTimeout;
var cachedClearTimeout;

function defaultSetTimout() {
    throw new Error('setTimeout has not been defined');
}
function defaultClearTimeout () {
    throw new Error('clearTimeout has not been defined');
}
(function () {
    try {
        if (typeof setTimeout === 'function') {
            cachedSetTimeout = setTimeout;
        } else {
            cachedSetTimeout = defaultSetTimout;
        }
    } catch (e) {
        cachedSetTimeout = defaultSetTimout;
    }
    try {
        if (typeof clearTimeout === 'function') {
            cachedClearTimeout = clearTimeout;
        } else {
            cachedClearTimeout = defaultClearTimeout;
        }
    } catch (e) {
        cachedClearTimeout = defaultClearTimeout;
    }
} ())
function runTimeout(fun) {
    if (cachedSetTimeout === setTimeout) {
        //normal enviroments in sane situations
        return setTimeout(fun, 0);
    }
    // if setTimeout wasn't available but was latter defined
    if ((cachedSetTimeout === defaultSetTimout || !cachedSetTimeout) && setTimeout) {
        cachedSetTimeout = setTimeout;
        return setTimeout(fun, 0);
    }
    try {
        // when when somebody has screwed with setTimeout but no I.E. maddness
        return cachedSetTimeout(fun, 0);
    } catch(e){
        try {
            // When we are in I.E. but the script has been evaled so I.E. doesn't trust the global object when called normally
            return cachedSetTimeout.call(null, fun, 0);
        } catch(e){
            // same as above but when it's a version of I.E. that must have the global object for 'this', hopfully our context correct otherwise it will throw a global error
            return cachedSetTimeout.call(this, fun, 0);
        }
    }


}
function runClearTimeout(marker) {
    if (cachedClearTimeout === clearTimeout) {
        //normal enviroments in sane situations
        return clearTimeout(marker);
    }
    // if clearTimeout wasn't available but was latter defined
    if ((cachedClearTimeout === defaultClearTimeout || !cachedClearTimeout) && clearTimeout) {
        cachedClearTimeout = clearTimeout;
        return clearTimeout(marker);
    }
    try {
        // when when somebody has screwed with setTimeout but no I.E. maddness
        return cachedClearTimeout(marker);
    } catch (e){
        try {
            // When we are in I.E. but the script has been evaled so I.E. doesn't  trust the global object when called normally
            return cachedClearTimeout.call(null, marker);
        } catch (e){
            // same as above but when it's a version of I.E. that must have the global object for 'this', hopfully our context correct otherwise it will throw a global error.
            // Some versions of I.E. have different rules for clearTimeout vs setTimeout
            return cachedClearTimeout.call(this, marker);
        }
    }



}
var queue = [];
var draining = false;
var currentQueue;
var queueIndex = -1;

function cleanUpNextTick() {
    if (!draining || !currentQueue) {
        return;
    }
    draining = false;
    if (currentQueue.length) {
        queue = currentQueue.concat(queue);
    } else {
        queueIndex = -1;
    }
    if (queue.length) {
        drainQueue();
    }
}

function drainQueue() {
    if (draining) {
        return;
    }
    var timeout = runTimeout(cleanUpNextTick);
    draining = true;

    var len = queue.length;
    while(len) {
        currentQueue = queue;
        queue = [];
        while (++queueIndex < len) {
            if (currentQueue) {
                currentQueue[queueIndex].run();
            }
        }
        queueIndex = -1;
        len = queue.length;
    }
    currentQueue = null;
    draining = false;
    runClearTimeout(timeout);
}

process.nextTick = function (fun) {
    var args = new Array(arguments.length - 1);
    if (arguments.length > 1) {
        for (var i = 1; i < arguments.length; i++) {
            args[i - 1] = arguments[i];
        }
    }
    queue.push(new Item(fun, args));
    if (queue.length === 1 && !draining) {
        runTimeout(drainQueue);
    }
};

// v8 likes predictible objects
function Item(fun, array) {
    this.fun = fun;
    this.array = array;
}
Item.prototype.run = function () {
    this.fun.apply(null, this.array);
};
process.title = 'browser';
process.browser = true;
process.env = {};
process.argv = [];
process.version = ''; // empty string to avoid regexp issues
process.versions = {};

function noop() {}

process.on = noop;
process.addListener = noop;
process.once = noop;
process.off = noop;
process.removeListener = noop;
process.removeAllListeners = noop;
process.emit = noop;
process.prependListener = noop;
process.prependOnceListener = noop;

process.listeners = function (name) { return [] }

process.binding = function (name) {
    throw new Error('process.binding is not supported');
};

process.cwd = function () { return '/' };
process.chdir = function (dir) {
    throw new Error('process.chdir is not supported');
};
process.umask = function() { return 0; };


/***/ }),
/* 19 */
/***/ (function(module, exports, __webpack_require__) {

/* WEBPACK VAR INJECTION */(function(global, process) {(function (global, undefined) {
    "use strict";

    if (global.setImmediate) {
        return;
    }

    var nextHandle = 1; // Spec says greater than zero
    var tasksByHandle = {};
    var currentlyRunningATask = false;
    var doc = global.document;
    var registerImmediate;

    function setImmediate(callback) {
      // Callback can either be a function or a string
      if (typeof callback !== "function") {
        callback = new Function("" + callback);
      }
      // Copy function arguments
      var args = new Array(arguments.length - 1);
      for (var i = 0; i < args.length; i++) {
          args[i] = arguments[i + 1];
      }
      // Store and register the task
      var task = { callback: callback, args: args };
      tasksByHandle[nextHandle] = task;
      registerImmediate(nextHandle);
      return nextHandle++;
    }

    function clearImmediate(handle) {
        delete tasksByHandle[handle];
    }

    function run(task) {
        var callback = task.callback;
        var args = task.args;
        switch (args.length) {
        case 0:
            callback();
            break;
        case 1:
            callback(args[0]);
            break;
        case 2:
            callback(args[0], args[1]);
            break;
        case 3:
            callback(args[0], args[1], args[2]);
            break;
        default:
            callback.apply(undefined, args);
            break;
        }
    }

    function runIfPresent(handle) {
        // From the spec: "Wait until any invocations of this algorithm started before this one have completed."
        // So if we're currently running a task, we'll need to delay this invocation.
        if (currentlyRunningATask) {
            // Delay by doing a setTimeout. setImmediate was tried instead, but in Firefox 7 it generated a
            // "too much recursion" error.
            setTimeout(runIfPresent, 0, handle);
        } else {
            var task = tasksByHandle[handle];
            if (task) {
                currentlyRunningATask = true;
                try {
                    run(task);
                } finally {
                    clearImmediate(handle);
                    currentlyRunningATask = false;
                }
            }
        }
    }

    function installNextTickImplementation() {
        registerImmediate = function(handle) {
            process.nextTick(function () { runIfPresent(handle); });
        };
    }

    function canUsePostMessage() {
        // The test against `importScripts` prevents this implementation from being installed inside a web worker,
        // where `global.postMessage` means something completely different and can't be used for this purpose.
        if (global.postMessage && !global.importScripts) {
            var postMessageIsAsynchronous = true;
            var oldOnMessage = global.onmessage;
            global.onmessage = function() {
                postMessageIsAsynchronous = false;
            };
            global.postMessage("", "*");
            global.onmessage = oldOnMessage;
            return postMessageIsAsynchronous;
        }
    }

    function installPostMessageImplementation() {
        // Installs an event handler on `global` for the `message` event: see
        // * https://developer.mozilla.org/en/DOM/window.postMessage
        // * http://www.whatwg.org/specs/web-apps/current-work/multipage/comms.html#crossDocumentMessages

        var messagePrefix = "setImmediate$" + Math.random() + "$";
        var onGlobalMessage = function(event) {
            if (event.source === global &&
                typeof event.data === "string" &&
                event.data.indexOf(messagePrefix) === 0) {
                runIfPresent(+event.data.slice(messagePrefix.length));
            }
        };

        if (global.addEventListener) {
            global.addEventListener("message", onGlobalMessage, false);
        } else {
            global.attachEvent("onmessage", onGlobalMessage);
        }

        registerImmediate = function(handle) {
            global.postMessage(messagePrefix + handle, "*");
        };
    }

    function installMessageChannelImplementation() {
        var channel = new MessageChannel();
        channel.port1.onmessage = function(event) {
            var handle = event.data;
            runIfPresent(handle);
        };

        registerImmediate = function(handle) {
            channel.port2.postMessage(handle);
        };
    }

    function installReadyStateChangeImplementation() {
        var html = doc.documentElement;
        registerImmediate = function(handle) {
            // Create a <script> element; its readystatechange event will be fired asynchronously once it is inserted
            // into the document. Do so, thus queuing up the task. Remember to clean up once it's been called.
            var script = doc.createElement("script");
            script.onreadystatechange = function () {
                runIfPresent(handle);
                script.onreadystatechange = null;
                html.removeChild(script);
                script = null;
            };
            html.appendChild(script);
        };
    }

    function installSetTimeoutImplementation() {
        registerImmediate = function(handle) {
            setTimeout(runIfPresent, 0, handle);
        };
    }

    // If supported, we should attach to the prototype of global, since that is where setTimeout et al. live.
    var attachTo = Object.getPrototypeOf && Object.getPrototypeOf(global);
    attachTo = attachTo && attachTo.setTimeout ? attachTo : global;

    // Don't get fooled by e.g. browserify environments.
    if ({}.toString.call(global.process) === "[object process]") {
        // For Node.js before 0.9
        installNextTickImplementation();

    } else if (canUsePostMessage()) {
        // For non-IE10 modern browsers
        installPostMessageImplementation();

    } else if (global.MessageChannel) {
        // For web workers, where supported
        installMessageChannelImplementation();

    } else if (doc && "onreadystatechange" in doc.createElement("script")) {
        // For IE 6–8
        installReadyStateChangeImplementation();

    } else {
        // For older browsers
        installSetTimeoutImplementation();
    }

    attachTo.setImmediate = setImmediate;
    attachTo.clearImmediate = clearImmediate;
}(typeof self === "undefined" ? typeof global === "undefined" ? this : global : self));

/* WEBPACK VAR INJECTION */}.call(exports, __webpack_require__(5), __webpack_require__(18)))

/***/ }),
/* 20 */
/***/ (function(module, exports, __webpack_require__) {

/* WEBPACK VAR INJECTION */(function(global) {var scope = (typeof global !== "undefined" && global) ||
            (typeof self !== "undefined" && self) ||
            window;
var apply = Function.prototype.apply;

// DOM APIs, for completeness

exports.setTimeout = function() {
  return new Timeout(apply.call(setTimeout, scope, arguments), clearTimeout);
};
exports.setInterval = function() {
  return new Timeout(apply.call(setInterval, scope, arguments), clearInterval);
};
exports.clearTimeout =
exports.clearInterval = function(timeout) {
  if (timeout) {
    timeout.close();
  }
};

function Timeout(id, clearFn) {
  this._id = id;
  this._clearFn = clearFn;
}
Timeout.prototype.unref = Timeout.prototype.ref = function() {};
Timeout.prototype.close = function() {
  this._clearFn.call(scope, this._id);
};

// Does not start the time, just sets up the members needed.
exports.enroll = function(item, msecs) {
  clearTimeout(item._idleTimeoutId);
  item._idleTimeout = msecs;
};

exports.unenroll = function(item) {
  clearTimeout(item._idleTimeoutId);
  item._idleTimeout = -1;
};

exports._unrefActive = exports.active = function(item) {
  clearTimeout(item._idleTimeoutId);

  var msecs = item._idleTimeout;
  if (msecs >= 0) {
    item._idleTimeoutId = setTimeout(function onTimeout() {
      if (item._onTimeout)
        item._onTimeout();
    }, msecs);
  }
};

// setimmediate attaches itself to the global object
__webpack_require__(19);
// On some exotic environments, it's not clear which object `setimmediate` was
// able to install onto.  Search each possibility in the same order as the
// `setimmediate` library.
exports.setImmediate = (typeof self !== "undefined" && self.setImmediate) ||
                       (typeof global !== "undefined" && global.setImmediate) ||
                       (this && this.setImmediate);
exports.clearImmediate = (typeof self !== "undefined" && self.clearImmediate) ||
                         (typeof global !== "undefined" && global.clearImmediate) ||
                         (this && this.clearImmediate);

/* WEBPACK VAR INJECTION */}.call(exports, __webpack_require__(5)))

/***/ })
/******/ ]);
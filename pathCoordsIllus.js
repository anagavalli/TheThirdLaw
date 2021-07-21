// JSON2: https://github.com/douglascrockford/JSON-js/blob/master/json2.js
// used to allow JSON parsing/exporting in Illustrator scripting
// BEGIN JSON SECTION
if (typeof JSON !== "object") {
    JSON = {};
}

(function () {
    "use strict";

    var rx_one = /^[\],:{}\s]*$/;
    var rx_two = /\\(?:["\\\/bfnrt]|u[0-9a-fA-F]{4})/g;
    var rx_three = /"[^"\\\n\r]*"|true|false|null|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?/g;
    var rx_four = /(?:^|:|,)(?:\s*\[)+/g;
    var rx_escapable = /[\\"\u0000-\u001f\u007f-\u009f\u00ad\u0600-\u0604\u070f\u17b4\u17b5\u200c-\u200f\u2028-\u202f\u2060-\u206f\ufeff\ufff0-\uffff]/g;
    var rx_dangerous = /[\u0000\u00ad\u0600-\u0604\u070f\u17b4\u17b5\u200c-\u200f\u2028-\u202f\u2060-\u206f\ufeff\ufff0-\uffff]/g;

    function f(n) {
        // Format integers to have at least two digits.
        return n < 10
            ? "0" + n
            : n;
    }

    function this_value() {
        return this.valueOf();
    }

    if (typeof Date.prototype.toJSON !== "function") {

        Date.prototype.toJSON = function () {

            return isFinite(this.valueOf())
                ? this.getUTCFullYear() + "-" +
                        f(this.getUTCMonth() + 1) + "-" +
                        f(this.getUTCDate()) + "T" +
                        f(this.getUTCHours()) + ":" +
                        f(this.getUTCMinutes()) + ":" +
                        f(this.getUTCSeconds()) + "Z"
                : null;
        };

        Boolean.prototype.toJSON = this_value;
        Number.prototype.toJSON = this_value;
        String.prototype.toJSON = this_value;
    }

    var gap;
    var indent;
    var meta;
    var rep;


    function quote(string) {

// If the string contains no control characters, no quote characters, and no
// backslash characters, then we can safely slap some quotes around it.
// Otherwise we must also replace the offending characters with safe escape
// sequences.

        rx_escapable.lastIndex = 0;
        return rx_escapable.test(string)
            ? "\"" + string.replace(rx_escapable, function (a) {
                var c = meta[a];
                return typeof c === "string"
                    ? c
                    : "\\u" + ("0000" + a.charCodeAt(0).toString(16)).slice(-4);
            }) + "\""
            : "\"" + string + "\"";
    }


    function str(key, holder) {

// Produce a string from holder[key].

        var i;          // The loop counter.
        var k;          // The member key.
        var v;          // The member value.
        var length;
        var mind = gap;
        var partial;
        var value = holder[key];

// If the value has a toJSON method, call it to obtain a replacement value.

        if (value && typeof value === "object" &&
                typeof value.toJSON === "function") {
            value = value.toJSON(key);
        }

// If we were called with a replacer function, then call the replacer to
// obtain a replacement value.

        if (typeof rep === "function") {
            value = rep.call(holder, key, value);
        }

// What happens next depends on the value's type.

        switch (typeof value) {
        case "string":
            return quote(value);

        case "number":

// JSON numbers must be finite. Encode non-finite numbers as null.

            return isFinite(value)
                ? String(value)
                : "null";

        case "boolean":
        case "null":

// If the value is a boolean or null, convert it to a string. Note:
// typeof null does not produce "null". The case is included here in
// the remote chance that this gets fixed someday.

            return String(value);

// If the type is "object", we might be dealing with an object or an array or
// null.

        case "object":

// Due to a specification blunder in ECMAScript, typeof null is "object",
// so watch out for that case.

            if (!value) {
                return "null";
            }

// Make an array to hold the partial results of stringifying this object value.

            gap += indent;
            partial = [];

// Is the value an array?

            if (Object.prototype.toString.apply(value) === "[object Array]") {

// The value is an array. Stringify every element. Use null as a placeholder
// for non-JSON values.

                length = value.length;
                for (i = 0; i < length; i += 1) {
                    partial[i] = str(i, value) || "null";
                }

// Join all of the elements together, separated with commas, and wrap them in
// brackets.

                v = partial.length === 0
                    ? "[]"
                    : gap
                        ? "[\n" + gap + partial.join(",\n" + gap) + "\n" + mind + "]"
                        : "[" + partial.join(",") + "]";
                gap = mind;
                return v;
            }

// If the replacer is an array, use it to select the members to be stringified.

            if (rep && typeof rep === "object") {
                length = rep.length;
                for (i = 0; i < length; i += 1) {
                    if (typeof rep[i] === "string") {
                        k = rep[i];
                        v = str(k, value);
                        if (v) {
                            partial.push(quote(k) + (
                                gap
                                    ? ": "
                                    : ":"
                            ) + v);
                        }
                    }
                }
            } else {

// Otherwise, iterate through all of the keys in the object.

                for (k in value) {
                    if (Object.prototype.hasOwnProperty.call(value, k)) {
                        v = str(k, value);
                        if (v) {
                            partial.push(quote(k) + (
                                gap
                                    ? ": "
                                    : ":"
                            ) + v);
                        }
                    }
                }
            }

// Join all of the member texts together, separated with commas,
// and wrap them in braces.

            v = partial.length === 0
                ? "{}"
                : gap
                    ? "{\n" + gap + partial.join(",\n" + gap) + "\n" + mind + "}"
                    : "{" + partial.join(",") + "}";
            gap = mind;
            return v;
        }
    }

// If the JSON object does not yet have a stringify method, give it one.

    if (typeof JSON.stringify !== "function") {
        meta = {    // table of character substitutions
            "\b": "\\b",
            "\t": "\\t",
            "\n": "\\n",
            "\f": "\\f",
            "\r": "\\r",
            "\"": "\\\"",
            "\\": "\\\\"
        };
        JSON.stringify = function (value, replacer, space) {

// The stringify method takes a value and an optional replacer, and an optional
// space parameter, and returns a JSON text. The replacer can be a function
// that can replace values, or an array of strings that will select the keys.
// A default replacer method can be provided. Use of the space parameter can
// produce text that is more easily readable.

            var i;
            gap = "";
            indent = "";

// If the space parameter is a number, make an indent string containing that
// many spaces.

            if (typeof space === "number") {
                for (i = 0; i < space; i += 1) {
                    indent += " ";
                }

// If the space parameter is a string, it will be used as the indent string.

            } else if (typeof space === "string") {
                indent = space;
            }

// If there is a replacer, it must be a function or an array.
// Otherwise, throw an error.

            rep = replacer;
            if (replacer && typeof replacer !== "function" &&
                    (typeof replacer !== "object" ||
                    typeof replacer.length !== "number")) {
                throw new Error("JSON.stringify");
            }

// Make a fake root object containing our value under the key of "".
// Return the result of stringifying the value.

            return str("", {"": value});
        };
    }


// If the JSON object does not yet have a parse method, give it one.

    if (typeof JSON.parse !== "function") {
        JSON.parse = function (text, reviver) {

// The parse method takes a text and an optional reviver function, and returns
// a JavaScript value if the text is a valid JSON text.

            var j;

            function walk(holder, key) {

// The walk method is used to recursively walk the resulting structure so
// that modifications can be made.

                var k;
                var v;
                var value = holder[key];
                if (value && typeof value === "object") {
                    for (k in value) {
                        if (Object.prototype.hasOwnProperty.call(value, k)) {
                            v = walk(value, k);
                            if (v !== undefined) {
                                value[k] = v;
                            } else {
                                delete value[k];
                            }
                        }
                    }
                }
                return reviver.call(holder, key, value);
            }


// Parsing happens in four stages. In the first stage, we replace certain
// Unicode characters with escape sequences. JavaScript handles many characters
// incorrectly, either silently deleting them, or treating them as line endings.

            text = String(text);
            rx_dangerous.lastIndex = 0;
            if (rx_dangerous.test(text)) {
                text = text.replace(rx_dangerous, function (a) {
                    return "\\u" +
                            ("0000" + a.charCodeAt(0).toString(16)).slice(-4);
                });
            }

// In the second stage, we run the text against regular expressions that look
// for non-JSON patterns. We are especially concerned with "()" and "new"
// because they can cause invocation, and "=" because it can cause mutation.
// But just to be safe, we want to reject all unexpected forms.

// We split the second stage into 4 regexp operations in order to work around
// crippling inefficiencies in IE's and Safari's regexp engines. First we
// replace the JSON backslash pairs with "@" (a non-JSON character). Second, we
// replace all simple value tokens with "]" characters. Third, we delete all
// open brackets that follow a colon or comma or that begin the text. Finally,
// we look to see that the remaining characters are only whitespace or "]" or
// "," or ":" or "{" or "}". If that is so, then the text is safe for eval.

            if (
                rx_one.test(
                    text
                        .replace(rx_two, "@")
                        .replace(rx_three, "]")
                        .replace(rx_four, "")
                )
            ) {

// In the third stage we use the eval function to compile the text into a
// JavaScript structure. The "{" operator is subject to a syntactic ambiguity
// in JavaScript: it can begin a block or an object literal. We wrap the text
// in parens to eliminate the ambiguity.

                j = eval("(" + text + ")");

// In the optional fourth stage, we recursively walk the new structure, passing
// each name/value pair to a reviver function for possible transformation.

                return (typeof reviver === "function")
                    ? walk({"": j}, "")
                    : j;
            }

// If the text is not JSON parseable, then a SyntaxError is thrown.

            throw new SyntaxError("JSON.parse");
        };
    }
}());

//END JSON SECTION

var idoc = app.activeDocument;  // get active document
var sel = idoc.selection;  
var canvas = idoc.artboards.getByName("Artboard 1"); // get artboard
 
// create level object
var levelObj = {};

// add player position
var playerLayer = idoc.layers.getByName("Player"); // get player position layer
var playerPos = playerLayer.pathItems[0].pathPoints[0]; // get path point on player position layer
levelObj.player = [playerPos.anchor[0], playerPos.anchor[1]]; // add player position to JSON object

// add enemies
var enemyLayer = idoc.layers.getByName("Enemy"); // get enemy layer
var enemyList = [];
for (i = 0; i < enemyLayer.layers.length; i++) { // iterate through enemy types
    var enemiesOfSameType = enemyLayer.layers[i];
    var enemyType = enemiesOfSameType.name; // get enemy type
    for (j = 0; j < enemiesOfSameType.pathItems.length; j++) { // iterate through enemy paths
        var enemyPath = enemiesOfSameType.pathItems[j];
        var verts = [];
        for (k = 0; k < enemyPath.pathPoints.length; k++) {
            //enemyList.push({type: enemyType, pos: [enemyPath.pathPoints[k].anchor[0], enemyPath.pathPoints[k].anchor[1]]}); 
            verts.push(enemyPath.pathPoints[k].anchor[0]); // add each point along path to verts list
            verts.push(enemyPath.pathPoints[k].anchor[1]);
        }
        var enemyInstance = {type: enemyType, pos: verts};
        enemyList.push(enemyInstance);
    }
}
levelObj.enemy = enemyList;

// add boundaries
var boundaryLayer = idoc.layers.getByName("Boundary"); // get boundary layer
var boundaryList = [];
for (i = 0; i < boundaryLayer.layers.length; i++) { // iterate through boundary types
    var boundariesOfSameType = boundaryLayer.layers[i];
    var boundaryType = boundariesOfSameType.name; // get boundary type
    for (j = 0; j < boundariesOfSameType.pathItems.length; j++) { // iterate through boundary paths
        var boundaryPath = boundariesOfSameType.pathItems[j];
        var boundaryPoints = [];
        for (k = 0; k < boundaryPath.pathPoints.length; k++) { // add all boundary points to verts list
            boundaryPoints.push(boundaryPath.pathPoints[k].anchor[0]);
            boundaryPoints.push(boundaryPath.pathPoints[k].anchor[1]);
        }

        boundaryList.push({type: boundaryType, verts: boundaryPoints}); // add boundary to boundary list
    }
}
levelObj.boundary = boundaryList;

// add hazards
var hazardLayer = idoc.layers.getByName("Hazard"); // get hazard layer
var hazardList = [];
for (i = 0; i < hazardLayer.layers.length; i++) { // iterate through hazard types
    var hazardsOfSameType = hazardLayer.layers[i];
    var hazardType = hazardsOfSameType.name; // get hazard type
    for (j = 0; j < hazardsOfSameType.pathItems.length; j++) { // iterate through hazard paths
        var hazardPath = hazardsOfSameType.pathItems[j];
        var hazardPoints = [];
        for (k = 0; k < hazardPath.pathPoints.length; k++) { // add all hazard points to verts list
            hazardPoints.push(hazardPath.pathPoints[k].anchor[0]);
            hazardPoints.push(hazardPath.pathPoints[k].anchor[1]);
        }

        hazardList.push({type: hazardType, verts: hazardPoints}); // add hazard to hazards list
    }
}
levelObj.hazard = hazardList;

// add circlezones
var zoneLayer = idoc.layers.getByName("Zones");
var zoneList = []
if (zoneLayer.pathItems.length > 0) {
    for (i = 0; i < zoneLayer.layers.length; i++) {
        var zonesOfSameType = zoneLayer.layers[i];
        var zoneType = zonesOfSameType.name;
        for (j = 0; j < zonesOfSameType.pathItems.length; j++) {
            var zonePath = zonesOfSameType.pathItems[j];
            for (k = 0; k < zonePath.pathPoints.length; k++) {
                var zonePos = zonePath.pathPoints[k];
                zoneList.push({type: zoneType, pos: [zonePos.anchor[0], zonePos.anchor[1]]});
            }
        }
    }
}
levelObj.circlezones = zoneList;

// add goal
var goalLayer = idoc.layers.getByName("Goal"); // get goal position layer
if (goalLayer.pathItems.length > 0) {
    var goalPos = goalLayer.pathItems[0].pathPoints[0]; // get path point on goal position layer
    levelObj.goal = [goalPos.anchor[0], goalPos.anchor[1]]; // add goal position to JSON object
}

var levelJSON = JSON.stringify(levelObj);
saveFile(levelJSON);

var itext = idoc.textFrames.add(); 
itext.contents = levelJSON;
itext.left = sel[0].left;
itext.top = sel[0].top;

function saveFile(txt)                                
{
    var Name = app.activeDocument.name.replace(/\.[^\.]+$/, '');
    var Ext = decodeURI(app.activeDocument.name).replace(/^.*\./,''); 
    var Path = app.activeDocument.path;
    var saveFile = File(Path + "/" + Name +".json");

    if(saveFile.exists)
    {
        saveFile.remove();
    }

    saveFile.encoding = "UTF8";
    saveFile.open("e", "TEXT", "????");
    saveFile.writeln(txt);
    saveFile.close();

    return true;
}


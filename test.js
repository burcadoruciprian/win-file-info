'use strict';

const assert = require('assert');
const fileInfo = require('./build/release/winfileinfo.node')

//Invalid aguments count
assert.throws(() => fileInfo.getFileVersion('foo', 'bar'), TypeError);
//Invalid aegument type
assert.throws(() => fileInfo.getFileVersion(1), TypeError);
//Invalid file path
assert.throws(() => fileInfo.getFileVersion("c:/foo/foo.exe"), TypeError);
//Valid file
assert.doesNotThrow(() => fileInfo.getFileVersion("c:/windows/notepad.exe"), TypeError);


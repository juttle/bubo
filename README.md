# Object Hash Set
This is a hash set for Javascript objects. Its object-encoding algorithm is quite space-efficient, especially in the case where the input objects have largely the same keys, and most or all keys have low cardinality (number of distinct values of that key in the data set). Data with these characteristics often arises in time series data analysis applications, which is where this originated.

## Downloading
`npm install object-hash-set`

## Example
```javascript
var ObjectHashSet = require('object-hash-set');

var set = new ObjectHashSet();

set.add({abc: 123});
console.log('does set contain {abc: 123}?: ' + set.contains({abc: 123}));
console.log('does set contain {def: 123}?: ' + set.contains({def: 123}));
```

## API

### new ObjectHashSet([options]) ###
Creates an instance of the Object Hash Set. `options`, if specified, is an object. The only supported option is `ignore`. The value of `ignore` is an array of keys that the set will not pay attention to during storage or lookup. The set will consider two objects identical if their values for all non-ignored keys are the same.

### add(object) ###
Adds the given object to the set if an equivalent object is not already in the set. Returns `true` if a new object was added or `false` if the object already existed in the set.

### contains(object) ###
Returns `true` if an object equivalent to `object` has already been `add`ed.

### delete(object) ###
Removes the object from the set. A future call to `add` or `contains` with an object identical to the given object will return `false`. Note that this will not reclaim the storage space used by the keys in the given object.

## Performance ##
Object Hash Set works its magic by storing each distinct value of each key once and compactly encoding combinations of keys with references to these stored values. You can use the provided `scripts/perf.js` to give it a test. `perf.js` takes two parameters: `num_keys` and `values_per_key`. It generates a data set of (`values_per_key`^`num_keys`) distinct points, adds them all to an Object Hash Set, and periodically logs memory stats. Here's an example:
```
node --expose-gc ./scripts/perf.js --num_keys 7 --values_per_key 10
stored 0 points so far in 0 sec, memory usage: { rss: 22519808, heapTotal: 10619424, heapUsed: 4954824 }
stored 100000 points so far in 1.11 sec, memory usage: { rss: 29716480, heapTotal: 10619424, heapUsed: 7000328 }
...
stored 9900000 points so far in 111.151 sec, memory usage: { rss: 420196352, heapTotal: 10619424, heapUsed: 6332040 }
Finished! Stored 10000000 points, final memory usage: { rss: 421847040, heapTotal: 10619424, heapUsed: 4697928 }
```
That comes out to 10,000,000 objects stored, taking up 421,847,040 bytes of RSS space (since Object Hash Set is a native C++ addon, it doesn't take up space in the Javascript heap for the objects it stores). If you naively hash these objects with `JSON.stringify` and store them as keys in a plain old Javascript object, the heap usage goes to 1.5 GB and the program crashes at around 6.5 million points. So Object Hash Set is over 5 times more efficient. Nice!

## Contributing

Want to contribute? Awesome! Don’t hesitate to file an issue or open a pull request. See the common [contributing guidelines for project Juttle](https://github.com/juttle/juttle/blob/master/CONTRIBUTING.md).

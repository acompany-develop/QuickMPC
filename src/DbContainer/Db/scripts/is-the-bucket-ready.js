const bucketName = process.argv[2];
const username = process.argv[3];
const password = process.argv[4];
const couchbaseVersion = process.argv[5];

const couchbaseVersionSplit = 5;
// This error code is expected so ignore it
const errorCodeToIgnore = 22;

var couchbase = require('couchbase');
var cluster = new couchbase.Cluster("http://127.0.0.1:8091");

// If the couchbase version is 5 or greater then authenticate. For older versions this is unnecessary and will cause an error
if (parseInt(couchbaseVersion.charAt(0), 10) >= couchbaseVersionSplit) {
  cluster.authenticate(username, password);
}

var bucket = cluster.openBucket(bucketName, err => {
  if (err) {
    console.error(`Failed to open bucket connection to ${bucketName}`);
    process.exit(1);
  }
});

var key = "ping";
bucket.upsert(key, "ping", err => {
  if (err) {
    if (err.code !== errorCodeToIgnore) {
      console.log(err);
    }
    process.exit(1);
  }

  // Don't keep the ping document in the bucket
  bucket.remove(key, error => {
    if (error) {
      console.log(`${key} document was not removed`);
      process.exit(1);
    }

    process.exit(0);
  });
});

var nodejshint = require( './nodejshint.js' ).test,

files = [
  'node_gir.js'
];

nodejshint( files, function( failures ) {
  if( !files.length ) {
    process.exit( 0 );
  }
  else {
    process.exit( 1 );
  }
});

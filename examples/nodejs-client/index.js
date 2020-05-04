const {HawkTracerClient} = require('@hawktracer/client')

const hawkTracerClient = new HawkTracerClient(require('path').join(__dirname, 'test.htdump'));
// const hawkTracerClient = new HawkTracerClient('127.0.0.1:5443');
/*
const hawkTracerClient = new HawkTracerClient({
    source: '127.0.0.1:5443',
    map_files: require('path').join(__dirname, 'nativetracepoints.map')
});
*/

hawkTracerClient.onEvents((events) => events.forEach(e => console.log(JSON.stringify(e))));

hawkTracerClient.start();

setTimeout(() => hawkTracerClient.stop(), 500);

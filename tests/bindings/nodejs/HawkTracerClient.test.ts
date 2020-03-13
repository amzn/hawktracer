describe("1. Initiating HawkTracerClient", () => {
    test("succeeds when constructor is given `source` parameter in string type", () => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');   // loads build/Release/hawk_tracer_client.node
        const hawkTracerClient = new HawkTracerClient("some-file");
        expect(hawkTracerClient).not.toBeNull();
    });

    test("fails without parameter", () => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        expect(() => {
            new HawkTracerClient();
        }).toThrow();
    });

    test("fails with parameter in wrong type", () => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        expect(() => {
            new HawkTracerClient(11);
        }).toThrow();
    });
});

describe("2. Set up callback", () => {
    test("invokes callback", (done) => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        const source = require('path').join(__dirname, 'test.htdump');
        const hawkTracerClient = new HawkTracerClient(source);

        hawkTracerClient.onEvents(() => {
            done();
        });
        hawkTracerClient.start();
    });

    test("receives all events when the events listener is replaced", (done) => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        const source = require('path').join(__dirname, 'test.htdump');
        const hawkTracerClient = new HawkTracerClient(source);

        const expectedCount = 56;   // number of events in test.htdump
        let firstCount = 0;
        let secondCount = 0;

        const secondCallback = (events) => {
            secondCount += events.length;
            checkEventsCount(firstCount + secondCount, expectedCount, done);
        };

        const firstCallback = (events) => {
            if (firstCount == 0) {
                hawkTracerClient.onEvents(secondCallback);
            }
            firstCount += events.length;
            checkEventsCount(firstCount + secondCount, expectedCount, done);
        };

        hawkTracerClient.onEvents(firstCallback);
        hawkTracerClient.start();
    });

    test.todo("receives all events if onEvents() is called after start().");
});

describe("3. Start HawkTracerClient", () => {
    test("succeeds with existing source file", () => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        const source = require('path').join(__dirname, 'test.htdump');
        const hawkTracerClient = new HawkTracerClient(source);
        expect(hawkTracerClient.start()).toBe(true);
    });

    test.todo("succeeds with source in 'x.x.x.x:p' format where x.x.x.x is the IP address, p is port number");

    test("fails with non-existing source file", () => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        const hawkTracerClient = new HawkTracerClient('non-existing file !@£$%^&*()');
        expect(hawkTracerClient.start()).toBe(false);
    });
});

describe("4. Receive events through callback", () => {
    test("correct number of events", (done) => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        const source = require('path').join(__dirname, 'test.htdump');
        const hawkTracerClient = new HawkTracerClient(source);

        const expectedCount = 56;   // number of events in test.htdump
        let count = 0;
        hawkTracerClient.onEvents((events: object[]) => {
            count += events.length;
            checkEventsCount(count, expectedCount, done);
        });
        hawkTracerClient.start();
    });

    function busySleep(seconds) {
        const sleepStart = process.hrtime();
        let sleptSeconds;
        do {
            [sleptSeconds,] = process.hrtime(sleepStart);
        } while (sleptSeconds < seconds);
    }

    test("do not lose events when the callback delays too long", (done) => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        const source = require('path').join(__dirname, 'test.htdump');
        const hawkTracerClient = new HawkTracerClient(source);

        const expectedCount = 56;   // number of events in test.htdump
        let count = 0;
        let delayed = false;
        hawkTracerClient.onEvents((events: object[]) => {
            count += events.length;
            checkEventsCount(count, expectedCount, done);

            if (!delayed) {
                delayed = true;
                // Block the main thread
                busySleep(1);
            }
        });
        hawkTracerClient.start();
    });
});

describe("5. Stop HawkTracerClient", () => {
    test("succeeds and events stop coming", (done) => {
        const {HawkTracerClient} = require('bindings')('hawk_tracer_client');
        const source = require('path').join(__dirname, 'test.htdump');
        const hawkTracerClient = new HawkTracerClient(source);

        let i = 0;
        let timer;
        hawkTracerClient.onEvents(() => {
            clearTimeout(timer);
            expect(++i).toBe(1);    // fails the second time
            // noinspection JSDeprecatedSymbols
            hawkTracerClient.stop();
            timer = setTimeout(done, 500); // Wait for a while in case more events come in.
        });
        hawkTracerClient.start();
    });

    test.todo("does not crash when not started.");
});

let countTimer;

function checkEventsCount(count, expectedCount, done) {
    clearTimeout(countTimer);
    expect(count).toBeLessThanOrEqual(expectedCount);
    if (count == expectedCount) {
        countTimer = setTimeout(done, 500); // Wait for a while in case more events come in.
    }
}

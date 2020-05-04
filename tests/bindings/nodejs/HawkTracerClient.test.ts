import bindings from 'bindings';

jest.mock('bindings');
const mockBindings = <jest.Mock<typeof bindings>>bindings;
const mockNativeClientClass = jest.fn();
const mockNativeClient = jest.fn();

// @ts-ignore
mockBindings.mockReturnValue({"HawkTracerClient": mockNativeClientClass});

// @ts-ignore
import {HawkTracerClient, BareEvent, Event} from "../../../index";

beforeEach(() => {
    mockNativeClientClass.mockClear();
    mockNativeClientClass.mockReturnValue(mockNativeClient)

    mockNativeClient.mockClear();
    mockNativeClient.start = jest.fn().mockReturnValue(true);
    mockNativeClient.onEvents = jest.fn();
    mockNativeClient.stop = jest.fn();
})

test("constructs with string", () => {
    new HawkTracerClient("abc");
    expect(mockNativeClientClass).toHaveBeenCalledWith("abc");
})

test("constructs with HawkTracerClientOptions ", () => {
    new HawkTracerClient({source: "abc", map_files: "def"});
    expect(mockNativeClientClass).toHaveBeenCalledWith("abc", "def");
})

test("Hides system events", (done) => {
    mockNativeClient.onEvents.mockImplementation((handler: (e: Event[]) => void) => {
        setTimeout(() => handler(FIRST_EVENTS), 100);
        setTimeout(() => handler(SECOND_EVENTS), 200);
    });

    const client = new HawkTracerClient({source: 'dummy_source'});

    let result: Event[] = [];
    let timeout: NodeJS.Timeout;
    client.onEvents((events: Event[]) => {
        result = result.concat(events);
        clearTimeout(timeout);
        timeout = setTimeout(() => {
            expect(result.length).toBe(1);
            expect(result[0].id).toBe(39);
            done();
        }, 200)
    });
})

test("Adds klass_name", (done) => {
    mockNativeClient.onEvents.mockImplementation((handler: (e: Event[]) => void) => {
        setTimeout(() => handler(FIRST_EVENTS), 100);
        setTimeout(() => handler(SECOND_EVENTS), 200);
    });

    const client = new HawkTracerClient({source: 'dummy_source'});

    let result: Event[] = [];
    let timeout: NodeJS.Timeout;
    client.onEvents((events: Event[]) => {
        result = result.concat(events);
        clearTimeout(timeout);
        timeout = setTimeout(() => {
            expect(result[0].klass_name).toBe("CallstackEvent");
            done();
        }, 200)
    });
})

const FIRST_EVENTS: BareEvent[] = [
    {"endianness": 0, "id": 0, "timestamp": 0, "klass_id": 0},
    {"info_klass_id": 0, "klass_id": 2, "event_klass_name": "HT_EndiannessInfoEvent", "field_count": 2, "id": 0, "timestamp": 198055101790315},
    {"info_klass_id": 0, "klass_id": 3, "field_type": "HT_Event", "data_type": 1, "id": 1, "field_name": "base", "size": 24, "timestamp": 198055101790923},
    {"info_klass_id": 0, "klass_id": 3, "field_type": "uint8_t", "data_type": 99, "id": 2, "field_name": "endianness", "size": 1, "timestamp": 198055101791458},
    {"info_klass_id": 1, "klass_id": 2, "event_klass_name": "HT_Event", "field_count": 3, "id": 3, "timestamp": 198055101791783},
    {"info_klass_id": 1, "klass_id": 3, "field_type": "HT_EventKlass*", "data_type": 6, "id": 4, "field_name": "klass", "size": 8, "timestamp": 198055101792217},
    {"info_klass_id": 1, "klass_id": 3, "field_type": "HT_TimestampNs", "data_type": 99, "id": 5, "field_name": "timestamp", "size": 8, "timestamp": 198055101792566},
    {"info_klass_id": 1, "klass_id": 3, "field_type": "HT_EventId", "data_type": 99, "id": 6, "field_name": "id", "size": 8, "timestamp": 198055101792871},
    {"info_klass_id": 2, "klass_id": 2, "event_klass_name": "HT_EventKlassInfoEvent", "field_count": 4, "id": 7, "timestamp": 198055101793162},
    {"info_klass_id": 2, "klass_id": 3, "field_type": "HT_Event", "data_type": 1, "id": 8, "field_name": "base", "size": 24, "timestamp": 198055101793428},
    {"info_klass_id": 2, "klass_id": 3, "field_type": "HT_EventKlassId", "data_type": 99, "id": 9, "field_name": "info_klass_id", "size": 4, "timestamp": 198055101793735},
    {"info_klass_id": 2, "klass_id": 3, "field_type": "const char*", "data_type": 2, "id": 10, "field_name": "event_klass_name", "size": 8, "timestamp": 198055101794040},
    {"info_klass_id": 2, "klass_id": 3, "field_type": "uint8_t", "data_type": 99, "id": 11, "field_name": "field_count", "size": 1, "timestamp": 198055101794384},
    {"info_klass_id": 3, "klass_id": 2, "event_klass_name": "HT_EventKlassFieldInfoEvent", "field_count": 6, "id": 12, "timestamp": 198055101794687},
    {"info_klass_id": 3, "klass_id": 3, "field_type": "HT_Event", "data_type": 1, "id": 13, "field_name": "base", "size": 24, "timestamp": 198055101794925},
    {"info_klass_id": 3, "klass_id": 3, "field_type": "HT_EventKlassId", "data_type": 99, "id": 14, "field_name": "info_klass_id", "size": 4, "timestamp": 198055101795158},
    {"info_klass_id": 3, "klass_id": 3, "field_type": "const char*", "data_type": 2, "id": 15, "field_name": "field_type", "size": 8, "timestamp": 198055101795404}
];

const SECOND_EVENTS: BareEvent[]  = [
    {"info_klass_id": 3, "klass_id": 3, "field_type": "const char*", "data_type": 2, "id": 16, "field_name": "field_name", "size": 8, "timestamp": 198055101795617},
    {"info_klass_id": 3, "klass_id": 3, "field_type": "uint64_t", "data_type": 99, "id": 17, "field_name": "size", "size": 8, "timestamp": 198055101795833},
    {"info_klass_id": 3, "klass_id": 3, "field_type": "uint8_t", "data_type": 99, "id": 18, "field_name": "data_type", "size": 1, "timestamp": 198055101796069},
    {"info_klass_id": 4, "klass_id": 2, "event_klass_name": "HT_CallstackBaseEvent", "field_count": 3, "id": 19, "timestamp": 198055101796272},
    {"info_klass_id": 4, "klass_id": 3, "field_type": "HT_Event", "data_type": 1, "id": 20, "field_name": "base", "size": 24, "timestamp": 198055101796502},
    {"info_klass_id": 4, "klass_id": 3, "field_type": "HT_DurationNs", "data_type": 99, "id": 21, "field_name": "duration", "size": 8, "timestamp": 198055101796708},
    {"info_klass_id": 4, "klass_id": 3, "field_type": "HT_ThreadId", "data_type": 99, "id": 22, "field_name": "thread_id", "size": 4, "timestamp": 198055101796898},
    {"info_klass_id": 5, "klass_id": 2, "event_klass_name": "HT_CallstackIntEvent", "field_count": 2, "id": 23, "timestamp": 198055101797167},
    {"info_klass_id": 5, "klass_id": 3, "field_type": "HT_CallstackBaseEvent", "data_type": 1, "id": 24, "field_name": "base", "size": 40, "timestamp": 198055101797383},
    {"info_klass_id": 5, "klass_id": 3, "field_type": "HT_CallstackEventLabel", "data_type": 99, "id": 25, "field_name": "label", "size": 8, "timestamp": 198055101797659},
    {"info_klass_id": 6, "klass_id": 2, "event_klass_name": "HT_CallstackStringEvent", "field_count": 2, "id": 26, "timestamp": 198055101797911},
    {"info_klass_id": 6, "klass_id": 3, "field_type": "HT_CallstackBaseEvent", "data_type": 1, "id": 27, "field_name": "base", "size": 40, "timestamp": 198055101798117},
    {"info_klass_id": 6, "klass_id": 3, "field_type": "const char*", "data_type": 2, "id": 28, "field_name": "label", "size": 8, "timestamp": 198055101798309},
    {"info_klass_id": 7, "klass_id": 2, "event_klass_name": "HT_StringMappingEvent", "field_count": 3, "id": 29, "timestamp": 198055101798562},
    {"info_klass_id": 7, "klass_id": 3, "field_type": "HT_Event", "data_type": 1, "id": 30, "field_name": "base", "size": 24, "timestamp": 198055101798757},
    {"info_klass_id": 7, "klass_id": 3, "field_type": "uint64_t", "data_type": 99, "id": 31, "field_name": "identifier", "size": 8, "timestamp": 198055101798957},
    {"info_klass_id": 7, "klass_id": 3, "field_type": "const char*", "data_type": 2, "id": 32, "field_name": "label", "size": 8, "timestamp": 198055101799155},
    {"info_klass_id": 8, "klass_id": 2, "event_klass_name": "HT_SystemInfoEvent", "field_count": 4, "id": 33, "timestamp": 198055101799363},
    {"info_klass_id": 8, "klass_id": 3, "field_type": "HT_Event", "data_type": 1, "id": 34, "field_name": "base", "size": 24, "timestamp": 198055101799560},
    {"info_klass_id": 8, "klass_id": 3, "field_type": "uint8_t", "data_type": 99, "id": 35, "field_name": "version_major", "size": 1, "timestamp": 198055101799736},
    {"info_klass_id": 8, "klass_id": 3, "field_type": "uint8_t", "data_type": 99, "id": 36, "field_name": "version_minor", "size": 1, "timestamp": 198055101799909},
    {"info_klass_id": 8, "klass_id": 3, "field_type": "uint8_t", "data_type": 99, "id": 37, "field_name": "version_patch", "size": 1, "timestamp": 198055101800152},
    {"version_major": 0, "version_minor": 10, "klass_id": 8, "version_patch": 0, "id": 0, "timestamp": 0},
    {"identifier": 4381601712, "label": "foo", "id": 38, "timestamp": 198055101873390, "klass_id": 7},
    {"thread_id": 1, "duration": 10084, "klass_id": 5, "label": "foo", "id": 39, "timestamp": 198055101874715}
];

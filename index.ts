const NativeClient = require('bindings')('hawk_tracer_client').HawkTracerClient;   // loads build/Release/hawk_tracer_client.node

export interface HawkTracerClientOptions {
    source: string;
    map_files?: string;
}

// HT_Event
interface BareEvent {
    klass_id: number;
    timestamp: number;
    id: number;
}

// HT_EventKlassInfoEvent
interface EventKlassInfoEvent extends BareEvent {
    info_klass_id: number;
    event_klass_name: string;
    field_count: number;
}

function isEventKlassInfoEvent(event: BareEvent): event is EventKlassInfoEvent {
    const e = event as EventKlassInfoEvent;
    return e.info_klass_id !== undefined && e.event_klass_name !== undefined;
}

export interface Event extends BareEvent {
    klass_name: string;
}

function isEvent(event: Event): event is Event {
    return event.klass_name !== undefined;
}

// HT_CallstackBaseEvent | HT_CallstackIntEvent | HT_CallstackStringEvent
export interface CallstackEvent extends Event {
    duration: number;
    thread_id: number;
    label?: string | number;
}

export function isCallstackEvent(event: CallstackEvent): event is CallstackEvent {
    const e = event as CallstackEvent;
    return e.duration !== undefined && e.thread_id !== undefined;
}

export class HawkTracerClient {
    private _client: any;
    private _klass_names: (string | undefined)[] = [];

    constructor(option: HawkTracerClientOptions | string) {
        if (typeof option === "string") {
            this._client = new NativeClient(option);
        }
        else {
            this._client = new NativeClient(option.source, option.map_files);
        }
    }

    public start(): boolean {
        return this._client.start();
    }

    public stop(): void {
        this._client.stop();
    }

    public onEvents(callback: (events: Event[]) => void): void {
        this._client.onEvents((events: BareEvent[]) => {
            this.registerEventKlass(events);
            callback(this.decorateEventWithKlass(events));
        });
    }

    private decorateEventWithKlass(bareEvents: BareEvent[]): Event[] {
        const events = bareEvents as Event[];
        events.forEach(e => {
            // @ts-ignore
            e.klass_name = this._klass_names[e.klass_id];
        });
        return events.filter(isEvent);
    }

    private registerEventKlass(events: BareEvent[]): void {
        function filterClassName(name: string): string | undefined {
            const SYSTEM_EVENT_NAMES = ["HT_EndiannessInfoEvent", "HT_Event", "HT_EventKlassInfoEvent",
                "HT_EventKlassFieldInfoEvent", "HT_StringMappingEvent", "HT_SystemInfoEvent"];
            const CALLSTACK_EVENT_NAMES = ["HT_CallstackBaseEvent", "HT_CallstackIntEvent", "HT_CallstackStringEvent"];
            if (SYSTEM_EVENT_NAMES.includes(name)) {
                return undefined;
            }
            if (CALLSTACK_EVENT_NAMES.includes(name)) {
                return "CallstackEvent"
            }
            return name;
        }

        events
            .filter(isEventKlassInfoEvent)
            .forEach(eventKlassInfoEvent => {
                this._klass_names[eventKlassInfoEvent.info_klass_id] = filterClassName(eventKlassInfoEvent.event_klass_name);
            });
    }
}

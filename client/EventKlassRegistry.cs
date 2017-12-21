using System;
using System.Collections.Generic;
using System.Linq;

namespace hawktracerclient
{
    public class EventKlassRegistry
    {
        private readonly Dictionary<UInt32, EventKlass> eventTypes = new Dictionary<uint, EventKlass>();

        public EventKlassRegistry()
        {
            var eventKlass = new EventKlass(1, "HT_Event");
            new Tuple<string, Type>[]
            {
                new Tuple<string, Type>("type", typeof(UInt32)),
                new Tuple<string, Type>("timestamp", typeof(UInt64)),
                new Tuple<string, Type>("id", typeof(UInt64))
            }.ToList().ForEach(e => eventKlass.AddField(new EventKlassField { Name = e.Item1, Type = e.Item2 }));
            RegisterKlass(eventKlass);
            
            var infoEventKlass = new EventKlass(2, "HT_EventKlassInfoEvent");
            new Tuple<string, Type>[]
            {
                new Tuple<string, Type>("event_type", typeof(UInt32)),
                new Tuple<string, Type>("event_klass_name", typeof(string)),
                new Tuple<string, Type>("field_count", typeof(byte))
            }.ToList().ForEach(e => infoEventKlass.AddField(new EventKlassField { Name = e.Item1, Type = e.Item2 }));
            RegisterKlass(infoEventKlass);

            var infoFieldKlass = new EventKlass(3, "HT_EventKlassFieldInfoEvent");
            new Tuple<string, Type>[]
            {
                new Tuple<string, Type>("event_type", typeof(UInt32)),
                new Tuple<string, Type>("field_type", typeof(string)),
                new Tuple<string, Type>("field_name", typeof(string)),
                new Tuple<string, Type>("size", typeof(UInt64)),
                new Tuple<string, Type>("data_type", typeof(byte))
            }.ToList().ForEach(e => infoFieldKlass.AddField(new EventKlassField { Name = e.Item1, Type = e.Item2 }));
            RegisterKlass(infoFieldKlass);
        }

        public EventKlass this [UInt32 eventType]
        {
            get
            {
                return eventTypes[eventType];
            }
        }

        public void RegisterKlass(EventKlass klass)
        {
            eventTypes.Add(klass.EventType, klass);
        }

        public EventKlass FindByKlassName(string typeName)
        {
            return eventTypes.First(x => x.Value.Name == typeName).Value;
        }
    }
}


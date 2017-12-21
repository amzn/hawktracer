using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;

namespace HawkTracer.Client
{
    public static class KnownEventTypes
    {
        public const uint KlassEvent = 1;

        public const uint KlassInfo = 2;

        public const uint FieldInfo = 3;

        public static bool IsKnownEventType(uint type)
        {
            return type == KlassInfo || type == FieldInfo || type == KlassEvent;
        }

        public static string BaseEventTypeName
        {
            get { return "HT_Event"; }
        }
    }

    public enum DataType
    {
        Structure = 1,
        String = 2,
        Integer = 3,
        Pointer = 6
    }

    public class EventKlass
    {
        private readonly List<EventKlassField> fields = new List<EventKlassField>();

        public UInt32 EventType { get; private set; }

        public string Name { get; private set; }

        public ReadOnlyCollection<EventKlassField> Fields { get { return fields.AsReadOnly(); } }

        public EventKlass(UInt32 eventType, string name)
        {
            EventType = eventType;
            Name = name;
        }

        public void AddField(EventKlassField field)
        {
            fields.Add(field);
        }
    }

    public class EventKlassField
    {
        public string Name { get; set; }

        public Type Type { get; set; }

        public string TypeName { get; set; }
    }

    public class EventFieldValue
    {
        public EventKlassField field;
        public object Value;
    }

    public class Event
    {
        private readonly Dictionary<string, EventFieldValue> values;

        internal EventKlass Klass { get; private set; }

        internal EventFieldValue this [string label]
        {
            get
            {
                return values[label];
            }
        }

        public ReadOnlyCollection<EventFieldValue> FieldValues
        {
            get
            {
                return values.Values.ToList().AsReadOnly();
            }
        }

        public Event(Dictionary<string, EventFieldValue> values, EventKlass klass)
        {
            this.values = values;
            Klass = klass;
        }
    }
}

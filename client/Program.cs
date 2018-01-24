using System;
using System.Collections.Generic;

namespace HawkTracer.Client
{
    public delegate void OnEventReceived(Event evt);

    class MainClass
    {
        readonly EventKlassRegistry klassRegistry = new EventKlassRegistry();
        readonly DataProvider dataProvider;
        ChromeTracingOutput chout;

        public OnEventReceived onEventReceived;

        public static void Main(string[] args)
        {
            var parser = new ArgParser("help", "--", args);
            parser.RegisterOption("source", typeof(string), "A source description. Either filename, or server address");
            parser.RegisterOption("output", typeof(string), "An output Chrome Tracing Json file");

            try
            {
                parser.Parse();
            }
            catch (Exception ex)
            {
                Console.WriteLine("Unable to parse command line arguments: " + ex.Message);
                parser.PrintHelp();
                return;
            }

            if (parser.HasOption("help"))
            {
                parser.PrintHelp();
                return;
            }

            if (!parser.HasOption("source"))
            {
                Console.WriteLine("Missing 'source' parameter");
                parser.PrintHelp();
                return;
            }

            if (!parser.HasOption("output"))
            {
                Console.WriteLine("Missing 'output' parameter");
                parser.PrintHelp();
                return;
            }

            System.IO.Stream stream = SourceStreamFactory.Create(parser.Get<string>("source"));
           
            if (stream == null)
            {
                Console.WriteLine("No stream found!");
                return;
            }

            var provider = new DataProvider(stream);

            var mc = new MainClass(provider);

            try
            {
                mc.chout = new ChromeTracingOutput(parser.Get<string>("output"));
                mc.run();
            }
            catch (Exception ex)
            {
                mc.chout.Dispose();
                Console.WriteLine(ex.Message);
            }
        }

        public MainClass(DataProvider provider)
        {
            dataProvider = provider;
        }

        void run()
        {
            onEventReceived += chout.HandleEvent;
            while (true)
            {
                Event header = ReadEventHeader();
                UInt32 type = (UInt32)header["type"].Value;

                Dictionary<string, EventFieldValue> fieldValues = ReadFieldValues(type, header);

                switch (type)
                {
                    case KnownEventTypes.KlassInfo:
                        AddEventKlass(fieldValues);
                        break;
                    case KnownEventTypes.FieldInfo:
                        AddFieldToEventKlass(fieldValues);
                        break;
                    case KnownEventTypes.StringMapping:
                        chout.AddMapping((UInt64)fieldValues["hash"].Value, fieldValues["label"].Value as string);
                        break;
                    default:
                        if (onEventReceived != null)
                        {
                            onEventReceived(new Event(fieldValues, klassRegistry[type]));
                        }
                        break;
                }
            }
        }

        void AddEventKlass(IDictionary<string, EventFieldValue> values)
        {
            var eventType = Convert.ToUInt32(values["event_type"].Value);

            if (KnownEventTypes.IsKnownEventType(eventType))
            {
                return;
            }

            klassRegistry.RegisterKlass(new EventKlass(eventType, values["event_klass_name"].Value.ToString()));
        }

        void AddFieldToEventKlass(IDictionary<string, EventFieldValue> values)
        {
            var event_type = Convert.ToUInt32(values["event_type"].Value);

            if (KnownEventTypes.IsKnownEventType(event_type))
            {
                return;
            }

            var data_type = (DataType)Enum.ToObject(typeof(DataType), (values["data_type"].Value));

            klassRegistry[event_type].AddField(
                new EventKlassField
                {
                    Name = values["field_name"].Value.ToString(),
                    Type = TypeHelper.GetType(data_type, Convert.ToUInt64(values["size"].Value)),
                    TypeName = values["field_type"].Value.ToString()
                }); // TODO check if event_type exists
        }

        Dictionary<string, EventFieldValue> ReadFieldValues(UInt32 event_type, Event header)
        {
            var fields = new Dictionary<string, EventFieldValue>();

            foreach (EventKlassField field in klassRegistry[event_type].Fields)
            {
                object value = null;
                if (TypeHelper.IsNumericType(field.Type))
                {
                    value = dataProvider.ReadInteger(field.Type);
                }
                else if (field.Type == typeof(string))
                {
                    value = dataProvider.ReadString();
                }
                else if (field.Type == typeof(Event))
                {
                    if (field.TypeName != KnownEventTypes.BaseEventTypeName)
                    {
                        value = new Event(ReadFieldValues(klassRegistry.FindByKlassName(field.TypeName).EventType, header), 
                            klassRegistry.FindByKlassName(field.TypeName));
                    }
                    else
                    {
                        value = header;
                    }
                }

                fields.Add(field.Name, new EventFieldValue{ field = field, Value = value });
            }

            return fields;
        }

        Event ReadEventHeader()
        {
            return new Event(ReadFieldValues(1, null), klassRegistry[1]);
        }
    }
}

using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;

namespace HawkTracer.Client
{
    public class ChromeTracingOutput : IDisposable
    {
        readonly StreamWriter writer;
        bool firstEntry = true;
        readonly Dictionary<UInt64, string> stringMapping = new Dictionary<UInt64, string>();

        public ChromeTracingOutput(string filename)
        {
            writer = new StreamWriter(filename);

            writer.Write(@"{""traceEvents"": [");
        }

        public void AddMapping(UInt64 id, string label)
        {
            stringMapping.Add(id, label);
        }

        // TODO not efficient...
        private static object FindValue(Event evt, string fieldName)
        {
            do
            {
                if (evt.HasField(fieldName))
                {
                    return evt[fieldName].Value;
                }
            } while (evt.HasField("base") && (evt = (Event)evt["base"].Value) != null);
            return null;
        }

        public void HandleEvent(Event evt)
        {
            if (!firstEntry)
            {
                writer.Write(",");
            }
            else
            {
                firstEntry = false;
            }

            var timestamp = FindValue(evt, "timestamp");
            var duration = FindValue(evt, "duration");
            var threadId = FindValue(evt, "thread_id");

            string label;

            var str = FindValue(evt, "label") as string;
            if (str != null)
            {
                label = str;
            }
            else
            {
                var labelInt = (UInt64) FindValue(evt, "label");
                label = stringMapping.ContainsKey(labelInt) ? stringMapping[labelInt] : labelInt.ToString();
            }
            List<EventFieldValue> attributes = new List<EventFieldValue>();
            foreach (var attr in evt.FieldValues)
            {
                if (attr.field.Name != "label" && attr.field.Name != "base")
                {
                    attributes.Add(attr);
                }
            }

            var attrStr = String.Join(", ", attributes.Select(v => string.Format("\"{0}\": \"{1}\"", v.field.Name, v.Value)));

            writer.Write(string.Format(@"{{""name"": ""{0}"", ""ph"": ""X"", ""ts"": {1}, ""dur"": {2}, ""pid"": 0, ""tid"": {3}, ""args"":{{{4}}}}}", label, timestamp, duration, threadId, attrStr));
        }

        public void Dispose()
        {
            writer.Write("]}");
            writer.Dispose();
        }
    }
}


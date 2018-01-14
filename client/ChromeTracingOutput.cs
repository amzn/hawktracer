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

            var labelObj = FindValue(evt, "label");

            if (labelObj != null)
            {
                if (!(labelObj is string))
                {
                    var labelInt = (UInt64)FindValue(evt, "label");
                    labelObj = stringMapping.ContainsKey(labelInt) ? stringMapping[labelInt] : labelInt.ToString();
                }
                WriteCallstackEvent(labelObj, timestamp, duration, threadId, GetAttributeString(evt));
            }
            else
            {
                WriteInstantEvent(FindValue(evt, "name") as string ?? evt.Klass.Name, timestamp, threadId ?? 1, GetAttributeString(evt));
            }
        }

        private static string GetAttributeString(Event evt)
        {
            List<EventFieldValue> attributes = new List<EventFieldValue>();
            foreach (var attr in evt.FieldValues)
            {
                if (attr.field.Name != "label" && attr.field.Name != "base")
                {
                    attributes.Add(attr);
                }
            }
            return String.Join(", ", attributes.Select(v => string.Format("\"{0}\": \"{1}\"", v.field.Name, v.Value)));
        }

        private void WriteCallstackEvent(object label, object timestamp, object duration, object threadId, string attrStr)
        {
            writer.Write(string.Format(@"{{""name"": ""{0}"", ""ph"": ""X"", ""ts"": {1}, ""dur"": {2}, ""pid"": 0, ""tid"": {3}, ""args"":{{{4}}}}}", 
                label, timestamp, duration, threadId, attrStr));
        }

        void WriteInstantEvent(string name, object timestamp, object threadId, string attrStr)
        {
            writer.Write(string.Format(@"{{""name"": ""{0}"", ""ph"": ""i"", ""ts"": {1}, ""pid"": 0, ""tid"": {2}, ""args"":{{{3}}}}}", 
                name, timestamp, threadId, attrStr));
        }

        public void Dispose()
        {
            writer.Write("]}");
            writer.Dispose();
        }
    }
}


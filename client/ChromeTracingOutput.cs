using System;
using System.IO;
using System.Collections.Generic;

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

            writer.Write(@"{""displayTimeUnit"": ""ns"",""traceEvents"": [");
        }

        public void AddMapping(UInt64 id, string label)
        {
            stringMapping.Add(id, label);
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

            var timestamp = ((evt["base"].Value as Event)["base"].Value as Event)["timestamp"].Value;
            var duration = (evt["base"].Value as Event)["duration"].Value;
            var threadId = (evt["base"].Value as Event)["thread_id"].Value;

            string label;

            var str = evt["label"].Value as string;
            if (str != null)
            {
                label = str;
            }
            else
            {
                var labelInt = (UInt64) evt["label"].Value;
                label = stringMapping.ContainsKey(labelInt) ? stringMapping[labelInt] : labelInt.ToString();
            }

            writer.Write(string.Format(@"{{""name"": ""{0}"", ""ph"": ""X"", ""ts"": {1}, ""dur"": {2}, ""pid"": 0, ""tid"": {3}}}", label, timestamp, duration, threadId));
        }

        public void Dispose()
        {
            writer.Write("]}");
            writer.Dispose();
        }
    }
}


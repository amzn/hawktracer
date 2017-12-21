using System;
using System.IO;

namespace HawkTracer.Client
{
    public class ChromeTracingOutput : IDisposable
    {
        readonly StreamWriter writer;
        bool firstEntry = true;

        public ChromeTracingOutput(string filename)
        {
            writer = new StreamWriter(filename);

            writer.Write(@"{""displayTimeUnit"": ""ns"",""traceEvents"": [");
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

            writer.Write(string.Format(@"{{""name"": ""{0}"", ""ph"": ""X"", ""ts"": {1}, ""dur"": {2}, ""pid"": 0, ""tid"": {3}}}", evt["label"].Value, timestamp, duration, threadId));
        }

        public void Dispose()
        {
            writer.Write("]}");
            writer.Dispose();
        }
    }
}


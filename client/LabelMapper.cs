using System;
using System.IO;
using System.Collections.Generic;

namespace HawkTracer.Client
{
    public class LabelMapper
    {
        public struct MapInfo
        {
            public string Label { get; set; }

            public string Category { get; set; }
        }

        private readonly Dictionary<UInt64, MapInfo> mapping = new Dictionary<ulong, MapInfo>();

        public LabelMapper(IEnumerable<string> mapFileNames)
        {
            foreach (var fileName in mapFileNames)
            {
                LoadMapping(fileName);
            }
        }

        public MapInfo GetLabelInfo(UInt64 labelId)
        {
            return mapping.ContainsKey(labelId) ? mapping[labelId] : 
                new MapInfo { Label = labelId.ToString(), Category = "unknown" };
        }

        private void LoadMapping(string fileName)
        {
            using (StreamReader sr = File.OpenText(fileName))
            {
                string line;
                while ((line = sr.ReadLine()) != null)
                {
                    var mappingInfo = line.Split(null);
                    UInt64 id;
                    if (mappingInfo.Length != 3 || !UInt64.TryParse(mappingInfo[2], out id))
                    {
                        Console.WriteLine("[LabelMapper] Invalid line {0}", line);
                        continue;
                    }

                    mapping.Add(id, new MapInfo
                        { 
                            Label = mappingInfo[1], 
                            Category = mappingInfo[0] 
                        });
                }
            }
        }
    }
}


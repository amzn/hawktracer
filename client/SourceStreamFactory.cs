using System.IO;
using System.Text.RegularExpressions;
using System;
using System.Net;
using System.Net.Sockets;

namespace HawkTracer.Client
{
    public static class SourceStreamFactory
    {
        private const string ipRegex = @"^(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})(:(\d{1,5}))?$";
        private const int defaultPort = 8765;

        public static Stream Create(string sourceDescription)
        {
            if (File.Exists(sourceDescription))
            {
                return new FileStream(sourceDescription, FileMode.Open);
            }

            var match = new Regex(ipRegex).Match(sourceDescription);
            return match.Groups.Count == 4 ? 
                CreateNetworkStream(match.Groups[1].Value, GetPort(match.Groups[3].Value)) : null;
        }

        private static int GetPort(string portStr)
        {
            int port = defaultPort;

            if (!String.IsNullOrEmpty(portStr))
            {
                port = int.Parse(portStr);
            }   

            return port;
        }

        private static Stream CreateNetworkStream(string ipAddress, int port)
        {
            IPAddress address = IPAddress.Parse(ipAddress);
            var socket = new Socket(
                address.AddressFamily,
                SocketType.Stream, 
                ProtocolType.Tcp);

            var startTime = DateTime.Now;

            while (!socket.Connected && (DateTime.Now - startTime).Seconds < 15)
            {
                try
                {
                    socket.Connect(new IPEndPoint(address, port));
                }
                catch (System.Net.Sockets.SocketException)
                {
                    System.Threading.Thread.Sleep(10);
                }
            }

            if (!socket.Connected)
            {
                Console.WriteLine("Unable to connect");
                return null;
            }

            return new System.Net.Sockets.NetworkStream(socket);
        }
    }
}


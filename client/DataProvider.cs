using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using System.Collections.Concurrent;

namespace HawkTracer.Client
{
    public class DataProvider
    {
        private readonly Stream streamReader;
        byte[] currentBuffer;
        byte[][] intBuffers;

        int currentBufferPtr;
        readonly BlockingCollection<byte[]> bufferQueue = new BlockingCollection<byte[]>();
        Thread readDataThread;

        public DataProvider(Stream streamReader)
        {
            this.streamReader = streamReader;
            IsEOF = false;

            InitIntBuffers();

            readDataThread = new Thread(ReadFromStream);
            readDataThread.Start();
        }

        private void ReadFromStream()
        {
            bool ok = true;
            while (ok)
            {
                var buff = new byte[1024]; // TODO configurable & use bufferpool
                int read = streamReader.Read(buff, 0, 1024);
                if (read == 0)
                {
                    ok = false;
                    bufferQueue.Add(null);
                }
                else
                {
                    Array.Resize(ref buff, read);
                    bufferQueue.Add(buff);
                }
            }
        }

        private void InitIntBuffers()
        {
            intBuffers = new byte[8][];

            foreach (var size in new []{1, 2, 4, 8})
            {
                intBuffers[size - 1] = new byte[size];
            }
        }

        private byte[] GetIntBuffer(int size)
        {
            return intBuffers[size - 1];
        }

        public bool IsEOF { get; private set; }

        public string ReadString()
        {
            string val = "";
            bool done = false;
            do
            {
                int data = ReadByte();
                switch (data)
                {
                    case -1:
                        throw new Exception("EOF");
                    case 0:
                        done = true;
                        break;
                    default:
                        val += (char)data;
                        break;
                }
            } while (!done);
            return val;
        }

        public object ReadInteger(Type type)
        {
            var size = Marshal.SizeOf(type);
            if (size == 1)
            {
                int data = ReadByte();
                if (data == -1)
                {
                    throw new Exception("EOF");
                }
                return (byte)data;
            }
            var buff = GetIntBuffer(size);
            if (Read(buff, 0) == 0)
            {
                throw new Exception("EOF");
            }

            return typeof(BitConverter).GetMethod("To" + type.Name).Invoke(null, new object[] { buff, 0 });
        }

        public T ReadInteger<T>()
        {
            return (T)ReadInteger(typeof(T));
        }

        private int ReadByte()
        {
            return Read(GetIntBuffer(1), 0) > 0 ? GetIntBuffer(1)[0] : -1;
        }

        private bool LoadNextBuffer()
        {
            if (currentBuffer != null && currentBuffer.Length > currentBufferPtr)
            {
                return true;
            }
            else if (IsEOF)
            {
                return false;
            }

            currentBuffer = bufferQueue.Take();
            if (currentBuffer == null)
            {
                IsEOF = true;
                return false;
            }
            else
            {
                currentBufferPtr = 0;
                return true;
            }
        }

        int Read(byte[] buffer, int offset)
        {
            int expectedSize = buffer.Length;
            int readData = offset;
            while (LoadNextBuffer() && expectedSize > 0)
            {
                var size = Math.Min(expectedSize, currentBuffer.Length - currentBufferPtr);
                Array.Copy(currentBuffer, currentBufferPtr, buffer, readData, size);
                currentBufferPtr += size;
                expectedSize -= size;
                readData += size;
            }

            return readData - offset;
        }
    }
}


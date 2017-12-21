using System;
using System.Collections.Generic;

namespace HawkTracer.Client
{
    public class BufferPool
    {
        private readonly int bufferSize;
        private readonly int minBufferCount;
        private readonly Queue<byte[]> buffers = new Queue<byte[]>();
        private readonly object queueLock = new object();

        public BufferPool(int initialBufferCount, int bufferSize)
        {
            this.bufferSize = bufferSize;
            minBufferCount = initialBufferCount;

            for (int i = 0; i < minBufferCount; i++)
            {
                buffers.Enqueue(new byte[bufferSize]);
            }
        }

        public byte[] Get()
        {
            lock (queueLock)
            {
                if (buffers.Count > 0)
                {
                    return buffers.Dequeue();
                }
            }
            return new byte[bufferSize];
        }

        public void Take(byte[] buffer)
        {
            lock (queueLock)
            {
                buffers.Enqueue(buffer);
            }
        }

    }
}


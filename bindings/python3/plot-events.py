"""Example HawkTracer client

This application demonstrates how to implement simple
HawkTracer client using hawktracer.client.Client class.
This application visualizes selected event's field values
on a graph.
"""

import argparse
import hawktracer.client
import matplotlib.pyplot as plt
import os
import time

example_usage = '''
Example:
    python {} -s 127.0.0.1:8765 -e HT_CallstackIntEvent -f duration
'''.format(os.path.basename(__file__))

parser = argparse.ArgumentParser(description='Draw selected field from the event\'s klass on a graph.',
                                 epilog=example_usage,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
parser.add_argument('-s', '--source', help='Description of the source; either file name or IP address', required=True)
parser.add_argument('-e', '--klass', help='Klass of event to draw on the graph', required=True)
parser.add_argument('-f', '--field', help='A field of event\'s klass to draw on the graph', required=True)
args = parser.parse_args()

client = hawktracer.client.Client()
client.start(args.source)

data = []
was_eos = False

while True:
    event = client.poll_event()
    if event:
        if event[0] == args.klass and args.field in event[1]:
            data.append(event[1][args.field])
        else:
            continue
    else:
        if was_eos:
            break
        was_eos = client.eos()
        time.sleep(0.1)
        continue

    length = len(data)
    plt.xlim(length - 10, length)
    plt.plot(list(range(1, length+1)), data)
    plt.show(block=False)
    plt.pause(0.05)

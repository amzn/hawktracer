""" Lognplot GUI client HawkTracer client

This application forwards hawktracer events into the lognplot
GUI tool for further visualization.
"""

import argparse
from hawktracer.client import Client as HawkTracerClient
from lognplot.client import LognplotTcpClient

import os
import time

example_usage = """
Example:
    python lognplot-adapter.py -s 127.0.0.1:8765
"""

parser = argparse.ArgumentParser(
    description="Forward events into lognplot GUI for real-time visualization.",
    epilog=example_usage,
    formatter_class=argparse.RawDescriptionHelpFormatter,
)
parser.add_argument(
    "-s",
    "--source",
    help="Description of the hawktracer source; either file name or IP address",
    required=True,
)
parser.add_argument("--lognplot-hostname", default="127.0.0.1", type=str)
parser.add_argument("--lognplot-port", default="12345", type=int)

args = parser.parse_args()

# Connect to hawktracer:
hawktracer_client = HawkTracerClient()
hawktracer_client.start(args.source)

# Connect to lognplot:
lognplot_client = LognplotTcpClient(
    hostname=args.lognplot_hostname, port=args.lognplot_port
)
lognplot_client.connect()

# Harvest data:

was_eos = False

while True:
    event = hawktracer_client.poll_event()

    if event:
        event_type, data = event
        if event_type == "HT_CallstackStringEvent":
            # Convert timestamp to second-ish units:
            timestamp = float(data["timestamp"]) / 1e9
            value = float(data["duration"])
            label = "hawk_tracer_{}".format(data["label"])
            lognplot_client.send_sample(label, timestamp, value)
        else:
            print("unhandled event", event_type, event)

    else:
        if was_eos:
            break
        was_eos = hawktracer_client.eos()
        time.sleep(0.01)

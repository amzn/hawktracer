import argparse
import HawkTracer
import matplotlib.pyplot as plt
import time


parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument('-s', '--source',
                    help='Description of the source; either file name or IP address',
                    required=True)
args = parser.parse_args()

client = HawkTracer.Client()
client.start(args.source)

max_inactive_duration = 5

last_active_time = time.time()
data = []

while time.time() - last_active_time < max_inactive_duration:
    event = client.poll_event()
    if event:
        last_active_time = time.time()
        if event[0] == "HT_CallstackStringEvent":
            data.append(event[1]["duration"])
        else:
            continue
    else:
        time.sleep(0.1)
        continue

    length = len(data)
    plt.xlim(length - 10, length)
    plt.plot(list(range(1, length+1)), data)
    plt.pause(0.05)

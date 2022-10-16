#stress test

import requests
from threading import Thread
addr = "http://localhost:8080/test"
class StressTester(Thread):

	def run(self):
		while(True):
			resp = requests.get(url=addr)

workers = []
for x in range(8):
    worker = StressTester()
    # Setting daemon to True will let the main thread exit even though the workers are blocking
    worker.daemon = True
    workers.append(worker)
    worker.start()



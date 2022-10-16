#stress test

import requests
from threading import Thread
addr = "http://localhost:8080/test"
class StressTester(Thread):

	def run(self):
		while(True):
			resp = requests.get(url=addr)

workers = []
for x in range(16):
    worker = StressTester()
    worker.daemon = True
    workers.append(worker)
    worker.start()


for i in workers:
	i.join()


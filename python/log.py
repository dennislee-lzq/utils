#!/usr/bin/python


import logging  
import sys
from logging.handlers import RotatingFileHandler

'''
logger = logging.getLogger("sslvpn")  
#formatter = logging.Formatter('%(name)-12s %(asctime)s %(levelname)-8s: %(message)s', '%a, %d %b %Y %H:%M:%S',)  
formatter = logging.Formatter('%(asctime)s-%(name)s-%(levelname)-8s: %(message)s', '%Y-%m-%d %H:%M:%S')  
file_handler = logging.FileHandler("test.log")  
file_handler.setFormatter(formatter)  
stream_handler = logging.StreamHandler(sys.stderr) 
stream_handler.setFormatter(formatter)  
logger.addHandler(file_handler)  
logger.addHandler(stream_handler)  
#logger.setLevel(logging.ERROR)  
logger.setLevel(logging.DEBUG)  

logger.critical("test error log")  
logger.error("test error log %s  %d", "this", 10)
logger.info("test error log %s  %d", "this", 10)  
logger.debug("test error log %s  %d", "this", 10)  
logger.warn("test error log %s  %d", "this", 10) 
logger.removeHandler(stream_handler)  
logger.error("fuckgov")  

sublog = logging.getLogger("sslvpn.sub")
sublog.addHandler(file_handler)  
sublog.addHandler(stream_handler) 
sublog.error("fuckgov")  
'''
Rthandler = RotatingFileHandler('myapp.log', maxBytes=512,backupCount=5)
Rthandler.setLevel(logging.DEBUG)

report = RotatingFileHandler('report.log', maxBytes=512,backupCount=5)
report.setLevel(logging.INFO)

formatter = logging.Formatter('%(name)-12s: %(levelname)-8s %(message)s')
Rthandler.setFormatter(formatter)
report.setFormatter(formatter)

logging.getLogger('').addHandler(Rthandler)
logging.getLogger('').addHandler(report)

logging.critical("test critical log")  
logging.error("test error log %s  %d", "this", 10)
logging.warn("test..warn...LOG...........");
logging.info("test error log %s  %d", "this", 10)  
logging.debug("test error log %s  %d", "this", 10)  
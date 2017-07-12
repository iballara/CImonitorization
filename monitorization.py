'''
    This script is for monitoring both jenkins and devel tomcat 
    of the back-end department of Vicens Vives Digital.
    
    The aim is to send the status of all jenkins jobs and the tomcat running on devel environment.
    
    The jenkins jobs to be monitored are all declared in "jobs" array:
        jobs = ["engine_api_ci_devel", "user_api_ci_devel", "engine_db_clean_devel", "engine_php_sdk_ci", "peacemaker_ci", "bff_ci_devel", "bfa_ci_devel"]
    
    Tomcat is running on http://devel:8080
    
    INPUT: no input
    OUTPUT: {jenkins_job_status}-{tomcat_status}
    WHERE: jenkins_status : {'b', 'r', 'a', 'y'}
           tomcat_status : {'1', '0'}
    
    When one of the jobs' result is different than "SUCCESS", it takes control of the NodeMcu LEDs
    until its status is SUCCESS again.

    We send packets using UDP transport layer protocol.

    Author: Ignasi Ballara - VVD
    Version: 4.0 - Including UDP.

'''

import json
import sys
import urllib2
import time
import socket
import requests

# Configurations
ping_server = 1
ping_when_not_success = 5
NAME = 0
BUILD = 1
UDP_IP = "192.168.2.23" # Address of NodeMcu
UDP_IP_FRONT = "192.168.2.26"
UDP_IP_MOBILE = "192.168.2.27"
UDP_IP_QA="192.168.2.28"
UDP_PORT = 2390 # Port of NodeMcu (arbitrary)
sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM) 
# Jobs to be monitored.
jobs = ["engine_api_ci_devel", "user_api_ci_devel", "engine_db_clean_devel", "engine_php_sdk_ci", "peacemaker_ci", "bff_ci_devel", "bfa_ci_devel"]

# Arduino Configuration
SUCCESS = 'b' 
FAILURE = 'r'
BUILDING = 'a'
UNSTABLE = 'y'
TOMCAT_OK = '1'
TOMCAT_NO_OK = '0'
time.sleep(5)

# Returns the status of a job ans its name.
def get_status_of_job(jobName):
    jenkinsUrl = "http://jenkins.vicensvivesdigital.com:10080/job/"
    try:
        request = urllib2.Request( jenkinsUrl + jobName + "/lastBuild/api/json", headers={"Authorization" : "Basic am9hcXVpbS5jYWx2bzpqb2FxdWltVlYyMDE2"} )
        contents = urllib2.urlopen(request)
    except urllib2.HTTPError, e:
        print "URL Error: " + str(e.code)
        print "      (job name [" + jobName + "] probably wrong)"
        sys.exit(2)
    try:
        buildStatusJson = json.load(contents)
    except:
        print "Failed to parse json"
        sys.exit(3)

    build_status = buildStatusJson["result"]

    if build_status == "SUCCESS":
        status = SUCCESS
    elif build_status == None:
        status = BUILDING
    elif build_status == "FAILURE":
        status = FAILURE
    elif build_status == "UNSTABLE":
        status = UNSTABLE

    print ""
    print "----------NEW--DATA-----------"
    if build_status != None:
        print "Job " + jobName + ": " + build_status
    else:
        print "Job " + jobName + " is building"

    return jobName, status

#Returns the status of the devel tomcat.
def check_devel_tomcat():
    try:
        status = requests.get('http://devel:8080').status_code
    except requests.exceptions.HTTPError:
        status = 500
    except requests.exceptions.ConnectionError:
        status = 500

    if status == 200:
        return TOMCAT_OK
    else:
        return TOMCAT_NO_OK

def send_data(data_to_send):
    try:
        sock.sendto(data_to_send, (UDP_IP, UDP_PORT))
        sock.sendto(data_to_send, (UDP_IP_FRONT, UDP_PORT))
        #sock.sendto(data_to_send, (UDP_IP_MOBILE, UDP_PORT))
        sock.sendto(data_to_send, (UDP_IP_QA, UDP_PORT))
    except socket.error, v:
        if v[0] == 65 or v[0] == 64:
            print "No route found"

# MAIN
while(1):

    for job in jobs:
        job_status = get_status_of_job(job)

        # If a job status is != from SUCCESS, it will take control of the UDP comm.
        while job_status[BUILD] != SUCCESS:
            # We check the Tomcat's status.
            tomcat_status = check_devel_tomcat()
            print "Tomcat Status: " + tomcat_status
            # We set the data we will send to arduino.
            data_to_send = job_status[BUILD] + "-" + tomcat_status
            print "data_to_send: " + data_to_send
            # We send the data. Typical: 'b-1'
            send_data(data_to_send)
            # We wait for some seconds before relaunching the request.
            time.sleep(ping_when_not_success)
            # We check the status of the job.
            job_status = get_status_of_job(job)

        tomcat_status = check_devel_tomcat()
        print "Tomcat Status: " + tomcat_status
        data_to_send = job_status[BUILD] + "-" + tomcat_status
        print "data_to_send: " + data_to_send
        send_data(data_to_send)
        time.sleep(ping_server)
    

#!/usr/bin/env python
# 2017 Bartholomaeus Dedersen
# Example OTA for ESP 8266 over http

from flask import Flask
from flask import request, jsonify, abort
from flask_api import status
from flask_restful import Resource, Api
from flask_restful import reqparse

from pprint import pformat
import netifaces as ni
import os

ni.ifaddresses('eth0')
md5Argument = 'HTTP_X_ESP8266_SKETCH_MD5'
#md5Argument = 'test'

ip = ni.ifaddresses('eth0')[ni.AF_INET][0]['addr']
parser = reqparse.RequestParser()

app = Flask(__name__)
api = Api(app)
wsgi_app = app.wsgi_app
app.debug = True


class FirmwareCheck(Resource):
  def get(self):
    args = parser.parse_args()

    clientMD5 = args.get(md5Argument)
    #clientMD5 = request.args.get(md5Argument)

    #if args is None or args[md5Argument] is None:
    if clientMD5 is None:
      return 'error from ' + args['User-Agent'], 500

    clientFirmwareMD5 = args[md5Argument]


    return 'sorry' + clientFirmwareMD5, 304

if __name__ == '__main__':
  parser.add_argument(md5Argument, location="headers", type=unicode, required=True)
  parser.add_argument('User-Agent', location="headers")
  api.add_resource(FirmwareCheck, '/esp.html')
  app.run(host=ip, port = 8887)

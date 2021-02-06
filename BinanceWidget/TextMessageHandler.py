import json
import twilio.base

from Alerter import IAlerter

from os import path
from twilio.rest import Client as SmsClient


def _initialize_sms_service(config_path):
    if path.isfile(config_path):
        with open(config_path) as sms_config:
            try:
                sms_config_json = json.load(sms_config)
                new_sms_client = SmsClient(sms_config_json['account_sid'], sms_config_json['auth_token'])
            except json.decoder.JSONDecodeError:
                print("Error: Wrong format in " + config_path)
                return [{}, {}]
    else:
        print("Error: Failed to find sms-config: " + config_path)
        return [{}, {}]

    return [new_sms_client, sms_config_json]


class CTextMessageHandler(IAlerter):

    def __init__(self):
        [self._sms_client, self._sms_config_data] = _initialize_sms_service('sms-config.json')
        if len(self._sms_config_data) < 4:
            print("Error: not enough data in sms-config, alert service will not work!")

    # IAlerter
    def __alert__(self, message):
        self._send_text_message(message)

    def _send_text_message(self, message):
        try:
            from_number = self._sms_config_data['from_phone_number']
            to_number = self._sms_config_data['to_phone_number']
            self._sms_client.messages.create(
                body=message,
                from_=from_number,
                to=to_number
            )

        except (twilio.base.exceptions.TwilioRestException, json.decoder.JSONDecodeError):
            print("Error: Can't send any text message to alert, check if sms-config is correct")
